// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// rbp.h

#ifndef rbp_h
#define rbp_h

#include "bp.h"
#include <Wizard>

class RBpConSpec;
class RBpUnit;
class RBpUnitSpec;//

// _

// the unit maintains activation and other state information automatically
// and the size of this list is used to determine when to backpropagate
// depending on the parameters in the unit spec (time window, bp window).
// also, the process can excplicitly backprop at any given point

// instead of keeping big lists of all sorts of things, only the essential forward
// pass values are maintained, and the rest is propagated sequentially during the
// backpropagation phase
// thus, prv_xxx values are kept of all variables, and arrays of only the activations
// are kept.  at each act step, the time is shifted forward (cur becomes prv)
// and at each bp step, time is shifted backwards (prv becomes cur)

// forward passes: do Compute_Netin() on whole net, then Compute_Act()
// store resulting act values, etc.

// backward passes: do Compute_Error, then Compute_Send_dEdNet, the Step_Back
// and iterate..

eTypeDef_Of(RBpConSpec);

class E_API RBpConSpec : public BpConSpec {
  // Recurrent Backprop Con Spec
INHERITED(BpConSpec)
public:
  inline void 	Compute_dWt(ConGroup* cg, Unit* ru, Network* net) override;
  // Compute dE with respect to the weights (using prv_act) as sender

  TA_BASEFUNS_NOCOPY(RBpConSpec);
private:
  void 	Initialize()		{ };
  void	Destroy()		{ };
};

eTypeDef_Of(SymRBpConSpec);

class E_API SymRBpConSpec : public RBpConSpec {
  // Recurrent Backprop Con Spec: option to maintain weight symmetry through simple averaging of two weight changes
INHERITED(RBpConSpec)
public:
  bool	sym_wt_updt;		// if true, use symmetric weight updates

  inline void C_Compute_dWt_Sym(float& dwt, const float ru_dEdNet,
                                const float su_act, const float su_dEdNet,
                                const float ru_act)
  { dwt += 0.5f * (su_act * ru_dEdNet + ru_act * su_dEdNet); }
  inline void 		Compute_dWt(ConGroup* cg, Unit* ru, Network* net) override;

  TA_SIMPLE_BASEFUNS(SymRBpConSpec);
private:
  void 	Initialize()		{ sym_wt_updt = true; }
  void	Destroy()		{ };
};


eTypeDef_Of(RBpUnitSpec);

class E_API RBpUnitSpec : public BpUnitSpec {
  // Recurrent Backprop Unit Specification
INHERITED(BpUnitSpec)
public:
  enum TimeAvgType {	// type of time-averaging to perform
    ACTIVATION,		// time-average the activations
    NET_INPUT 		// time-average the net inputs
  };

  float		dt;		// grain of time computing on (must be in [0..1] range)
  TimeAvgType	time_avg;	// type of time-averaging to perform
  bool		soft_clamp;	// apply input data as a net input instead of as a direct activation value to clamp to
  bool		fast_hard_clamp_net; // #CONDEDIT_OFF_soft_clamp:true compute netin from hard clamped layers only once: ONLY IF ALL LAYERS HARD CLAMPED
  float		soft_clamp_gain; // #CONDEDIT_ON_soft_clamp:true gain on the soft clamping
  bool		teacher_force;	 // use teacher forcing
  bool		store_states;    // store activity states (usually true, except in AP mode)
  RandomSpec	initial_act;	 // initial activation value
  bool		updt_clamped_wts; // update weights for clamped units: need this for symmetric cons!

  virtual void	ResetStored(RBpUnit* u, BpNetwork* net, int thread_no=-1);
  // reset the stored state values, which clears bp sequence and resets time, but leaves current state intact
  virtual void  Compute_ClampExt(RBpUnit* u, BpNetwork* net, int thread_no=-1);
  // compute activations resulting from clamped external input (for initial state)

  virtual void	Compute_HardClampNet(RBpUnit* u, BpNetwork* net, int thread_no=-1);
  // for fast-hard-clamp-net: call this first
  virtual void	Compute_Act_impl(RBpUnit* u, BpNetwork* net, int thread_no=-1);

  void	Init_Acts(Unit* u, Network* net) override;
  void	Compute_Netin(Unit* u, Network* net, int thread_no=-1) override;
  void	Compute_Act(Unit* u, Network* net, int thread_no=-1) override;
  void	Compute_dEdA(BpUnit* u, BpNetwork* net, int thread_no=-1) override;
  void Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no=-1) override;
  void Compute_dWt(Unit* u, Network* net, int thread_no=-1) override;
  void Compute_Weights(Unit* u, Network* net, int thread_no=-1) override;

  TA_SIMPLE_BASEFUNS(RBpUnitSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

eTypeDef_Of(RBpUnit);

class E_API RBpUnit : public BpUnit {
  // recurrent BP unit
INHERITED(BpUnit)
public:
  float		da;		// delta-activation (change in activation value)
  float		ddE;		// delta-delta-Error (change in error derivative)

  ExtType	prv_ext_flag;	// #HIDDEN previous external input flag
  float		prv_targ;	// #NO_VIEW #HIDDEN previous target value
  float		prv_ext;	// #NO_VIEW #HIDDEN previous external input value
  float		prv_act;	// #NO_VIEW #HIDDEN previous activation value
  float		act_raw;	// #NO_VIEW current raw activation value
  float		prv_net;	// #NO_VIEW #HIDDEN previous net-input value
  float		prv_dEdA;	// #NO_VIEW #HIDDEN previous dEdA value
  float		prv_dEdNet;	// #NO_VIEW #HIDDEN previous dEdNet value
  float		clmp_net;	// #NO_VIEW #HIDDEN net input from hard-clamped input layers

  float_CircBuffer ext_flags;	// #NO_VIEW array of external_flag values
  float_CircBuffer targs;	// #NO_VIEW array of target values
  float_CircBuffer exts;	// #NO_VIEW array of external input values
  float_CircBuffer acts;	// #NO_VIEW array of activation values

  void 		Init_InputData();	// keep prv_values..
  virtual void	StoreState();
  // store state information in buffers
  void		InitForBP()	{ prv_dEdA = prv_dEdNet = 0.0f; }
  // initialization prior to backpropagation (prv_dEdA, prv_dEdNet)
  virtual bool	StepBack(int tick);
  // move units backwards in time at given tick position in array
  virtual bool	RestoreState(int tick);
  // restore state of units to current values at given tick
  virtual void	ShiftBuffers(int ticks);
  // shift unit data buffers by given number of ticks

  // spec functions
  void    ResetStored(BpNetwork* net, int thread_no=-1)
  { ((RBpUnitSpec*)GetUnitSpec())->ResetStored(this, net, thread_no); }
  void    Compute_ClampExt(BpNetwork* net, int thread_no=-1)
  { ((RBpUnitSpec*)GetUnitSpec())->Compute_ClampExt(this, net, thread_no); }
  void    Compute_HardClampNet(BpNetwork* net, int thread_no=-1)
  { ((RBpUnitSpec*)GetUnitSpec())->Compute_HardClampNet(this, net, thread_no); }

  void	InitLinks() override;
  void	Copy_(const RBpUnit& cp);
  TA_BASEFUNS(RBpUnit);
private:
  void 	Initialize();
  void	Destroy()		{ };
};

inline void RBpConSpec::Compute_dWt(ConGroup* cg, Unit* ru, Network* net) {
  const float ru_dEdNet = ((BpUnit*)ru)->dEdNet;
  float* dwts = cg->OwnCnVar(DWT);
  CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_dEdNet,
                                   ((RBpUnit*)cg->Un(i,net))->prv_act));
}

inline void SymRBpConSpec::Compute_dWt(ConGroup* cg, Unit* ru, Network* net) {
  const float ru_dEdNet = ((BpUnit*)ru)->dEdNet;
  const float ru_act = ((RBpUnit*)ru)->prv_act;
  float* dwts = cg->OwnCnVar(DWT);
  if(sym_wt_updt) {
    CON_GROUP_LOOP(cg, C_Compute_dWt_Sym(dwts[i], ru_dEdNet,
                                         ((RBpUnit*)cg->Un(i,net))->prv_act,
                                         ((RBpUnit*)cg->Un(i,net))->dEdNet,
                                         ru_act));
  }
  else {
    CON_GROUP_LOOP(cg, C_Compute_dWt(dwts[i], ru_dEdNet,
                                     ((RBpUnit*)cg->Un(i,net))->prv_act));
  }
}

eTypeDef_Of(RBpContextSpec);

class E_API RBpContextSpec : public RBpUnitSpec {
  // RBp version of context units in simple recurrent nets (SRN), expects one-to-one prjn from layer it copies, Trial->CopyContext() must be called by script to update!
INHERITED(RBpUnitSpec)
public:
  float		hysteresis;	 // hysteresis factor: (1-hyst)*new + hyst*old
  float		hysteresis_c;	 // #READ_ONLY complement of hysteresis
  String	variable;	 // name of unit variable to copy into
  Unit::ExtType	unit_flags;	 // flags to set on the unit after copying value
  MemberDef*	var_md;		 // #IGNORE memberdef of variable

  virtual void	CopyContext(RBpUnit* u);
  // copy the values in to the context units: called by trial CopyContext under control of a script..
  void 		Compute_Act(Unit* u, Network* net, int thread_no=-1) override;

  // nullify all other functions..
  void 		Compute_Netin(Unit*, Network* net, int thread_no=-1) 	override { };
  void 		Init_dWt(Unit*, Network* net, int thread_no=-1) 	override { };
  void 		Compute_dWt(Unit*, Network* net, int thread_no=-1) 	override { };
  void 		Compute_Weights(Unit*, Network* net, int thread_no=-1) 	override { };

  // bp special functions
  void	Compute_HardClampNet(RBpUnit*, BpNetwork* net, int thread_no=-1) override { };
  void Compute_Error(BpUnit*, BpNetwork* net, int thread_no=-1)	override { };
  void Compute_dEdA(BpUnit*, BpNetwork* net, int thread_no=-1)		override { };
  void Compute_dEdNet(BpUnit*, BpNetwork* net, int thread_no=-1)	override { }; //

//obs  bool  CheckConfig(Unit* un, Layer* lay, TrialProcess* tp);

  TA_SIMPLE_BASEFUNS(RBpContextSpec);
protected:
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
};

//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

eTypeDef_Of(NoisyRBpUnitSpec);

class E_API NoisyRBpUnitSpec : public RBpUnitSpec {
  // RBp with noisy output signal (act plus noise)
INHERITED(RBpUnitSpec)
public:
  RandomSpec	noise;		// what kind of noise to add to activations
  float		sqrt_dt; 	// #HIDDEN square-root of dt for noise

  void 		Compute_Act_impl(RBpUnit* u, BpNetwork* net, int thread_no=-1);

  TA_SIMPLE_BASEFUNS(NoisyRBpUnitSpec);
protected:
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void 	Destroy()		{ };
};

//////////////////////////////////
//	Bp Wizard		//
//////////////////////////////////

eTypeDef_Of(BpWizard);

class E_API BpWizard : public Wizard {
  // #STEM_BASE backprop-specific wizard for automating construction of simulation objects
INHERITED(Wizard)
public:
  virtual bool	SRNContext(Network* net);
  // #MENU_BUTTON #MENU_ON_Network #MENU_SEP_BEFORE configure a simple-recurrent-network context layer in the network

//obs  virtual bool	ToTimeEvents(Environment* env);
  // #MENU_BUTTON #MENU_ON_Environment #MENU_SEP_BEFORE convert events, groups, and environment to TimeEvent format
//obs  virtual void	ToRBPEvents(Environment* env, int targ_time = 2);
  // #MENU_BUTTON convert events to format suitable for training by RBP, with inputs coming on first, and then targets coming on after targ_time time steps

  bool	StdProgs() override;
  bool	TestProgs(Program* call_test_from, bool call_in_loop=true, int call_modulus=1) override;

  TA_BASEFUNS_NOCOPY(BpWizard);
protected:
  String RenderWizDoc_network() override;
private:
  void 	Initialize();
  void 	Destroy()	{ };
};

#endif // rbp_h


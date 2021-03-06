// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef GPiInvUnitSpec_h
#define GPiInvUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GPiMiscSpec);

class E_API GPiMiscSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weighting of Go vs. NoGo inputs
INHERITED(SpecMemberBase)
public:
  float         net_gain;        // #DEF_3 extra netinput gain factor to compensate for reduction in netinput from subtracting away nogo -- this is IN ADDITION to adding the nogo factor as an extra gain: net = (net_gain + nogo) * (go_in - nogo * nogo_in)
  float         nogo;            // #MIN_0 #DEF_1;0.1 how much to weight NoGo inputs relative to Go inputs (which have an implied weight of 1.0)
  float         gate_thr;        // #DEF_0.2 threshold applied to activation to drive gating -- when any unit activation gets above this threshold, it sends the activation through sending projections to the thal field in Matrix units, otherwise it sends a 0
  bool          thr_act;         // #DEF_true apply threshold to unit act_eq activations (i.e., turn off units that are below threshold) -- this is what is sent to the InvertUnitSpec, so we effectively threshold the gating output
  float         min_thal;        // #DEF_0.2 minimum thal value to send to thal on layers we project to -- range between gate_thr and 1.0 is mapped into min_thal:1.0 range -- e.g., setting to 1.0 will effectively produce binary gating outputs -- set to 0 (or < gate_thr) to retain raw gate_thr to 1.0 range

  float         tot_gain;        // #HIDDEN #EXPERT net_gain + nogo
  float         thal_rescale;    // #HIDDEN #EXPERT (1.0 - min_thal) / (1.0 - gate_thr) -- multiplier for rescaling thal value from act_eq >= gate_thr onto min_thal range
  
  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(GPiMiscSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiInvUnitSpec);

class E_API GPiInvUnitSpec : public LeabraUnitSpec {
  // #AKA_GPiUnitSpec Inverted GPi globus pallidus internal segment, analogous with SNr -- major output pathway of the basal ganglia.  This integrates Go and NoGo inputs, computing netin = Go - go_nogo.nogo * NoGo -- unlike real GPi units are typically off, and the most active wins through inhibitory competition -- also sends act to thal field on layers that it sends to -- can be used directly as a sole SNrThal gating layer, or indrectly with InvertUnitSpec to mimic actual non-inverted GPi in a projection to thalamus layer
INHERITED(LeabraUnitSpec)
public:
  GPiMiscSpec    gpi;      // parameters controlling the gpi functionality: how to weight the Go vs. NoGo pathway inputs, and gating threshold

  void	Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  virtual void  Send_Thal(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the act value as thal to sending projections: every cycle

  void	Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  // no learning in this one..
  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(UnitVars* u, Network* net, int thr_no) { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Layer* lay, bool quiet=false) override;
  void  HelpConfig();   // #BUTTON get help message for configuring this spec

  TA_SIMPLE_BASEFUNS(GPiInvUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // GPiInvUnitSpec_h

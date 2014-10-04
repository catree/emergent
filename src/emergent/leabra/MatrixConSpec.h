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

#ifndef MatrixConSpec_h
#define MatrixConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>
#include <MatrixCon>

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixLearnSpec);

class E_API MatrixLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for learning in the matrix 
INHERITED(SpecMemberBase)
public:
  float         da_learn_thr;   // Threshold on dopamine magnitude required to drive learning of matrix weights, and consequent clearing of the accumulated trace of gating activations -- set this high enough that it filters out small random DA fluctuations, but not the significant learning events

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixLearnSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(MatrixConSpec);

class E_API MatrixConSpec : public LeabraConSpec {
  // Learning of matrix input connections based on sender * receiver activation product and thal gating activation signal, which accumulate in an ongoing synaptic trace over time, until they are multiplied by a later dopamine dav value that is typically driven by primary value (US) outcome at end of a sequence of actions -- dwt = dav * thal * su * ru -- the trace is reset when this weight change is computed, as a result of an over-threshold level of dopamine.  Patch units shunt dopamine from actively maintaining stripes / information processing channels
INHERITED(LeabraConSpec)
public:
  enum MtxConVars {
    NTR = SWT+1,           // new trace -- drives updates to trace value -- thal * ru * su
    TR,                    // current ongoing trace that drives learning -- adds ntr and decays after learning on current values
  };
  bool                  nogo;    // are these nogo con specs -- if so, flip the sign of the dopamine signal
  MatrixLearnSpec       matrix;  // parameters for special matrix learning dynamics

  inline void Init_Weights(BaseCons* cg, Unit* ru, Network* net) override {
    Init_Weights_symflag(net);
    if(cg->prjn->spec->init_wts) return; // we don't do it, prjn does

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i]);
        C_Init_dWt(dwts[i]);
        trs[i] = 0.0f;
        ntrs[i] = 0.0f;
      }
    }
  }

  inline void C_Compute_dWt_Matrix_Tr
    (float& dwt, float& tr, float& ntr, const float decay_dt, const float mtx_da,
     const float ru_thal, const float ru_act, const float su_act) {
    if(fabs(mtx_da) >= matrix.da_learn_thr) {
      dwt += cur_lrate * mtx_da * tr; // first learn based on cur trace
      tr = 0.0f;                      // and reset trace
    }
    ntr = ru_thal * ru_act * su_act; // then add in any new trace increment
    tr += ntr;                       // just keep accumulating..
  }
  // #IGNORE

  inline void Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                       LeabraNetwork* net) override {
    if(ignore_unlearnable && net->unlearnable_trial) return;

    float* dwts = cg->OwnCnVar(DWT);
    float* ntrs = cg->OwnCnVar(NTR);
    float* trs = cg->OwnCnVar(TR);
    
    const float decay_dt = matrix.tr_decay_dt;
    
    const int sz = cg->size;
    if(nogo) {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        C_Compute_dWt_Matrix_Tr(dwts[i], trs[i], ntrs[i], decay_dt,
                                -ru->dav, ru->thal, ru->act_eq, su->act_eq);
        // only diff is -dav for nogo
      }
    }
    else {
      for(int i=0; i<sz; i++) {
        LeabraUnit* ru = (LeabraUnit*)cg->Un(i,net);
        C_Compute_dWt_Matrix_Tr(dwts[i], trs[i], ntrs[i], decay_dt,
                                ru->dav, ru->thal, ru->act_eq, su->act_eq);
      }
    }
  }

  TA_SIMPLE_BASEFUNS(MatrixConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // MatrixConSpec_h

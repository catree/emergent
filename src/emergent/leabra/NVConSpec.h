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

#ifndef NVConSpec_h
#define NVConSpec_h 1

// parent includes:
#include <PVConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NVConSpec);

class E_API NVConSpec : public PVConSpec {
  // novelty value connection spec: learns using delta rule from act_p - act_m values -- does not use hebb or err_sb parameters -- has decay to refresh novelty if not seen for a while..
INHERITED(PVConSpec)
public:
  float         decay;          // #MIN_0 amount to decay weight values every time they are updated, restoring some novelty (also multiplied by lrate to compute weight change, so it automtically takes that into account -- think of as a pct to decay)

  inline void C_Compute_Weights_NV(float& wt, float& dwt, float& pdw,
                                   float& lwt, const float swt, const float dkfact) {
    float lin_wt = LinFmSigWt(lwt);
    dwt -= dkfact * lin_wt;
    if(dwt != 0.0f) {
      if(lmix.err_sb) {         // check for soft-bounding -- typically not enabled for pv
        if(dwt > 0.0f)	dwt *= (1.0f - lin_wt);
        else		dwt *= lin_wt;
      }
      lwt = SigFmLinWt(lin_wt + dwt);
      C_Compute_EffWt(wt, swt, lwt);
    }
    pdw = dwt;
    dwt = 0.0f;
  }
  // #IGNORE

  inline void Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                    LeabraNetwork* net) override {
    float dkfact = cur_lrate * decay;
    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* pdws = cg->OwnCnVar(PDW);
    float* lwts = cg->OwnCnVar(LWT);
    float* swts = cg->OwnCnVar(SWT);

    CON_GROUP_LOOP(cg, C_Compute_Weights_NV(wts[i], dwts[i], pdws[i], 
                                            lwts[i], swts[i], dkfact));
    //  ApplyLimits(cg, ru, net); limits are automatically enforced anyway
  }

  inline void Compute_Weights_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
  }

  inline void Compute_Weights_CtLeabraCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                   LeabraNetwork* net) override {
    Compute_Weights_CtLeabraXCAL(cg, su, net);
  }

  TA_SIMPLE_BASEFUNS(NVConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init() { Initialize(); }
};

#endif // NVConSpec_h

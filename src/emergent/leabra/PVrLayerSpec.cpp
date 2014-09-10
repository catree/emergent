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

#include "PVrLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>
#include <PVrLayerSpec>
#include <PVrConSpec>
#include <ExtRewLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PVrLayerSpec);

TA_BASEFUNS_CTORS_DEFN(PVDetectSpec);



void PVDetectSpec::Initialize() {
  thr = 0.7f;
}

void PVrLayerSpec::Initialize() {
  // SetUnique("decay", true);
  decay.phase = 0.0f;

  bias_val.un = ScalarValBias::NO_UN;
  bias_val.val = .5f;           // default is no-information case; extrew = .5
}

void PVrLayerSpec::HelpConfig() {
  String help = "PVrLayerSpec Primary Value Reward Detection Computation:\n\
 Continuously learns about primary rewards, and cancels (inhibits) them in DA computation.\
 It is always trained on the current primary reward (PVe) value from the ExtRew layer\
 with extremization: PVe > norew_val = 1, PVe < norew_val = 0, else .5.\n\
 - Activation is always expectation of reward detection for current trial\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nPVrLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (as PVConSpec)\n\
 - [Optional: Marker recv con from PVe/ExtRew layer -- for display only] \n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool PVrLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  SetUnique("decay", true);
  decay.phase = 0.0f;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* ext_rew_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
      if(fls->InheritsFrom(TA_ExtRewLayerSpec)) ext_rew_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_PVrConSpec), quiet, rval,
                  "requires recv connections to be of type PVrConSpec")) {
      return false;
    }
  }

  return true;
}

bool PVrLayerSpec::Compute_PVDetect(LeabraLayer* lay, LeabraNetwork* net) {
  bool pv_detected = false;
  LeabraUnit* pvru = (LeabraUnit*)lay->units.Leaf(0);
  if(pvru->act_eq > pv_detect.thr)
    pv_detected = true;
  net->pv_detected = pv_detected;
  net->pvlv_pvr = pvru->act_eq;
  return pv_detected;
}

void PVrLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  inherited::Compute_CycleStats(lay, net, thread_no);
  if(net->phase_no == 0)
    Compute_PVDetect(lay, net); // detect in the minus phase -- continuous!
}

void PVrLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  float pvr_targ_val = 0.5f;
  if(net->ext_rew_avail)
    pvr_targ_val = 1.0f;        // any kind of primary reward is trained with a 1

  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
     u->ext = pvr_targ_val;     // clamp to target value
     ClampValue_ugp(lay, acc_md, gpidx, net);   // apply new value
     Compute_ExtToPlus_ugp(lay, acc_md, gpidx, net); // copy ext values to act_p
     );
}

void PVrLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_PVPlusPhaseDwt(lay, net);
}



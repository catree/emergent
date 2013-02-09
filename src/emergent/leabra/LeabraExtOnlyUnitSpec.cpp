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

#include "LeabraExtOnlyUnitSpec.h"
#include <LeabraNetwork>


void LeabraExtOnlyUnitSpec::Initialize() {

}

void LeabraExtOnlyUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    if(act_fun == SPIKE) {
      u->gc.i = MAX(u->g_i_raw, 0.0f);
      Compute_NetinInteg_Spike_i(u, net);
    }
    else {
      u->gc.i = u->prv_g_i + dt.net * (u->g_i_raw - u->prv_g_i);
      u->prv_g_i = u->gc.i;
    }
  }

  u->net_raw += u->net_delta;
  float tot_net = (u->bias_scale * u->bias.OwnCn(0)->wt) + u->net_raw;

  if(ls->inhib.avg_boost > 0.0f && u->act_eq > 0.0f && net->ct_cycle > 0) {
    LeabraInhib* thr;
    int gpidx = u->UnitGpIdx();
    if(gpidx < 0 || ls->inhib_group == LeabraLayerSpec::ENTIRE_LAYER) {
      thr = (LeabraInhib*)lay;
    }
    else {
      thr = (LeabraInhib*)lay->ungp_data.FastEl(gpidx);
    }
    tot_net += thr->netin.avg * ls->inhib.avg_boost * u->act_eq;
  }

  if(u->HasExtFlag(Unit::EXT)) {
    float extin = u->ext * ls->clamp.gain;
    if(extin < opt_thresh.send)
      tot_net = 0.0f;           // not allowed to activate
    else
      tot_net += extin;
  }
  else {
    // get first projection, first connection
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(TestError(!cg, "Compute_NetinInteg", "requires one recv projection!")) {
      return;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(TestError(!su, "Compute_NetinInteg", "requires one unit in recv projection!")) {
      return;
    }
    float extin = su->act_eq;
    if(extin < opt_thresh.send)
      tot_net = 0.0f;           // veto
  }

  u->net_delta = 0.0f;  // clear for next use
  u->g_i_delta = 0.0f;  // clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(tot_net, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u,net);
  }
  else {
    float dnet = dt.net * (tot_net - u->prv_net);
    u->net = u->prv_net + dnet;
    u->prv_net = u->net;
    u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
  }

  // add just before computing i_thr -- after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }
  u->i_thr = Compute_IThresh(u, net);
}

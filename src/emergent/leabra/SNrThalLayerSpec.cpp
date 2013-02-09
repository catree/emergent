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

#include "SNrThalLayerSpec.h"
#include <LeabraNetwork>
#include <MatrixLayerSpec>
#include <PBWMUnGpData>

#include <taMisc>


void SNrThalMiscSpec::Initialize() {
  go_thr = 0.5f;
  gate_cycle = 25;
  force = false;
}

void SNrThalLayerSpec::Initialize() {
  gating_types = IN_MNT_OUT;

  Defaults_init();
}

void SNrThalLayerSpec::Defaults_init() {
  // SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;

  // SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .8f;

  // SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;

  // SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;

  // SetUnique("tie_brk", false);
  tie_brk.on = true;
  tie_brk.thr_gain = 0.2f;
  tie_brk.loser_gain = 1.0f;
}

void SNrThalLayerSpec::HelpConfig() {
  String help = "SNrThalLayerSpec Computation:\n\
 - net = Go if no NoGo, else Go / (Go + nogo_gain * NoGo + leak)\n\
 - act = raw activation from netin\n\
 - act_mid = winner-filtered gating activations -- drives learning in Matrix\n\
 - No learning, wt init variance, in afferent cons\n\
 \nSNrThalLayerSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure.\n\
 - Matrix_Go now projects using regular leabra cons (non-MarkerCons) to create basic standard netinput\n\
 - optional NoGo projection with MarkerCons to provide divisive renorm of signal\n";
  taMisc::Confirm(help);
}

void SNrThalLayerSpec::GatingTypesNStripes(LeabraLayer* lay, 
					   int& n_in, int& n_in_mnt, int& n_out, int& n_mnt_out, int& n_out_mnt) {
  n_in = 0;
  n_in_mnt = 0;
  n_out = 0;
  n_mnt_out = 0;
  n_out_mnt = 0; 

  gating_types = NO_GATE_TYPE;

  for(int g=0; g<lay->projections.size; g++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)prjn->from.ptr();
    if(!fmlay->GetLayerSpec()->InheritsFrom(TA_MatrixLayerSpec)) continue;
    MatrixLayerSpec* mls = (MatrixLayerSpec*)fmlay->GetLayerSpec();
    if(mls->go_nogo != MatrixLayerSpec::GO) continue;

    gating_types = (GatingTypes)(gating_types | mls->gating_type);
    if(mls->gating_type == INPUT) n_in += fmlay->gp_geom.n;
    if(mls->gating_type == IN_MNT) n_in_mnt += fmlay->gp_geom.n;
    if(mls->gating_type == OUTPUT) n_out += fmlay->gp_geom.n;
    if(mls->gating_type == MNT_OUT) n_mnt_out += fmlay->gp_geom.n;
    if(mls->gating_type == OUT_MNT) n_out_mnt += fmlay->gp_geom.n;
    
  }
}

int SNrThalLayerSpec::SNrThalStartIdx(LeabraLayer* lay, GatingTypes gating_type,
				      int& n_in, int& n_in_mnt, int& n_out, int& n_mnt_out, int& n_out_mnt) {
  GatingTypesNStripes(lay, n_in, n_in_mnt, n_out, n_mnt_out, n_out_mnt);
  int snr_st_idx = -1;
  switch(gating_type) {
  case INPUT:
    snr_st_idx = 0;
    break;
  case IN_MNT:
    snr_st_idx = n_in;
    break;
  case MNT_OUT:
    snr_st_idx = n_in + n_in_mnt;
    break;
  case OUTPUT:
    snr_st_idx = n_in + n_in_mnt + n_mnt_out;
    break;
  case OUT_MNT:
    snr_st_idx = n_in + n_in_mnt + n_mnt_out + n_out;
    break; 
  default:			// compiler food
    break;
  }
  return snr_st_idx;
}

bool SNrThalLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // some kind of check on cycle??

  // if(lay->CheckError(net->mid_minusmax_go_cycle <= snrthal.min_go_cycle, quiet, rval,
  //               "min go cycle not before max go cycle -- adjusting min_go_cycle -- you should double check")) {
  // }

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "layer must have unit_groups = true (= stripes) (multiple are good for indepent searching of gating space)!  I just set it for you -- you must configure groups now")) {
    lay->unit_groups = true;
    return false;
  }

  int n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  GatingTypesNStripes(lay, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  int snr_stripes = n_in + n_in_mnt + n_mnt_out + n_out + n_out_mnt;
  
  if(lay->CheckError(snr_stripes != lay->gp_geom.n, quiet, rval,
		     "SNrThalLayer does not have an appropriate number of unit groups given the inputs receiving from the Matrix. Should be:", String(snr_stripes), "is:", 
		     String(lay->gp_geom.n), "updated n stripes but this may not be sufficient")) {
    lay->unit_groups = true;
    lay->gp_geom.n = snr_stripes;
  }

  return true;
}

void SNrThalLayerSpec::Init_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    // update mnt count at start of trial!
    if(gpd->mnt_count < 0)
      gpd->mnt_count--;	// more empty
    else
      gpd->mnt_count++;	// more maint
    gpd->go_fired_now = false;
    gpd->go_fired_trial = false;
    gpd->go_forced = false;
    gpd->go_cycle = -1;

    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, mg);
      if(u->lesioned()) continue;
      u->act_mid = 0.0f;	// reset gating act
    }
  }

  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);
}

void SNrThalLayerSpec::Compute_GateActs(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle < snrthal.gate_cycle)
    return;			// nothing to do before gating starts

  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_fired_trial = 0;
  int n_fired_now = 0;

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, mg); // assuming one unit
    if(u->lesioned()) continue;

    if(net->ct_cycle == snrthal.gate_cycle) {
      if(u->act_eq >= snrthal.go_thr) {
	n_fired_trial++;
	n_fired_now++;
	gpd->go_fired_now = true;
	gpd->go_fired_trial = true;
	gpd->go_cycle = net->ct_cycle;
	gpd->prv_mnt_count = gpd->mnt_count;
	gpd->mnt_count = 0;	// reset
	u->act_mid = u->act_eq;	// gating act
      }
    }
    else {
      gpd->go_fired_now = false; // turn it off after one cycle
      u->act = u->act_eq = u->act_nd = u->act_mid; // activity is always gating signal
    }
  }

  // no stripe has fired yet -- force max go firing
  if(net->ct_cycle == snrthal.gate_cycle && snrthal.force && n_fired_trial == 0) {
    int go_idx = lay->netin.max_i;
    if(go_idx < 0) {		// this really shouldn't happen, but if it does..
      go_idx = Random::IntZeroN(lay->units.leaves);
    }
    int gp_idx = go_idx / nunits;
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gp_idx); // assuming one unit
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(gp_idx);
    n_fired_trial++;
    n_fired_now++;
    gpd->go_fired_now = true;
    gpd->go_fired_trial = true;
    gpd->go_forced = true;	// yes this is forced
    gpd->go_cycle = net->ct_cycle;
    gpd->prv_mnt_count = gpd->mnt_count;
    gpd->mnt_count = 0;	// reset
    u->act_mid = snrthal.go_thr; // must be at least at threshold -- otherwise won't gate for pbwm!
  }

  if(net->ct_cycle == snrthal.gate_cycle) {
    Compute_GateStats(lay, net); // update overall stats at this point
    lay->SetUserData("n_fired_trial", n_fired_trial);
    lay->SetUserData("n_fired_now", n_fired_now);
  }
}


void SNrThalLayerSpec::Compute_GateStats(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  int nunits = lay->UnitAccess_NUnits(acc_md); // this should be just 1 -- here for generality but some of the logic doesn't really go through for n >= 2 at this point..

  int n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt;
  GatingTypesNStripes(lay, n_in, n_in_mnt, n_mnt_out, n_out, n_out_mnt);
  int mnt_st = n_in;
  int out_st = n_in + n_in_mnt;

  const int n_stats = 4;	// 0 = global, 1 = in, 2 = mnt, 3 = out
  int	n_gated[n_stats] = {0,0,0,0};
  int	n_forced[n_stats] = {0,0,0,0};
  int	min_mnt_count[n_stats] = {0,0,0,0};
  int	max_mnt_count[n_stats] = {0,0,0,0};

  for(int mg=0; mg<lay->gp_geom.n; mg++) {
    PBWMUnGpData* gpd = (PBWMUnGpData*)lay->ungp_data.FastEl(mg);
    if(gpd->go_fired_trial) {
      if(mg < n_in) n_gated[1]++;
      else if(mg >= mnt_st && mg < out_st) n_gated[2]++;
      else if(mg >= out_st) n_gated[3]++;
      n_gated[0]++;		// global
    }
    if(gpd->go_forced) {
      if(mg < n_in) n_forced[1]++;
      else if(mg >= mnt_st && mg < out_st) n_forced[2]++;
      else if(mg >= out_st) n_forced[3]++;
      n_forced[0]++;		// global
    }

    // only mnt layers contribute to this!
    if(mg >= mnt_st && mg < out_st) {
      min_mnt_count[2] = MIN(min_mnt_count[2], gpd->mnt_count);
      max_mnt_count[2] = MAX(max_mnt_count[2], gpd->mnt_count);
    }
  }

  lay->SetUserData("n_gated_all", (float)n_gated[0]);
  if(n_in > 0) lay->SetUserData("n_gated_in", (float)n_gated[1]);
  if(n_in_mnt > 0) lay->SetUserData("n_gated_in_mnt", (float)n_gated[2]);
  if(n_out > 0) lay->SetUserData("n_gated_out", (float)n_gated[3]);

  if(snrthal.force) {
    lay->SetUserData("n_forced_all", (float)n_forced[0]);
    if(n_in > 0) lay->SetUserData("n_forced_in", (float)n_forced[1]);
    if(n_in_mnt > 0) lay->SetUserData("n_forced_in_mnt", (float)n_forced[2]);
    if(n_out > 0) lay->SetUserData("n_forced_out", (float)n_forced[3]);
  }

  if(n_in_mnt > 0) {
    lay->SetUserData("min_mnt_count", (float)min_mnt_count[2]);
    lay->SetUserData("max_mnt_count", (float)max_mnt_count[2]);
  }
}

void SNrThalLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_GateActs(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void SNrThalLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // nop -- don't do the default thing -- already done by GatedActs
}

void SNrThalLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  lay->SetUserData("n_fired_trial", 0);
  lay->SetUserData("n_fired_now", 0);

  lay->SetUserData("n_gated_all", 0.0f);
  lay->SetUserData("n_gated_in",  0.0f);
  lay->SetUserData("n_gated_in_mnt", 0.0f);
  lay->SetUserData("n_gated_out", 0.0f);

  if(snrthal.force) {
    lay->SetUserData("n_forced_all", 0.0f);
    lay->SetUserData("n_forced_in",  0.0f);
    lay->SetUserData("n_forced_in_mnt", 0.0f);
    lay->SetUserData("n_forced_out", 0.0f);
  }

  lay->SetUserData("min_mnt_count", 0.0f);
  lay->SetUserData("max_mnt_count", 0.0f);
}

void SNrThalLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Trial_Init_Layer(lay, net);
  Init_GateStats(lay, net);
}
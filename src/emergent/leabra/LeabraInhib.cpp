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

#include "LeabraInhib.h"

TA_BASEFUNS_CTORS_DEFN(LeabraInhibVals);

void LeabraInhibVals::Initialize() {
  InitVals();
}

void LeabraInhibVals::InitVals() {
  ffi = 0.0f;
  fbi = 0.0f;
  g_i = 0.0f;
  g_i_orig = 0.0f;
  lay_g_i = 0.0f;
}

void LeabraInhibVals::Copy_(const LeabraInhibVals& cp) {
  ffi = cp.ffi;
  fbi = cp.fbi;
  g_i = cp.g_i;
  g_i_orig = cp.g_i_orig;
  lay_g_i = cp.lay_g_i;
}
 
void LeabraInhib::Inhib_Initialize() {
  i_val.InitVals();
  acts_m_avg = .1f;
  acts_p_avg = .1f;
  acts_p_avg_eff = 1.f;
  un_g_i.cmpt = false;          // don't compute by default
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  i_val.InitVals();
  netin.InitVals();
  netin_raw.InitVals();
  acts.InitVals();
  acts_eq.InitVals();
  acts_raw.InitVals();
  un_g_i.InitVals();
  am_deep_raw.InitVals();
  am_deep_raw_net.InitVals();
  am_deep_mod_net.InitVals();
  am_deep_ctxt.InitVals();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  acts_m_avg = cp.acts_m_avg;
  acts_p_avg = cp.acts_p_avg;
  acts_p_avg_eff = cp.acts_p_avg_eff;
  i_val = cp.i_val;
  netin = cp.netin;
  netin_raw = cp.netin_raw;
  acts = cp.acts;
  acts_eq = cp.acts_eq;
  acts_raw = cp.acts_raw;
  acts_prvq = cp.acts_prvq;
  acts_m = cp.acts_m;
  acts_p = cp.acts_p;
  acts_q0 = cp.acts_q0;
  un_g_i = cp.un_g_i;
  am_deep_raw = cp.am_deep_raw;
  am_deep_raw_net = cp.am_deep_raw_net;
  am_deep_mod_net = cp.am_deep_mod_net;
  am_deep_ctxt = cp.am_deep_ctxt;
}


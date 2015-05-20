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

#include "TiledGpRFPrjnSpec.h"
#include <Network>
#include <int_Array>
#include <taMath_float>
#include <taMisc>

#include <LeabraConSpec>

TA_BASEFUNS_CTORS_DEFN(TiledGpRFPrjnSpec);

void TiledGpRFPrjnSpec::Initialize() {
  send_gp_size = 4;
  send_gp_skip = 2;
  send_gp_start = 0;
  wrap = false;
  share_cons = false;
  reciprocal = false;
  wts_type = GAUSSIAN;
  gauss_sig = 1.0f;
  gauss_ctr_mv = 0.5f;
  wrap_wts = false;
  wt_range.min = 0.1f;
  wt_range.max = 0.9f;
  p_high = 0.25f;
}

void TiledGpRFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  wt_range.UpdateAfterEdit_NoGui();
  
  if(taMisc::is_loading) {
    taVersion v705(7, 0, 5);
    if(taMisc::loading_version < v705) { // set send_gp_start to prev val
      if(wrap) {
        send_gp_start = -send_gp_skip;
      }
      else {
        send_gp_start = 0;
      }
    }
  }
}

void TiledGpRFPrjnSpec::Connect_impl(Projection* prjn, bool make_cons) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;

  if(wts_type == BIMODAL_PERMUTED) {
    high_low_wts.Reset();       // keep it fresh
  }
  
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {
    recv_lay = prjn->from; // from perspective of non-recip!
    send_lay = prjn->layer;
  }

  taVector2i ru_geo = recv_lay->gp_geom;
  if(!recv_lay->unit_groups) {
    ru_geo = 1;
  }
  taVector2i su_geo = send_lay->gp_geom;
  if(!send_lay->unit_groups) {
    su_geo = 1;
  }
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  int sg_sz_tot = send_gp_size.Product();
  int alloc_no = sg_sz_tot * su_nunits;

  taVector2i ruc;
  int rgpidx = 0;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++, rgpidx++) {
      if(!recv_lay->unit_groups) {
        rgpidx = -1;
      }
      taVector2i su_st;
      su_st = send_gp_start + ruc * send_gp_skip;
      taVector2i suc;
      taVector2i suc_wrp;
      for(suc.y = su_st.y; suc.y < su_st.y + send_gp_size.y; suc.y++) {
        for(suc.x = su_st.x; suc.x < su_st.x + send_gp_size.x; suc.x++) {
          suc_wrp = suc;
          if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
            continue;
          int sgpidx = -1;
          if(send_lay->unit_groups) {
            sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
            if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;
          }

          Connect_UnitGroup(prjn, recv_lay, send_lay, rgpidx, sgpidx, make_cons);
        }
      }
    }
  }
  if(!make_cons) { // on first pass through alloc loop, do sending allocations
    prjn->layer->RecvConsPostAlloc(prjn);
    prjn->from->SendConsPostAlloc(prjn);
  }
}

void TiledGpRFPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                             Layer* send_lay,
                                             int rgpidx, int sgpidx, bool make_cons) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;

  Network* net = recv_lay->own_net;

  if(reciprocal) {              // reciprocal is backwards!
    // todo: share_cons here requires a separate loop for reciprocal
    
    for(int sui=0; sui < su_nunits; sui++) {
      Unit* su_u;
      if(sgpidx >= 0)
        su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      else
        su_u = send_lay->units.SafeEl(sui);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u;
        if(rgpidx >= 0)
          ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        else
          ru_u = recv_lay->units.SafeEl(rui);
        if(!self_con && (su_u == ru_u)) continue;
        if(!make_cons) {
          su_u->RecvConsAllocInc(1, prjn); // recip!
          ru_u->SendConsAllocInc(1, prjn); // recip!
        }
        else {
          su_u->ConnectFrom(ru_u, prjn); // recip!
        }
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u;
      if(rgpidx >= 0) {
        ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(share_cons && net->RecvOwnsCons() && rgpidx > 0) {
          Unit* shru = recv_lay->UnitAtUnGpIdx(rui, 0); // group 0
          ru_u->ShareRecvConsFrom(shru, prjn);
        }
      }
      else
        ru_u = recv_lay->units.SafeEl(rui);
      for(int sui=0; sui < su_nunits; sui++) {
        Unit* su_u;
        if(sgpidx >= 0)
          su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        else
          su_u = send_lay->units.SafeEl(sui);
        if(!self_con && (su_u == ru_u)) continue;
        if(!make_cons) {
          ru_u->RecvConsAllocInc(1, prjn);
          su_u->SendConsAllocInc(1, prjn);
        }
        else {
          ru_u->ConnectFrom(su_u, prjn);
        }
      }
    }
  }
}


bool TiledGpRFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom +1) * send_gp_skip);

  SigEmitUpdated();
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool TiledGpRFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * send_gp_skip);
  else
    trg_send_geom = ((trg_recv_geom+1) * send_gp_skip);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / send_gp_skip);
  else
    trg_recv_geom = (trg_send_geom / send_gp_skip) - 1;

  SigEmitUpdated();
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}

void TiledGpRFPrjnSpec::Init_Weights_Prjn(Projection* prjn, ConGroup* cg,
                                          Network* net, int thr_no) {
  if(set_scale) {
    if(TestWarning(!cg->GetConSpec()->InheritsFrom(&TA_LeabraConSpec),
                   "Init_Weights_Prjn", "set_scale can only apply to Leabra connections!"))
      return;
  }
  if(wts_type == GAUSSIAN) {
    Init_Weights_Gaussian(prjn, cg, net, thr_no);
  }
  else {
    Init_Weights_BimodalPermuted(prjn, cg, net, thr_no);
  }    
}

void TiledGpRFPrjnSpec::Init_Weights_Gaussian(Projection* prjn, ConGroup* cg,
                                              Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // todo: need to fix for reciprocal?

  taVector2f full_size = send_lay->un_geom * send_gp_size;
  taVector2f half_size = full_size;
  half_size *= 0.5f;

  float eff_sig = gauss_sig * (float)half_size.x;
  
  taVector2i ru_pos;
  Unit* ru = cg->ThrOwnUn(net, thr_no);
  ru->UnitGpLogPos(ru_pos);

  taVector2f rugpctr = recv_lay->un_geom;
  rugpctr *= 0.5f;
  taVector2f ru_nrm_pos = gauss_ctr_mv * (((taVector2f)ru_pos - rugpctr) / rugpctr);

  taVector2f s_ctr = (ru_nrm_pos * half_size) + half_size;

  for(int i=0; i<cg->size; i++) {
    // note: these are organized within unit group first, then by groups
    int ug_idx = i / send_lay->un_geom.n; // which unit group, ordinally
    int un_idx = i % send_lay->un_geom.n; // index in unit group

    int un_x = un_idx % send_lay->un_geom.x;
    int un_y = un_idx / send_lay->un_geom.x;

    int ug_x = ug_idx % send_gp_size.x;
    int ug_y = ug_idx / send_gp_size.x;

    float su_x = ug_x * send_lay->un_geom.x + un_x;
    float su_y = ug_y * send_lay->un_geom.y + un_y;

    // wrap coords around to get min dist from ctr either way
    if(wrap_wts) {
      if(fabs((su_x + full_size.x) - s_ctr.x) < fabs(su_x - s_ctr.x))
        su_x = su_x + full_size.x;
      else if(fabs((su_x - full_size.x) - s_ctr.x) < fabs(su_x - s_ctr.x))
        su_x = su_x - full_size.x;
      if(fabs((su_y + full_size.y) - s_ctr.y) < fabs(su_y - s_ctr.y))
        su_y = su_y + full_size.y;
      else if(fabs((su_y - full_size.y) - s_ctr.y) < fabs(su_y - s_ctr.y))
        su_y = su_y - full_size.y;
    }
    
    float dst = taMath_float::euc_dist(su_x, su_y, s_ctr.x, s_ctr.y);
    float wt = taMath_float::gauss_den_nonorm(dst, eff_sig);
    wt = wt_range.min + wt_range.range * wt;

    if(set_scale) {
      SetCnWtRnd(cg, i, net, thr_no);
      SetCnScale(wt, cg, i, net, thr_no);
    }
    else {
      SetCnWt(wt, cg, i, net, thr_no);
    }
  }
}

void TiledGpRFPrjnSpec::Init_Weights_BimodalPermuted(Projection* prjn, ConGroup* cg,
                                                     Network* net, int thr_no) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  // todo: need to fix for reciprocal?

  taVector2i full_size = send_lay->un_geom * send_gp_size;
  int n_sz = full_size.Product();
  
  if(high_low_wts.size != n_sz) {
    high_low_wts.SetSize(n_sz);
    int n_on = (int)((float)n_sz * p_high);
    if(n_on == 0) n_on = 1;
    if(n_on > n_sz) n_on = n_sz;
    int i;
    for(i=0; i<n_on; i++) {
      high_low_wts[i] = 1;
    }
    for(;i<n_sz;i++) {
      high_low_wts[i] = 0;
    }
  }

  high_low_wts.Permute();
  
  for(int i=0; i<cg->size; i++) {
    int hilo = high_low_wts[i];
    float wt;
    if(hilo == 1) {
      wt = wt_range.max;
    }
    else {
      wt = wt_range.min;
    }
    if(set_scale) {
      SetCnWtRnd(cg, i, net, thr_no);
      SetCnScale(wt, cg, i, net, thr_no);
    }
    else {
      SetCnWt(wt, cg, i, net, thr_no);
    }
  }
}


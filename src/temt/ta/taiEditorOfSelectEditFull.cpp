// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "taiEditorOfSelectEditFull.h"
#include <SelectEdit>
#include <iLabel>
#include <iFlowLayout>
#include <taiMember>
#include <taiWidgetMenuBar>

#include <taMisc>
#include <taiMisc>


taiEditorOfSelectEditFull::taiEditorOfSelectEditFull(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
}

taiEditorOfSelectEditFull::~taiEditorOfSelectEditFull() {
}

void taiEditorOfSelectEditFull::Initialize()
{
}

void taiEditorOfSelectEditFull::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  if(frmMethButtons) {
    if(layMethButtons && (layMethButtons != (iFlowLayout*)frmMethButtons->layout())) {
      delete layMethButtons;
    }
    if(frmMethButtons->layout()) {
      delete frmMethButtons->layout();
    }
    layMethButtons = NULL;
    taiMisc::DeleteChildrenLater(frmMethButtons);
  }
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void taiEditorOfSelectEditFull::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  dat_cnt = 0;
  // mark place
  String nm;
  String help_text;
  taiMemberWidgets* memb_set = NULL;

  int set_idx = 0;
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(EditMbrItem_Group, grp, sele->mbrs) {
    bool def_grp = (grp == &(sele->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    memb_set = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      iLabel* lbl = new iLabel(grp->GetName(), body);
      AddSectionLabel(-1, lbl,
        "");
    }
    for (int i = 0; i < grp->size; ++i) {
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL)) continue; // should only happen if created manually (Bad!)
      taiWidget* mb_dat = md->im->GetWidgetRep(this, NULL, body);
      memb_set->memb_el.Add(md);
      memb_set->data_el.Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);

      help_text = item->GetDesc();
      String new_lbl = item->caption();
      //obs GetName(md, nm, help_text); //note: we just call this to get the help text
      AddNameData(-1, new_lbl, help_text, data, mb_dat, md);
      ++dat_cnt;
    }
    def_grp = false;
    ++set_idx;
  }
}

void taiEditorOfSelectEditFull::DoRemoveSelEdit() {
   // removes the sel_item_index item
  int sel_item_index = membs.GetFlatDataIndex(sel_item_dat);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
  else
    taMisc::DebugInfo("taiEditorOfSelectEditFull::DoRemoveSelEdit: could not find item");
}

void taiEditorOfSelectEditFull::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = membs.GetFlatDataIndex(sel_item_mbr, sel_item_base);
  if (sel_item_index < 0) return;
  //QAction* act =
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void taiEditorOfSelectEditFull::GetImage_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiWidget* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL)) {
        taMisc::DebugInfo("taiEditorOfSelectEditFull::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
      }
      ++itm_idx;
    }
  }
}

void taiEditorOfSelectEditFull::GetValue_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    taiMemberWidgets* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiWidget* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL)) {
        taMisc::DebugInfo("taiEditorOfSelectEditFull::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, item->base, first_diff);
        if (!first_diff)
          taiMember::EndScript(item->base);
        item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
      }
      ++itm_idx;
    }
  }
}

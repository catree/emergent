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

#include "ProgCode.h"
#include <Program>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taProject>
#include <ProgElChoiceDlg>
#include <iDialogChoice>

TA_BASEFUNS_CTORS_DEFN(ProgCode);

taTypeDef_Of(CssExpr);
taTypeDef_Of(AssignExpr);
taTypeDef_Of(Comment);
taTypeDef_Of(If);
taTypeDef_Of(ElseIf);
taTypeDef_Of(Else);
taTypeDef_Of(UserScript);
taTypeDef_Of(BlankLineEl);
taTypeDef_Of(MethodCall
             );

void ProgCode::Initialize() {
  SetProgExprFlags();
}

void ProgCode::SetProgExprFlags() {
  code.SetExprFlag(ProgExpr::NO_PARSE); // do not parse at all -- often nonsense..
  ClearProgFlag(CAN_REVERT_TO_CODE); // we are code!
}

void ProgCode::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(true, quiet, rval,  "uncompiled program code (ProgCode) element still present -- fix it or remove it -- code is:", code.expr);
}

void ProgCode::CvtCodeCheckType(ProgEl_List& candidates, TypeDef* td,
				const String& code_str) {
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  if(obj) {
    if(obj->InheritsFrom(&TA_UserScript)) return; // don't even check it
    if(obj->CanCvtFmCode(code_str, this)) {
      candidates.LinkUnique(obj);
    }
  }
  for(int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    CvtCodeCheckType(candidates, chld, code_str);
  }
}

ProgEl* ProgCode::CvtCodeToProgEl() {
  ProgEl_List candidates;
  String code_mod = code.expr;
  String code_orig = code.expr;  // will need to reset if Css expression
  bool has_final_semi = false;
  if(code_mod.empty()) {
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_BlankLineEl));
  }
  if(code_mod.startsWith("//") || code_mod.startsWith("/*")) {
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_Comment));
  }
  else if (code_mod.endsWith(';')) {
    has_final_semi = true;
    candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
  }
  else {
    bool had_var = CvtCodeToVar(code_mod);
    if(had_var) {
      code.expr = code_mod;     // code was truncated..
      SigEmitUpdated();          // update us
      if(code_mod.empty())
        return NULL;              // that's it, we're done!  avail for something else..
    }
    CvtCodeCheckType(candidates, &TA_ProgEl, code_mod);
    if(candidates.size == 0)
      return NULL;
  }
  // assign is a fall back when there is more than one choice - methods take precence over assignment
  if (candidates.size > 1) {
    candidates.RemoveEl_((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_AssignExpr));
  }
  // no ProgEl for MethodMethodCall so convert to CSS
  bool isMethodCall = candidates.FindType(&TA_MethodCall);
  if (isMethodCall) {
    String after_method = code.expr.after(')', -1);
    if (after_method.length() > 0) {
      candidates.RemoveEl_((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_MethodCall));
      ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(&TA_AssignExpr);
      if(obj) {
        if(obj->CanCvtFmCode(code_orig, this)) {
          candidates.LinkUnique(obj);  // make it an AssignExpr
        }
        else {
          candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
        }
      }
      else {  // best to be safe
        candidates.Link((ProgEl*)tabMisc::root->GetTemplateInstance(&TA_CssExpr));
      }
    }
  }
  ProgEl* cvt = candidates[0];
  if(candidates.size > 1) {
    int ctrl_n = 0;
    for(int i=candidates.size-1; i>= 0; i--) {
      ProgEl* pel = candidates[i];
      if(pel->IsCtrlProgEl()) {
        cvt = pel;		// take precidence
        ctrl_n++;
      }
    }
    if(ctrl_n != 1) {		// still need to pick -- ctrl_n should never be > 1 but who knows
      cvt = candidates[0];
      if(candidates.size > 1) {
        String delimiter = iDialogChoice::delimiter;
        String sep_but = candidates[0]->GetToolbarName();
        for(int i=1;i<candidates.size;i++) {
          sep_but += delimiter + candidates[i]->GetToolbarName();
        }
        int chs = iDialogChoice::ChoiceDialogSepBut
        (NULL, "Multiple program elements match code string:\n"
         + code_mod + "\nPlease choose correct one.",
         sep_but,
         "Cancel" + delimiter);
        if(chs == 0) return NULL;
        cvt = candidates[chs-1];
      }
      if(cvt == tabMisc::root->GetTemplateInstance(&TA_CssExpr)) {  // User Script already excluded
        code.expr = code_orig;  // Css - reset the original expression
      }
      else if(has_final_semi) {
        code.expr = code.expr.before(';',-1); // chop it off if not using css expr
      }
    }
  }
  
  ProgEl* rval = (ProgEl*)cvt->MakeToken();
  return rval;
}

bool ProgCode::BrowserEditSet(const String& code_str, int move_after) {
  edit_move_after = move_after;
  if(code_str.startsWith("//") || code_str.startsWith("/*")) {
    code.expr = code_str;
  }
  else {
    code.expr = CodeGetDesc(code_str);
  }
  // rohrlich 8/3/2014 - see comment at start of CvtCodeToProgEl()
  // uncomment these lines if that is reversed
//  if(code.expr.nonempty()) {
    tabMisc::DelayedFunCall_gui(this, "ConvertToProgEl"); // do it later..
//  }
  return false;
}

void ProgCode::ConvertToProgEl() {
  if(HasBaseFlag(BF_MISC4)) return; // already did the conversion -- going to be nuked!
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return;
  code.expr = trim(code.expr);
  // if(code.expr.empty()) return;
  ProgEl* cvt = CvtCodeToProgEl();
  if(!cvt) return;
  Program* prog = GET_OWNER(own, Program);
  if(prog) {
    taProject* proj = prog->GetMyProj();
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "ProgCodeCvt", prog); 
    }
  }
  cvt->edit_move_after = edit_move_after; // these are the ones who need it
  edit_move_after = 0;
  cvt->desc = desc;         // transfer description
  cvt->code_string = code.expr;
  cvt->SetProgFlag(CAN_REVERT_TO_CODE);
  int myidx = own->FindEl(this);
  // if we just typed an else or elseif, see if we did this within an existing if/elseif, without
  // just following an if/elseif -- this will auto-move it up to the right level
  if((cvt->InheritsFrom(&TA_Else) || cvt->InheritsFrom(&TA_ElseIf))
     && own->owner
     && (own->owner->InheritsFrom(&TA_If) || own->owner->InheritsFrom(&TA_ElseIf))) {
    if(!(myidx > 0 && (own->SafeEl(myidx-1)->InheritsFrom(&TA_If) || 
                       own->SafeEl(myidx-1)->InheritsFrom(&TA_ElseIf)))) {
      own->MoveElseLater(cvt, myidx, "CvtFmSavedCode");
      SetBaseFlag(BF_MISC4); // indicates that we're done..
      return;
    }
  }
  own->ReplaceLater(cvt, myidx, "CvtFmSavedCode");
  SetBaseFlag(BF_MISC4); // indicates that we're done..
}

void ProgCode::UpdateAfterEdit_impl() {
  SetProgExprFlags();
  inherited::UpdateAfterEdit_impl();
  if(HasBaseFlag(BF_MISC4)) return; // already did the conversion -- going to be nuked!
  if(!taMisc::gui_active || taMisc::is_loading) return;
  ProgEl_List* own = GET_MY_OWNER(ProgEl_List);
  if(!own) return;
  String code_str = trim(code.expr);
  if(code_str.empty()) return;
  code.expr = CodeGetDesc(code_str);
  tabMisc::DelayedFunCall_gui(this, "ConvertToProgEl"); // do it later..
}  

String ProgCode::GetDisplayName() const {
  return code.expr;
}


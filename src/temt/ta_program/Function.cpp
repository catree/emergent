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

#include "Function.h"
#include <Program>
#include <taProject>
#include <taBase_PtrList>
#include <FunctionCall>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Function);
SMARTREF_OF_CPP(Function);

taTypeDef_Of(LocalVars);
taTypeDef_Of(ProgCode);

void Function::Initialize() {
  return_type = ProgVar::T_Int;
  object_type = &TA_taOBase;
  args.var_context = ProgVar_List::VC_FuncArgs;
}

void Function::InitLinks() {
  inherited::InitLinks();
  taBase::Own(args, this);
  taBase::Own(fun_code, this);

  if(!taMisc::is_loading) {
    if(fun_code.size == 0) {
      //      LocalVars* pv = (LocalVars*)
      fun_code.New(1, &TA_LocalVars); // make this by default because it is typically needed!
    }
  }
  fun_code.el_typ = &TA_ProgCode;  // make sure this is default
}

void Function::SetDefaultName() {
  if(taMisc::not_constr || taMisc::in_init)
    return;
  SetDefaultName_();
}

void Function::MakeNameUnique() {
  if(owner && owner->InheritsFrom(&TA_taList_impl)) {
    ((taList_impl*)owner)->MakeElNameUnique(this); // just this guy
  }
}

bool Function::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (name == new_name) return true;
  name = new_name;
  if (!taMisc::is_changing_type)
    MakeNameUnique();
  //UpdateAfterEdit();          // this turns out to be a bad idea -- just do it where needed
  return true;
}

void Function::Copy_(const Function& cp) {
  // note: do not copy name so it can be uniquified -- Don't understand -- setting name - rohrlich 10/8/15
  SetBaseFlag(COPYING); // ala Copy__
  String new_name = cp.name + "_copy"; // SetCopyName is in taNBase so we don't have access - this is good enough
  SetName(new_name);
  return_type = cp.return_type;
  object_type = cp.object_type;
  args = cp.args;
  fun_code = cp.fun_code;
  ClearBaseFlag(COPYING); // ala Copy__
}

void Function::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // When a user enters a name in a dialog, it updates the 'name' member
  // without making a SetName call.  Make the call here so it can do its
  // validation.
  SetName(name); 
  if(Program::IsForbiddenName(this, name)) {
    SetName("My" + name);
  }
  fun_code.el_typ = &TA_ProgCode;  // make sure this is default
  UpdateCallers();
}

void Function::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(name.empty(), quiet, rval, "name is empty -- functions must be named");
  CheckError(Program::IsForbiddenName(this, name, false), quiet, rval,
	     "Name:",name,"is a css reserved name used for something else -- please choose another name");
  Function_List* flo = GET_MY_OWNER(Function_List);
  CheckError(!flo, quiet, rval, "Function must only be in .functions -- cannot be in .prog_code or .init_code -- this is the DEFINITION of the function, not calling the function (which is FunctionCall)");
}

void Function::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  args.CheckConfig(quiet, rval);
  fun_code.CheckConfig(quiet, rval);
}

String Function::GetFunDecl() {
  ProgVar rvt(false); rvt.var_type = return_type;  rvt.object_type = object_type;
  String rval = rvt.GenCssType() + " " + name + "(";
  if(args.size > 0) {
    rval += args.GenCss_FuncArgs();
  }
  rval += ")";
  return rval;
}  

void Function::GenCss_Decl(Program* prog) {
  for(int i=0; i<args.size; i++) {
    args[i]->UpdateAfterEdit_NoGui(); // make sure everything updated
  }
  
  String rval = GetFunDecl() + ";";
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
}

void Function::GenCssBody_impl(Program* prog) {
  String rval = GetFunDecl() + " {";
  prog->AddLine(this, rval, ProgLine::MAIN_LINE);
  prog->IncIndent();

  // todo: output *values* of all the vars
  prog->AddVerboseLine(this, false, "\"starting function: " + name + "\""); // not start

  fun_code.GenCss(prog);
  prog->DecIndent();
  prog->AddLine(this, "}");
}

const String Function::GenListing_children(int indent_level) {
  return fun_code.GenListing(indent_level + 1);
}

String Function::GetDisplayName() const {
  String rval;
  rval += name + "(";
  if(args.size > 0) {
    rval += args.GenCss_FuncArgs();
  }
  rval += ")";
  ProgVar rvt(false); rvt.var_type = return_type;  rvt.object_type = object_type;
  rval += " returns: " + rvt.GenCssType();
  return rval;
}

void Function::PreGenChildren_impl(int& item_id) {
  fun_code.PreGen(item_id);
}

ProgVar* Function::FindVarName(const String& var_nm) const {
  ProgVar* pv = args.FindName(var_nm);
  if(pv) return pv;
  return fun_code.FindVarName(var_nm);
}

void Function::GetCallers(taBase_PtrList& callers) {
  Program* prog = program();
  if(!prog) return;
  
  prog->Search("FunctionCall", callers, NULL,
               false,  // text_only
               false,  // contains
               true,   // case_sensitive
               false,  // obj_name
               true,   // obj_type
               false,  // obj_desc
               false,  // obj_val
               false,  // mbr_name
               false); // type_desc
}

void Function::ListCallers() {
  Program* prog = program();
  if(!prog)
    return;
  
 prog->GuiFindFromMe("\"" + name + "(\"");    // find all refs to me
}

void Function::UpdateCallers() {
  Program* prog = program();
  if(!prog)
    return;
  
  taBase_PtrList callers;
  GetCallers(callers);
  
  for(int i=0;i<callers.size; i++) {
    taBase* it = callers[i];
    if(!it || !it->InheritsFrom(&TA_FunctionCall)) continue;
    FunctionCall* fc = (FunctionCall*)it;
    if(fc->fun.ptr() == this) {
      fc->UpdateArgs();
      fc->UpdateAfterEdit();
    }
  }
}

void Function::UpdateCallerArgs() {
  UpdateCallers();
  ListCallers();
}

bool Function::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  return false; // never convert from a ProgCode guy..
}

bool Function::BrowserEditSet(const String& code, int move_after) {
  if(move_after != -11) {
    Program* prog = GET_MY_OWNER(Program);
    if(prog) {
      taProject* proj = prog->GetMyProj();
      if(proj) {
        proj->undo_mgr.SaveUndo(this, "BrowserEditSet", prog);
      }
    }
  }
  // always convert -- don't check and don't revert to ProgCode
  edit_move_after = 0;
  String cd = CodeGetDesc(code);
  bool rval = CvtFmCode(cd);
  UpdateAfterEdit();
  return rval;
}

bool Function::CvtFmCode(const String& code) {
  String fnm = trim(code);
  if(code.contains('(')) {
    fnm = trim(code.before('('));
  }
  if(fnm.contains(' ')) {       // type funame
    String retyp = fnm.before(' ');
    bool ref;
    TypeDef* td = ProgVar::GetTypeDefFromString(retyp, ref);
    if(td) {
      return_type = ProgVar::GetTypeFromTypeDef(td);
    }
    fnm = fnm.after(' ');
  }
  SetName(fnm);
  String ars = code.after('(');
  if(ars.contains(')')) ars = ars.before(')',-1);
  ars = trim(ars);
  String_Array ar;
  ar.FmDelimString(ars, ",");
  args.SetSize(ar.size);
  for(int i=0; i<ar.size; i++) {
    String av = trim(ar[i]);
    ProgVar* pv = args[i];
    pv->SetTypeAndName(av);
  }
  if(code.contains("returns: ")) {
    String retyp = trim(code.after("returns: "));
    bool ref;
    TypeDef* td = ProgVar::GetTypeDefFromString(retyp, ref);
    if(td) {
      return_type = ProgVar::GetTypeFromTypeDef(td);
    }
  }
  return true;
}


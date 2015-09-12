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

#include "ProgramCallFun.h"

#include <Program>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(ProgramCallFun);

String ProgramCallFun::GetDisplayName() const {
  String rval;
  if (target) {
    rval = target->GetName();
      rval += "(";
      for(int i=0;i<prog_args.size;i++) {
        ProgArg* pa = prog_args.FastEl(i);
        if(i > 0) rval += ", ";
        rval += pa->expr.expr;   // GetDisplayName();
      }
      rval += ") " + function + "()";
  }
  return rval;
}

void ProgramCallFun::GenCssBody_impl(Program* prog) {
  if (!target) return;
  prog->AddLine(this, String("Program* target = this") + GetPath(NULL, program())+ "->GetTarget();");
  prog->AddLine(this, "if(target) {");
  prog->IncIndent();

  prog->AddVerboseLine(this, false, "\"calling program:\",target->name"); // not start
  GenCssArgSet_impl(prog, "target");

  prog->AddLine(this, "{ target->CallFunction(this,\"" + function + "\"); }");
  prog->DecIndent();
  prog->AddLine(this, "}");
}

bool ProgramCallFun::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String code_dn = code; code_dn.downcase();
  // if(code_dn.startsWith("call ")) return true; // definitely
  // if(code_dn.startsWith("prog ")) return true; // definitely
  if(!code.contains('(')) return false;
  String lhs = code.before('(');
  String funm = lhs;
  if(lhs.contains('=')) return false; // no rval for progcall
  if((funm.freq('.') + funm.freq("->")) > 0) return false; // exclude method call
  String rhs = trim(code.after('('));
  if(!rhs.endsWith("()")) return false; // must end in ()
  if(!scope_el) return false;
  taProject* proj = scope_el->GetMyProj();
  if(!proj) return false;
  Program* prg = proj->programs.FindLeafName(funm);
  if(prg) return true;
  return false;
}

bool ProgramCallFun::CvtFmCode(const String& code) {
  String cd = code;
  if(cd.startsWith("Call "))
    cd = cd.after("Call ");
  if(cd.startsWith("call "))
    cd = cd.after("call ");
  String lhs = cd;
  if(lhs.contains('('))
    lhs = lhs.before('(');
  String funm = lhs;
  taProject* proj = GetMyProj();
  if(!proj) return false;
  Program* prg = proj->programs.FindLeafName(funm);
  if(!prg) return false;
  target = prg;
  UpdateAfterEdit_impl();                          // update based on targ
  // now tackle the args
  String args = trim(cd.after('('));
  args = args.before("()",-1);
  function = trim(args.after(" ", -1));
  args = trim(args.before(" ", -1));
  prog_args.ParseArgString(args);
  UpdateAfterEdit_impl();
  return true;
}


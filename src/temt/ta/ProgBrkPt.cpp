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

#include "ProgBrkPt.h"
#include <Program>
#include <ProgLine>
#include <taSigLink>
#include <SigLinkSignal>

void ProgBrkPt::Initialize() {
  enabled = true;
}

void ProgBrkPt::InitLinks() {
  taBase::Own(prog_el, this);
  inherited::InitLinks();
}

void ProgBrkPt::CutLinks() {
  if (prog_el) {
    Program* prog = GET_MY_OWNER(Program);
    if (prog) {
      // start the clear the progline - not from the program
      int code_line = prog->script_list.FindMainLine(prog_el);
      ProgLine* pl = prog->script_list.SafeEl(code_line);
      prog_el->ClearBreakpoint();
      if (pl) {
        pl->ClearBreakpoint();
      }
    }
    prog_el.CutLinks();
  }
  inherited::CutLinks();
}

void ProgBrkPt::Enable() {
  if (enabled == true)
    return;
  enabled = true;
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    int code_line = prog->script_list.FindMainLine(prog_el);
    ProgLine* pl = prog->script_list.SafeEl(code_line);
    pl->SetBreakpoint();
  }
  SigEmitUpdated();
}

void ProgBrkPt::Disable() {
  if (enabled == false)
    return;
  enabled = false;
  Program* prog = GET_MY_OWNER(Program);
  if (prog) {
    int code_line = prog->script_list.FindMainLine(prog_el);
    ProgLine* pl = prog->script_list.SafeEl(code_line);
    pl->DisableBreakpoint();
  }
  SigEmitUpdated();
}
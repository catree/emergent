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

#include "Deep5bConSpec.h"

TA_BASEFUNS_CTORS_DEFN(Deep5bConSpec);

void Deep5bConSpec::Initialize() {
  SetUnique("wt_scale", true);
  wt_scale.rel = 3.0;           // good default
}

void Deep5bConSpec::GetPrjnName(Projection& prjn, String& nm) {
  nm = "Deep5b_" + nm;
}
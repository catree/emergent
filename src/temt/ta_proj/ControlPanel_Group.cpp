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

#include <ControlPanel>
#include "ControlPanel_Group.h"

TA_BASEFUNS_CTORS_DEFN(ControlPanel_Group);

void ControlPanel_Group::AutoEdit() { // Obsolete - see RestorePanels()
  FOREACH_ELEM_IN_GROUP(ControlPanel, se, *this) {
    if(se->autoEdit()) {
      se->EditPanel(true, true);        // true,true = new tab, pinned in place
    }
  }
}

void ControlPanel_Group::RestorePanels() {
  FOREACH_ELEM_IN_GROUP(ControlPanel, cp, *this) {
    if(cp->GetUserDataAsBool("user_pinned")) {
      cp->EditPanel(true, true); // true,true = new tab, pinned in place
    }
  }
}

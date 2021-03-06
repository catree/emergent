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

#include "NewViewHelper.h"

#include <T3Panel>
#include <T3DataView>
#include <T3PanelViewer>
#include <taBase>

taTypeDef_Of(taProject);

#include <taMisc>
#include <taiMisc>


// Ctor
NewViewHelper::NewViewHelper(
  T3Panel *&fr, taBase *obj, const char *objName)
  : fr_(fr)
  , obj_(obj)
  , objName_(objName)
{
}

bool NewViewHelper::isValid(bool enforceOneObjectInstancePerFrame) {
  // Ensure an object has been selected to add a view for.
  if (!obj_) return false;
  
  // Ensure the selected frame is in the same project as the object.
  if (!inSameProject()) return false;

  // Only allow one net view per frame (if that restriction was specified).
  if (enforceOneObjectInstancePerFrame && objectAlreadyInFrame())
    return false;

  // Ensure a frame is available to add the new object to.
  if (!getFrame()) return false;
  
  // All checks passed, this is a valid action.
  return true;
}

bool NewViewHelper::inSameProject() {
  if (fr_ && !obj_->SameScope(fr_, &TA_taProject)) {
    // Usually this can't happen, since the dialog to select a frame
    // filters on only those frames in the current project.
    const char *errorText =
      "The viewer you specified is not in the same Project as the";
    taMisc::Error(errorText, objName_);
    return false;
  }
  return true;
}

bool NewViewHelper::objectAlreadyInFrame() {
  if (fr_) {
    // check if already viewing this obj there, warn user
    if (T3DataView *dv = fr_->FindRootViewOfData(obj_)) {
      const char *question =
        "This network is already shown in that frame -- would you like"
        " to show it in a new frame?";
      if (taMisc::Choice(question, "&Ok", "&Cancel") != 0) {
        // User chose cancel, so return true to indicate that the
        // object already exists in this frame.
        return true;
      }
      
      // User wants a new frame.  Null the existing frame pointer
      // so a new one will be created in getFrame().
      fr_ = 0;
    }
  }
  
  // Three ways to get here:
  // * null frame passed in (indicating user wants a new one created)
  // * object not already being viewed in frame
  // * object was already viewed in frame, so user requested a new frame
  return false;
}

bool NewViewHelper::getFrame() {
  // If an existing frame wasn't provided,
  if (!fr_) {
    // Then create a new one to put the object in.
    fr_ = T3PanelViewer::GetBlankOrNewT3Panel(obj_);
  }
  
  // Return true to indicate that a frame was successfully identified
  // for the new object.
  return fr_; // should always be non-null.
}

void NewViewHelper::showFrame() {
  fr_->Render();
  fr_->ViewAll();
  if (fr_->singleChild())
    fr_->SaveCurView(0);
}

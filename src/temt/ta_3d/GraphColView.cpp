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

#include "GraphColView.h"
#include <DataCol>

TA_BASEFUNS_CTORS_DEFN(GraphColView);



void GraphColView::Initialize(){
}

void GraphColView::Destroy() {
}

void GraphColView::InitLinks() {
  inherited::InitLinks();
}

void GraphColView::CopyFromView(GraphColView* cp){
  fixed_range = cp->fixed_range;
}

String GraphColView::GetDisplayName() const {
  DataCol* dc = dataCol(); //note: exists, because we were called
  if(dc) return dc->GetDisplayName();
  return inherited::GetDisplayName();
}

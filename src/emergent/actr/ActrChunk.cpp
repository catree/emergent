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

#include "ActrChunk.h"
#include <ActrSlotType>
#include <ActrSlot>

#include <taMisc>

void ActrChunk::Initialize() {
  n_act = 0.0f;
  t_new = 0.0f;
  base_act = 0.0f;
}

void ActrChunk::Destroy() {
  CutLinks();
}

void ActrChunk::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFromType();
}

bool ActrChunk::UpdateFromType() {
  if(!chunk_type) return false;

  bool any_changes = false;
  int i;  int ti;
  ActrSlot* sv;
  ActrSlotType* st;
  // delete slots not in type; freshen those that are
  for (i = slots.size - 1; i >= 0; --i) {
    sv = slots.FastEl(i);
    st = chunk_type->slots.FindName(sv->name);
    if(st) {
      any_changes |= sv->UpdateFromType(*st);
    }
    else {
      slots.RemoveIdx(i);
      any_changes = true;
    }
  }
  // add in type not in us, and put in the right order
  for (ti = 0; ti < chunk_type->slots.size; ++ti) {
    st = chunk_type->slots.FastEl(ti);
    i = slots.FindNameIdx(st->name);
    if (i < 0) {
      sv = new ActrSlot();
      sv->UpdateFromType(*st);
      slots.Insert(sv, ti);
      any_changes = true;
    }
    else if (i != ti) {
      slots.MoveIdx(i, ti);
      any_changes = true;
    }
  }
  return any_changes;
}


String& ActrChunk::Print(String& strm, int indent) const {
  taMisc::IndentString(strm, indent);
  strm << name;
  if(chunk_type) {
    strm << " ISA " << chunk_type->name;
  }
  strm << " (";
  for(int i=0; i<slots.size; i++) {
    ActrSlot* sl = slots.FastEl(i);
    strm << " ";
    sl->Print(strm, 0);
    strm << ";";                // need some kind of sep!!
  }
  strm << " )";
  return strm;
}

String ActrChunk::GetDisplayName() const {
  return inherited::GetDisplayName();
}

String ActrChunk::GetDesc() const {
  return PrintStr();            // calls Print above
}

ActrSlot* ActrChunk::NewSlot() {
  return (ActrSlot*)New(1);
}

bool ActrChunk::MatchesProd(ActrProduction& prod, ActrChunk* cmp, bool exact, 
                            bool why_not) {
  if(!cmp) return false;
  if((bool)chunk_type && (bool)cmp->chunk_type) {
    if(chunk_type != cmp->chunk_type) {
      if(why_not) {
        taMisc::Info("chunk:", GetDisplayName(), "type mismatch");
      }
      return false; // must be same type..
    }
    for(int i=0; i<slots.size; i++) {
      ActrSlot* sl = slots.FastEl(i);
      ActrSlot* os = cmp->slots.SafeEl(i);
      if(!sl->MatchesProd(prod, os, exact, why_not))
        return false;
    }
    return true;                // pass through -- all good!
  }
  // todo: what do we do here??  lookup by names or something?
  return false;                 // not yet
}

bool ActrChunk::MatchesMem(ActrChunk* cmp, bool exact, bool why_not) {
  if(!cmp) return false;
  if((bool)chunk_type && (bool)cmp->chunk_type) {
    if(chunk_type != cmp->chunk_type) {
      if(why_not) {
        taMisc::Info("chunk:", GetDisplayName(), "type mismatch");
      }
      return false; // must be same type..
    }
    for(int i=0; i<slots.size; i++) {
      ActrSlot* sl = slots.FastEl(i);
      ActrSlot* os = cmp->slots.SafeEl(i);
      if(!sl->MatchesMem(os, exact, why_not))
        return false;
    }
    return true;                // pass through -- all good!
  }
  // todo: what do we do here??  lookup by names or something?
  return false;                 // not yet
}

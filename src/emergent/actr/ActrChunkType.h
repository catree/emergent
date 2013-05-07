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

#ifndef ActrChunkType_h
#define ActrChunkType_h 1

// parent includes:
#include "network_def.h"
#include <ActrChunkTypeRef>
#include <taNBase>

// member includes:
#include <ActrSlotType_List>

#ifdef slots
#undef slots
#endif

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrChunkType);

class E_API ActrChunkType : public taNBase {
  // ##INSTANCE ##EDIT_INLINE ##CAT_ActR ##SCOPE_ActrModel ##DEF_CHILD_slots ##DEF_CHILDNAME_Slots a chunk type for defining ActR chunks
INHERITED(taNBase)
public:
  String               desc; // #EDIT_DIALOG #HIDDEN_INLINE description of this chunk type
  ActrSlotType_List    slots;  // #SHOW_TREE the names and types of the slots within this chunk

  virtual ActrSlotType* NewSlot();
  // #BUTTON make a new slot in this chunk

  override taList_impl*	children_() {return &slots;}	
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return slots.Elem(idx, mode); }
  override String       GetDesc() const {return desc;}
  override String       GetTypeDecoKey() const { return "ProgType"; }

  TA_SIMPLE_BASEFUNS(ActrChunkType);
protected:
  //  override void	CheckChildConfig_impl(bool quiet, bool& rval);
private:
  void Initialize();
  void Destroy();
};

#endif // ActrChunkType_h
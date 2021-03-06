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

#ifndef MethodSpace_h
#define MethodSpace_h 1

// parent includes:
#include <Method_List>

// member includes:

// declare all other types mentioned but not required to include:
class MethodDef; //
class String_PArray; //
class TypeDef; //

#ifndef NO_TA_BASE
class taSigLink; //
#endif


taTypeDef_Of(MethodSpace);

class TA_API MethodSpace: public Method_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of methods
INHERITED(Method_List)
protected:
  String        GetListName_() const            { return name; }
  String El_GetName_(void* it) const;
  taPtrList_impl* El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
#ifndef NO_TA_BASE
  taSigLink*   sig_link;
#endif

  void          Initialize();
  MethodSpace()                         { Initialize(); }
  MethodSpace(const MethodSpace& cp)    { Initialize(); Borrow(cp); }
  ~MethodSpace();

  void operator=(const MethodSpace& cp) { Borrow(cp); }

  bool          AddUniqNameNew(MethodDef* it);

  MethodDef*    FindVirtualBase(MethodDef* it, int& idx);
  // find the virtual method with same name and signature

  // IO
  virtual String&      PrintType(String& strm, int indent = 0) const;

};

#endif // MethodSpace_h

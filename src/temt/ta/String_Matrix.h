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

#ifndef String_Matrix_h
#define String_Matrix_h 1

// parent includes:
#include <taString>
#include <taMatrixT>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MatrixIndex; // 


class TA_API String_Matrix: public taMatrixT<String> {
  // #INSTANCE a matrix of strings
INHERITED(taMatrixT<String>)
public:
  override int          defAlignment() const;
  override TypeDef*     GetDataTypeDef() const {return &TA_taString;}
  override ValType      GetDataValType() const {return VT_STRING;}

  String        ToDelimString(const String& delim = " ");
  // generates a string of all the items on the list, using given delimiter between items
  void          FmDelimString(const String& str, const String& delim = " ");
  // fill this matrix by parsing given string using given delimiter separating strings -- increases size as necessary to fit everything

  TA_MATRIX_FUNS_SLOW(String_Matrix, String)

public:
  override float        El_GetFloat_(const void* it) const
    { return ((String*)it)->toFloat(); } // #IGNORE
  override const String El_GetStr_(const void* it) const {return *((String*)it); } // #IGNORE
  override void         El_SetFmStr_(void* it, const String& str) {*((String*)it) = str;}  // #IGNORE
  override const Variant El_GetVar_(const void* it) const {return Variant(*((String*)it));} // #IGNORE
  override void         El_SetFmVar_(void* it, const Variant& var) {*((String*)it) = var.toString(); };  // #IGNORE
protected:
  STATIC_CONST String   blank; // #IGNORE
  override void         Dump_Save_Item(ostream& strm, int idx);
  override int          Dump_Load_Item(istream& strm, int idx);
  override void         ReclaimOrphans_(int from, int to); // called when elements can be reclaimed, ex. for strings

private:
  void          Initialize() {}
  void          Destroy() { CutLinks(); }
};

TA_SMART_PTRS(String_Matrix);

#endif // String_Matrix_h
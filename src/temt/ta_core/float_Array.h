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

#ifndef float_Array_h
#define float_Array_h 1

// parent includes:
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(float_Array);

class TA_API float_Array : public taArray<float> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of floats
INHERITED(taArray<float>)
public:
  static const float blank; // #HIDDEN #READ_ONLY
  TA_BASEFUNS_NOCOPY(float_Array);
  TA_ARRAY_FUNS(float_Array, float)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (Variant)(*(float*)itm); }
  int           El_Compare_(const void* a, const void* b) const override
  { int rval=-1; if(*((float*)a) > *((float*)b)) rval=1; else if(*((float*)a) == *((float*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const override
    { return (*((float*)a) == *((float*)b)); }
  String        El_GetStr_(const void* it) const override { return (*((float*)it)); }
  void          El_SetFmStr_(void* it, const String& val) override
  { float tmp = (float)val; *((float*)it) = tmp; }
private:
  void Initialize()     {err = 0.0f; };
  void Destroy()        { };
};

TA_ARRAY_OPS(TA_API, float_Array)

#endif // float_Array_h

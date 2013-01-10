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

#include "taVector2i.h"

taVector2i::taVector2i(const taVector2f& cp) {
  Register(); Initialize(); x = (int)cp.x; y = (int)cp.y;
}


taVector2i& taVector2i::operator=(const taVector2f& cp) {
  x = (int)cp.x; y = (int)cp.y;
  return *this;
}

void taVector2i::CopyToMatrixGeom(MatrixGeom& geom) {
// NOTE: taVector3i just replaces this whole routine, for simplicity
  geom.SetDims(2);
  geom.Set(0, x);
  geom.Set(1, y);
}

void taVector2i::CopyToMatrixIndex(MatrixIndex& idx) {
// NOTE: taVector3i just replaces this whole routine, for simplicity
  idx.SetDims(2);
  idx.Set(0, x);
  idx.Set(1, y);
}

bool taVector2i::FitN(int n) {
  if((x * y) == n)	return false;
  y = (int)sqrtf((float)n);
  if(y < 1)
    y = 1;
  x = n / y;
  if(x*y == n) return true; // got lucky

  // next try a range of y's to fit evenly
  int sqrty = y;
  int lwy = y/2;  int hiy = y*2;
  if(lwy == 0) lwy = 1;
  for(y = lwy; y<=hiy; y++) {
    x = n / y;
    if(x*y == n) return true; // got lucky
  }
  if(n < 20) {
    x = n;    y = 1;		// just go linear for small values
    return true;
  }
  // else just go with an imperfect fit
  y = sqrty;
  x = n / y;
  while((x * y) < n)
    x++;
  return true;
}

bool taVector2i::WrapClipOne(bool wrap, int& c, int max) {
  bool out_of_range = false;
  if(wrap) {
    if(c >= max) {
      if(c > max + max/2) out_of_range = true; // wraps past half way to other side
      c = c % max;
    }
    else if(c < 0) {
      if(c < -max/2) out_of_range = true; // wraps past half way to other side
      c = max + (c % max);
    }
  }
  else {
    if(c >= max) {
      out_of_range = true;
      c = max-1;
    }
    else if(c < 0) {
      out_of_range = true;
      c = 0;
    }
  }
  return out_of_range;
}
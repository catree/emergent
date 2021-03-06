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

#ifndef T3TransformBoxDragger_h
#define T3TransformBoxDragger_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#endif

#include <T3Node>

// member includes:

// declare all other types mentioned but not required to include:
class SoTransformBoxDragger;
class SoTransform;
class SoCalculator;

taTypeDef_Of(T3TransformBoxDragger);

class TA_API T3TransformBoxDragger: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS my custom transform box dragger kit for positioning T3 view guys
#ifndef __MAKETA__
typedef SoSeparator inherited;
  TA_SO_NODE_HEADER(T3TransformBoxDragger);
#endif // def __MAKETA__
public:
  static void		initClass();
  T3TransformBoxDragger(float half_size=1.1f, float cube_size=0.6f, float line_width=0.4f);

  // elements within me:
  SoTransform*		xf_;		// #IGNORE transform -- set as you need
  SoTransformBoxDragger* dragger_;	// #IGNORE dragger
  SoCalculator*		trans_calc_;    // #IGNORE translation calculator -- gets from translation of dragger
protected:
  const char*  	getFileFormatName() const override {return "Separator"; } 
};

#endif // T3TransformBoxDragger_h

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

#ifndef T3GraphLine_h
#define T3GraphLine_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:
class SoFont; // 
class iVec3f; // 
class T3Color; // 


class TA_API T3GraphLine: public T3NodeLeaf {
  // a graph line -- data is GraphColView
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GraphLine);
#endif // def __MAKETA__
public:
  enum LineStyle { // corresponds to GraphColView::LineStyle
    SOLID,			// -----
    DOT,			// .....
    DASH,			// - - -
    DASH_DOT			// _._._
#ifndef __MAKETA__
    ,LineStyle_MIN = SOLID, // also the default
    LineStyle_MAX = DASH_DOT
#endif
  };

  enum MarkerStyle { // MUST correspond to GraphColView::PointStyle
    CIRCLE = 1,			// o
    SQUARE,			// []
    DIAMOND,			// <>
    TRIANGLE,			//
    MINUS,			// -
    BACKSLASH,		//
    BAR,			// |
    SLASH,			// /
    PLUS,			// +
    CROSS,			// x
    STAR			// *
#ifndef __MAKETA__
    ,MarkerStyle_NONE = 0, // pseudo value, not actually used
    MarkerStyle_MIN = CIRCLE,
    MarkerStyle_MAX = STAR
#endif
  };

  enum MarkerSize {
    SMALL,
    MEDIUM,
    LARGE,
  };

  static void		initClass();

  SoFont*		labelFont() const {return labelFont_;} // setup after creating
  void 			setLineStyle(LineStyle value, float line_width = 0.0f);
  void			setMarkerSize(MarkerSize sz);
  bool			valueColorMode(){return valueColorMode_;}
  void			setValueColorMode(bool value);

  void			clear(); //

  // text api
  void			textAt(const iVec3f& pt, const char* str);
  // render indicated text at the pt

  void			startBatch();
  // start a batch edit of the lines -- must be followed by a finishBatch call!  just turns off notification of updates
  void			finishBatch();
  // done with batch update -- turn notification back on and do a touch

  // non valueColor drawing api
  void			setDefaultColor(const T3Color& color); // call after reset to set default color (black if not called)
  void			moveTo(const iVec3f& pt); // use to start a new line segment
  void			lineTo(const iVec3f& to); // add arc to current line, in default color
  void			errBar(const iVec3f& pt, float err, float bar_width); // render error bar at given point
  void			markerAt(const iVec3f& pt, MarkerStyle style); // render a marker at indicated location in default color

  // valueColor drawing api
  void			moveTo(const iVec3f& pt, const T3Color& color);
  // use to start a new line segment in valueColor mode
  void			lineTo(const iVec3f& to, const T3Color& color);
  //  add arc to current line, in valueColor mode
  void			errBar(const iVec3f& pt, float err, float bar_width, const T3Color& color);
  // render error bar at given point
  void			markerAt(const iVec3f& pt, MarkerStyle style, const T3Color& color);
  // render a marker at indicated location in valueColor mode

  T3GraphLine(T3DataView* dataView_ = NULL, float fnt_sz = .05f);

protected:
  uint32_t		defColor_; // def is black
  bool			valueColorMode_;
  LineStyle		lineStyle_;
  MarkerSize		marker_size_;
  SoSeparator*		line_sep;
  SoDrawStyle*		lineDrawStyle_;
  SoLineSet*		lines;		// we use the vertexProperty for points etc.
  SoLineSet*		errbars;
  SoSeparator*		marker_sep;
  SoMarkerSet*		markerSet_; 
  SoSeparator*		textSep_; // optional text separator
  SoComplexity*		complexity_;
  SoFont*		labelFont_;
  float			font_size_;
  SoPackedColor*	textColor_;
  iVec3f		lastText_; // where last text was rendered, for our next translate

  void			assertMarkerSet();
    // makes sure marketset is created, and initialized base on modes
  void			assertText();
    // makes sure text separator is created;
  void			initValueColorMode(); // called in several places
  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  ~T3GraphLine();
};

#endif // T3GraphLine_h
// Copyright, 1995-2007, Regents of the University of Colorado,
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



// netstru_so.h -- inventor controls/objects for network structures

// NOTE: functions inside "ifdef GUI" blocks are implemented in netstru_qtso.cc

#ifndef NETSTRU_SO_H
#define NETSTRU_SO_H

#include "network_def.h"
#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCone; // #IGNORE
class SoCube; // #IGNORE
class SoCylinder; // #IGNORE
class SoFont; // #IGNORE
class SoIndexedTriangleStripSet; // #IGNORE
class SoVertexProperty;		 // #IGNORE
class SoTranslate2Dragger; // #IGNORE
class SoTransformBoxDragger; // #IGNORE
class SoCalculator; // #IGNORE
class SoComplexity; // #IGNORE
class SoIndexedLineSet; // #IGNORE
class SoDrawStyle; // #IGNORE

// forwards
class T3UnitNode;
class T3UnitGroupNode;
class T3LayerNode;
class T3PrjnNode;
class T3LayerGroupNode;
class T3NetNode;


////////////////////////////////////////////////////
//   T3UnitNode	

/*
    this: SoSeparator - the object itself
      transform: SoTransform
      material: SoMaterial
      [drawStyle: SoDrawStyle] (inserted when picked, to make wireframe instead of solid)
      shape: SoCylinder (could be changed to something else)
*/

class EMERGENT_API T3UnitNode: public T3NodeLeaf { //
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_ABSTRACT_HEADER(T3UnitNode);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float	base_height; // #DEF_0.1 height when not active or empty
  static float	max_height; // #DEF_0.8 height when fully active

  static void	initClass();

  void		setAppearance(float act, const T3Color& color, float max_z, float trans);
  // act is -1:1; max_z is net->max_size.z; trans is transparency
  virtual void 	setPicked(bool value);
  T3UnitNode(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
	     float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  float			spacing;		      // unit spacing
  float			disp_scale;		      // overall scaling
  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  virtual void	setAppearance_impl(float act, const T3Color& color, float max_z,
    float trans, bool act_invalid);
  // act is -1:1; max_z is net->max_size.z; trans is transparency; act_invalid true for nan/inf -- act has been set to 0.0f
  ~T3UnitNode();
};

class EMERGENT_API T3UnitNode_Cylinder: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Cylinder);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  T3UnitNode_Cylinder(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		      float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  override void	 setAppearance_impl(float act, const T3Color& color,
    float max_z, float trans, bool act_invalid);
  ~T3UnitNode_Cylinder();
private:
  SoCylinder*		shape_; //#IGNORE
};

class EMERGENT_API T3UnitNode_Circle: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Circle);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  T3UnitNode_Circle(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		    float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Circle();
private:
  SoCylinder*		shape_; //#IGNORE
};


class EMERGENT_API T3UnitNode_Block: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Block);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  T3UnitNode_Block(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		   float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  override void	 setAppearance_impl(float act, const T3Color& color,
    float max_z, float trans, bool act_invalid);
  ~T3UnitNode_Block();
private:
  SoCube*		shape_; //#IGNORE
};

class EMERGENT_API T3UnitNode_Rect: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Rect);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void		initClass();

  T3UnitNode_Rect(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		  float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Rect();
private:
  SoCube*		shape_; //#IGNORE
};


////////////////////////////////////////////////////
//   T3UnitGroupNode	

class EMERGENT_API T3UnitGroupNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3UnitGroupNode);
#endif // def __MAKETA__
public:
  static float		height; // nominal amount of height, so doesn't vanish
  static float		inset; // amount inset from the layer

  static void		initClass();
  static void		shapeCallback(void* data, SoAction* act);
  static void		drawGrid(T3UnitGroupNode* node);

//  void			clearUnits(); // remove all units
//  void			addUnit(int x, int y); // remove all units
  SoFont*		unitCaptionFont(bool auto_create = false);

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z,
				float disp_scale);

  // sets (actual) geom of group; creates/positions units; setes max_size
  SoIndexedTriangleStripSet* shape() {return shape_;}
  SoVertexProperty* 	vtxProp() {return vtx_prop_;}
  SoSeparator*		unitText() {return unit_text_;} // extra text of unit vars
  SoSeparator*		getUnitText();		      // get a new unit text separator
  void			removeUnitText();	      // remove unit text separator

  SoSeparator*		snapBordSep() {return snap_bord_;}
  SoDrawStyle*		snapBordDraw() {return snap_bord_draw_;}
  SoIndexedLineSet*	snapBordSet() {return snap_bord_set_;}
  SoVertexProperty* 	snapBordVtxProp() {return snap_bord_vtx_prop_;}

  T3UnitGroupNode(T3DataView* dataView_ = NULL, bool no_unts = false);

protected:
  iVec2i		geom; //note, not a field
  float			disp_scale;
  iVec2i		scaled_geom; // layer disp_scale * geom
  iVec3f		max_size; // maximum size of network x,y,z
  SoFont*		unitCaptionFont_;
  bool			no_units; // summary mode: no unit objects exist

  SoSeparator* 		snap_bord_; // weight lines
  SoDrawStyle*		snap_bord_draw_;
  SoIndexedLineSet*	snap_bord_set_;
  SoVertexProperty*	snap_bord_vtx_prop_;

  ~T3UnitGroupNode();

protected:
  SoIndexedTriangleStripSet*	shape_;
  SoVertexProperty*	vtx_prop_;
  SoSeparator* 		unit_text_; // unit text variables
};


////////////////////////////////////////////////////
//   T3LayerNode

class EMERGENT_API T3LayerNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3LayerNode);
#endif // def __MAKETA__

public:
  static const float 	height; // = 0.05f height of layer frame shape itself (in fractions of a unit)
  static const float 	width; // = 0.5f width of layer frame shape (in frac of unit)
  static const float 	max_width; // = 0.05f maximum absolute width value (prevent mondo frames for very small nets)

  static void		initClass();

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z, float disp_scale);

  T3LayerNode(T3DataView* dataView_ = NULL, bool show_draggers = true);

protected:
  
  iVec2i		geom; //note, not a field
  float			disp_scale;
  iVec2i		scaled_geom; // disp_scale * geom
  iVec3f		max_size; // maximum size of network x,y,z
  void			render(); // called after pos/geom changes
  ~T3LayerNode();

private:
  SoFrame*		shape_;

  bool			show_drag_;

  SoSeparator*		xy_drag_sep_;
  SoTransform*		xy_drag_xf_;
  SoTranslate2Dragger*	xy_dragger_;
  SoCalculator*		xy_drag_calc_;

  SoSeparator*		z_drag_sep_;
  SoTransform*		z_drag_xf_;
  SoTranslate1Dragger*	z_dragger_;
  SoCalculator*		z_drag_calc_;
};


////////////////////////////////////////////////////
//   T3PrjnNode	

class EMERGENT_API T3PrjnNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3PrjnNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  void		setEndPoint(const SbVec3f& value); // #IGNORE sets endpoint, relative to its origin
  void		setArrowColor(const SbColor& clr, float transp);
  // set arrow color

  T3PrjnNode(T3DataView* dataView_ = NULL, bool projected = true, float rad = .01f);

protected:
  bool			projected_;
  float			radius;
  SoComplexity*		complexity;
  SoTransform*		trln_prjn; // #IGNORE
  SoTransform*		rot_prjn; // #IGNORE
  SoTransform*		trln_arr; // #IGNORE
  SoCone*		arr_prjn;  // #IGNORE arrow head
  SoCylinder*		line_prjn;  // #IGNORE line
  SoMaterial*		arr_mat;  // #IGNORE arrow material (color)

  ~T3PrjnNode();
private:
  void			init();
};


////////////////////////////////////////////////////
//   T3LayerGroupNode

class EMERGENT_API T3LayerGroupNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3LayerGroupNode);
#endif // def __MAKETA__

public:
  static void		initClass();

  void 			setGeom(int px, int py, int pz, 
				float lg_max_x, float lg_max_y, float lg_max_z,
				float max_x, float max_y, float max_z);

  bool			hideLines() { return hide_lines_; }
  SoDrawStyle* 		drawStyle() { return drw_styl_; }

  T3LayerGroupNode(T3DataView* dataView_ = NULL, bool show_draggers = true, bool hide_lines = false);

protected:
  
  iVec3i		pos; 	// starting position
  iVec3i		lgp_max_size; 	// layer group max_size
  iVec3f		max_size; // maximum size of network x,y,z

  void			render(); // called after pos/geom changes
  ~T3LayerGroupNode();

private:
  SoLineBox3d*		shape_;
  SoDrawStyle*		drw_styl_;

  bool			show_drag_;
  bool			hide_lines_;

  SoSeparator*		xy_drag_sep_;
  SoTransform*		xy_drag_xf_;
  SoTranslate2Dragger*	xy_dragger_;
  SoCalculator*		xy_drag_calc_;

  SoSeparator*		z_drag_sep_;
  SoTransform*		z_drag_xf_;
  SoTranslate1Dragger*	z_dragger_;
  SoCalculator*		z_drag_calc_;
};


////////////////////////////////////////////////////
//   T3NetViewObjNode

class EMERGENT_API T3NetViewObj : public T3NodeLeaf {
  // network view object
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3NetViewObj);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3NetViewObj(T3DataView* dataView_ = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3NetViewObj();
};


////////////////////////////////////////////////////
//   T3NetNode

class EMERGENT_API T3NetNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3NetNode);

#endif // def __MAKETA__

public:
  static void		initClass();

  SoDrawStyle*		shapeDraw() {return shape_draw_;}
  SoSeparator*		netText() {return net_text_;} // extra text of network vars etc
  SoTransform* 		netTextXform() {return net_text_xform_;}
  SoTransform* 		netTextDragXform() {return net_text_drag_xform_;}
  bool			netTextDrag() {return show_net_text_drag_;}
  SoSeparator*		wtLinesSep() {return wt_lines_;}
  SoDrawStyle*		wtLinesDraw() {return wt_lines_draw_;}
  SoIndexedLineSet*	wtLinesSet() {return wt_lines_set_;}
  SoVertexProperty* 	wtLinesVtxProp() {return wt_lines_vtx_prop_;}

  T3NetNode(T3DataView* dataView_ = NULL, bool show_draggers = true,
	    bool show_net_text = true, bool show_nt_drag = true);

protected:
  void		setDefaultCaptionTransform(); // override
  ~T3NetNode();

protected:
  SoDrawStyle*		shape_draw_;
  SoLineBox3d*		shape_; //#IGNORE
  SoSeparator* 		net_text_; // network text variables
  SoSeparator* 		wt_lines_; // weight lines
  SoDrawStyle*		wt_lines_draw_;
  SoIndexedLineSet*	wt_lines_set_;
  SoVertexProperty*	wt_lines_vtx_prop_;

  bool			show_drag_;
  bool			show_net_text_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger
  T3TransformBoxDragger* net_text_drag_;	// my net text dragger
  SoTransform*		net_text_xform_;
  SoTransform*		net_text_drag_xform_;
};

#endif // NETSTRU_SO_H


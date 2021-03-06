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

#ifndef T3ExaminerViewer_h
#define T3ExaminerViewer_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#include <T3SavedView_List>
#include <NameVar_PArray>
#include <iAction_List>

// declare all other types mentioned but not required to include:
class iT3ViewspaceWidget; //
class T3Panel; //
class iThumbWheel; //
class QVBoxLayout; // 
class QHBoxLayout; // 
class QLabel; //
class iMenuButton; //

#ifdef TA_QT3D
#else
class SoCamera; //
class SbViewportRegion; //
#endif

#ifndef __MAKETA__
#ifdef TA_QT3D

namespace Qt3DCore {
  class QAspectEngine;
  class QEntity;
}

namespace Qt3DRender {
  class QRenderAspect;
  class QCamera;
  class QFrameGraphNode;
  class QViewport;
  class QDirectionalLight;
}

namespace Qt3DInput {
  class QInputAspect;
  class QMouseDevice;
}

namespace Qt3DExtras {
  class QOrbitCameraController;
}

#include <Qt3DExtras/Qt3DWindow>

class T3CameraParams; //

class T3RenderView : public Qt3DExtras::Qt3DWindow {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS rendering surface for Qt3D rendering
  Q_OBJECT
public:
  explicit T3RenderView();
  ~T3RenderView();

protected:
  virtual void keyPressEvent(QKeyEvent *e);
};

#else
#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>
using SIM::Coin3D::Quarter::QuarterWidget;
#endif
#endif

class TA_API T3ExaminerViewer : public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS provides a full examiner viewer interface built on top of the QuarterWidget viewer, replicating the GUI of the SoQtExaminerViewer
  Q_OBJECT
  INHERITED(QWidget)
public:
  T3ExaminerViewer(iT3ViewspaceWidget* parent = NULL);
  ~T3ExaminerViewer();

  enum ViewerMode {             // our persistent sense of the view mode -- quarter may get out of sync
    VIEW = 0,                   // move camera around (must be 0 so equiv to interaction off)
    INTERACT = 1,               // interactive (can select -- red arrow) (must be 1 so equiv to interaction on)
    PAN = 2,                    // panning instead of rotating (not yet avail -- for future use)
    SEEK = 3,                   // seeking (not a persistent state -- not used currently -- for future use)
  };


  ViewerMode      viewer_mode;    // our current viewer mode -- used to make sure that quarter is in appropriate state

  iT3ViewspaceWidget*   t3vw;           // owner widget
  T3Panel*      GetPanel();     // get my owning panel, from t3vw

  //////////////////////////////////////////////
  //   Overall widget structure

  QVBoxLayout*    main_vbox;       // overall vertical box, containing main_hbox, bot_hbox
  QHBoxLayout*    main_hbox;       // overall horizontal box, containing lhs_vbox, quarter, rhs_vbox
  QVBoxLayout*    lhs_vbox;     // overall box for all decoration on the left-hand-side of widget -- contains (optional -- none in default impl) buttons at top and vrot_wheel at bottom
  QVBoxLayout*    lhs_button_vbox; // buttons on the top-left hand side (empty by default)
  QVBoxLayout*    rhs_vbox;     // overall box for all decoration on the right-hand-side of widget -- contains buttons at top and zoom_wheel at bottom
  QVBoxLayout*    rhs_button_vbox; // buttons on the top-right hand side
  QHBoxLayout*    bot_hbox;        // overall box for all decoration on the bottom of widget -- contains (optional -- none in default impl) buttons at right and hrot_wheel at left
  QHBoxLayout*    bot_button_hbox; // buttons on the bottom right side

#ifdef TA_QT3D
#ifndef __MAKETA__
  T3RenderView*        view3d;       // surface that we render onto
  Qt3DCore::QEntity*   root_entity;  // root of entire scenegraph, containing camera, then scene
  Qt3DRender::QCamera* camera;       // camera
  Qt3DExtras::QOrbitCameraController*  camera_ctrl;        // controls camera
  Qt3DCore::QEntity*   camera_light_ent;  // camera's light entity
  Qt3DCore::QEntity*   sun_light_ent;     // sun light entity
  Qt3DRender::QDirectionalLight*   sun_light;     // sun light
  Qt3DRender::QDirectionalLight*   camera_light;  // camera light
  Qt3DCore::QEntity*   scene;        // root of the actual objects being viewed -- below camera
  Qt3DRender::QViewport*   viewport;     // viewport for rendering
  Qt3DInput::QMouseDevice* mouse_dev; // overall mouse device for scene -- lives in root_entity
  QColor               bg_color;     // background color
#endif
#else
  QuarterWidget*  quarter;      // the quarter viewer -- in the middle of the widget
#endif
  
  //////////////////////////////////////////////
  //   Thumbwheels

  iThumbWheel*   zoom_wheel;   // the zoom wheel (dolly in SoQt)
  iThumbWheel*   vrot_wheel;   // the vertical rotation wheel (Rotx in SoQt)
  iThumbWheel*   hrot_wheel;   // the horizontal rotation wheel (Roty in SoQt)
  iThumbWheel*   vpan_wheel;   // the vertical panning wheel
  iThumbWheel*   hpan_wheel;   // the horizontal panning wheel
  QLabel*         zoom_lbl;     // labels
  QLabel*         vrot_lbl;
  QLabel*         hrot_lbl;
  QLabel*         vpan_lbl;
  QLabel*         hpan_lbl;

  //////////////////////////////////////////////
  //   Standard view buttons (on RHS)

  QToolButton*    interact_button; // (red arrow) -- mouse actions interact with elements in the display
  QToolButton*    view_button;     // (white hand) -- mouse actions move view around
  QToolButton*    view_all_button; // (eyeball) -- repositions camera so everything is in view
  QToolButton*    seek_button;     // (flashlight) -- zooms display to view clicked objects
  QToolButton*    snapshot_button; // (camera) -- save an image of the current view to a file
  QToolButton*    print_button;    // (printer) -- print current view to a file
  iMenuButton*    annote_button;   // (+) -- add annotations to the view

  static const int   n_views;      // number of saved view parameters to save (length of saved_views)
  T3SavedView_List saved_views; // saved view information
  int              cur_view_no; // current view number -- last one to have gotoView called -- -1 if not done yet
  NameVar_PArray     dyn_buttons; // dynamic button names
  iAction_List     dyn_actions; // dynamic button actions -- has all the relevant properties

  static bool     so_scrollbar_is_dragging;
  // SoScrollbar sets this when it is being dragged -- keeps everything inside a tight event loop until dragging completes

  //////////////////////////////////////////////
  //   Constructor helper methods

  void    Constr_RHS_Buttons();         // construct right-hand-side pushbuttons
  void    Constr_LHS_Buttons();         // construct left-hand-side pushbuttons
  void    Constr_Bot_Buttons();         // construct bottom pushbuttons

  int     addDynButton(const String& label, const String& tooltip);
  // add a new dynamic button -- returns button number (may already exist)
  iAction* getDynButton(int but_no);
  // get given dynamic button action -- has all the relevant info
  iAction* getDynButtonName(const String& label);
  // get dynamic button action by name -- has all the relevant info
  void    setDynButtonChecked(int but_no, bool onoff, bool mutex = true);
  // set the "checked" status of the dyn button to given state -- if mutex is true, then all other buttons are turned off when current one is changed (regardless of on/off state, all are off)

  void    removeAllDynButtons();
  // remove all the dynamic buttons
  bool    removeDynButton(int but_no);
  // remove given dynamic button
  bool    removeDynButtonName(const String& label);
  // remove given dynamic button by name

  //////////////////////////////////////////////
  //   Functions that actually do stuff

#ifdef TA_QT3D
  Qt3DRender::QCamera*  getViewerCamera() const  { return camera; }
  void                  setSceneGraph(Qt3DCore::QEntity* root);
  // set scene graph to given new root -- added under root_entity
  Qt3DCore::QEntity*        getSceneGraph() const  { return scene; }
  // current scene being viewed
  void                  setBackgroundColor(const QColor & color);
  QColor                backgroundColor() const { return bg_color; }
  void                  setCameraParams(const T3CameraParams& cps);
  void                  updateAspectRatio(); // get aspect ratio from current window size
#else
  SoCamera*             getViewerCamera() const;
  // helper function get the quarter viewer camera (not immediately avail on quarter widget)
  const SbViewportRegion& getViewportRegion() const;
  // helper function get the quarter viewer viewport region (not immediately avail on quarter widget)
  void                  setSceneGraph(SoNode* root)
  { quarter->setSceneGraph(root); }
  // set the scene graph on the quarter widget
  SoNode*               getSceneGraph() const
  { return quarter->getSceneGraph(); }

  void setBackgroundColor(const QColor & color)
  { quarter->setBackgroundColor(color); }
  QColor backgroundColor() const
  { return quarter->backgroundColor(); }
#endif
  
  virtual void          viewAll();
  // view all objects in scene -- compared to QuarterWidget's default version, this one doesn't leave such a huge margin around everything so you really fill the window

  virtual void          zoomView(const float zoom_value);
  // zoom view camera in/out by given amount: adjusts both camera pos and focal length -- associated with the zoom_wheel on the right side of viewer
  virtual void          horizRotateView(const float rot_value);
  // horizontally rotate view camera (actually around the vertical Y axis) -- associated with the hrot_wheel on bottom of viewer
  virtual void          vertRotateView(const float rot_value);
  // vertically rotate view camera (actually around the horizontal or X axis) -- associated with the vrot_wheel on left hand side of viewer
  virtual void          horizPanView(const float pan_value);
  // horizontally pan (move) view camera -- associated with the hpan_wheel on bottom of viewer
  virtual void          vertPanView(const float pan_value);
  // vertically pan (move) view camera  -- associated with the vpan_wheel on right hand side of viewer

  virtual void          syncViewerMode();
  // ensure that quarter is in the same viewer mode that we think it should be in -- this should be called upon any redraw that might knock quarter out of whack.
  bool                  syncCursor();
  // set our cursor to match what quarter says it should be..

  virtual void          setInteractionModeOn(bool onoff, bool re_render = true);
  // set the interaction mode on or off (if off, then it is in view mode) -- also updates button states -- also calls Render on parent dataview if mode has changed, so that interactors can be updated (unless re_render = false)
  bool                  interactionModeOn() { return (bool)viewer_mode; }
  // is interaction mode on or not -- this is the interface that all 3rd parties should consult -- not the one on quarter which can often be out of sync!

  virtual void          saveView(int view_no);
  // save the current camera view information to given saved view
  virtual void          gotoView(int view_no);
  // restore the saved camera view information to the current view
  virtual void          nextView();
  // move to the next higher number view
  virtual void          previousView();
  // move to the next lower number view
  virtual bool          nameView(int view_no, const String& name);
  // add a new label for given saved view location
  virtual void          updtViewName(int view_no);
  // update displayed name from saved_views -- during init

  virtual QImage        grabImage();
  // grab the current viewer image to a pixmap
  virtual void          saveImage(const QString& fname);
  // save the current viewer image to given file name
  virtual void          printImage();
  // print the current viewer image to a printer

public slots:
  void hrotwheelChanged(int value);
  void vrotwheelChanged(int value);
  void zoomwheelChanged(int value);
  void hpanwheelChanged(int value);
  void vpanwheelChanged(int value);

  void interactbuttonClicked();
  void viewbuttonClicked();
  void viewallbuttonClicked();
  void seekbuttonClicked();
  void snapshotbuttonClicked();
  void printbuttonClicked();

  void annoteLineClicked();
  void annoteStartArrowClicked();
  void annoteEndArrowClicked();
  void annoteDoubleArrowClicked();
  void annoteRectangleClicked();
  void annoteEllipseClicked();
  void annoteCircleClicked();
  void annoteTextClicked();
  void annoteObjectClicked();
  void annoteClearAllClicked();
  void annoteEditAllClicked();

  void gotoviewbuttonClicked(int view_no);
  void saveviewTriggered(int view_no);
  void nameviewTriggered(int view_no);
  void savenameviewTriggered(int view_no);

  void dynbuttonClicked(int but_no);

#ifndef __MAKETA__
signals:
  void  viewSaved(int view_no);  // the given view location was saved (e.g., can now save to more permanent storage)
  void  viewSelected(int view_no); // the given view location was selected for viewing -- other saved elements can now be activated by this signal
  void  dynbuttonActivated(int but_no); // dynamic button of given number was activated by user
  void  unTrappedKeyPressEvent(QKeyEvent* e);
#endif

protected:
  // start values on wheels for computing deltas
  int   hrot_start_val;
  int   vrot_start_val;
  int   zoom_start_val;
  int   hpan_start_val;
  int   vpan_start_val;

#ifdef TA_QT3D
  virtual void  RotateView(const QVector3D& axis, const float ang);
  // implementation function that will rotate view camera given angle (in radians) around given axis
  virtual void  PanView(const QVector3D& dir, const float dist);
  // implementation function that will move (pan) view camera given distance in given direction
  void    showEvent(QShowEvent* ev) override;
#else
  virtual void  RotateView(const SbVec3f& axis, const float ang);
  // implementation function that will rotate view camera given angle (in radians) around given axis
  virtual void  PanView(const SbVec3f& dir, const float dist);
  // implementation function that will move (pan) view camera given distance in given direction
#endif

  bool event(QEvent* ev_) override;
  void keyPressEvent(QKeyEvent* e) override;
  bool eventFilter(QObject *obj, QEvent *event) override;
  void resizeEvent(QResizeEvent* ev) override;
};

#endif // T3ExaminerViewer_h

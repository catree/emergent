// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef BRAIN_VIEW_PANEL_H
#define BRAIN_VIEW_PANEL_H

#include "netstru_qtso.h" // overkill #include
class QString;

class EMERGENT_API BrainViewPanel: public iViewPanelFrame {
  // frame for gui interface to a BrainView -- usually posted by the netview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:

  QWidget*          widg;
  QVBoxLayout*		layTopCtrls;
  QVBoxLayout*		layViewParams;
  QHBoxLayout*		layDispCheck;
  QCheckBox*		chkDisplay;
  // QCheckBox*		chkLayMove;
  QCheckBox*		    chkNetText;
  //QLabel*		    lblTextRot;
  //taiField*		    fldTextRot;
  //QLabel*		    lblDispMode;
  //taiComboBox*		cmbDispMode;

  QHBoxLayout*		layFontsEtc;
  QLabel*		    lblUnitTrans;
  taiField*		    fldUnitTrans;
  //QLabel*		    lblUnitFont;
  //taiField*		    fldUnitFont;
  //QLabel*		    lblLayFont;
  //taiField*		    fldLayFont;
  //QLabel*		    lblMinLayFont;
  //taiField*		    fldMinLayFont;
  //QCheckBox*		chkXYSquare;
  //QCheckBox*		chkLayGp;

  QVBoxLayout*		layDisplayValues;
  QHBoxLayout*		layColorScaleCtrls;
  QCheckBox*		chkAutoScale;       // autoscale ck_box
  QPushButton*		butScaleDefault;    // revert to default  

  QHBoxLayout*          layColorBar;
  //QLabel*           lblUnitSpacing;
  //taiField*		    fldUnitSpacing;
  ScaleBar*         cbar;	      // colorbar
  QPushButton*		butSetColor;

  QTabWidget*           tw;
  QTreeWidget*          lvDisplayValues;

  iMethodButtonMgr*     meth_but_mgr;
  QWidget*          widCmdButtons;

  BrainView*            bv();

  void              ColorScaleFromData();
  virtual void          GetVars();
  virtual void          InitPanel();

  BrainViewPanel(BrainView* dv_);
  ~BrainViewPanel();

public: // IDataLinkClient interface
  override void*        This();
  override TypeDef*     GetTypeDef() const;

protected:
  int                   cmd_x; // current coords of where to place next button/ctrl
  int                   cmd_y;
  bool                  req_full_render; // when updating, call Render on netview
  bool                  req_full_build;  // when updating, call Build on netview
  override void         UpdatePanel_impl();
  override void         GetValue_impl();
  override void         CopyFrom_impl();

public slots:
  void                  viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void                  dynbuttonActivated(int but_no); // for hot member buttons
  void          UpdateViewFromState(int state);

#ifndef __MAKETA__
signals:
  void          nameChanged(const QString& name);
  void          dimensionsChanged(TDCoord& d);
  void          viewPlaneChanged(int p);
  void          numSlicesChanged(int nSlices);
  void          sliceStartChanged(int start);
  void          sliceEndChanged(int end);
  void          sliceSpacingChanged(int spacing);
  void          sliceTransparencyChanged(int pctTrans);
  void          actsTransparencyChanged(int pctTrans);
#endif
  
protected slots:
  void			butScaleDefault_pressed();
  void 			butSetColor_pressed();
  void			lvDisplayValues_selectionChanged();

private:
  QComboBox*        view_plane_comb_;
  QSpinBox*         slice_strt_sbox_;
  QSlider*          slice_strt_slid_;
  QCheckBox*        lock_slices_chbox_;
  QSpinBox*         slice_end_sbox_;
  QSlider*          slice_end_slid_;
//  QSpinBox*         slice_spac_sbox_;
//  QSlider*          slice_spac_slid_;
  QSpinBox*         unit_val_tran_sbox_;
  QSlider*          unit_val_tran_slid_;
  QSpinBox*         slice_trans_sbox_;
  QSlider*          slice_tran_slid_;
  QString           name;
  void              UpdateWidgetLimits();

};

#endif // BRAIN_VIEW_PANEL_H

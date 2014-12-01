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

#include "iDialogKeyBindings.h"

#include <taMisc>
#include <taiMisc>
#include <iMainWindowViewer>
#include <EnumSpace>
#include <TypeDef>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QGroupBox>

iDialogKeyBindings* iDialogKeyBindings::New(iMainWindowViewer* par_window_)
{
  iDialogKeyBindings* rval = new iDialogKeyBindings(par_window_);
//  wflg &= ~Qt::WindowStaysOnTopHint;
//  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iDialogKeyBindings::iDialogKeyBindings(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(true);
  setWindowTitle("Key Binding Preferences");
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

iDialogKeyBindings::iDialogKeyBindings() {
}

void iDialogKeyBindings::Constr() {
  QGroupBox* header_box = new QGroupBox("");
  QGroupBox* body_box = new QGroupBox("");
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  
  // two boxes
  layOuter->addWidget(header_box);
  layOuter->addWidget(body_box);
  
  // instruction box
  QHBoxLayout* header_layout = new QHBoxLayout();
  String instruction_str = "Under Construction - not functional yet\n\nSteps: 1) Select 2) Press a key or key combination 3) Tab out";
  QLabel* instruction = new QLabel(instruction_str);
  header_layout->addWidget(instruction);
  header_box->setLayout(header_layout);
  
  // set key bindings box - broken down by tabs
  QHBoxLayout* body_layout = new QHBoxLayout();
  QTabWidget* tabWidget = new QTabWidget;
  body_layout->addWidget(tabWidget);
  body_box->setLayout(body_layout);
  
  String context_label;
  String action_label;
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int i=0; i<context_count-1; i++) {
    taiMisc::BindingContext current_context = taiMisc::BindingContext(i);
    context_label = TA_taiMisc.GetEnumString("BindingContext", i);
    context_label = context_label.before("_CONTEXT");  // strip off "_CONTEXT"
    QWidget* some_tab = new QWidget();
    tabWidget->addTab(some_tab, context_label);
    body_layout->QLayout::addWidget(tabWidget);
    
    // add all of the actions that can be bound to keys
    QFormLayout* bindings_layout = new QFormLayout;
    bindings_layout->setLabelAlignment(Qt::AlignLeft);
    some_tab  ->setLayout(bindings_layout);
    
    QKeySequenceEdit* edit;
    int action_count = static_cast<int>(taiMisc::ACTION_COUNT);
    for (int i=0; i<action_count-1; i++) {
      action_label = TA_taiMisc.GetEnumString("BoundAction", i);
      if (action_label.startsWith(context_label)) {
        action_label = action_label.after(context_label + "_");
        QLabel* action = new QLabel(action_label);
        taiMisc::BoundAction current_action = taiMisc::BoundAction(i);
        QKeySequence key_seq = taiMisc::GetSequenceFromAction(current_context, current_action);
        QKeySequenceEdit* edit = new QKeySequenceEdit(key_seq);
        bindings_layout->addRow(action, edit);
      }
    }
  }
  
  QHBoxLayout* button_layout = new QHBoxLayout();
  button_layout->addStretch();
  button_ok = new QPushButton("&Ok", this);
  button_ok->setDefault(true);
  button_layout->addWidget(button_ok);
  button_layout->addSpacing(taiM->vsep_c);
  button_revert = new QPushButton("&Revert", this);
  button_layout->addWidget(button_revert);
  layOuter->addLayout(button_layout);

  connect(button_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(button_revert, SIGNAL(clicked()), this, SLOT(reject()) );
}

void iDialogKeyBindings::accept() {
  inherited::accept();
  // here is where we execute actions!
}

void iDialogKeyBindings::reject() {
  inherited::reject();
}
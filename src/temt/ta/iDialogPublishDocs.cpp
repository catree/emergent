// Copyright, 1995-2012, Regents of the University of Colorado,
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

#include "iDialogPublishDocs.h"

#include <taiMisc>

#include <QDialogButtonBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QStatusTipEvent>
#include <QCheckBox>

namespace // anon
{
  // Add a label and a widget to a layout as buddies.
  void addLabeledWidget(QLayout *layout, const QString &labelText, QWidget *widget)
  {
    QLabel *label = new QLabel(labelText);
    label->setBuddy(widget);
    layout->addWidget(label);
    layout->addWidget(widget);
  }

  // Create and return a new hbox inside the given vbox.
  QHBoxLayout * newHBox(QVBoxLayout *vbox)
  {
    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    return hbox;
  }
}

iDialogPublishDocs::iDialogPublishDocs(const char *repositoryName)
//  : inherited(), nameEdit(0), descEdit(0), tagsEdit(0), statusBar(0), upload_project(0)
: inherited()
{
  // Size the dialog.
  resize(taiM->dialogSize(taiMisc::dlgSmall | taiMisc::dlgHor));
  setFont(taiM->dialogFont(taiM->ctrl_size));

  // Dialog title.
  QString title("Publish project to: ");
  title.append(repositoryName);
  setWindowTitle(title);

  // Create a marginless layout to hold the status bar,
  // plus an inner layout (with margins) to hold everything else.
  QVBoxLayout *topVbox = new QVBoxLayout(this);
  QVBoxLayout *vbox = new QVBoxLayout; // inner
#if (QT_VERSION >= 0x040600) // QMargins didn't exist until Qt 4.6
  vbox->setContentsMargins(topVbox->contentsMargins());
  topVbox->setContentsMargins(0, 0, 0, 0);
#endif
  topVbox->addLayout(vbox);

  // Add a status bar for instructions.
  statusBar = new QStatusBar;
  topVbox->addWidget(statusBar);

  // All other widgets get added to the inner vbox.
  // Project name
  nameEdit = new QLineEdit;
  nameEdit->setStatusTip("Instructions: Enter a human-readable name for the project (used for wiki page name)");
  nameEdit->installEventFilter(this);
  
  QHBoxLayout* project_box = newHBox(vbox);
  addLabeledWidget(project_box, "Project &name:", nameEdit);

  // upload project - do it now - default is true
  upload_project = new QCheckBox;
  upload_project->setChecked(true);
  upload_project->setStatusTip("You can upload the project when you publish or just create the wiki page and later upload the project. You can always upload a new version of the project");
  addLabeledWidget(project_box, "Upload Project File", upload_project);

  // Description
  descEdit = new QTextEdit;
  descEdit->setTabChangesFocus(true);
  descEdit->setStatusTip("Instructions: Enter a brief description of the project (more detail can be added later on the wiki)");
  descEdit->installEventFilter(this);
  addLabeledWidget(vbox, "&Description:", descEdit);

  // Tags
  tagsEdit = new QLineEdit;
  tagsEdit->setStatusTip("Instructions: Enter categories relevant to this project (space spearated)");
  tagsEdit->installEventFilter(this);
  addLabeledWidget(newHBox(vbox), "&Categories:", tagsEdit);
  
  
  // OK, Cancel buttons
  QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  vbox->addWidget(buttonBox);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString iDialogPublishDocs::GetName() const
{
  return nameEdit->text();
}

QString iDialogPublishDocs::GetDesc() const
{
  return descEdit->toPlainText();
}

QString iDialogPublishDocs::GetTags() const
{
  QString tags = tagsEdit->text();
//  return tags.split(QRegExp("[,\\s]+"), QString::SkipEmptyParts);
  return tags;
}

bool iDialogPublishDocs::GetUploadChoice() const
{
  return upload_project->isChecked();
}

bool iDialogPublishDocs::event(QEvent *event)
{
  // When a status tip event occurs (e.g., from mouseover), display the tip.
  if (event->type() == QEvent::StatusTip) {
    if (QStatusTipEvent *stev = dynamic_cast<QStatusTipEvent *>(event)) {
      statusBar->showMessage(stev->tip());
      return true;
    }
  }

  // Other events, let the parent handle them.
  return inherited::event(event);
}

bool iDialogPublishDocs::eventFilter(QObject *obj, QEvent *event)
{
  // When a widget (obj) comes into focus, display its status tip.
  if (event->type() == QEvent::FocusIn) {
    if (QWidget *widget = dynamic_cast<QWidget *>(obj)) {
      statusBar->showMessage(widget->statusTip());
    }
  }
  else if (event->type() == QEvent::FocusOut) {
    statusBar->clearMessage();
  }

  // Always return false, so the event will get processed further.
  return false;
}

void iDialogPublishDocs::SetName(const QString& name) {
  nameEdit->setText(name);
}

void iDialogPublishDocs::SetDesc(const QString& desc) {
  descEdit->setText(desc);
}

void iDialogPublishDocs::SetTags(const QString& tags) {
  tagsEdit->setText(tags);
}

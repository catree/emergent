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

#include "iEditGrid.h"

#include <iStripeWidget>
#include <iScrollArea>

#include <qlayout.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
#include <QScrollArea>
#include <QScrollBar>

#define GROUP_FRAME_SIZE 2

/*NOTE: to insure proper visual appearance when an hscrollbar appears in the data area, we always
  have one extra empty dummy name slot at the end -- this pushes the entire structure down, and
  therefore the last actual data line doesn't get clipped */

iEditGrid::iEditGrid (bool header_, QWidget* parent)
: QWidget(parent)
{
  init(header_, 2, 1, 1, 1);
}

iEditGrid::iEditGrid (bool header_, int hmargin_, int vmargin_, QWidget* parent)
: QWidget(parent)
{
  init(header_, hmargin_, vmargin_, 1, 1);
}

iEditGrid::iEditGrid (bool header_, int hmargin_, int vmargin_, int rows_, 
  int cols_, QWidget* parent)
: QWidget(parent)
{
  init(header_, hmargin_, vmargin_, rows_, cols_);
}

iEditGrid::iEditGrid (bool names_, bool header_, int hmargin_, int vmargin_, int rows_, 
  int cols_, QWidget* parent)
: QWidget(parent)
{
  init(header_, hmargin_, vmargin_, rows_, cols_, names_);
}

void iEditGrid::init(bool header_, int hmargin_, int vmargin_,
  int rows_, int cols_, bool names_) 
{
  mnames = names_;
  mhead = (header_) ? 1 : 0;
  if (hmargin_ < 0) hmargin_ = 0;
  if (vmargin_ < 0) vmargin_ = 0;
  mhmargin = hmargin_;
  mvmargin = vmargin_;
  if (rows_ < 1) rows_ = 1;
  if (cols_ < 1) cols_ = 1;
  mrows = rows_;
  mmin_vis_rows = MAX(2, mrows);
  mcols = cols_;
  mrow_height = 25; //should set later
  if (!mnames) {
    bodyNames = NULL;
    layNames = NULL;
  }
  createContent();
  adjustMinHeight();
}

void iEditGrid::adjustMinHeight() {
  int min_rows = MAX(mmin_vis_rows, mrows);
  /*obs provide room for scrollbar
  int new_body_ht = ((min_rows + mhead) * (mrow_height + (2 * mvmargin))) + 
    (((mrow_height + (2 * mvmargin)) * 2) / 3);*/
  //note: we use iScrollbar which always provides room for sb area
  int new_body_ht = ((min_rows + mhead) * (mrow_height + (2 * mvmargin)));
  body->setMinimumHeight(new_body_ht);
  scrBody->setMinimumHeight(new_body_ht + scrBody->horizontalScrollBar()->height() + 2);
  this->setMinimumHeight(scrBody->minimumHeight() + 2);
}

void iEditGrid::createContent() {

  layOuter = new QHBoxLayout(this);
  layOuter->setMargin(0);
  layOuter->setSpacing(0);
//  layNamesOuter = new QVBoxLayout();
//  layOuter->addLayout(layNamesOuter);
  QVBoxLayout* vbl;
  if (mnames) {
    bodyNames = new iStripeWidget(this);
    vbl = new QVBoxLayout(bodyNames);
    vbl->setMargin(0); // shift for scrollarea frame size
    if (mhead > 0) 
      vbl->addSpacing(mrow_height + GROUP_FRAME_SIZE); // also added to stripe widget's top height
    else
      vbl->addSpacing(GROUP_FRAME_SIZE); // also added to stripe widget's top height
    layNames = new QGridLayout(); 
    layNames->setSpacing(0);
    layNames->setMargin(0);
    vbl->addLayout(layNames);
    vbl->addStretch();
    layOuter->addWidget(bodyNames);
  }
//nn  layOuter->addSpacing(mhmargin);
  scrBody = new iScrollArea(this);
//  scrBody->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  
  scrBody->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // use outer container for scrolling, to keep names in sync
  scrBody->setWidgetResizable(true);
  body = new iStripeWidget();		// parent for the data items
//  body->resize(1, 1); // let it expand
  scrBody->setWidget(body);

  vbl = new QVBoxLayout(body);
  vbl->setMargin(0);
  layBody = new QGridLayout(); //note: vmargin passed for "spacing", applies to both dims
  layBody->setSpacing(0);
  layBody->setMargin(0);
  vbl->addLayout(layBody);
  vbl->addStretch();
  layOuter->addWidget(scrBody); 
  setRowHeight(mrow_height, true); // force it
  resizeRows_impl();
}

void iEditGrid::checkSetParent(QWidget* widget, QWidget* parent) {
  if (widget->parent() != parent) {
//Qt3    widget->reparent(parent, QPoint(0,0)); //get's moved anyway, based on layout
    widget->setParent(parent); //get's moved anyway, based on layout
  }
}

void iEditGrid::clearLater() { // clears all contained items, but does it via deleteLater, not delete
  mrows = 1;
  mcols = 1;
  scrBody->setParent(NULL);
  scrBody->deleteLater();
  if (mnames) {
    bodyNames->setParent(NULL);
    bodyNames->deleteLater();
  //  layOuter->setParent((QObject*)NULL);
  //  layOuter->deleteLater();
  }
  delete layOuter;
  createContent();
  adjustMinHeight();
  setColors(mhilightColor, palette().color(this->backgroundRole()));
}

void iEditGrid::resizeRows_impl() {
  int ht = mrow_height + (2 * mvmargin);
  for (int i = 0; i < (mrows + mhead); ++i) {
    if (mnames && (i < mrows)) {
      layNames->setRowMinimumHeight(i, ht);
    }
    layBody->setRowMinimumHeight(i,  ht);
  }
}

void iEditGrid::setColNameWidget(int col, QWidget* name) {
  checkSetParent(name, (QWidget*)body);
  layBody->addWidget(name, 0, col, (Qt::AlignCenter | Qt::AlignVCenter));
  updateDimensions(-1, col);
}

void iEditGrid::setColors(const QColor& hilight, const QColor& bg) {
  QPalette palette;
  palette.setColor(this->backgroundRole(), bg);
  this->setPalette(palette);
  mhilightColor = hilight; // cached for rebuild
  if (mnames) {
    bodyNames->setColors(hilight, bg);
  }
  body->setColors(hilight, bg);
}

void iEditGrid::setDataWidget(int row, int col, QWidget* data) {
  checkSetParent(data, (QWidget*)body);
  layBody->setRowMinimumHeight(row + mhead,  mrow_height + (2 * mvmargin));
  layBody->addItem(new QSpacerItem(mhmargin, 0, QSizePolicy::Fixed,
    QSizePolicy::Minimum), row + mhead, col);
  layBody->addWidget(data, row + mhead, col, (Qt::AlignLeft | Qt::AlignVCenter));
  layBody->addItem(new QSpacerItem(mhmargin, 0, QSizePolicy::Fixed,
    QSizePolicy::Minimum), row + mhead, col);
  updateDimensions(row, col);
}

void iEditGrid::setDataLayout(int row, int col, QLayout* data) {
  layBody->setRowMinimumHeight(row + mhead,  mrow_height + (2 * mvmargin));
  layBody->addLayout(data, row + mhead, col);
//  resizeRows_impl();
  updateDimensions(row, -1);
}

void iEditGrid::setDimensions(int rows_, int cols_) {
  if ((rows_ == mrows) && (cols_ == mcols)) return;
  //note: layouts won't shrink, only expand
  mrows = MAX(mrows, rows_);
  mcols = MAX(mcols, cols_);
  resizeRows_impl();
}

void iEditGrid::setMinVisibleRows(int value) {
  if (value < 0) value = 0;
  if (mmin_vis_rows == value) return;
  mmin_vis_rows = value;
  adjustMinHeight();
}

void iEditGrid::setHiLightColor(const QColor& val) {
  mhilightColor = val; // cached for rebuild
  if (mnames) {
    bodyNames->setHiLightColor(val);
  }
  body->setHiLightColor(val);
}

void iEditGrid::setRowHeight(int value, bool force) {
  if ((mrow_height == value) && (!force)) return;
  mrow_height = value;
  int sh = value + (2 * mvmargin);
  // if using headers, we set a top margin for stripe same as header line
  int tm = (hasHeader()) ? sh : 0;
  if (mnames) {
    bodyNames->setStripeHeight(sh);
    bodyNames->setTopMargin(tm + GROUP_FRAME_SIZE); //+ for frame border of group items
  }
  body->setStripeHeight(sh);
  body->setTopMargin(tm);
  adjustMinHeight();
  resizeRows_impl();
}

void iEditGrid::setPaletteBackgroundColor3 (const QColor& c) {
  QPalette pal(palette());
  pal.setColor(QPalette::Background,c); 
  setPalette(pal);
  pal = body->palette();
  pal.setColor(QPalette::Background,c); 
  body->setPalette(pal); //note: may not be necessary
  pal = scrBody->viewport()->palette();
  pal.setColor(QPalette::Background,c); 
  scrBody->viewport()->setPalette(pal);
}

void iEditGrid::setRowNameWidget(int row, QWidget* name) {
  if (!mnames) return; // shouldn't be calling this!!!
  //NOTE: there is no heading row in the body widget, so row=row (no mhead adjust)
  checkSetParent(name, (QWidget*)bodyNames);
  layNames->setRowMinimumHeight(row,  mrow_height + (2 * mvmargin));
  layNames->addItem(new QSpacerItem(mhmargin, mrow_height + (2 * mvmargin), 
    QSizePolicy::Fixed, QSizePolicy::Fixed), row, 0); 

  layNames->addWidget(name, row, 0, (Qt::AlignLeft | Qt::AlignVCenter));
  layNames->addItem(new QSpacerItem(mhmargin, mrow_height + (2 * mvmargin),
    QSizePolicy::Fixed, QSizePolicy::Fixed), row, 0);
  updateDimensions(row, -1);
}

void iEditGrid::updateDimensions(int row, int col) {
  // note: we get passed ordinals, not counts, -1 ignores that dim
  if (row >= mrows) {
    mrows = row + 1;
    // make sure the n+1 row is valid so we have room for the bottom scroll bar
    // need to do both, otherwise one will try to squish
    // setting rowMinimumHeight didn't work -- so what we do, is make an invisible
    // layout item, 4/5 the row height, and add it in -- it is harmless later
    // GD thing won't later size to larger!!! int strut = (mrow_height * 4) / 5;
/*todo    int strut = mrow_height + (2 * mvmargin);
    if (mnames) {
      layNames->addItem(
        new QSpacerItem(0, strut, QSizePolicy::Fixed, QSizePolicy::Minimum),
        row + mhead + 1, 0); 
    }
    layBody->addItem(
        new QSpacerItem(0, strut, QSizePolicy::Fixed, QSizePolicy::Minimum),
        row + mhead + 1, 0); 
  */
//TODO: when added, then all the rows get squished , when not, then no room for the scrollbar!
/*    layNames->setRowMinimumHeight(row + mhead + 1,  mrow_height + (2 * mvmargin));
    layBody->setRowMinimumHeight(row + mhead + 1,  mrow_height + (2 * mvmargin)); */
  }
  if (col >= mcols) mcols = col - 1;
  adjustMinHeight();
}

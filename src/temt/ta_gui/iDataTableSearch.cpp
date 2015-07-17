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

#include "iDataTableSearch.h"
#include <iLineEdit>
#include <taString>
#include <DataTable>
#include <iDataTableView>
#include <iDataTableModel>
#include <iActionMenuButton>
#include <iMenuButton>

#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QToolButton>


iDataTableSearch::iDataTableSearch(QWidget* parent) : QWidget(parent) {
  table_view = NULL;
  table_model = NULL;
  Constr();
}

iDataTableSearch::iDataTableSearch(iDataTableView* table_view_, QWidget* parent)  : QWidget(parent) {
  table_view = table_view_;
  table_model = NULL;
  Constr();
}

iDataTableSearch::~iDataTableSearch() {
}

void iDataTableSearch::Constr() {
  search_mode = CONTAINS;  //  default
  taiMisc::SizeSpec currentSizeSpec = taiM->GetCurrentSizeSpec();

  QHBoxLayout* lay = new QHBoxLayout(this);
  srch_bar = new QToolBar(this);
  lay->addWidget(srch_bar);
  lay->setMargin(0);
  lay->setSpacing(0);
  
  srch_mode_button = new iActionMenuButton();
  srch_bar->addWidget(srch_mode_button);
  
  contains_action = new QAction("Find", this);
  matches_action = new QAction("Find Matching", this);

  srch_mode_menu = new QMenu(this);
  srch_mode_button->setMenu(srch_mode_menu);
  srch_mode_button->setDefaultAction(contains_action);
  srch_mode_button->setFont(taiM->nameFont(currentSizeSpec));
  srch_mode_button->setToolTip(taiMisc::ToolTipPreProcess("Find cells within the above data table: always case insensitive"));

  srch_text = new iLineEdit();
  srch_bar->addWidget(srch_text);
  
  srch_nfound = new QLabel(" 0");
  srch_nfound->setFont(taiM->nameFont(currentSizeSpec));
  srch_nfound->setToolTip(taiMisc::ToolTipPreProcess("Number of items found"));
  srch_bar->addWidget(srch_nfound);
  
  srch_clear = srch_bar->addAction("x");
  srch_clear->setToolTip(taiMisc::ToolTipPreProcess("Clear search text and cell highlighting"));
  srch_prev = srch_bar->addAction("<");
  srch_prev->setToolTip(taiMisc::ToolTipPreProcess("Find previous occurrence of find text within table"));
  srch_next = srch_bar->addAction(">");
  srch_next->setToolTip(taiMisc::ToolTipPreProcess("Find next occurrence of find text within table"));
  
  srch_mode_menu->addAction(contains_action);
  srch_mode_menu->addAction(matches_action);

  connect(srch_clear, SIGNAL(triggered()), this, SLOT(SearchClear()));
  connect(srch_next, SIGNAL(triggered()), this, SLOT(SelectNext()));
  connect(srch_prev, SIGNAL(triggered()), this, SLOT(SelectPrevious()));
  connect(srch_text, SIGNAL(returnPressed()), this, SLOT(TextEntered()));
  connect(contains_action, SIGNAL(triggered()), this, SLOT(ContainsSelected()));
  connect(matches_action, SIGNAL(triggered()), this, SLOT(MatchesSelected()));
}

void iDataTableSearch::TextEntered() {
  Search(search_mode);
}

void iDataTableSearch::Search(iDataTableSearch::SearchMode mode) {
  if (!table_view) {
    return;
  }
  if (!table_model) {
      table_model = table_view->dataTable()->GetTableModel();
  }
  if (!table_model) {
    return;
  }
  
  // clear
  table_view->dataTable()->DataUpdate(true);
  table_model->ClearFoundList();
  table_view->dataTable()->DataUpdate(false);

  // start next search
  found_list = new taVector2i_List();
  bool contains = (search_mode == CONTAINS);
  table_view->dataTable()->FindAllScalar(found_list, srch_text->text(), contains);

  table_view->dataTable()->DataUpdate(true);
  for (int i=0; i<found_list->size; i++) {
    table_model->AddToFoundList(found_list->FastEl(i)->x, found_list->FastEl(i)->y);
  }
  table_view->dataTable()->DataUpdate(false);
  srch_nfound->setText(String((int)found_list->size));
  SelectNext();
  
  // cleanup
  found_list = NULL;
  delete found_list;
}

void iDataTableSearch::SelectNext() {
  if (table_model == NULL) {
    return;
  }
  const QModelIndex* model_index = table_model->GetNextFound();
  if (model_index) {
    table_view->SetCurrentAndSelect(model_index->row(), model_index->column());
  }
}

void iDataTableSearch::SelectPrevious() {
  if (table_model == NULL) {
    return;
  }
  const QModelIndex* model_index = table_model->GetPreviousFound();
  if (model_index) {
    table_view->SetCurrentAndSelect(model_index->row(), model_index->column());
  }
}

void iDataTableSearch::SearchClear() {
  if (table_model == NULL) {
    return;
  }
  table_view->dataTable()->DataUpdate(true);
  table_model->ClearFoundList();
  srch_nfound->setText("0");
  srch_text->setText("");
  table_view->dataTable()->DataUpdate(false);
}

void iDataTableSearch::ContainsSelected() {
  search_mode = CONTAINS;
}

void iDataTableSearch::MatchesSelected() {
  search_mode = MATCHES;
}

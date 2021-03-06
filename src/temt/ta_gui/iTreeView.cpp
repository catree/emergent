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

#include "iTreeView.h"

#include <taRootBase>
#include <iMainWindowViewer>
#include <String_PArray>
#include <iTreeViewItem>
#include <taSigLinkItr>
#include <taiSigLink>
#include <taiWidgetTypeDefChooser>
#include <taProject>
#include <Program>
#include <iClipData>
#include <taiObjectMimeFactory>
#include <iBrowseHistory>
#include <iVec2i>
#include <iBrowseViewer>
#include <iProgramEditor>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>
#include <tabMisc>
#include <taRootBase>
#include <BrowseViewer>
#include <PanelViewer>

#include <QHeaderView>
#include <QTimer>
#include <QApplication>
#include <QScrollBar>
#include <QKeyEvent>

const String iTreeView::opt_treefilt("TREEFILT_");

void iTreeView::FillTypedList(const QList<QTreeWidgetItem*>& items,
  ISelectable_PtrList& list)
{
  for (int i = 0; i < items.count(); ++i) {
    ISelectable* tvi = dynamic_cast<iTreeViewItem*>(items.at(i));
    if (tvi)
      list.Add(tvi);
  }
}

/*  TEMP: good colors for red highlighting of errors
  QColor h_base(0xFF, 0x99, 0x99); // pale dull red
  QColor h_rev(0x99, 0x33, 0x33); // dark dull red
*/
iTreeView::iTreeView(QWidget* parent, int tv_flags_)
:inherited(parent)
{
  possibleDropTargetItem = NULL;
  focus_next_widget = NULL;
  focus_prev_widget = NULL;
  main_window = NULL;
  parent_type = TYPE_NULL;

  setFont(taiM->dialogFont(taiM->sizBig));
  QFont cur_font = QFont();
  if(dynamic_cast<iBrowseViewer*>(parent)) {
    parent_type = TYPE_BROWSEVIEWER;
    cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
  else if(dynamic_cast<iProgramEditor*>(parent)) {
    parent_type = TYPE_PROGRAMEDITOR;
    cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
  
  tv_flags = tv_flags_;
  m_filters = NULL; // only created if needed
  m_def_exp_levels = 2; // works well for most contexts
  m_show = (TypeItem::ShowMembs)(TypeItem::USE_SHOW_GUI_DEF | taMisc::show_gui);
  m_decorate_enabled = true;
  italic_font = NULL;
  in_mouse_press = 0;
  m_saved_scroll_pos = 0;
  setIndentation(taMisc::tree_indent);
  
  // set default 'invalid' highlight colors, but don't enable highlighting by default
  setHighlightColor(1,
    QColor(0xFF, 0x99, 0x99),  // pale dull red
    QColor(0x99, 0x33, 0x33) // dark dull red
  );
  setHighlightColor(2,
    QColor(0xFF, 0xFF, 0x99),  // pale dull yellow
    QColor(0x99, 0x99, 0x33) // dark dull yellow
  );
  setHighlightColor(3,          // disabled
    QColor(0xa0, 0xa0, 0xa0),  // light grey
    QColor(0x80, 0x80, 0x80) // medium grey
  );
  
  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
          this, SLOT(this_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );
  //  connect(this, SIGNAL(itemSelectionChanged()),
  //    this, SLOT(this_itemSelectionChanged()) );
  //note: can't use "activate" because that is only for ex. double-clicking the item
  // can't use "pressed" because that solves problem, but then screws up drags, etc.
  connect(this, SIGNAL(clicked(const QModelIndex&)),
    this, SLOT(this_itemSelectionChanged()) );
  connect(this, SIGNAL(contextMenuRequested(QTreeWidgetItem*, const QPoint&, int)),
    this, SLOT(this_contextMenuRequested(QTreeWidgetItem*, const QPoint&, int)) );
}

iTreeView::~iTreeView() {
  if (m_filters) {
    delete m_filters;
    m_filters = NULL;
  }
  if (italic_font) {
    delete italic_font;
    italic_font = NULL;
  }
}

QMap_qstr_qvar iTreeView::colDataKeys(int col) const {
  QMap_qstr_qvar map;
  if ((col >= 0) || (col < columnCount())) {
    QVariant vmap = headerItem()->data(col, ColDataRole);
    if (vmap.canConvert(QVariant::Map)) {
      map = vmap.toMap();
    }
  }
  return map;
}

void iTreeView::AddColDataKey(int col, const KeyString& key, int role) {
//NOTE: the role is the map key
  if ((col < 0) || (col >= columnCount())) return;
  QMap_qstr_qvar map;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  if (vmap.canConvert(QVariant::Map)) {
    map = vmap.toMap();
  }
  // note: ok to call multiple times; only sets once
  map[QString::number(role)] = key;
  headerItem()->setData(col, ColDataRole, map);
}

bool iTreeView::RemoveColDataKey(int col, const KeyString& key, int role) {
  if ((col < 0) || (col >= columnCount())) return false;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  // if no map at all, then the key itself is definitely not set
  if (!vmap.canConvert(QVariant::Map)) return false;

  QMap_qstr_qvar map(vmap.toMap());
  bool rval = (map.remove(QString::number(role)) > 0);
  if (rval) // only need to re-set if it was actually removed
    headerItem()->setData(col, ColDataRole, map);
  return rval;
}

void iTreeView::ClearColDataKeys(int col) {
  if ((col < 0) || (col >= columnCount())) return;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  if (!vmap.canConvert(QVariant::Map)) return;

  headerItem()->setData(col, ColDataRole, QVariant());
}

void iTreeView::AddFilter(const String& value) {
  if (!m_filters) {
    m_filters = new String_PArray;
  }
  m_filters->AddUnique(value);
}

iTreeViewItem* iTreeView::AssertItem(taiSigLink* link, bool super) {
  // first, check if already an item in our tree
  taSigLinkItr itr;
  iTreeViewItem* el;
  FOR_DLC_EL_OF_TYPE(iTreeViewItem, el, link, itr) {
    if (el->treeWidget() == this) {
      return el;
    }
  }
  if (!super) return NULL; // when we are called by ourself
  // failed, so try to assert the owner
  taiSigLink* own_link = link->ownLink();
  if (!own_link) return NULL;
  iTreeViewItem* own_el = AssertItem(own_link);
  // note: don't bial if no own_el, because could be a defchild parent
  // then try making sure owner's children asserted
  if (own_el) { // && own_el->lazyChildren()) {
    own_el->CreateChildren();
    TypeDef* td = link->taData()->GetTypeDef();
//    if (!td->HasOption("NO_EXPAND_ON_PANEL_VIEW")) {
    if (!link->taData()->InheritsFrom(&TA_taDataView)) {
      own_el->setExpanded(true);
    }
    taiMisc::ProcessEvents();
  }
  // and try again, but not supercursively of course!
  return AssertItem(link, false);
}

iTreeViewItem* iTreeView::PrevItem(iTreeViewItem* itm) {
  QTreeWidgetItemIterator it(this);
  QTreeWidgetItem* item_;
  iTreeViewItem* rval = NULL;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if(item == itm) {
      return rval;
    }
    rval = item;
  }
  return NULL;
}

void iTreeView::CollapseAll() {
  for (int i = topLevelItemCount() - 1; i >= 0; --i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(i));
    if (node)
      CollapseAllUnder(node);
  }
}

void iTreeView::CollapseAllUnder(iTreeViewItem* item) {
  //note: iterator didn't work, because it collapsed the siblings too!
  if (!item) return;
  taMisc::Busy(true); //note: should have ample count capacity for levels
  // first, the children (if any)...
  for (int i = item->childCount() - 1; i >=0 ; --i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(item->child(i));
    if (node)
      CollapseAllUnder(node);
  }
  // then ourself
  item->setExpanded(false);
  taMisc::Busy(false);
}

void iTreeView::CollapseAllUnderInt(void* item) {
  CollapseAllUnder((iTreeViewItem*)item);
}


void iTreeView::InsertEl(bool after) {
  ISelectable* si = curItem();
  if(!si || !si->link()) return;                // nothing selected
  taBase* sb = si->link()->taData();
  if(!sb) return;
  taList_impl* sbo = NULL;
  if(sb->InheritsFrom(&TA_taList_impl)) {
    sbo = (taList_impl*)sb;
  }
  else {
    sbo = GET_OWNER(sb, taList_impl);
  }
  if(!sbo) return;
  if(sbo->HasOption("FIXED_SIZE")) return; // cannot manipulate in gui
  taiWidgetTypeDefChooser* typlkup =
    new taiWidgetTypeDefChooser(sbo->el_base, NULL, NULL, NULL, taiWidget::flgAutoApply);
  TypeDef* td = sbo->el_typ;
  typlkup->GetImage(td, sbo->el_base);
  bool okc = false;
  if(typlkup->hasOnlyOneItem()) {
    okc = true;
  }
  else {
    okc = typlkup->OpenChooser();
    td = typlkup->td();
  }
  if(okc && td) {
    taProject* proj = myProject();
    if(proj) {
      proj->undo_mgr.SaveUndo(sbo, "InsertEl", NULL, false, sbo); // global save
    }
    taBase* nwi = taBase::MakeToken(td);
    if(nwi) {
      int idx = 0;
      if(sbo == sb) {           // it is the list
        idx = sbo->size;
      }
      else {
        idx = sbo->FindEl(sb);
        if(after) idx++;
      }
      if(idx < 0) idx = 0;
      if(idx > sbo->size) idx = sbo->size;
      sbo->Insert(nwi, idx);
      // nwi->UpdateAfterEdit();
      selectionModel()->clearSelection(); // force clear!
      tabMisc::DelayedFunCall_gui(nwi, "BrowserExpandAll");
      tabMisc::DelayedFunCall_gui(nwi, "BrowserSelectMe");
    }
  }
  delete typlkup;
}

void iTreeView::InsertDefaultEl(bool after) {
  ISelectable* si = curItem();
  if(!si || !si->link())
    return;                // nothing selected
  taBase* sb = si->link()->taData();
  if(!sb) return;
  taList_impl* sbo = NULL;
  if(sb->InheritsFrom(&TA_taList_impl)) {
    sbo = (taList_impl*)sb;
  }
  else {  // not a list
    String mbr = sb->GetTypeDef()->OptionAfter("DEF_CHILD_");
    if(mbr.nonempty()) {
      MemberDef* md = sb->FindMember(mbr);
      if(md) {   // should always be true
        sbo = (taList_impl*)md->GetOff(sb);
        if(sbo->size > 0 || sbo->HasOption("FIXED_SIZE")) {     // only select def child if nothing in it yet -- otherwise you should be inside list to add
          sbo = GET_OWNER(sb, taList_impl);
        }
      }
    }
    else {
      sbo = GET_OWNER(sb, taList_impl);
    }
  }
  if(!sbo) return;
  if (sbo->HasOption("NO_DEFAULT_CHILD")) return;  // used by ProgBrkPts - only allow creation through a ProgEl
  if(sbo->HasOption("FIXED_SIZE")) return; // cannot manipulate in gui
  TypeDef* td = sbo->el_typ;    // default type

  if(td) {
    taProject* proj = myProject();
    if(proj) {
      proj->undo_mgr.SaveUndo(sbo, "InsertDefaultEl", NULL, false, sbo); // global save
    }

    taBase* nwi = taBase::MakeToken(td);

    if(nwi) {
      int idx = 0;
      if(sbo == sb) {           // it is the list
        idx = sbo->size;
      }
      else {
        idx = sbo->FindEl(sb);
        if(after) idx++;
      }
      if(idx < 0) idx = 0;
      if(idx > sbo->size) idx = sbo->size;
      sbo->Insert(nwi, idx);
      // nwi->UpdateAfterEdit();
      selectionModel()->clearSelection(); // force clear!
      tabMisc::DelayedFunCall_gui(nwi, "BrowserSelectMe");
      tabMisc::DelayedFunCall_gui(nwi, "BrowserExpandAll");
    }
  }
}

const KeyString iTreeView::colKey(int col) const {
  if ((col < 0) || (col >= columnCount())) return _nilKeyString;
  KeyString rval = (headerItem()->data(col, ColKeyRole)).toString();
  return rval;
}

void iTreeView::ExpandAll(int max_levels) {
  ExpandAll_impl(max_levels);
}

void iTreeView::ExpandAll_impl(int max_levels, int exp_flags) {
  // NOTE: we can't user iterators for expanding, because we add/remove items which
  // crashes the iterator
  for (int i = 0; i < topLevelItemCount(); ++i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(i));
    if (!node) continue;
    taBase* tab = node->link()->taData();
    ExpandItem_impl(node, 0, max_levels, exp_flags); // false - this is the root node for this expansion
  }
  if (header()->isVisible() && (header()->count() > 1)) {
    resizeColumnsToContents();
  }
}

void iTreeView::ExpandItem_impl(iTreeViewItem* item, int level,
                                int max_levels, int exp_flags, bool is_subgroup)
{
  if (!item) return;
  if (isItemHidden(item)) return;

  taBase* tab = item->link()->taData();
  
  bool expand = false;
  if (tab->InheritsFrom(&TA_taProject)) { // always expand project
    expand = true;
  }
  
  if (exp_flags & EF_EXPAND_FULLY) {
    expand = true;
  }
  
    // figure out default if not otherwise saved
    if(tab && tab->HasOption("NO_EXPAND_ALL")) return;
    if(item->md() && item->md()->HasOption("NO_EXPAND_ALL")) return;
    
    if (!(exp_flags & EF_CUSTOM_FILTER) && tab && (!(exp_flags & EF_EXPAND_FULLY))) {
      // if top level node or being treated like one - top level guys are docs, ctrl_panels, data, programs, networks, etc
      // those being treated like top level are specific networks (i.e. network -- not an actual group but has spec and layer groups
      if (tab->InheritsFrom(&TA_taGroup_impl) || tab->GetTypeDef()->HasOption("EXPAND_AS_GROUP")) {
        String name;
        if (tab->GetTypeDef()->HasOption("EXPAND_AS_GROUP")) {
          name = tab->GetTypeDef()->OptionAfter("FILETYPE_");  // I didn't want to add another directive for this single case
          name.downcase();
        }
        else {
          name = tab->GetName();
        }
        int depth = taiMisc::GetGroupDefaultExpand(name);
        
        if (depth == 0 && exp_flags & EF_DEFAULT_UNDER) {
          depth = 1;  // if user asked for expansion expand 1 level even when default is zero
        }
        if (depth >= 0) {
          is_subgroup = true;
          max_levels = depth;
          if (max_levels > 0) {
            expand = true;
          }
        }
        else if (!is_subgroup) {  // expand INITIATED on this non top-level group -- thus not being expanded as a subgroup
          String exp_def_str = tab->GetTypeDef()->OptionAfter("EXPAND_DEF_");
          if (exp_def_str.nonempty()) {
            max_levels = (int)exp_def_str;
          }
          else {
            max_levels = 1;
          }
          if (level <= max_levels) {
            expand = true;
          }
          else {
            expand = false;
          }
        }
        else if (level <= max_levels) {  // expand initiated by ancestor group -- just check level
          expand = true;
        }
        else {
          expand = false;  // this level is > max_levels
        }
      }
      else {  // not a top-level group - get class default -- if none set max_levels to 1
        String exp_def_str = tab->GetTypeDef()->OptionAfter("EXPAND_DEF_");
        if (exp_def_str.nonempty()) {
          max_levels = (int)exp_def_str;
        }
        else {
          max_levels = 1;
        }
        
        if (level < max_levels) {
          expand = true;
        }
        else {
          expand = false;
        }
      }
    }
    
    if (!(exp_flags & EF_EXPAND_DISABLED)) {
      if (!item->link()->isEnabled())
        expand = false;
    }
    if (exp_flags & EF_CUSTOM_FILTER) {
      max_levels = 1;  // this gets it open, then custom will take over
      expand = true;
      emit CustomExpandFilter(item, level, expand);
    }
    if (exp_flags & EF_NAVIGATOR_FILTER) {
      max_levels = 1;  // this gets it open, then custom will take over
      expand = true;
      emit CustomExpandNavigatorFilter(item, level, expand);
    }
  
  if (expand) {
    // first expand the guy...
    if (!isItemExpanded(item)) { // ok, eligible...
      item->setExpanded(true);  // should trigger CreateChildren for lazy
    }
    // check if we've expanded deeply enough
    // (works for finite (>=1) and infinite (<0) cases)
    if (max_levels == 0) return;
    
    if (level >= 0) ++level;
    // and expand item's children -- lazy children should be created by now
    for (int i = 0; i < item->childCount(); ++i) {
      iTreeViewItem* child = dynamic_cast<iTreeViewItem*>(item->child(i));
      if (child) {
        // make sure the TREE_EXPANDED flag is cleared
        taBase* child_tab = child->link()->taData();
        ExpandItem_impl(child, level, max_levels, exp_flags, is_subgroup);
      }
    }
  }
  else {
    // note: following test not needed for 1st time, but is
    // needed for subsequent ExpandDefault
    if(!exp_flags & (EF_DEFAULT | EF_CUSTOM_FILTER)) {
      // for auto-expand, do NOT collapse expanded items!
      if (isItemExpanded(item)) {
        item->setExpanded(false);
      }
    }
  }
}

void iTreeView::ExpandAllUnder(iTreeViewItem* item, int max_levels) {
  if (!item) return;
  taMisc::Busy(true);
  ExpandItem_impl(item, -1, max_levels, EF_EXPAND_FULLY);
  if (header()->isVisible() && (header()->count() > 1)) {
    resizeColumnsToContents();
  }
  taMisc::Busy(false);
}

void iTreeView::ExpandAllUnderInt(void* item) {
  ExpandAllUnder((iTreeViewItem*)item);
}

void iTreeView::ExpandDefaultUnder(iTreeViewItem* item) {
  if (!item) return;
  taBase* tab = item->link()->taData();
  int exp_flags = 0;
  
  if (parent_type == iTreeView::TYPE_BROWSEVIEWER && tab->GetTypeDef() == &TA_Program && useNavigatorCustomExpand()) {
    exp_flags |= EF_NAVIGATOR_FILTER;
  }
  else if (parent_type == iTreeView::TYPE_BROWSEVIEWER && tab->GetOwner(&TA_Program) && useNavigatorCustomExpand()) {
    exp_flags |= EF_NAVIGATOR_FILTER;
  }
  else if (parent_type == iTreeView::TYPE_PROGRAMEDITOR && useEditorCustomExpand()) {
    exp_flags |= EF_CUSTOM_FILTER;
  }
  
  exp_flags |= EF_DEFAULT_UNDER;
  
  taMisc::Busy(true);
  ExpandItem_impl(item, 0, m_def_exp_levels, exp_flags);
  if (header()->isVisible() && (header()->count() > 1)) {
    resizeColumnsToContents();
  }
  taMisc::Busy(false);
}

void iTreeView::ExpandDefaultUnderInt(void* item) {
  CollapseAllUnderInt(item);
  ExpandDefaultUnder((iTreeViewItem*)item);
}

void iTreeView::ExpandDefault() {
  int exp_flags = EF_DEFAULT;
  
  if (parent_type == iTreeView::TYPE_PROGRAMEDITOR && useEditorCustomExpand()) {
    exp_flags |= EF_CUSTOM_FILTER;
  }
  
  if (tv_flags & TV_EXPAND_DISABLED) exp_flags |= EF_EXPAND_DISABLED;
  ExpandAll_impl(m_def_exp_levels, exp_flags);
  if(topLevelItemCount() > 0) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(0));
    if(node)
      scrollTo(node);
  }
}

void iTreeView::GetTreeState_impl(iTreeViewItem* node, String_Array& tree_state) {
  if (!myProject()) return;
  
  if (node->isExpanded()) {
    taBase* tab = node->link()->taData();
    tree_state.Add(tab->GetPath());
  }
  for (int i = 0; i < node->childCount(); ++i) {
    iTreeViewItem* child = dynamic_cast<iTreeViewItem*>(node->child(i));
    if (child) {
      GetTreeState_impl(child, tree_state);
    }
  }
}

void iTreeView::RestoreTreeState_impl(iTreeViewItem* node, String_Array& tree_state) {
  if (!myProject()) return;
  if (tree_state.size == 0) return;
  
  bool expand = false;
  if (node) {
    taBase* tab = node->link()->taData();
    for (int i=0; i<tree_state.size; i++) {
      if (tab->GetPath() == tree_state.SafeEl(i)) {
        expand = true;
        tree_state.RemoveIdx(i);
        break;
      }
    }
    if (expand) {
      if (!node->isExpanded()) {
        node->setExpanded(true);
      }
    }
    else {
      if (node->isExpanded()) {
        node->setExpanded(false);
      }
    }
    
    for (int i = 0; i < node->childCount(); ++i) {
      iTreeViewItem* child = dynamic_cast<iTreeViewItem*>(node->child(i));
      RestoreTreeState_impl(child, tree_state);
    }
  }
}

void iTreeView::GetTreeState(String_Array& tree_state) {
  if (!myProject()) return;
  
  tree_state.Reset();
  // we only restore the entire tree so start with the root item
  iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(0));
  GetTreeState_impl(node, tree_state);
}

void iTreeView::RestoreTreeState(String_Array& tree_state) {
  if (IsTreeDirty()) {
    taMisc::Info("The tree has been expanded or collapsed since the project was opened - can't be restored");
    return;
  }
  // we only restore the entire tree - so start with the root item
  iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(0));
  int counter = 0;
  RestoreTreeState_impl(node, tree_state);
}

void iTreeView::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev); // prob does nothing
  if(main_window) {
    main_window->cur_tree_view = this; // always overwrite with current
    if(this == main_window->GetMainTreeView()) {
      main_window->FocusIsLeftBrowser(); // this just records that this is active..
    }
    else {                      // assume prog editor!
      main_window->FocusIsMiddlePanel(iMainWindowViewer::PROG_TREE);
    }
  }
  Emit_GotFocusSignal();
}

void iTreeView::GetSelectedItems(ISelectable_PtrList& lst) {
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);
  QTreeWidgetItem* item;
  while ( (item = *it) ) {
    ISelectable* si = dynamic_cast<ISelectable*>(item);
    if (si)
      lst.Add(si);
    ++it;
  }
}

bool iTreeView::HasFilter(TypeItem* ti) const {
  if (m_filters) for (int i = 0; i < m_filters->size; ++i) {
    if (ti->HasOptionAfter(opt_treefilt, m_filters->FastEl(i)))
      return true;
  }
  return false;
}

bool iTreeView::hasMultiSelect() const {
  QAbstractItemView::SelectionMode sm = selectionMode();
  return ((sm == ContiguousSelection) ||
    (sm == ExtendedSelection) ||
    (sm == MultiSelection));
}

QFont& iTreeView::italicFont() const {
  if (!italic_font) {
    italic_font = new QFont(font());
    italic_font->setItalic(true);
  }
  return *italic_font;
}

iTreeViewItem* iTreeView::item(int i) const {
  if ((i < 0) || (i >= topLevelItemCount()))
    return NULL;
  return dynamic_cast<iTreeViewItem*>(topLevelItem(i));
}

void iTreeView::ItemDestroyingCb(iTreeViewItem* item) {
  if(selItems().FindEl((ISelectable*)item) >= 0) {
    // if we are selected, select next guy..
    QTreeWidgetItem* nxt = getNextItem(item);
    if (!nxt) {
      nxt = getPrevItem(item);
      if (nxt && nxt->isHidden()) {
        nxt = NULL;
      }
    }
    if (!nxt) {
      nxt = GetParentItem(item);
    }
    if(nxt) {
      selectItem(nxt);
    }
    RemoveSelectedItem((ISelectable*)item, false); // not forced, because it is gui
  }
}

QMimeData* iTreeView::mimeData(const QList<QTreeWidgetItem*> items) const {
  //NOTE: in Qt4, we no longer know if we are starting a drag operation
  if (items.count() == 0) return NULL; // according to Qt spec
  else if (items.count() == 1) {
    iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(items.at(0));
    if (!tvi) return NULL; // prob shouldn't happen
// #ifdef DEBUG
//     taMisc::Info("get mime data 1 item:", tvi->GetColText(0));
// #endif
    return tvi->GetClipDataSingle(iClipData::EA_SRC_OPS, false);
  } else { // multi case
    ISelectable_PtrList list;
    FillTypedList(items, list);
    if (list.size == 0) return NULL; // not likely
// #ifdef DEBUG
//     taMisc::Info("get mime data multi item");
// #endif
    return list.FastEl(0)->GetClipData(list, iClipData::EA_SRC_OPS, false);
  }
}

QStringList iTreeView::mimeTypes () const {
 //NOTE: for dnd to work, we just permit our own special mime type!!!
  QStringList rval;
  rval.append(taiObjectMimeFactory::tacss_objectdesc);
  return rval;
}

bool iTreeView::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::Paint) {
    QFont cur_font = QFont();
    if(parent_type == TYPE_BROWSEVIEWER) {
      cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
      setFont(cur_font);
    }
    else if(parent_type == TYPE_PROGRAMEDITOR) {
      cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
      setFont(cur_font);
    }
  }

  return QWidget::eventFilter(obj, event);
}

void iTreeView::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::UpdateUiOnCtrlPressed(this, key_event);
  
  bool stru_actions_enabled = true; // enabled by default
  ISelectable* si = curItem();
  if(si && si->link()) {
    taBase* sb = si->link()->taData();
    if(sb) {
      taList_impl* sbo = NULL;
      if(sb->InheritsFrom(&TA_taList_impl)) {
        sbo = (taList_impl*)sb;
      }
      else {
        sbo = GET_OWNER(sb, taList_impl);
      }
      if(sbo && sbo->HasOption("FIXED_SIZE")) {
        stru_actions_enabled = false;
      }
    }
  }
  
  iMainWindowViewer* imw = mainWindow();
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::TREE_CONTEXT, key_event);
  
  if(stru_actions_enabled) {
    switch(action) {
      case taiMisc::TREE_DECREASE_FONTSIZE:
      {
        QFont cur_font(font());
        if(parent_type == TYPE_BROWSEVIEWER) {
          tabMisc::root->navigator_font_size -= 1;
          cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
          setFont(cur_font);
        }
        else if(parent_type == TYPE_PROGRAMEDITOR) {
          tabMisc::root->program_font_size -= 1;
          cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
          setFont(cur_font);
        }
        key_event->accept();
        return;
      }
      case taiMisc::TREE_INCREASE_FONTSIZE:
      {
        QFont cur_font(font());
        if(parent_type == TYPE_BROWSEVIEWER) {
          tabMisc::root->navigator_font_size += 1;
          cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
          setFont(cur_font);
        }
        else if(parent_type == TYPE_PROGRAMEDITOR) {
          tabMisc::root->program_font_size += 1;
          cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
          setFont(cur_font);
        }
        if (myProject()) {
          myProject()->RefreshAllViews();
        }
        key_event->accept();
        return;
      }
        
      case taiMisc::TREE_NEW_DEFAULT_ELEMENT:
      case taiMisc::TREE_NEW_DEFAULT_ELEMENT_II:
        ext_select_on = false;
        InsertDefaultEl(true);            // after
        key_event->accept();
        return;
      case taiMisc::TREE_NEW_DEFAULT_ELEMENT_ABOVE:
      case taiMisc::TREE_NEW_DEFAULT_ELEMENT_ABOVE_II:
        ext_select_on = false;
        InsertDefaultEl(false);            // above
        key_event->accept();
        return;
      case taiMisc::TREE_NEW_ELEMENT_ABOVE:
      case taiMisc::TREE_NEW_ELEMENT_ABOVE_II:
        ext_select_on = false;
        InsertEl();             // at
        key_event->accept();
        return;
      case taiMisc::TREE_NEW_ELEMENT_BELOW:
      case taiMisc::TREE_NEW_ELEMENT_BELOW_II:
        ext_select_on = false;
        InsertEl(true);         // after
        key_event->accept();
        return;
      case taiMisc::TREE_CUT:
      case taiMisc::TREE_CUT_II:
        ext_select_on = false;
        if (ISelectable *si = curItem()) {
          if (ISelectableHost *host = si->host()) {
            int ea = 0;
            host->EditActionsEnabled(ea);
            if (ea & iClipData::EA_CUT) {
              host->EditAction(iClipData::EA_CUT);
            }
          }
        }
        key_event->accept();
        return;
      case taiMisc::TREE_COPY:
      case taiMisc::TREE_COPY_II:
        ext_select_on = false;
        if (ISelectable *si = curItem()) {
          if (ISelectableHost *host = si->host()) {
            int ea = 0;
            host->EditActionsEnabled(ea);
            if (ea & iClipData::EA_COPY) {
              host->EditAction(iClipData::EA_COPY);
            }
          }
        }
        key_event->accept();
        return;
      case taiMisc::TREE_PASTE:
      case taiMisc::TREE_PASTE_II:
        ext_select_on = false;
        if (ISelectable *si = curItem()) {
          if (ISelectableHost *host = si->host()) {
            int ea = 0;
            host->EditActionsEnabled(ea);
            if (ea & iClipData::EA_PASTE) {
              host->EditAction(iClipData::EA_PASTE);
            }
          }
        }
        key_event->accept();
        return;
      case taiMisc::TREE_DUPLICATE:
      case taiMisc::TREE_DUPLICATE_II:
        ext_select_on = false;
        if (ISelectable *si = curItem()) {
          if (ISelectableHost *host = si->host()) {
            int ea = 0;
            host->EditActionsEnabled(ea);
            if (ea & iClipData::EA_DUPE) {
              host->EditAction(iClipData::EA_DUPE);
            }
          }
        }
        key_event->accept();
        return;
      case taiMisc::TREE_DELETE:
      case taiMisc::TREE_DELETE_II:
        ext_select_on = false;
        if (ISelectable *si = curItem()) {
          if (ISelectableHost *host = si->host()) {
            int ea = 0;
            host->EditActionsEnabled(ea);
            if (ea & iClipData::EA_DELETE) {
              host->EditAction(iClipData::EA_DELETE);
              //WARNING: we may be deleted at this point!!!
            }
          }
        }
        key_event->accept();
        return;
      case taiMisc::TREE_HISTORY_FORWARD:
      case taiMisc::TREE_HISTORY_FORWARD_II:
        if(imw && imw->brow_hist) {
          imw->brow_hist->forward();
        }
        key_event->accept();
        return;
      case taiMisc::TREE_HISTORY_BACKWARD:
      case taiMisc::TREE_HISTORY_BACKWARD_II:
        if(imw && imw->brow_hist) {
          imw->brow_hist->back();
        }
        key_event->accept();
        return;
      case taiMisc::TREE_FIND:
      case taiMisc::TREE_FIND_II:
        if(si && si->link()) {   // si is curItem()
          taiSigLink* link = si->link();
          iMainWindowViewer* imw = mainWindow();
          if(imw) imw->Find(link);
        }
        key_event->accept();
        return;
      case taiMisc::TREE_FIND_REPLACE:
      case taiMisc::TREE_FIND_REPLACE_II:
        if(si && si->link()) {  // si is curItem()
          taiSigLink* link = si->link();
          iMainWindowViewer* imw = mainWindow();
          if(imw) imw->Replace(link, selItems());
        }
        key_event->accept();
        return;
      default:
        inherited::keyPressEvent(key_event);
    }
  }
  else {
    inherited::keyPressEvent(key_event);
  }
}

bool iTreeView::focusNextPrevChild(bool next) {
  if(next && focus_next_widget) {
    focus_next_widget->setFocus();
    return true;
  }
  if(!next && focus_prev_widget) {
    focus_prev_widget->setFocus();
    return true;
  }
  return inherited::focusNextPrevChild(next);
}

void iTreeView::mnuFindFromHere(iAction* mel) {
  iMainWindowViewer* imw = mainWindow();
  if (!imw) return;
  iTreeViewItem* node = (iTreeViewItem*)(mel->usr_data.toPtr());
  taiSigLink* dl = node->link();
  imw->Find(dl);
}

void iTreeView::mnuReplaceFromHere(iAction* mel) {
  iMainWindowViewer* imw = mainWindow();
  if (!imw) return;
  iTreeViewItem* node = (iTreeViewItem*)(mel->usr_data.toPtr());
  taiSigLink* dl = node->link();
  imw->Replace(dl, selItems());
}

void iTreeView::mouseDoubleClickEvent(QMouseEvent* event) {
  // NOTE: we replace all the default behavior with our custom exp/coll all shtick
  QModelIndex index = indexAt(event->pos());
  if (!index.isValid()) {
    inherited::mouseDoubleClickEvent(event); // does some other stuff, prob should keep
    return;
  }
  QTreeWidgetItem* item_ = itemFromIndex(index);
  iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
  if (!item) return;
  if (item->isExpanded()) {
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {  // command key on mac
      ExpandAllUnder(item);
    }
    else {
      CollapseAllUnder(item);
    }
  }
  else {
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {  // command key on mac
      ExpandAllUnder(item);
    }
    else {
      ExpandDefaultUnder(item);
    }
  }
  emit itemDoubleClicked(item_, index.column()); // still need to emit the signal for other consumers!
  // i.e., the iPanelOfList
}

void iTreeView::mousePressEvent(QMouseEvent* event) {
  ++in_mouse_press;
  inherited::mousePressEvent(event);
  --in_mouse_press;
}

bool iTreeView::useEditorCustomExpand() const {
  return (receivers(SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&))) > 0);
}

bool iTreeView::useNavigatorCustomExpand() const {
  return (receivers(SIGNAL(CustomExpandNavigatorFilter(iTreeViewItem*, int, bool&))) > 0);
}


void iTreeView::setColKey(int col, const KeyString& key) {
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, ColKeyRole, key);
}

void iTreeView::setHeaderText(int col, const String& value) {
  headerItem()->setText(col, value);
}

int iTreeView::maxColChars(int col) {
  if ((col < 0) || (col >= columnCount())) return -1;
  QVariant v = headerItem()->data(col, MaxColCharsRole);
  if (v.isNull()) return -1;
  else return v.toInt();
}

void iTreeView::setMaxColChars(int col, int value) {
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, MaxColCharsRole, value);
}

int iTreeView::colFormat(int col) {
  if ((col < 0) || (col >= columnCount())) return 0;
  QVariant v = headerItem()->data(col, ColFormatRole);
  if (v.isNull()) return 0;
  else return v.toInt();
}

void iTreeView::setColFormat(int col, int value) {
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, ColFormatRole, value);
}

void iTreeView::setDecorateEnabled(bool value) {
  if (m_decorate_enabled == value) return;
  m_decorate_enabled = value;
}

void iTreeView::setShow(TypeItem::ShowMembs value) {
  if (m_show == value) return;
  m_show = value;
  Show_impl();
}

void iTreeView::itemWasEdited(const QModelIndex& index) const {
  iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(itemFromIndex(index));
  if(item) {
    item->itemEdited(index.column(), move_after_edit);
  }
  inherited::itemEdited(index);
}

void iTreeView::lookupKeyPressed(iLineEdit* le) const {
  iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(currentItem());
  if(item) {
    item->lookupKeyPressed(le, 0); // todo: no col
  }
  inherited::lookupKeyPressed(le);
}

void iTreeView::setTvFlags(int value) {
  if (tv_flags == value) return;
  tv_flags = value;
  //nothing to do yet
}

void iTreeView::Refresh_impl() {
  QFont cur_font = QFont();
  if(parent_type == TYPE_BROWSEVIEWER) {
    cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
  else if(parent_type == TYPE_PROGRAMEDITOR) {
    cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
  
  SaveScrollPos();
  //note: very similar to Show_impl
  QTreeWidgetItemIterator it(this);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !ShowNode(item);
      bool is_hid = isItemHidden(item);
      if (hide_it != is_hid) {
        item->setHidden(hide_it);
      }
      // always refresh visible guys
      if (!hide_it) {
        // simulate update notification
        item->SigEmit(SLS_ITEM_UPDATED, NULL, NULL);
      }
    }
    ++it;
  }
  RestoreScrollPos();
}

void iTreeView::Show_impl() {
  SaveScrollPos();
  //note: very similar to Refresh_impl
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    //note: always show QTreeWidgetItem, since we don't know what to do with them
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !ShowNode(item);
      bool is_hid = isItemHidden(item);
      if (hide_it != is_hid) {
        item->setHidden(hide_it);
        // if we are making shown a hidden item, we also refresh it for safety
        if (!hide_it) {
          // simulate update notification
          item->SigEmit(SLS_ITEM_UPDATED, NULL, NULL);
        }
      }
    } else {
      item_->setHidden(false);
    }
    ++it;
  }
  RestoreScrollPos();
}

void iTreeView::EmitTreeStructToUpdate() {
  emit TreeStructToUpdate();
}

void iTreeView::EmitTreeStructUpdated() {
  emit TreeStructUpdated();
}

void iTreeView::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  if ((tv_flags & TV_AUTO_EXPAND) && (!(tv_flags & TV_AUTO_EXPANDED))) {
    QTimer::singleShot(250, this, SLOT(ExpandDefault()) );
    tv_flags = (TreeViewFlags)(tv_flags | TV_AUTO_EXPANDED);
  }
  
  QFont cur_font = QFont();
  if(parent_type == TYPE_BROWSEVIEWER) {
    cur_font.setPointSize(tabMisc::root->navigator_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
  else if(parent_type == TYPE_PROGRAMEDITOR) {
    cur_font.setPointSize(tabMisc::root->program_font_size + tabMisc::root->global_font_incr_decr);
    setFont(cur_font);
  }
}

bool iTreeView::ShowNode(iTreeViewItem* item) const {
  if (!item) return false; // bad caller!
  return item->ShowNode(show(), m_show_context);
}

void iTreeView::FillContextMenu_pre(ISelectable_PtrList& sel_items, taiWidgetActions* menu) {
  emit FillContextMenuHookPre(sel_items, menu);
}

void iTreeView::this_contextMenuRequested(QTreeWidgetItem* item, const QPoint & pos,
                                          int col ) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  // note: we must force the sel_item to be the item, otherwise we frequently
  // are refering to the wrong item (not what user right clicked on)
  // this seems to be the easiest way:
  this_itemSelectionChanged();
  FillContextMenu(menu);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iTreeView::FillContextMenu_post(ISelectable_PtrList& sel_items, taiWidgetActions* menu) {
  menu->AddSep();
  taiWidgetMenu* men_exp = menu->AddSubMenu("Expand/Collapse");

  if (sel_items.size == 1) {
    ISelectable* si = sel_items.FastEl(0);
    if (si && si->GetTypeDef()->InheritsFrom(&TA_iTreeViewItem)) {
      void* nd = si->This(); // don't need to detype, because we pass as void anyway
      men_exp->AddItem("Expand Default", taiWidgetMenu::normal, iAction::ptr_act,
                       this, SLOT(ExpandDefaultUnderInt(void*)), (void*)nd );
      men_exp->AddItem("Expand Fully", taiWidgetMenu::normal, iAction::ptr_act,
                       this, SLOT(ExpandAllUnderInt(void*)), (void*)nd );
      men_exp->AddItem("Collapse", taiWidgetMenu::normal, iAction::ptr_act,
                       this, SLOT(CollapseAllUnderInt(void*)), (void*)nd );
    }
  }  
  emit FillContextMenuHookPost(sel_items, menu);
}

//NOTE: this is a widget-level guy that just forwards to our signal --
// it presumably is ALSO emitted in addition to itemSelectionChanged
void iTreeView::this_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
  iTreeViewItem* it = dynamic_cast<iTreeViewItem*>(curr); //note: we want null if curr is not itvi
  //NOTE: the default QAbstractItemView guy doesn't seem to handle the statustip
  // very well == it barely gets activated, only if you click an item then drag a bit --
  // so we are doing it manually here
  if (it) {
    QString statustip = it->data(0, Qt::StatusTipRole).toString();
    if (parent() && !statustip.isEmpty()) {
      QStatusTipEvent tip(statustip);
      QApplication::sendEvent(parent(), &tip);
    }
  }
  // needed esp. when we call setCurrentItem(x)
  // except skip in the mouse-down routine (we get in the clicked, via mouse up)
  if (!in_mouse_press) {
    this_itemSelectionChanged();
  }
}

void iTreeView::this_itemSelectionChanged() {
  if (selectionChanging()) return; // ignore
  SelectionChanging(true, false); // not forced, because gui-driven
  sel_items.Reset();
  GetSelectedItems(sel_items);
  SelectionChanging(false, false);
}

void iTreeView::UpdateSelectedItems_impl() {
  //note: we are already guarded against spurious gui updates
  // our approach is to copy the list, then iterate all currently selected items,
  // removing those from selection not in the list or removing from list if in
  // selection already; then we select any that remain in the list
  ISelectable_PtrList sel_items = selItems(); // copies
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);
  while (QTreeWidgetItem *item = *it) {
    if (ISelectable* si = dynamic_cast<ISelectable*>(item)) {
      int lst_idx = sel_items.FindEl(si);
      if (lst_idx >= 0) {
        sel_items.RemoveIdx(lst_idx);
      }
      else setItemSelected(item, false); // hope this is ok while iterating!!!!
    }
    ++it;
  }
  // now, select any remaining
  for (int lst_idx = 0; lst_idx < sel_items.size; ++lst_idx) {
    ISelectable* si = sel_items.FastEl(lst_idx);
    if (si->GetTypeDef()->InheritsFrom(TA_iTreeViewItem)) { // should
      if (QTreeWidgetItem *item = (iTreeViewItem*) (si->This())) {
        setItemSelected(item, true);
      }
    }
  }
}


void iTreeView::SaveScrollPos() {
  m_saved_scroll_pos = verticalScrollBar()->value();
}

void iTreeView::RestoreScrollPos() {
  ScrollTo(m_saved_scroll_pos);
}

void iTreeView::ScrollTo(int scr_pos) {
  taiMisc::ScrollTo_SA(this, scr_pos);
}

bool iTreeView::PosInView(int scr_pos) {
  return taiMisc::PosInView_SA(this, scr_pos);
}

void iTreeView::dragMoveEvent(QDragMoveEvent* ev) {
#if (QT_VERSION >= 0x040700)
  iTreeWidgetItem* foo;
  if (taMisc::tree_spring_loaded) {
    int item_idx = -1;
    QModelIndex index = indexAt(ev->pos());
    iTreeWidgetItem* item = dynamic_cast<iTreeWidgetItem*>(itemFromIndex(index));
    if (item) {
      if (item != possibleDropTargetItem) { // if true we are over a different item
        if ((expandedItemList.FindEl((const iTreeWidgetItem*)item->parent()) == -1) && (expandedItemList.FindEl((const iTreeWidgetItem*)item) == -1)) { // no parent or self on list
          // remove all expanded from list and collapse
          while ((foo = expandedItemList.Peek()) != NULL) {
            foo = expandedItemList.Pop();
            foo->setExpanded(false);
          }
        }
        else { // someone on list is parent of possible drop target
          while (expandedItemList.size > 0 && item->parent() != expandedItemList.Peek() && item != expandedItemList.Peek()) {
            foo = expandedItemList.Pop();
            foo->setExpanded(false);
          }
        }
        possibleDropTargetItem = item;  // new possible target
        dropTimer.restart();
      }
      else {  // still over same possible target
        if (dropTimer.elapsed() > taMisc::spring_loaded_delay) {
          if (!item->isExpanded()) {  // if previously expanded don't put on stack
            expandedItemList.Push(item);
            item->setExpanded(true);
          }
        }
      }
    }
  }
#endif
  inherited::dragMoveEvent(ev);
}

void iTreeView::dropEvent(QDropEvent* e) {
  // reset all of the springing item info
  possibleDropTargetItem = NULL;
  expandedItemList.Reset();

  inherited::dropEvent(e);
}

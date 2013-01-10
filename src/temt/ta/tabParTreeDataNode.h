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

#ifndef tabParTreeDataNode_h
#define tabParTreeDataNode_h 1

// parent includes:
#include <tabTreeDataNode>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API tabParTreeDataNode: public tabTreeDataNode {
INHERITED(tabTreeDataNode)
public:
  taList_impl*          list() const {return ((tabODataLink*)m_link)->list();}
//nuke  tabParDataLink*         link() const {return (tabParDataLink*)m_link;}

  void                  AssertLastListItem(); // #IGNORE updates last_list_items_node -- called by Group node before dynamic inserts/updates etc.
  override void         UpdateChildNames(); // #IGNORE update child names of the indicated node

  virtual bool          RebuildChildrenIfNeeded();
  // checks if child count != list count, and rebuilds children if so

  tabParTreeDataNode(tabODataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabParTreeDataNode(tabODataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabParTreeDataNode();
public: // IDataLinkClient interface
//  override void*      This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_tabParTreeDataNode;}
protected:
  taiTreeDataNode*      last_list_items_node; // #IGNORE last list member node created, so we know where to start group items
  override void         DataChanged_impl(int dcr, void* op1, void* op2);
  override void         CreateChildren_impl();
  taiTreeDataNode*      CreateListItem(taiTreeDataNode* par_node,
    taiTreeDataNode* after, taBase* el);
  void                  UpdateListNames(); // #IGNORE updates names after inserts/deletes etc.
  override void         willHaveChildren_impl(bool& will) const;
private:
  void                  init(tabODataLink* link_, int dn_flags_); // #IGNORE
};

#endif // tabParTreeDataNode_h
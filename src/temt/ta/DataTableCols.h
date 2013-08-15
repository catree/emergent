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

#ifndef DataTableCols_h
#define DataTableCols_h 1

// parent includes:
#include <DataCol>
#include <taList>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 


taTypeDef_Of(DataTableCols);

class TA_API DataTableCols: public taList<DataCol> {
  // ##CAT_Data columns of a datatable
INHERITED(taList<DataCol>)
friend class DataTable;
public:
  override void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
#ifdef __MAKETA__
  taBase*               New_gui(int n_objs=1, TypeDef* typ=NULL,
    const String& name="(default name)"); // #NO_MENU
#endif
  override int          NumListCols() const {return 3;}
  // name, val_type (float, etc.), desc
  override String       GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;

  override String       GetTypeDecoKey() const { return "DataTable"; }
  override taBase*      ChildDuplicate(const taBase* chld);

  TA_BASEFUNS(DataTableCols);
protected: // these guys must only be used by DataTable, but no external guys
  virtual void  Copy_NoData(const DataTableCols& cp);
  // #IGNORE #CAT_ObjectMgmt copy only the column structure, but no data, from other data table
  virtual void  CopyFromRow(int dest_row, const DataTableCols& cp, int src_row);
  // #IGNORE #CAT_ObjectMgmt copy one row from source to given row in this object: source must have exact same column structure as this!!

private:
  NOCOPY(DataTableCols)
  void  Initialize();
  void  Destroy()               {}
};

TA_SMART_PTRS(DataTableCols);

#endif // DataTableCols_h

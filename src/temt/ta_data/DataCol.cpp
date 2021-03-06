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

#include "DataCol.h"
#include <DataTable>
#include <double_Matrix>
#include <float_Matrix>
#include <int_Matrix>
#include <taHashTable>
#include <taMath_float>
#include <taMath_double>
#include <MemberDef>
#include <MinMax>
#include <MatrixIndex>
#include <taObjDiffRec>
#include <taObjDiff_List>
#include <DataSelectSpec>
#include <DataSelectEl>
#include <String_Array>
#include <AnalysisRun>  // need for analysis type enum

#include <SigLinkSignal>
#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataCol);
SMARTREF_OF_CPP(DataCol);

const String DataCol::udkey_width("WIDTH");
const String DataCol::udkey_narrow("NARROW");
const String DataCol::udkey_hidden("HIDDEN");

void DataCol::Initialize() {
  col_flags = (ColFlags)(SAVE_DATA);
  col_idx = -1;
  is_matrix = false;
  // default initialize to scalar
  cell_geom.SetDims(1);
  cell_geom.Set(0, 1);
  hash_table = NULL;
  width = 0;
}

void DataCol::InitLinks() {
  inherited::InitLinks();
  taBase::Own(cell_geom, this);
  taBase::Own(calc_expr, this);
  taBase::Own(dim_names, this);
  taMatrix* ar = AR();
  if (ar != NULL) {
    taBase::Own(ar, this);
    SetMatrixViewMode();
  }
  AutoNameMyMembers();
//  taBase::Own(control_panel_cell, this);
}

void DataCol::SetMatrixViewMode() {
  taMatrix* ar = AR();
  if (ar != NULL) {
    DataTable* dt = GET_MY_OWNER(DataTable);
    if(dt) {
      ar->el_view = &dt->row_indexes;
      ar->el_view_mode = IDX_FRAMES;
    }
  }
}

void DataCol::UnSetMatrixViewMode() {
  taMatrix* ar = AR();
  ar->el_view = NULL;
}

void DataCol::CutLinks() {
  cell_geom.CutLinks();
  calc_expr.CutLinks();
  dim_names.CutLinks();
  RemoveHashTable();
//  control_panel_cell.CutLinks();
  inherited::CutLinks();
}

void DataCol::Copy_Common_(const DataCol& cp) {
  RemoveHashTable();
  // things common to full and schema-only copy
  name = cp.name; // this is the default we want for DataCol
  desc = cp.desc;
  col_flags = cp.col_flags;
  is_matrix = cp.is_matrix;
  cell_geom = cp.cell_geom;
  calc_expr = cp.calc_expr;
  dim_names = cp.dim_names;
  width = cp.width;
}

void DataCol::Copy_(const DataCol& cp) {
  Copy_Common_(cp);
}

void DataCol::CopyFromCol_Robust(const DataCol& cp) {
  RemoveHashTable();
  // note: caller has asserted Struct guys, but ok for us to do it again
  // assumes copy has been validated
  StructUpdate(true);
  String save_name(this->name); // hold on to the current column name
  inherited::Copy_impl(cp);     // do all common generic class copying
  this->name = save_name;       // restore the name because copy adds _copy
  SetBaseFlag(COPYING); // note: still have to set/reset here, because not nestable
  // assume name already copied, else don't want it overwritten
  // don't do these -- just the data!!
  //   desc = cp.desc;
  //   col_flags = cp.col_flags;
  //   calc_expr = cp.calc_expr;

  //NOTE: we only copy data, leaving geom and type as is (since we can't change anyway)
  int rows = cp.rows();
  EnforceRows(rows);
  for (int i = 0; i < rows; ++i) {
    CopyFromRow_Robust(i, cp, i);
  }
  ClearBaseFlag(COPYING);
  StructUpdate(false);
}

void DataCol::Copy_NoData(const DataCol& cp) {
  RemoveHashTable();
  StructUpdate(true);
  inherited::Copy_impl(cp);
  SetBaseFlag(COPYING);
  Copy_Common_(cp);
  Init(); //note: table should have no rows, so won't make any
  ClearBaseFlag(COPYING);
  StructUpdate(false);                  // initialize array to new geom
}

void DataCol::CopyFromRow(int dest_row, const DataCol& src, int src_row) {
  RemoveHashTable();
  if (dest_row < 0) dest_row = rows() + dest_row; // abs row, if request was from end
  if (src_row < 0) src_row = src.rows() + src_row; // abs row, if request was from end
  if(src.is_matrix) {
    taMatrix* mat = ((DataCol&)src).GetValAsMatrix(src_row);
    if(mat) {
      taBase::Ref(mat);
      SetValAsMatrix(mat, dest_row);
      taBase::unRefDone(mat);
    }
  }
  else {
    SetValAsVar(src.GetValAsVar(src_row), dest_row);
  }
}

void DataCol::CopyFromRow_Robust(int dest_row, const DataCol& src, int src_row) {
  RemoveHashTable();
  if(src.is_matrix) {
    if(is_matrix) {
      int mx_sz = MIN(cell_size(), src.cell_size());
      for(int i=0; i<mx_sz; i++) {
        SetValAsVarM(src.GetValAsVarM(src_row, i), dest_row, i);
      }
    }
    else {
      SetValAsVar(src.GetValAsVarM(src_row, 0), dest_row);
    }
  }
  else {
    if(is_matrix) {
      SetValAsVarM(src.GetValAsVar(src_row), dest_row, 0);
    }
    else {
      SetValAsVar(src.GetValAsVar(src_row), dest_row);
    }
  }
}

void DataCol::CopyExtras(const DataCol& cp) {
  desc = cp.desc;
  col_flags = cp.col_flags;
  calc_expr = cp.calc_expr;
  dim_names = cp.dim_names;
  width = cp.width;
  CopyUserData(cp);
}

void DataCol::Init() {
  taMatrix* ar = AR(); //cache
  int rows = 0; // rows, based on table (not our frames, which may have changed)
  DataTable* tab = dataTable();
  if (tab)
    rows = tab->rows_total;    // all rows, not just the visible rows

  if (is_matrix) {
    MatrixGeom tdim = cell_geom;
    tdim.SetDims(tdim.dims() + 1);
    tdim.Set(tdim.dims()-1, rows);
    UnSetMatrixViewMode();        // reset view temporarily
    ar->SetGeomN(tdim);
    SetMatrixViewMode();          // reset it
    dim_names.SetGeom(1,cell_geom.dims());
  }
  else {
    UnSetMatrixViewMode();        // reset view temporarily
    ar->SetGeom(1, rows);
    SetMatrixViewMode();          // reset it
    dim_names.SetGeom(1,0);
  }
  // transfer READ_ONLY to mat
  ar->ChangeBaseFlag(BF_GUI_READ_ONLY, (col_flags & READ_ONLY));
}

bool DataCol::isImage() const {
  return GetUserDataAsBool("IMAGE");
}

int DataCol::imageComponents() const {
  if (cell_geom.dims() <= 2) return 1;
  else return cell_geom.FastEl(2);
}

void DataCol::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Init();
  DataTable* dt = dataTable();
  if (dt) {
    dt->ColumnUpdate(this);
  }
}

void DataCol::SigEmit(int sls, void* op1, void* op2) {
  RemoveHashTable();
  inherited::SigEmit(sls, op1, op2);
  // treat item changes here as struct changes to the table
  if (sls <= SLS_ITEM_UPDATED_ND) {
    DataTable* dt = dataTable();
    if (dt) {
      dt->StructUpdate(true);
      dt->StructUpdate(false);
      dt->UpdateColCalcs();
    }
  }
}

int DataCol::GetSpecialState() const {
  if(!HasColFlag(SAVE_DATA)) return 2;
  if(HasColFlag(READ_ONLY)) return 4; // light red -- 3 is green
  return 0;
}

void DataCol::ChangeColType_gui(ValType new_type) {
  if (valType() == new_type) return;
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  own->change_col_type = new_type;
  tabMisc::DelayedFunCall_nogui(own, "ChangeColType_impl");
}

void DataCol::ChangeColType(ValType new_type) {
  if (valType() == new_type) return;
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  own->change_col_type = new_type;
  own->ChangeColType_impl();
}

void DataCol::ChangeColCellGeom(const MatrixGeom& new_geom) {
  if ((!is_matrix && (new_geom.dims() == 0)) ||
      cell_geom.Equal(new_geom)) {
    return;
  }
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  own->change_col_geom = new_geom;
  own->ChangeColCellGeom_impl();
}

void DataCol::ChangeColCellGeom_gui(const MatrixGeom& new_geom) {
  if ((!is_matrix && (new_geom.dims() == 0)) ||
      cell_geom.Equal(new_geom)) {
    return;
  }
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  own->change_col_geom = new_geom;
  tabMisc::DelayedFunCall_nogui(own, "ChangeColCellGeom_impl");
}

void DataCol::ChangeColCellGeomNs(int dims, int d0, int d1, int d2, int d3,
    int d4, int d5, int d6) {
  MatrixGeom mg;
  mg.SetGeom(dims, d0, d1, d2, d3, d4, d5, d6);
  ChangeColCellGeom(mg);
}

void DataCol::ChangeColMatToScalar() {
  if (!is_matrix) return;
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  own->ChangeColMatToScalar_impl();
}

void DataCol::ChangeColMatToScalar_gui() {
  if (!is_matrix) return;
  DataTable* own = dataTable();
  if(!own) return;
  RemoveHashTable();
  own->change_col = this;
  tabMisc::DelayedFunCall_nogui(own, "ChangeColMatToScalar_impl");
}

DataTable* DataCol::dataTable() const {
  DataTable* rval = GET_MY_OWNER(DataTable);
  return rval;
}

bool DataCol::EnforceRows(int rws) {
  RemoveHashTable();
  taMatrix* mat = AR();
  if (!mat) return false;
  UnSetMatrixViewMode();        // reset view temporarily
  bool rval = false;
  rval = mat->EnforceFrames(rws);
  SetMatrixViewMode();          // reset it
  return rval;
}

// note: this is ever called and is invalid to do at the col level anyway
// bool DataCol::InsertRows(int st_row, int n_rows) {
//   RemoveHashTable();
//   taMatrix* mat = AR();
//   if (!mat) return false;
//   UnSetMatrixViewMode();          // reset view temporarily
//   bool rval = mat->InsertFrames(st_row, n_rows);
//   SetMatrixViewMode();          // reset it
//   if (!rval) return rval;
//   return rval;
// }

int DataCol::FindVal(const Variant& val, int st_row) const {
  if(TestError(isMatrix(), "FindVal", "column must be scalar, not matrix")) return -1;
  if(st_row == 0 && hash_table) {
    return hash_table->FindHashValString(val.toString());
  }
  if(st_row >= 0) {
    for(int i=st_row; i<rows(); i++) {
      if(GetVal(i) == val) return i;
    }
    return -1;
  }
  else {
    for(int i=rows()+st_row; i>=0; i--) {
      if(GetVal(i) == val) return i;
    }
    return -1;
  }
}

int DataCol::FindValPartial(const Variant& val, int st_row) const {
  if(TestError(isMatrix(), "FindValPartialCi", "column must be scalar, not matrix")) return -1;
  String val_str = val.toString();
  if(st_row >= 0) {
    for(int i=st_row; i<rows(); i++) {
      String row_str = GetValAsString(i);
      if(row_str.contains(val_str))
        return i;
    }
    return -1;
  }
  else {
    for(int i=rows()+st_row; i>=0; i--) {
      String row_str = GetValAsString(i);
      if(row_str.contains(val_str))
        return i;
    }
    return -1;
  }
}

int DataCol::FindValPartialCi(const Variant& val, int st_row) const {
  if(TestError(isMatrix(), "FindValPartialCi", "column must be scalar, not matrix")) return -1;
  String val_str = val.toString();
  if(st_row >= 0) {
    for(int i=st_row; i<rows(); i++) {
      String row_str = GetValAsString(i);
      if(row_str.contains_ci(val_str))
        return i;
    }
    return -1;
  }
  else {
    for(int i=rows()+st_row; i>=0; i--) {
      String row_str = GetValAsString(i);
      if(row_str.contains_ci(val_str))
        return i;
    }
    return -1;
  }
}

void DataCol::BuildHashTable() {
  RemoveHashTable();
  if(TestError(isMatrix(), "BuildHashTable", "column must be scalar, not matrix"))
    return;
  hash_table = new taHashTable;
  if(!hash_table->Alloc(rows() + 10)) return;
  for(int i=0; i<rows(); i++) {
    String strval = GetVal(i).toString();
    hash_table->AddHash(taHashEl::HashCode_String(strval), i, strval);
  }
}

void DataCol::RemoveHashTable() {
  if(hash_table) {
    delete hash_table;
    hash_table = NULL;
  }
}

String DataCol::ColStats() {
  if (TestError(isMatrix(), "ColStats", "column must be scalar, not matrix"))
    return _nilString;
  if(valType() == VT_DOUBLE) {
    return taMath_double::vec_stats((double_Matrix*)AR());
  }
  else if(valType() == VT_FLOAT) {
    return taMath_float::vec_stats((float_Matrix*)AR());
  }
  else if(valType() == VT_INT) {
    float_Matrix float_tmp(false);
    int_Matrix* mat = (int_Matrix*)AR();
    taMath_float::vec_fm_ints(&float_tmp, mat);
    return taMath_float::vec_stats(&float_tmp);
  }
  TestError(true, "ColStats", "column data type is not double, float or int");
  return _nilString;
}

int DataCol::displayWidth() const {
  // explicit width has highest priority
  int rval = GetUserData(udkey_width).toInt();
  if (rval == 0) {
    if(isMatrix()) {
      int raw_width = 1;
      int raw_height = 1;
      Get2DCellGeom(raw_width, raw_height);
      rval = raw_width;
    }
    else {
      switch (valType()) {
        case VT_STRING: rval = 16; break; // maximum width for strings
        case VT_DOUBLE: rval = 16; break;
        case VT_FLOAT: rval = 8; break;
        case VT_INT: rval = 8; break;
        case VT_BYTE: rval = 3; break;
        case VT_VARIANT: rval = 10; break;
        case VT_BOOL: rval = 5; break;
        default: break;
      }
    }
  }
  if (rval == 0)
    rval = 8; // default
  // include name: not
  //  rval = MAX(rval, name.length());
  
  return rval;
}

taBase::DumpQueryResult DataCol::Dump_QuerySaveMember(MemberDef* md) {
  static DataTable* last_dt = NULL;
  if(md->name != "ar")
    return inherited::Dump_QuerySaveMember(md);
    
  DataTable* dt = dataTable();
  if(!dt)
    return DQR_NO_SAVE;   // should not happen
  
  if(!taMisc::is_undo_saving) { // if not undo, logic is simple..
    if(dt->HasDataFlag(DataTable::SAVE_ROWS)) {
      if(dt->Cells() > taMisc::auto_save_data_max_cells) {
        taMisc::Info("auto save - not saving data for datatable -- too big:", dt->GetPathNames(), "cells:", String(dt->Cells()));
        return DQR_NO_SAVE;
      }
      else {
        return DQR_SAVE;
      }
    }
    else {
      return DQR_NO_SAVE;
    }
  }
  
  // always save for obj itself
  if(tabMisc::cur_undo_mod_obj == dt || tabMisc::cur_undo_mod_obj == this) {
    if(dt->Cells() > taMisc::undo_data_max_cells) {
      if(taMisc::undo_debug && last_dt != dt) {
        taMisc::Info("not undo saving directly affected datatable -- too big:",
                     dt->GetPathNames(), "cells:", String(dt->Cells()));
        last_dt = dt;
      }
      return DQR_NO_SAVE; // too big or no save!
    }
    if(taMisc::undo_debug && last_dt != dt) {
      taMisc::Info("YES undo saving directly affected datatable:",
                   dt->GetPathNames(), "cells:", String(dt->Cells()));
      last_dt = dt;
    }
    return DQR_SAVE;
  }
  
  if(!tabMisc::cur_undo_save_owner || !IsChildOf(tabMisc::cur_undo_save_owner)) {
    // no need to save b/c unaffected by changes elsewhere..
    if(taMisc::undo_debug && last_dt != dt) {
      // taMisc::Info("not undo saving datatable -- should be unaffected:",
      //              dt->GetPathNames());
      // if(tabMisc::cur_undo_save_owner) {
      //   taMisc::Info("undo save owner:", tabMisc::cur_undo_save_owner->GetPathNames());
      // }
      last_dt = dt;
    }
    return DQR_NO_SAVE;
  }
  
  if(dt->Cells() > taMisc::undo_data_max_cells) {
    if(taMisc::undo_debug && last_dt != dt) {
      taMisc::Info("not undo saving datatable -- too big:",
                   dt->GetPathNames(), "cells:", String(dt->Cells()));
      last_dt = dt;
    }
    return DQR_NO_SAVE;   // too big!
  }
  
  if(dt->HasDataFlag(DataTable::SAVE_ROWS)) {
    if(taMisc::undo_debug && last_dt != dt) {
      taMisc::Info("YES undo saving datatable -- affected and small enough:",
                   dt->GetPathNames(), "cells:", String(dt->Cells()));
      last_dt = dt;
    }
    return DQR_SAVE;
  }
  
  if(taMisc::undo_debug && last_dt != dt) {
    // taMisc::Info("not undo saving datatable -- affected but no save at data table level:",
    //              dt->GetPathNames(), "cells:", String(dt->Cells()));
    last_dt = dt;
  }
  return DQR_NO_SAVE;
}

void DataCol::Get2DCellGeom(int& x, int& y) const {
  if (isMatrix()) {
    cell_geom.Get2DGeom(x, y);
  } else {
    x = 1;
    y = 1;
  }
}

void DataCol::Get2DCellGeomGui(int& x, int& y, bool odd_y, int spc) const {
  if (isMatrix()) {
    cell_geom.Get2DGeomGui(x, y, odd_y, spc);
  } else {
    x = 1;
    y = 1;
  }
}

const KeyString DataCol::key_val_type("val_type");

String DataCol::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_name) return GetDisplayName(); // override
  if (key == key_val_type) return ValTypeToStr(valType());
  else return inherited::GetColText(key, itm_idx);
}

String DataCol::GetDisplayName() const {
  return name;
}

/////////////////////
// Access Index

int DataCol::IndexOfEl_Flat(int row, int cell) const {
  if(TestError((cell < 0) || (cell >= cell_size()), "IndexOfEl_Flat",
               "cell index out of range")) return -1;
  const taMatrix* ar = AR(); //cache, and preserves constness
  int nRows = ar->Frames();
  if(row < 0) row = nRows + row; // abs row, if request was from end
  if(TestError((row < 0 || row >= nRows), "IndexOfEl_Flat", "row out of range"))
    return -1;
  return ar->FrameStartIdx(row) + cell;
}

int DataCol::IndexOfEl_Flat_Dims(int row, int d0, int d1, int d2, int d3, int d4) const {
  // note: any extra args will be 0 and ignored.  we're just getting index into row 0
  // so we don't care where the row appears
  int cell = AR()->SafeElIndex(d0, d1, d2, d3, d4, 0);
  return IndexOfEl_Flat(row, cell);
}

/////////////////////
// Access Var/String

const String DataCol::GetValAsString_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsStr_Flat(IndexOfEl_Flat(row, cell));
}

const Variant DataCol::GetValAsVar_impl(int row, int cell) const {
  const taMatrix* ar = AR(); //cache, and preserves constness
  return ar->SafeElAsVar_Flat(IndexOfEl_Flat(row, cell));
}

bool DataCol::SetValAsString_impl(const String& val, int row, int cell) {
  AR()->SetFmStr_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
}

bool DataCol::SetValAsVar_impl(const Variant& val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
}

bool DataCol::SetValAsBool_impl(bool val, int row, int cell) {
  AR()->SetFmVar_Flat(val, IndexOfEl_Flat(row, cell)); // note: safe operation
  return true;
}

bool DataCol::InitVals(const Variant& init_val)  {
  AR()->InitValsFmVar(init_val);
  return true;
}

bool DataCol::InitValsToRowNo()  {
  if(TestError(is_matrix, "InitValsToRowNo", "column is a matrix")) return false;
  for(int i=0; i<rows(); i++) {
    SetValAsInt(i, i);
  }
  return true;
}

bool DataCol::InitValsByIncrement(int first_value, int increment)  {
  if(TestError(is_matrix, "InitValsByIncrement", "column is a matrix")) return false;
  SetValAsInt(first_value, 0);
  for(int i=1; i<rows(); i++) {
    SetValAsInt(first_value + i*increment, i);
  }
  return true;
}

void DataCol::SortAscending() {
  dataTable()->SortCol(this, true);
}

void DataCol::SortDescending() {
  dataTable()->SortCol(this, false);
}

void DataCol::Filter(Relation::Relations operator_1, const String& value_1,
       Relation::CombOp comb_op, Relation::Relations operator_2, const String& value_2) {
  DataSelectSpec* select_spec = new DataSelectSpec; taBase::Ref(select_spec);
  DataSelectEl* select_el = (DataSelectEl*)select_spec->AddColumn(this->name, dataTable());
  select_el->cmp = value_1;
  select_el->rel = operator_1;
  select_spec->comb_op = comb_op;
  if (value_2 != "") {
    DataSelectEl* select_el_2 = (DataSelectEl*)select_spec->AddColumn(this->name, dataTable());
    select_el_2->cmp = value_2;
    select_el_2->rel = operator_2;
  }
  dataTable()->FilterBySpec(select_spec);
  taBase::unRefDone(select_spec);
}

void DataCol::UnFilter() {
  dataTable()->UnFilter();
}

void DataCol::CatRelation(String& expr, Relation::Relations oper) {
  switch(oper) {
  case Relation::EQUAL:
    expr.cat(" == ");
    break;
  case Relation::NOTEQUAL:
    expr.cat(" != ");
    break;
  case Relation::LESSTHAN:
    expr.cat(" < ");
    break;
  case Relation::GREATERTHAN:
    expr.cat(" > ");
    break;
  case Relation::LESSTHANOREQUAL:
    expr.cat(" <= ");
    break;
  case Relation::GREATERTHANOREQUAL:
    expr.cat(" >= ");
    break;
  default:
     taMisc::Error("DataCol::CatRelation, unknown switch value");
     break;
  }
}

void DataCol::CatConjunction(String& expr, Relation::CombOp conjunction) {
  switch(conjunction) {
  case Relation::AND:
    expr.cat(" && ");
    break;
  case Relation::OR:
    expr.cat(" || ");
    break;
  default:
    taMisc::Error("DataCol::CatConjunction, unknown switch value");
    break;
  }
}

void DataCol::CatColumnName(String& expr) {
  expr.cat(this->name);
}

void DataCol::CatValue(String& expr, const String& value) {
  if(!(valType() == VT_INT || valType() == VT_FLOAT || valType() == VT_DOUBLE)) {
  expr.cat(" \"");
  expr.cat(value);
  expr.cat("\"");
  }
  else {
    expr.cat(value);
  }
}

////////////////////
// Access: Matrix

taMatrix* DataCol::GetValAsMatrix(int row) {
  taMatrix* ar = AR();
  if(row < 0) row = rows() + row;
  taMatrix* rval = ar->GetFrameSlice_(row);
  if(rval && (col_flags & READ_ONLY))
    rval->SetBaseFlag(BF_READ_ONLY | BF_GUI_READ_ONLY);
  return rval;
}

taMatrix* DataCol::GetRangeAsMatrix(int st_row, int n_rows) {
  taMatrix* ar = AR();
  if(st_row < 0) st_row = rows() + st_row;
  taMatrix* rval = ar->GetFrameRangeSlice_(st_row, n_rows);
  if(rval && (col_flags & READ_ONLY))
    rval->SetBaseFlag(BF_READ_ONLY | BF_GUI_READ_ONLY);
  return rval;
}

bool DataCol::SetValAsMatrix(const taMatrix* val, int row) {
  if (TestError(!val, "SetValAsMatrix", "val is null")) return false;
  if (row < 0) row = rows() + row; // abs row, if request was from end
  //note: the mat function does most of the parameter checking
  return AR()->CopyFrame(*val, row);
}

void DataCol::WriteFmSubMatrix(int row,
    const taMatrix* src, taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  taMatrix* mat = GetValAsMatrix(row);
  if(mat) {
    taBase::Ref(mat);
    mat->WriteFmSubMatrix(src, render_op, off0, off1, off2, off3, off4, off5, off6);
    taBase::unRefDone(mat);
  }
}

void DataCol::WriteFmSubMatrix2DWrap(int row, const taMatrix* src,
                               taMatrix::RenderOp render_op, int off0, int off1) {
  taMatrix* mat = GetValAsMatrix(row);
  if(mat) {
    taBase::Ref(mat);
    mat->WriteFmSubMatrix2DWrap(src, render_op, off0, off1);
    taBase::unRefDone(mat);
  }
}

void DataCol::ReadToSubMatrix(int row,
    taMatrix* dest, taMatrix::RenderOp render_op,
    int off0, int off1, int off2,
    int off3, int off4, int off5, int off6) {
  taMatrix* mat = GetValAsMatrix(row);
  if(mat) {
    taBase::Ref(mat);
    mat->ReadToSubMatrix(dest, render_op, off0, off1, off2, off3, off4, off5, off6);
    taBase::unRefDone(mat);
  }
}

//////////////////

bool DataCol::GetMinMaxScale(MinMax& mm) {
  if(!isNumeric()) return false;
  int idx;
  if(valType() == VT_FLOAT) {
    float_Matrix* fm = (float_Matrix*)AR();
    mm.min = taMath_float::vec_min(fm, idx);
    mm.max = taMath_float::vec_max(fm, idx);
  }
  else if(valType() == VT_DOUBLE) {
    double_Matrix* fm = (double_Matrix*)AR();
    mm.min = taMath_double::vec_min(fm, idx);
    mm.max = taMath_double::vec_max(fm, idx);
  }
  else if(valType() == VT_INT) {
    int_Matrix* fm = (int_Matrix*)AR();
    if(fm->size > 0) {
      mm.min = fm->FastEl_Flat(0);
      mm.max = fm->FastEl_Flat(0);
      for(int i=1;i<fm->size;i++) {
        int val = fm->FastEl_Flat(i);
        if(val > mm.max) mm.max = val;
        if(val < mm.min) mm.min = val;
      }
    }
  }
  else {
    return false;               // not worth it!
  }
  return true;
}

String DataCol::EncodeHeaderName(const MatrixIndex& dims) const {
  String typ_info;
  switch (valType()) {
    case VT_STRING:       typ_info = "$"; break;
    case VT_FLOAT:        typ_info = "%"; break;
    case VT_DOUBLE:       typ_info = "#"; break;
    case VT_INT:          typ_info = "|"; break;
    case VT_BYTE:         typ_info = "@"; break;
    case VT_VARIANT:      typ_info = "&"; break;
    case VT_VOIDPTR:      typ_info = "*"; break;
    case VT_BOOL:         typ_info = "^"; break;
  }
  String mat_info;
  if(is_matrix) {               // specify which cell in matrix this is [dims:d0,d1,d2..]
    mat_info = dims.ToString("[", "]");
    if(cell_geom.IndexFmDimsN(dims) == 0) { // first guy
      mat_info += cell_geom.ToString("<", ">");
    }
  }
  return typ_info + name + mat_info; // e.g., $StringVecCol[2:2,3]
}

void DataCol::DecodeHeaderName(String nm, String& base_nm, int& vt,
    MatrixIndex& mat_idx, MatrixGeom& mat_geom)
{
  base_nm = nm;
  vt = -1; // unknown
  mat_idx.SetDims(0);
  mat_geom.SetDims(0);
  if (nm.empty()) return;

  // first check for type info:
  if (nm[0] == '$') {
    nm = nm.after('$');
    vt = VT_STRING;
  } else if (nm[0] == '%') {
    nm = nm.after('%');
    vt = VT_FLOAT;
  } else if (nm[0] == '#') {
    nm = nm.after('#');
    vt = VT_DOUBLE;
  } else if (nm[0] == '|') {
    nm = nm.after('|');
    vt = VT_INT;
  } else if (nm[0] == '@') {
    nm = nm.after('@');
    vt = VT_BYTE;
  } else if (nm[0] == '&') {
    nm = nm.after('&');
    vt = VT_VARIANT;
  } else if (nm[0] == '*') {
    nm = nm.after('*');
    vt = VT_VOIDPTR;
  } else if (nm[0] == '^') {
    nm = nm.after('^');
    vt = VT_BOOL;
  } else {
    vt = VT_STRING;             // string holds anything.. is default
  }

  if(nm.contains('[')) {
    String mat_info = nm;
    nm = nm.before('[');
    mat_idx.FromString(mat_info, "[", "]");
    if(mat_info.contains('<')) {
      mat_geom.FromString(mat_info, "<", ">");
    }
  }
  base_nm = nm;
}

taObjDiffRec* DataCol::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def,
    const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // this is same as base objdiff but puts children last to make more sense for user
  taObjDiffRec* odr = inherited::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);

  for(int i=0;i<rows();i++) {
    if(isMatrix()) {
      taMatrix* mat = ((DataCol*)this)->GetValAsMatrix(i);
      taBase::Ref(mat);
      // stream each cell separately..
      for(int j=0;j<cell_size();j++) {
        taObjDiffRec* clodr = new taObjDiffRec(odl, nest_lev+1, valTypeDef(), NULL,
            mat->FastEl_Flat_(j),
            (void*)this, GetTypeDef(), odr);
        clodr->name = String(i) + "," + String(j);      // row, cell
        clodr->ComputeHashCode();                       // need to update
        odl.Add(clodr);
      }
      taBase::unRefDone(mat);
    }
    else {
      taObjDiffRec* clodr = new taObjDiffRec(odl, nest_lev+1, valTypeDef(), NULL,
          (void*)AR()->FastEl_Flat_(i),
          (void*)this, GetTypeDef(), odr);
      clodr->name = String(i);  // row
      clodr->ComputeHashCode(); // need to update
      odl.Add(clodr);
    }
  }
  return odr;
}

void DataCol::RunClusterAnalysis() {
  dataTable()->RunAnalysis(this, AnalysisRun::CLUSTER);
}

void DataCol::RunPCA2dAnalysis() {
  dataTable()->RunAnalysis(this, AnalysisRun::PCA_2d);
}

void DataCol::RunPCA_EigenAnalysis() {
  dataTable()->RunAnalysis(this, AnalysisRun::PCA_EIGEN);
}

void DataCol::ComputeDistanceMatrix() {
  dataTable()->RunAnalysis(this, AnalysisRun::DISTANCE_MATRIX);
}

void DataCol::RunLinearRegression() {
  dataTable()->RunAnalysis(this, AnalysisRun::REGRESS_LINEAR);
}

void DataCol::GetUniqueColumnValues(String_Array& groups) {
  String_Array temp;
  for (int i=0; i<this->rows(); i++) {
    temp.Add(this->GetValAsString(i));
  }
  temp.Sort();
  groups.Reset();
  String prior = "";
  for (int i=0; i<temp.size; i++) {
    String next = temp.FastEl(i);
    if (next != prior) {
      groups.Add(next);
      prior = next;
    }
  }
}

// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#include "WtBasedRF.h"
#include <taMath_float>

#include <TiledGpRFPrjnSpec>
#include <V1RegionSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(WtBasedRF);

void WtBasedRF::Initialize() {
}

void WtBasedRF::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void WtBasedRF::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!trg_layer, quiet, rval,"trg_layer is NULL");
  CheckError(!snd_layer, quiet, rval,"snd_layer is NULL");
  String projection_name = "Fm_" + snd_layer->name;
  Projection* projection = trg_layer->projections.FindLeafName(projection_name);
  CheckError(!projection, quiet, rval,"projection " + projection_name + " not found. The projection name should be in the form Fm_NameOfLayer");
}

String WtBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(rf_data) rval += " to data: " + rf_data->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

void WtBasedRF::ConfigDataTable(DataTable* dt, Network* net, Layer* tlay) {
  dt->StructUpdate(true);
  dt->Reset();                  // nuke cols -- ensure matching
  int rows = trg_layer->units.leaves;
  int idx;
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if(lay->lesioned()) continue;
    DataCol* da;
    if(lay->unit_groups)
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
                               lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    else
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
                               lay->un_geom.y);
  }
  dt->EnforceRows(rows);
  dt->StructUpdate(false);
}

void WtBasedRF::InitAll(DataTable* dt, Network* net, Layer* tlay, Layer* slay, V1RetinaProc* rproc) {
  rf_data = dt;
  network = net;
  trg_layer = tlay;
  snd_layer = slay;
  v1_retinaProc = rproc;
  
  if(!network || !rf_data || !trg_layer || !snd_layer || !v1_retinaProc)
    return;
  rf_data->ResetData();
  InitData();
}

void WtBasedRF::InitData() {
  if(!network || !rf_data || !trg_layer || !snd_layer)
    return;
//  ConfigDataTable(rf_data, <#Network *net#>, <#Layer *tlay#>);
}

bool WtBasedRF::ComputeV2RF() {
  if (!trg_layer || !snd_layer)
    return false;
  // build weights table
  trg_layer->WeightsToTable(&trg_layer_wts, snd_layer);
  taVector2i snd_layer_unit_grp_geom = snd_layer->un_geom;
  String prjn_name = "Fm_" + snd_layer->name;
  Projection* prjn = trg_layer->projections.FindLeafName(prjn_name);
  TiledGpRFPrjnSpec* tiled_prjn_spec = dynamic_cast<TiledGpRFPrjnSpec*>(prjn->GetPrjnSpec());
  DataCol* dc = trg_layer_wts.data.SafeEl(0);
  MatrixGeom trg_layer_mg;
  trg_layer_mg.SetGeom(4, snd_layer_unit_grp_geom.x, snd_layer_unit_grp_geom.y, tiled_prjn_spec->send_gp_size.x, tiled_prjn_spec->send_gp_size.y);
  dc->ChangeColCellGeom(trg_layer_mg);
  
  // config the rf_data table
  rf_data->StructUpdate(true);
  rf_data->Reset();                  // nuke cols -- ensure matching
  int rows = trg_layer_wts.rows;
  taVector2i snd_layer_grp_geom = snd_layer->gp_geom;
  DataCol* rf_data_values_col;
  DataCol* rf_data_count_col;
  // column of calculated image values
  rf_data_values_col = rf_data->FindMakeColMatrix("values", VT_FLOAT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  // column to keep track of the number of values summed so we can correctly average
  rf_data_count_col = rf_data->FindMakeColMatrix("count", VT_FLOAT, 2, snd_layer_grp_geom.x, snd_layer_grp_geom.y);
  rf_data->EnforceRows(rows);
  rf_data->StructUpdate(false);

  V1RegionSpec* region_spec = dynamic_cast<V1RegionSpec*>(v1_retinaProc->regions.SafeEl(0));
  region_spec->GridGaborFilters(&v1Gabor_GridFilters);
  float_Matrix* filter = NULL;
  // the V1 unit group geometry is the same as the filter
  filter = new float_Matrix[snd_layer_unit_grp_geom.x * snd_layer_unit_grp_geom.y];
  for (int col=0; col<snd_layer_unit_grp_geom.x; col++) {
    for (int row=0; row<snd_layer_unit_grp_geom.y; row++) {
      if (row % 2 == 0) {
        filter[2*col+row] = v1Gabor_GridFilters.GetValAsMatrixColName("Filter", col);
      }
      else {
        filter[2*col+row] = filter[2*col+row-1] * -1;
      }
    }
  }
  
  int filter_size_x;
  int filter_size_y;
  DataCol* filter_col = v1Gabor_GridFilters.GetColData("Filter");
  if(TestError(!filter_col, "ComputeV2RF", "filter column not found in v1Gabor_GridFilters table"))
    return false;
  filter_col->Get2DCellGeom(filter_size_x, filter_size_y);
  
  float_Matrix* sum_matrix = new float_Matrix(); // sums of filter * weights
  float_Matrix* tmp_matrix = new float_Matrix(); // filter * weights (gets added to running sum)
  float_Matrix* marker_matrix = new float_Matrix(); // all 1's - used to mark the large count matrix for averaging of summed values
  float_Matrix* count_matrix = new float_Matrix(); // tracks the number of values that are summed for each unit of the original image
  sum_matrix->SetGeom(2, filter_size_x, filter_size_y);
  tmp_matrix->SetGeom(2, filter_size_x, filter_size_y);
  marker_matrix->SetGeom(2, filter_size_x, filter_size_y);
  *marker_matrix = 1;
  count_matrix->SetGeom(2, snd_layer_grp_geom.x, snd_layer_grp_geom.y); // same size as rf_data
  
  for (int wts_row=0; wts_row<trg_layer_wts.rows-1; wts_row++) {
    Unit* trg_layer_unit = trg_layer->UnitAccess(Layer::ACC_LAY, wts_row, 0);
    int unit_no = 0;  //
    ConGroup* recv_cons = trg_layer_unit->FindRecvConGroupFrom(snd_layer);
    int dim_0;  // filter angle
    int dim_1;  // filter on_center/off_center
    for (int dim_3=0; dim_3<trg_layer_mg.dim(3); dim_3++) {
      for (int dim_2=0; dim_2<trg_layer_mg.dim(2); dim_2++) {
        *sum_matrix = 0;  //reset
        for (dim_1=0; dim_1<trg_layer_mg.dim(1); dim_1++) {
          for (dim_0=0; dim_0<trg_layer_mg.dim(0); dim_0++) {
            int filter_no = dim_0 + 2*dim_1;
            DataCol* wts_col = trg_layer_wts.GetColData(0);  // only one column
            float weight = wts_col->GetValAsFloatMDims(wts_row, dim_0, dim_1, dim_2, dim_3);
            //            taMisc::DebugInfo((String)weight);
            for (int i=0; i<tmp_matrix->size; i++) {
              tmp_matrix->FastEl1d(i) = filter[filter_no].FastEl1d(i) * weight;
            }
            *sum_matrix += *tmp_matrix; //
          }
        }
        Unit* snd_layer_unit = recv_cons->SafeUn(unit_no);
        taVector2i snd_layer_log_pos;
        snd_layer_unit->LayerLogPos(snd_layer_log_pos);
        unit_no = unit_no + (dim_0 * dim_1);  // the first unit of each group gets us what we need - jump to next group
        snd_layer_log_pos.x = snd_layer_log_pos.x/dim_0;
        snd_layer_log_pos.y = snd_layer_log_pos.y/dim_1;
        // write the sum values to the rf_data table
        rf_data_values_col->WriteFmSubMatrix(wts_row, sum_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
        // up the count for those cells of rf_data we just added to
        count_matrix->WriteFmSubMatrix(marker_matrix, taMatrix::ADD, snd_layer_log_pos.x, snd_layer_log_pos.y);
      }
    }
    rf_data_count_col->SetValAsMatrix(count_matrix, wts_row);
    // can't divide by zero
    count_matrix[count_matrix == 0] = 1;
    rf_data_values_col->WriteFmSubMatrix(wts_row, count_matrix, taMatrix::DIV, 0, 0);
  }
  delete [] filter;
  filter = NULL;
  delete sum_matrix;
  delete tmp_matrix;
  delete marker_matrix;
  delete count_matrix;
  return true;
}

bool	WtBasedRF::ComputeHigherLayerRF() {
  return true;
}

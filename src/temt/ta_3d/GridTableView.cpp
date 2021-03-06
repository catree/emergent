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

#include "GridTableView.h"
#include <GridColView>
#include <DataTable>
#include <T3ExaminerViewer>
#include <T3GridViewNode>
#include <T3GridColViewNode>
#include <T3Panel>
#include <taSigLinkItr>
#include <MainWindowViewer>
#include <NewViewHelper>
#include <T3Misc>
#include <iViewPanelOfGridTable>
#include <iT3Panel>
#include <taMath_float>
#include <taProject>
#include <taSvg>

#include <taMisc>

#ifdef TA_QT3D

#include <T3LineStrip>
#include <T3TwoDText>
#include <T3LineBox>
#include <T3MatrixGrid>
#include <T3Cube>

#else // TA_QT3D

#include <SoScrollBar>
#include <SoImageEx>
#include <SoMatrixGrid>

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoEventManager.h>

#endif // TA_QT3D


TA_BASEFUNS_CTORS_DEFN(GridTableView);


GridTableView* DataTable::NewGridView(T3Panel* fr) {
  return GridTableView::New(this, fr);
}

GridTableView* DataTable::FindMakeGridView(T3Panel* fr) {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    GridTableView* el;
    FOR_DLC_EL_OF_TYPE(GridTableView, el, dl, itr) {
      // update from user stuff
      el->UpdateFromDataTable(true); // pretend its the first time -- initfromuserdata
      el->InitDisplay();
      el->UpdateDisplay();
      fr = el->GetFrame();
      if(fr) {
        MainWindowViewer* mwv = GET_OWNER(fr, MainWindowViewer);
        if(mwv) {
          mwv->SelectT3ViewTabName(fr->name);
        }
      }
      return el;
    }
  }

  return GridTableView::New(this, fr);
}

GridTableView* DataTable::FindGridView() {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    GridTableView* el;
    FOR_DLC_EL_OF_TYPE(GridTableView, el, dl, itr) {
      return el;
    }
  }
  return NULL;
}

bool DataTable::GridViewGotoRow(int row_no) {
  GridTableView* gv = FindGridView();
  if(!gv) return false;
  gv->ViewRow_At(row_no);
  return true;
}

GridTableView* GridTableView::New(DataTable* dt, T3Panel*& fr) {
  NewViewHelper new_net_view(fr, dt, "table");
  if (!new_net_view.isValid()) return NULL;

  GridTableView* vw = new GridTableView;
  fr->AddView(vw);
  vw->setDataTable(dt);
  // make sure we get it all setup!
  vw->BuildAll();
  vw->GetScaleRange();

  new_net_view.showFrame();
  return vw;
}

iViewPanelOfGridTable* GridTableView::lvp() {
  return (iViewPanelOfGridTable*)(iViewPanelOfDataTable*)m_lvp;
}

void GridTableView::Initialize() {
  view_rows = 10;
  view_cols = 5;
  page_cols = view_cols; // default to view size
  page_rows = view_rows; // default to view size
  use_custom_paging = false;

  need_scale_update = true;
  
  width = 1.0f;
  grid_on = true;
  header_on = true;
  row_num_on = false;
  two_d_font = false;           // true -- this is causing mysterious so crash, disabled for now
  two_d_font_scale = 350.0f;
  mat_val_text = false;

  grid_margin = 0.01f;
  grid_line_size = 0.002f;
  row_num_width = 4;

  mat_block_spc = 0.1f;
  mat_block_height = 0.0f;
  mat_rot = 0.0f;
  mat_trans = 0.6f;

  mat_size_range.min = 4;
  mat_size_range.max = 16;
  text_size_range.min = .02f;
  text_size_range.max = .05f;

  scrolling_ = false;

  colorscale.auto_scale = false;
  colorscale.SetMinMax(-1.0f, 1.0f);

  row_height = 0.1f; // non-zero dummy value
  head_height = .1f;
  font_scale = .1f;

  click_vals = false;
  lmb_val = 1.0f;
  mmb_val = 0.0f;

  last_sel_val = 0.0f;
  last_sel_got = false;

  children.SetBaseType(&TA_GridColView); // subclasses need to set this to their type!
}

void GridTableView::InitLinks() {
  inherited::InitLinks();
  taBase::Own(mat_size_range, this);
  taBase::Own(text_size_range, this);
  taBase::Own(col_range, this);
  taBase::Own(colorscale, this);
  taBase::Own(vis_cols, this);
  taBase::Own(col_widths_raw, this);
  taBase::Own(col_widths, this);
}

void GridTableView::CutLinks() {
  colorscale.CutLinks();
  col_range.CutLinks();
  inherited::CutLinks();
}

void GridTableView::Copy_(const GridTableView& cp) {
  view_cols = cp.view_cols;
  page_cols = cp.page_cols;
  col_range = cp.col_range;
  width = cp.width;
  grid_on = cp.grid_on;
  header_on = cp.header_on;
  row_num_on = cp.row_num_on;
  two_d_font = cp.two_d_font;
  two_d_font_scale = cp.two_d_font_scale;
  mat_val_text = cp.mat_val_text;
  colorscale = cp.colorscale;

  grid_margin = cp.grid_margin;
  grid_line_size = cp.grid_line_size;
  row_num_width = cp.row_num_width;
  mat_block_spc = cp.mat_block_spc;
  mat_block_height = cp.mat_block_height;
  mat_rot = cp.mat_rot;
  mat_trans = cp.mat_trans;

  mat_size_range = cp.mat_size_range;
  text_size_range = cp.text_size_range;
}

void GridTableView::CopyFromView(GridTableView* cp) {
  Copy_(*cp);                   // get stuff as above
  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    GridColView* cpvs = (GridColView*)cp->children.FindName(cvs->name);
    if(cpvs) {
      cvs->CopyFromView(cpvs);
    }
  }
  T3DataViewMain::CopyFromViewFrame(cp);
}

void GridTableView::UpdateAfterEdit_impl(){
  inherited::UpdateAfterEdit_impl();
  if (grid_margin < 0.0f) grid_margin = 0.0f;
  if (grid_line_size <  0.0f) grid_line_size =  0.0f;
}

void GridTableView::SigRecvUpdateView_impl() {
  if(!display_on) return;
  // int old_rows;
  // int delta_rows = CheckRowsChanged(old_rows);
  
  if (!isVisible()) return;

#ifdef TA_QT3D
  // Render();                     // this is brute force and works, for now..
  // return;
  //  RemoveGraph();
#endif // TA_QT3D
  UpdateDisplay(true);
}

void GridTableView::UpdateName() {
  DataTable* dt = dataTable();
  if (dt) {
    if (!name.contains(dt->name))
      SetName(dt->name + "_Grid");
  }
  else {
    if (name.empty())
      SetName("grid_no_table");
  }
}

const String GridTableView::caption() const {
  String rval = inherited::caption();
  if(last_sel_got) {
//     rval += " val: " + last_sel_col_nm + " = " + String(last_sel_val);
    rval += " val: " + String(last_sel_val);
  }
  return rval;
}

void GridTableView::InitFromUserData() {
  DataTable* dt = dataTable();
  if(!dt) return;
  if(dt->HasUserData("N_ROWS")) {
    view_rows = dt->GetUserDataAsInt("N_ROWS");
  }
  if(dt->HasUserData("AUTO_SCALE")) {
    colorscale.auto_scale = true;
  }
  if(dt->HasUserData("WIDTH")) {
    width = dt->GetUserDataAsFloat("WIDTH");
  }
  if(dt->HasUserData("SCALE_MIN")) {
    colorscale.min = dt->GetUserDataAsFloat("SCALE_MIN");
  }
  if(dt->HasUserData("SCALE_MAX")) {
    colorscale.max = dt->GetUserDataAsFloat("SCALE_MAX");
  }
  if(dt->HasUserData("BLOCK_HEIGHT")) {
    mat_block_height = dt->GetUserDataAsFloat("BLOCK_HEIGHT");
  }
  if(dt->HasUserData("BLOCK_SPACE")) {
    mat_block_spc = dt->GetUserDataAsFloat("BLOCK_SPACE");
  }
  if(dt->HasUserData("MAT_VAL_TEXT")) {
    mat_val_text = true;
  }
  if(dt->HasUserData("HEADER_OFF")) {
    header_on = false;
  }
  if(dt->HasUserData("ROW_NUM_ON")) {
    row_num_on = true;
  }
  if(dt->HasUserData("TWO_D_FONT")) {
    two_d_font = true;
  }
  if(dt->HasUserData("TWO_D_FONT_SCALE")) {
    two_d_font_scale = dt->GetUserDataAsFloat("TWO_D_FONT_SCALE");
  }
  if(dt->HasUserData("MAT_ROT")) {
    mat_rot = dt->GetUserDataAsFloat("MAT_ROT");
  }
  if(dt->HasUserData("MAT_TRANS")) {
    mat_trans = dt->GetUserDataAsFloat("MAT_TRANS");
  }
}

void GridTableView::UpdateFromDataTable_this(bool first) {
  inherited::UpdateFromDataTable_this(first);
  if (first) {
    InitFromUserData();
  }
}

void GridTableView::DataUnitsXForm(taVector3f& pos, taVector3f& size) {
  float gr_mg_sz2 = 2.0f * grid_margin;

  int trow = (int)pos.y;
  trow = MAX(0, trow);
  float rrmdr = (pos.y - (float)trow);

  pos.y = row_height * trow;
  if(rrmdr != 0.0f) {
    pos.y += grid_margin + rrmdr * (row_height - gr_mg_sz2);
  }
  size.y = (row_height - gr_mg_sz2) * size.y;

  int tcol = (int)pos.x;
  tcol = MAX(0, tcol);
  float crmdr = (pos.x - (float)tcol);

  pos.x = 0.0f;
  int col_idx = 0;
  int col_ctr = 0;
  if(row_num_on)
    pos.x = col_widths[col_idx++];

  for (int col = col_range.min; col <= col_range.max; col++) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    DataCol* dc = cvs->dataCol();
    if(col_ctr < tcol) {
      pos.x += col_widths[col_idx++];
      col_ctr++;
      continue;
    }
    // this is our target column
    if(crmdr != 0.0f) {
      pos.x += grid_margin + crmdr * (col_widths[col_idx] - gr_mg_sz2);
    }
    size.x *= col_widths[col_idx];
    break;
  }
}

#ifndef TA_QT3D
void T3GridViewNode_MouseCB(void* userData, SoEventCallback* ecb);
#endif // TA_QT3D

void GridTableView::Render_pre() {
  bool show_drag = manip_ctrl_on;
  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
    if(!vw->interactionModeOn())
      show_drag = false;
  }

#ifdef TA_QT3D
  setNode(new T3GridViewNode(NULL, this, width, show_drag));
#else // TA_QT3D
  setNode(new T3GridViewNode(this, width, show_drag));
  if(vw && vw->interactionModeOn()) {
    SoEventCallback* ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(), T3GridViewNode_MouseCB, this);
    node_so()->addChild(ecb);
  }
#endif // TA_QT3D
  
  colorscale.SetColorSpec(colorscale.spec);  // Call set to force the saved color to be restored
  UpdatePanel();                // otherwise doesn't get updated without explicit click..
  inherited::Render_pre();
}

void GridTableView::Render_impl() {
  inherited::Render_impl();
  T3GridViewNode* node_so = this->node_so(); // cache
  if(!node_so || !dataTable())
    return;
  node_so->setWidth(width);     // does a render too -- ensure always up to date on width
  int orig_rows;
  CheckRowsChanged(orig_rows);  // don't do anything with this here, but just make sure m_rows is up to date
  MakeViewRangeValid();
  CalcViewMetrics();
  GetScaleRange();
  SetScrollBars();
  RenderGrid();
  RenderHeader();
  RenderLines();
}

void GridTableView::InitDisplay(bool init_panel) {
  RemoveLines();
  RemoveHeader();
  RemoveGrid();
  MakeViewRangeValid();

  if(init_panel) {
    InitPanel();
  }
}

void GridTableView::UpdateDisplay(bool update_panel) {
  int old_rows;
  int delta_rows = CheckRowsChanged(old_rows);

  if (!isVisible()) return;

  if(delta_rows > 0) {
    // if we were not at the very end, then don't scroll, but do update the panel
    if(update_panel && (view_range.max < old_rows-1) && (view_range.max > 0)) {
      UpdatePanel();
#ifdef TA_QT3D
      // Render();                     // this is brute force and works, for now..
      // return;
      //  RemoveGraph();
#endif // TA_QT3D
      Render_impl();
      return;
    }
    // scroll down to end of new data
    view_range.max = m_rows - 1;
    view_range.min = view_range.max - view_rows + 1;
    view_range.min = MAX(0, view_range.min);
  }

  if (update_panel) UpdatePanel();

#ifdef TA_QT3D
  // Render();                     // this is brute force and works, for now..
  // return;
  //  RemoveGraph();
#endif // TA_QT3D
  
  Render_impl();
}

void GridTableView::MakeViewRangeValid() {
  DataTable* dt = dataTable();
  if(!dt) return;
  if(children.size != dt->data.size) { // just to make sure..
    UpdateFromDataTable();
  }
  inherited::MakeViewRangeValid();

  // get the list of visible columns
//  vis_cols_prior.Copy(vis_cols);  // save for comparison
  vis_cols.Reset();
  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    if(cvs->isVisible()) vis_cols.Add(i);
  }

  int cols = vis_cols.size;
  if (col_range.min >= cols) {
    col_range.min = MAX(0, (cols - view_cols - 1));
  }
  if(cols == 0) {
    col_range.max = -1;
    return;
  }
  col_range.max = col_range.min + view_cols-1;
  col_range.MaxLT(cols - 1); // keep it less than max
}

void GridTableView::CalcViewMetrics() {
  DataTable* dt = dataTable();
  if(!dt) return;

  for(int i=0;i<children.size;i++) {
    GridColView* cvs = (GridColView*)colView(i);
    cvs->ComputeColSizes();
  }

  float tot_wd_raw = 0.0f;
  col_widths_raw.Reset();
  if(row_num_on) {
    col_widths_raw.Add((float)row_num_width);
    tot_wd_raw += (float)row_num_width;
  }

  bool has_mat = false;         // has a matrix?
  row_height_raw = 1.0f;
  for(int col = col_range.min; col<=col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if(!cvs) continue;
    if(cvs->dataCol()->is_matrix)
      has_mat = true;
    col_widths_raw.Add(cvs->col_width);
    tot_wd_raw += cvs->col_width;
    row_height_raw = MAX(row_height_raw, cvs->row_height);
  }

  if(tot_wd_raw == 0.0f) return;        // something wrong

  // now normalize
  col_widths.SetSize(col_widths_raw.size);

  bool redo = false;
 normalize: // can goto here to redo..
  for(int i=0;i<col_widths_raw.size;i++) {
    col_widths[i] = width * (col_widths_raw[i] / tot_wd_raw);
  }

  // compute row height: if no header, it is simply 1.0 / view_rows
  float tot_ht_raw = row_height_raw * view_rows;
  if(!header_on) {
    row_height = 1.0f / view_rows;
    head_height = 0.0f;
  }
  else {
    // otherwise, need to figure out how much head height is in proportion to everything else
    tot_ht_raw += 1.0f; // header = 1.0
    float per_raw = 1.0f / tot_ht_raw;
    head_height = per_raw;
    row_height = row_height_raw * per_raw;
  }

  // finally: compute font scale for entire display.  this is the minimum size based
  // on row/col constraints (units are 1 char, so that should be easy):
  // note that scale is in units of height, so col width needs to be converted to height

  float col_font_scale = T3Misc::char_ht_to_wd_pts / tot_wd_raw;
  //  float row_font_scale = (row_height / row_height_raw);
  float row_font_scale = row_height;

  if(!redo) {
    font_scale = MIN(col_font_scale, row_font_scale);
    if(font_scale > text_size_range.max && has_mat) {
      redo = true;              // don't do more than once
      float red_rat = text_size_range.max / font_scale;
      // we have a matrix and font size is too big: rescale it and then recompute
      font_scale = text_size_range.max;
      tot_wd_raw = 0.0f;
      int cidx = 0;
      if(row_num_on) {
        col_widths_raw[cidx] *= red_rat;
        tot_wd_raw += col_widths_raw[cidx];
        cidx++;
      }
      for(int col = col_range.min; col<=col_range.max; ++col) {
        GridColView* cvs = (GridColView*)colVis(col);
        if(!cvs) continue;
        if(!cvs->dataCol()->is_matrix)
          col_widths_raw[cidx] *= red_rat;
        tot_wd_raw += col_widths_raw[cidx];
        cidx++;
      }
      goto normalize;           // redo
    }
  }

  if(font_scale < text_size_range.min) {
    font_scale = text_size_range.min; // that's just it!
  }
  // now just enforce max
  if(font_scale > text_size_range.max) {
    font_scale = text_size_range.max; // that's just it!
  }

  // need another iteration now that we know the true font scale!
  if(header_on) {
    head_height = font_scale;
    float per_raw = (1.0f - head_height) / (tot_ht_raw - 1.0f);
    row_height = row_height_raw * per_raw;
  }
}

void GridTableView::GetScaleRange() {
  if(!colorscale.auto_scale) {
    colorscale.FixRangeZero();
    return;
  }
  
  if (need_scale_update) {
    bool got_one = false;
    MinMax sc_rg;
    for(int col = col_range.min; col<=col_range.max; ++col) {
      GridColView* cvs = (GridColView*)colVis(col);
      if(!cvs || !cvs->scale_on)
        continue;
      DataCol* da = cvs->dataCol();
      if(!da->isNumeric() || !da->is_matrix) continue;
      da->GetMinMaxScale(sc_rg);
      sc_rg.SymRange();           // keep range symmetric around zero!
      if(!got_one)
        colorscale.SetMinMax(sc_rg.min, sc_rg.max);
      else
        colorscale.UpdateMinMax(sc_rg.min, sc_rg.max);
      got_one = true;
    }
    need_scale_update = false; // just did it
  }
}

#ifndef TA_QT3D
void GridTableView_ColScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GridTableView* gtv = (GridTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewCol_At(val);
  gtv->scrolling_ = false;
}

void GridTableView_RowScrollCB(SoScrollBar* sb, int val, void* user_data) {
  GridTableView* gtv = (GridTableView*)user_data;
  gtv->scrolling_ = true;
  gtv->ViewRow_At(val);
  gtv->scrolling_ = false;
}
#endif // TA_QT3D

void GridTableView::SetScrollBars() {
  if(scrolling_) return;                     // do't redo if currently doing!
  T3GridViewNode* node = this->node_so(); // cache
  if(!node) return;

#ifdef TA_QT3D
#else // TA_QT3D
  SoScrollBar* csb = node->ColScrollBar();
//   csb->setMinimum(0);
//   csb->setSingleStep(1);
  int eff_col_n = MIN(view_cols, vis_cols.size);
  csb->setMaximum(vis_cols.size - eff_col_n);
  csb->setPageStep(eff_col_n);
  csb->setValue(col_range.min);
  csb->setValueChangedCB(GridTableView_ColScrollCB, this);

  SoScrollBar* rsb = node->RowScrollBar();
//   rsb->setMinimum(0);
//   rsb->setSingleStep(1);
  int mx = MAX((rows() - view_rows), 0);
  rsb->setMaximum(mx);
  int pg_step = MAX(view_rows, 1);
  rsb->setPageStep(pg_step);
  rsb->setValue(MIN(view_range.min, mx));
  rsb->setValueChangedCB(GridTableView_RowScrollCB, this);
#endif // TA_QT3D
}

void GridTableView::ClearViewRange() {
  col_range.min = 0;
  inherited::ClearViewRange();
}

void GridTableView::Clear_impl() {
  RemoveHeader();
  RemoveGrid();
  RemoveLines();
  inherited::Clear_impl();
}

void GridTableView::OnWindowBind_impl(iT3Panel* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_lvp) {
    m_lvp = new iViewPanelOfGridTable(this);
    vw->RegisterPanel(m_lvp);
  }
}

void GridTableView::RemoveGrid() {
  T3GridViewNode* node = this->node_so();
  if (node) {
#ifdef TA_QT3D
    node->grid->restart();
#else // TA_QT3D
    node->grid()->removeAllChildren();
#endif // TA_QT3D
  }
}

void GridTableView::RemoveHeader() {
  T3GridViewNode* node = this->node_so();
  if (node) {
#ifdef TA_QT3D
    node->header->removeAllChildren();
#else // TA_QT3D
    node->header()->removeAllChildren();
#endif // TA_QT3D
  }
}

void GridTableView::RemoveLines(){
  T3GridViewNode* node = this->node_so();
  if (!node) return;
#ifdef TA_QT3D
  node->body->removeAllChildren();
#else // TA_QT3D
  node->body()->removeAllChildren();
#endif // TA_QT3D
}

void GridTableView::SaveImageSVG(const String& svg_fname) {
  T3ExaminerViewer* vw = GetViewer();
  if(!vw) return;
  render_svg = true;
  svg_str = "";
  svg_str << taSvg::Header(vw, this);
  taSvg::cur_inst->coord_mult.z = -1.0f;
  RenderGrid();
  RenderHeader();
  RenderLines();
  RenderAnnoteSvg();
  svg_str << taSvg::Footer();
  render_svg = false;
  svg_str.SaveToFile(svg_fname);
}

void GridTableView::RenderGrid() {
  T3GridViewNode* node = this->node_so();
  if (!node) return;

#ifdef TA_QT3D
  T3LineStrip* grid = node->grid;
  grid->setNodeUpdating(true);
  grid->TranslateLLFSz1To(QVector3D(0.0f, 0.0f, 0.0f), width, 0.0f);
  grid->line_width = grid_line_size;
#else // TA_QT3D
  SoGroup* grid = node->grid();
  grid->removeAllChildren(); // should have been done
#endif // TA_QT3D
  if (!grid_on) return;

  float gr_ln_sz = grid_line_size;

  // vertical lines
#ifdef TA_QT3D
#else // TA_QT3D
  SoSeparator* vert = new SoSeparator;
  // note: VERY unlikely to not need at least one line, so always make it
  SoCube* ln = new SoCube;
  ln->ref(); // our unref later will delete it if we never use it
  ln->width = gr_ln_sz;
  ln->depth = gr_ln_sz; //note: give it depth to try to avoid disappearing lines issue
  ln->height = 1.0f;
  SoTranslation* tr = new SoTranslation();
  vert->addChild(tr);
  tr->translation.setValue(0.0f, -.5f, 0.0f);
#endif // TA_QT3D

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  float col_pos = 0.0f;

  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    col_pos += col_wd_lst;
#ifdef TA_QT3D
    grid->moveTo(col_pos, 0.0f, 0.0f);
    grid->lineTo(col_pos, 1.0f, 0.0f);
#else // TA_QT3D
    SoTranslation* tr = new SoTranslation();
    vert->addChild(tr);
    tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    vert->addChild(ln);
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Path(iColor(), 1.0f) // gr_ln_sz 
              << "M " << taSvg::Coords(col_pos, 0.0f, 0.0f)
              << "L " << taSvg::Coords(col_pos, 1.0f, 0.0f)
              << taSvg::PathEnd();
    }
  }

  for (int col = col_range.min; col < col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    col_wd_lst = col_widths[col_idx++];
    col_pos += col_wd_lst;
#ifdef TA_QT3D
    grid->moveTo(col_pos, 0.0f, 0.0f);
    grid->lineTo(col_pos, 1.0f, 0.0f);
#else // TA_QT3D
    SoTranslation* tr = new SoTranslation();
    vert->addChild(tr);
    tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    vert->addChild(ln);
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Path(iColor(), 1.0f) // gr_ln_sz
              << "M " << taSvg::Coords(col_pos, 0.0f, 0.0f)
              << "L " << taSvg::Coords(col_pos, 1.0f, 0.0f)
              << taSvg::PathEnd();
    }
  }

  // horizontal lines
#ifdef TA_QT3D
#else // TA_QT3D
  grid->addChild(vert);
  ln->unref(); // deleted if not used

  SoSeparator* horiz = new SoSeparator;
  // note: VERY unlikely to not need at least one line, so always make it
  ln = new SoCube;
  ln->ref(); // our unref later will delete it if we never use it
  ln->width = width;
  ln->depth = gr_ln_sz; //note: give it depth to try to avoid disappearing lines issue
  ln->height = gr_ln_sz;
  tr = new SoTranslation();
  horiz->addChild(tr);
  tr->translation.setValue(.5f * width, 0.0f, 0.0f);
#endif // TA_QT3D

  float row_pos = 0.0f;

  if (header_on) {
    row_pos += head_height;
#ifdef TA_QT3D
    grid->moveTo(0.0f, 1.0f-row_pos, 0.0f);
    grid->lineTo(width, 1.0f-row_pos, 0.0f);
#else // TA_QT3D
    SoTranslation* tr = new SoTranslation;
    horiz->addChild(tr);
    tr->translation.setValue(0.0f, -head_height, 0.0f);
    horiz->addChild(ln);
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Path(iColor(), 1.0f) // gr_ln_sz
              << "M " << taSvg::Coords(0.0f, 1.0f-row_pos, 0.0f)
              << "L " << taSvg::Coords(width, 1.0f-row_pos, 0.0f)
              << taSvg::PathEnd();
    }
  }

  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    row_pos += row_height;
#ifdef TA_QT3D
    grid->moveTo(0.0f, 1.0f-row_pos, 0.0f);
    grid->lineTo(width, 1.0f-row_pos, 0.0f);
#else // TA_QT3D
    SoTranslation* tr = new SoTranslation();
    horiz->addChild(tr);
    tr->translation.setValue(0.0f, -row_height, 0.0f);
    horiz->addChild(ln);
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Path(iColor(), 1.0f) // gr_ln_sz
              << "M " << taSvg::Coords(0.0f, 1.0f-row_pos, 0.0f)
              << "L " << taSvg::Coords(width, 1.0f-row_pos, 0.0f)
              << taSvg::PathEnd();
    }
  }
#ifdef TA_QT3D
   grid->setNodeUpdating(false);
#else // TA_QT3D
  grid->addChild(horiz);
  ln->unref(); // deleted if not used
#endif // TA_QT3D
}

void GridTableView::RenderHeader() {
  T3GridViewNode* node = this->node_so();
  if (!node) return;

#ifdef TA_QT3D
  T3Entity* hdr = node->header;
#else // TA_QT3D
  // safely/correctly clear all the column headers
  // we remove first manually from us...
  SoSeparator* hdr = node->header();
#endif // TA_QT3D

  hdr->removeAllChildren();
  // and then remove ref in the ColView guys
  for (int i = 0; i < colViewCount(); i++) {
    GridColView* gcv = (GridColView*)colView(i);
    gcv->Clear();
  }

  if(!header_on || dataTable()->data.size == 0) return; // normally shouldn't be called if off

#ifdef TA_QT3D
#else // TA_QT3D
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  hdr->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = (const char*)taMisc::t3d_font_name;
  if(two_d_font)
    fnt->size.setValue(two_d_font_scale * font_scale);
  else
    fnt->size.setValue(font_scale);
  hdr->addChild(fnt);
#endif // TA_QT3D

  if(render_svg) {
    svg_str << taSvg::Group();
  }

  T3Panel* fr = GetFrame();
  iColor txtcolr = fr->GetTextColor();

  float gr_mg_sz = grid_margin;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  // margin and baseline adj
  float base_adj = (head_height * T3Misc::char_base_fract);
#ifdef TA_QT3D
  hdr->TranslateLLFSz1To(QVector3D(0.0f, (head_height - base_adj), -gr_mg_sz), width, 0.0f);
#else // TA_QT3D
  SoTranslation* tr = new SoTranslation();
  hdr->addChild(tr);
  tr->translation.setValue(0.0f, - (head_height - base_adj), -gr_mg_sz); // set back just a hair
#endif // TA_QT3D

  float row_pos = head_height - base_adj;

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col
  float col_pos = 0.0f;

  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
#ifdef TA_QT3D
    T3TwoDText* txt = new T3TwoDText(hdr);
    txt->setText("#");
    txt->setTextColor(txtcolr);
    txt->Scale(font_scale);
    txt->Translate
      (QVector3D(col_pos + 0.5f * col_wd_lst, 1.0f - row_pos, -gr_mg_sz));
#else // TA_QT3D
    SoSeparator* txsep = new SoSeparator;
    hdr->addChild(txsep);
    SoTranslation* ttr = new SoTranslation();
    txsep->addChild(ttr);
    ttr->translation.setValue(.5f * col_wd_lst, 0.0f, 0.0f);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      txsep->addChild(txt);
      txt->string.setValue("#");
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      txsep->addChild(txt);
      txt->string.setValue("#");
    }
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Text(String("#"), col_pos + .5f * col_wd_lst, 1.0f - row_pos,
                             -gr_mg_sz, iColor(), font_scale, taSvg::CENTER);
    }
    col_pos += col_wd_lst;
  }

  for (int col = col_range.min; col <= col_range.max; ++col) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    if (col_wd_lst > 0.0f) {
#ifdef TA_QT3D
#else // TA_QT3D
      tr = new SoTranslation();
      hdr->addChild(tr);
      tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
#endif // TA_QT3D
    }
    col_wd_lst = col_widths[col_idx++]; // note incr
    int max_chars = (int)(T3Misc::char_ht_to_wd_pts * col_wd_lst / font_scale) + 1;
    String cnm = cvs->GetDisplayName().elidedTo(max_chars);

#ifdef TA_QT3D
    // T3GridColViewNode* colnd = cvs->MakeGridColViewNode(); //note: non-standard semantics
    T3TwoDText* txt = new T3TwoDText(hdr);
    txt->setText(cnm);
    txt->setTextColor(txtcolr);
    txt->Scale(font_scale);
    txt->Translate(col_pos + 0.5f * col_wd_lst, 1.0f - row_pos, 0.0f);
#else // TA_QT3D
    T3GridColViewNode* colnd = cvs->MakeGridColViewNode(); //note: non-standard semantics
    SoSeparator* colsep = colnd->topSeparator();
    colnd->material()->diffuseColor.setValue(txtcolr.redf(), txtcolr.greenf(),
                                             txtcolr.bluef());
    SoTranslation* ttr = new SoTranslation();
    ttr->translation.setValue(.5f * col_wd_lst, 0.0f, 0.0f);
    colsep->addChild(ttr);
    hdr->addChild(colnd);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      colsep->addChild(txt);
      txt->string.setValue(cnm.chars());
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      colsep->addChild(txt);
      txt->string.setValue(cnm.chars());
    }
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Text(cnm, col_pos + .5f * col_wd_lst, 1.0f - row_pos,
                             -gr_mg_sz, iColor(), font_scale, taSvg::CENTER);
    }

    // light aqua transparent background
#ifdef TA_QT3D
    T3Cube* rect = new T3Cube(hdr);
    rect->setSize(col_wd_lst - gr_mg_sz2, head_height, gr_mg_sz);
    rect->Translate(col_pos + 0.5f * col_wd_lst, 1.0f - row_pos, -1.05f * gr_mg_sz);
    rect->color_type = T3ColorEntity::TRANS;
    rect->setColor(QColor::fromRgbF(0.0f, 1.0f, 1.0f, 0.4f), 0.2f, 0.01f, 150.0f);
    // rect->setColor(QColor::fromRgbF(0.0f, 1.0f, 1.0f, 1.0f));
#else // TA_QT3D
    SoTranslation* rectr = new SoTranslation();
    colnd->addChild(rectr);
    rectr->translation.setValue(0.0f, .5f * head_height-base_adj, 0.0f);
    SoMaterial* rectmat = new SoMaterial;
    rectmat->diffuseColor.setValue(0.0f, 1.0f, 1.0f);
    rectmat->transparency.setValue(0.8f);
    colnd->topSeparator()->addChild(rectmat);
    SoCube* rect = new SoCube;
    rect->width = col_wd_lst - gr_mg_sz2;
    rect->height = head_height;
    rect->depth = gr_mg_sz;
    colnd->topSeparator()->addChild(rect);
    cvs->setNode(colnd);
#endif // TA_QT3D

    col_pos += col_wd_lst;
  }

  if(render_svg) {
    svg_str << taSvg::GroupEnd();
  }
}

// todo: support renderValues() for matrixgrid??

void GridTableView::RenderLine(int view_idx, int data_row) {
  T3GridViewNode* node = this->node_so();
  if (!node) return;
  float gr_mg_sz = grid_margin;
  float gr_mg_sz2 = 2.0f * gr_mg_sz;
  // origin is top-left of body area
  DataTable* dt = dataTable(); //cache
  String el;

  float row_pos = head_height + (row_height * (float)view_idx);
  float text_ht = font_scale;
  // following metrics will be adjusted for mat font scale, when necessary
  float txt_base_adj = text_ht * T3Misc::char_base_fract;

  T3Panel* fr = GetFrame();
  iColor txtcolr = fr->GetTextColor();

  float y_offs = ((row_height - text_ht) * 0.5f) + text_ht - txt_base_adj;

#ifdef TA_QT3D
  T3ExaminerViewer* vw = GetViewer();
  T3Entity* body = node->body;
  T3Entity* ln = new T3Entity(body);
  ln->Translate(0.0f, 1.0f - (row_pos + y_offs), 0.0f);
#else // TA_QT3D

  SoSeparator* ln = new SoSeparator();
  // place row and first margin here, simplifies everything...
  SoTranslation* tr = new SoTranslation();
  ln->addChild(tr);
  // 1st tr places origin for the row
  tr->translation.setValue(0.0f, -(row_height * (float)view_idx), 0.0f);
#endif // TA_QT3D

  int col_idx = 0;
  float col_wd_lst = 0.0f; // width of last col

  float row_ht = row_height - gr_mg_sz2;
  float mat_rot_rad = mat_rot * taMath_float::rad_per_deg;

  float col_pos = 0.0f;

  if(render_svg) {
    svg_str << taSvg::Group();
  }

  // render row_num cell, if on
  if (row_num_on) {
    col_wd_lst = col_widths[col_idx++];
    el = String(data_row);
#ifdef TA_QT3D
    T3TwoDText* txt = new T3TwoDText(ln);
    txt->setText(el);
    txt->setTextColor(txtcolr);
    txt->Scale(font_scale);
    txt->Translate(col_pos + 0.5f * col_wd_lst, 0.5f * text_ht, 0.0f);
#else // TA_QT3D
    SoSeparator* row_sep = new SoSeparator;
    tr = new SoTranslation;
    row_sep->addChild(tr);
    SoBaseColor* bc = new SoBaseColor;
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
    row_sep->addChild(bc);
    tr->translation.setValue(.5 * col_wd_lst, -y_offs, 0.0f);
    if(two_d_font) {
      SoText2* txt = new SoText2;
      txt->justification = SoText2::CENTER;
      row_sep->addChild(txt);
      txt->string.setValue(el.chars());
    }
    else {
      SoAsciiText* txt = new SoAsciiText;
      txt->justification = SoAsciiText::CENTER;
      row_sep->addChild(txt);
      txt->string.setValue(el.chars());
    }
    ln->addChild(row_sep);
#endif // TA_QT3D

    if(render_svg) {
      svg_str << taSvg::Text(el, col_pos + .5f * col_wd_lst, 1.0f - (row_pos + y_offs),
                             0.0f, iColor(), font_scale, taSvg::CENTER);
    }
    col_pos += col_wd_lst;
  }
  for (int col = col_range.min; col <= col_range.max; col++) {
    GridColView* cvs = (GridColView*)colVis(col);
    if (!cvs) continue;
    DataCol* dc = cvs->dataCol();

    //calculate the actual col row index, for the case of a jagged data table
    int act_idx; // <0 for null
    dt->idx(data_row, act_idx);

    // translate the col to proper location
#ifdef TA_QT3D
#else // TA_QT3D
    if (col_wd_lst > 0.0f) {
      tr = new SoTranslation();
      ln->addChild(tr);
      tr->translation.setValue(col_wd_lst, 0.0f, 0.0f);
    }
#endif // TA_QT3D
    col_wd_lst = col_widths[col_idx++]; // index is now +1 already..

    float col_wd = col_wd_lst - gr_mg_sz2;

    if(dc->is_matrix) {
      if(cvs->mat_image && !render_svg) { // svg does not handle images..
#ifdef TA_QT3D
#else // TA_QT3D
        SoSeparator* img = new SoSeparator;
        ln->addChild(img);
        SoTransform* tr = new SoTransform();
        img->addChild(tr);
        // scale the image according to pixel metrics
        // note: image defaults to 1 geom unit width, so we scale by our act width
        tr->scaleFactor.setValue(col_wd, row_ht, 1.0f);
        // center the shape in the middle of the cell
        tr->translation.setValue(col_wd_lst * .5f, -(row_height *.5f), 0.0f);
        SoImageEx* img_so = new SoImageEx;
        img->addChild(img_so);

        taMatrix* cell_mat =  dc->GetValAsMatrix(act_idx);
        if(cell_mat) {
          taBase::Ref(cell_mat);
          bool top_zero = (cvs->mat_layout == taMisc::TOP_ZERO);
          img_so->setImage(*cell_mat, top_zero);
          taBase::UnRef(cell_mat);
        }
#endif // TA_QT3D
      }
      else {
        float mat_ht = row_ht;
        // if(col_wd > mat_ht)     // make it more square as height increases..
        //   mat_ht += (col_wd - mat_ht) * fabsf(sinf(mat_rot_rad));
        taMatrix* cell_mat =  dc->AR();
        if(cell_mat) {
#ifdef TA_QT3D
          T3MatrixGrid* sogr = new T3MatrixGrid
            (ln, cell_mat, act_idx, cvs->mat_odd_vert, &colorscale, 
             (T3MatrixGrid::MatrixLayout)cvs->mat_layout, mat_val_text);
          sogr->Translate(col_pos + gr_mg_sz, -0.5f * row_height + 2.0f * gr_mg_sz, 0.0f);
          sogr->Scale3D(col_wd, mat_ht, 1.0f);
          sogr->RotateDeg(1.0f, 0.0f, 0.0f, mat_rot);
          sogr->spacing = mat_block_spc;
          sogr->block_height = mat_block_height;
          sogr->trans_max = mat_trans;
          sogr->user_data = dc; // needed for point picking
          sogr->addMouseInput(vw->mouse_dev);
          
#else // TA_QT3D
          SoSeparator* grsep = new SoSeparator;
          ln->addChild(grsep);
          SoTransform* tr = new SoTransform(); // todo: add this to image object! get rid of extra
          grsep->addChild(tr);
          tr->scaleFactor.setValue(col_wd, mat_ht, 1.0f);
          tr->translation.setValue(gr_mg_sz, -(row_height-gr_mg_sz), 0.0f);
          tr->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), mat_rot_rad);

          SoMatrixGrid* sogr = new SoMatrixGrid
            (cell_mat, act_idx, cvs->mat_odd_vert, &colorscale, 
             (SoMatrixGrid::MatrixLayout)cvs->mat_layout, mat_val_text);
          grsep->addChild(sogr);
          sogr->spacing = mat_block_spc;
          sogr->block_height = mat_block_height;
          sogr->trans_max = mat_trans;
          sogr->user_data = dc; // needed for point picking
#endif // TA_QT3D

          if(render_svg) {
            sogr->render_svg = true;
            sogr->svg_str = &svg_str;
            sogr->svg_off.x = col_pos + gr_mg_sz;
            sogr->svg_off.y = row_pos + gr_mg_sz;
            sogr->svg_sz.x = col_wd;
            sogr->svg_sz.y = mat_ht;
          }

          sogr->render();
        }
      }
    }
    else {                      // scalar: always text
      float x_offs = gr_mg_sz; // default for left
      int just = T3_ALIGN_LEFT;
      if (dc->isNumeric()) {
        just = T3_ALIGN_RIGHT;
        x_offs = col_wd - gr_mg_sz2;
        el = Variant::formatNumber(dc->GetValAsVar(act_idx),6); // 6 = precision
      }
      else {
        int max_chars = (int)(T3Misc::char_ht_to_wd_pts * col_wd / font_scale) + 1;
        el = dc->GetValAsString(act_idx).elidedTo(max_chars);
      }
#ifdef TA_QT3D
      T3TwoDText* txt = new T3TwoDText(ln);
      txt->align = (T3AlignText)just;
      txt->setText(el);
      txt->setTextColor(txtcolr);
      txt->Scale(font_scale);
      txt->Translate(col_pos + x_offs, 0.5f * text_ht, 0.0f);
#else // TA_QT3D
      SoSeparator* txt_sep = new SoSeparator;
      ln->addChild(txt_sep);
      // text origin is bottom left (Left) or bottom right (Right)
      // and we want to center vertically when  height is greater than txt height
      tr = new SoTranslation;
      txt_sep->addChild(tr);
      SoBaseColor* bc = new SoBaseColor;
      bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
      txt_sep->addChild(bc);
      tr->translation.setValue(x_offs, -y_offs, 0.0f);
      if(two_d_font) {
        SoText2* txt = new SoText2;
        txt_sep->addChild(txt);
        txt->justification = (SoText2::Justification)just;
        txt->string.setValue(el.chars());
      }
      else {
        SoAsciiText* txt = new SoAsciiText;
        txt_sep->addChild(txt);
        txt->justification = just;
        txt->string.setValue(el.chars());
      }
#endif // TA_QT3D

      if(render_svg) {
        if(just == T3_ALIGN_LEFT) {
          svg_str << taSvg::Text(el, col_pos + x_offs, 1.0f - (row_pos + y_offs),
                                 0.0f, iColor(), font_scale, taSvg::LEFT);
        }
        else {
          svg_str << taSvg::Text(el, col_pos + x_offs, 1.0f - (row_pos + y_offs),
                                 0.0f, iColor(), font_scale, taSvg::RIGHT);
        }
      }

    } //text

    col_pos += col_wd_lst;
  }
#ifdef TA_QT3D
#else // TA_QT3D
  node->body()->addChild(ln);
#endif // TA_QT3D

  if(render_svg) {
    svg_str << taSvg::GroupEnd();
  }
}

void GridTableView::RenderLines(){
  // this updates the data area
  T3GridViewNode* node = this->node_so();
  if (!node) return;

#ifdef TA_QT3D
  T3Entity* body = node->body;
  float hh = 0.0f;
  body->TranslateLLFSz1To(QVector3D(0.0f, 0.0f, 0.0f), width, 0.0f);
#else // TA_QT3D
  SoSeparator* body = node->body(); // cache
#endif // TA_QT3D
  
  body->removeAllChildren(); //should already have been done

  if(dataTable()->data.size == 0) return;

  // master font -- we only add a child font if different
  // doesn't seem to make much diff:
#ifdef TA_QT3D
#else // TA_QT3D
  SoComplexity* cplx = new SoComplexity;
  cplx->value.setValue(taMisc::text_complexity);
  body->addChild(cplx);

  SoFont* fnt = new SoFont();
  fnt->name = (const char*)taMisc::t3d_font_name;
  if(two_d_font)
    fnt->size.setValue(two_d_font_scale * font_scale);
  else
    fnt->size.setValue(font_scale);
  body->addChild(fnt);
  body->addChild(fnt);
  if (header_on) {
    SoTranslation* tr = new SoTranslation;
    body->addChild(tr);
    tr->translation.setValue(0.0f, -head_height, 0.0f);
  }
#endif // TA_QT3D

  // this is the index (in view_range units) of the first view line of data in the buffer
  int view_idx = 0;
  for (int data_row = view_range.min; data_row <= view_range.max; ++data_row) {
    RenderLine(view_idx, data_row);
    ++view_idx;
  }
}

void GridTableView::SetColorSpec(ColorScaleSpec* color_spec) {
  colorscale.SetColorSpec(color_spec);
  UpdateDisplay(true);
}

void GridTableView::ShowAllCols() {
  for (int i = 0; i < colViewCount(); i++) {
    GridColView* gcv = (GridColView*)colView(i);
    gcv->visible = true;
  }
  UpdateDisplay(true);
}

void GridTableView::setWidth(float wdth) {
  width = wdth;
  T3GridViewNode* node = this->node_so();
  if (!node) return;
  node->setWidth(wdth);
//  UpdateDisplay(true);
}

void GridTableView::setScaleData(bool auto_scale_, float min_, float max_) {
  if (colorscale.auto_scale != auto_scale_) {
    need_scale_update = true;
  }
  colorscale.SetScaleData(auto_scale_, min_, max_);
}

void GridTableView::VScroll(bool left) {
  if (left) {
    ViewCol_At(col_range.min - 1);
  } else {
    ViewCol_At(col_range.min + 1);
  }
}

void GridTableView::ViewCol_At(int start) {
  if (start < 0) start = 0;
  col_range.min = start;
  need_scale_update = true;
  UpdateDisplay();              // takes care of keeping col in range
}

void GridTableView::ColBackAll() {
  ViewCol_At(0);
}
void GridTableView::ColBackPg() {
  int cur_col = col_range.min;
  int goto_col = cur_col - page_cols;
  goto_col = MAX(0, goto_col);
  ViewCol_At(goto_col);
}
void GridTableView::ColBack1() {
  int cur_col = col_range.min;
  int goto_col = cur_col - 1;
  goto_col = MAX(0, goto_col);
  ViewCol_At(goto_col);
}
void GridTableView::ColFwd1() {
  int cur_col = col_range.min;
  int goto_col = cur_col + 1;
  goto_col = MIN(vis_cols.size-view_cols, goto_col);

  ViewCol_At(goto_col);
}
void GridTableView::ColFwdPg() {
  int cur_col = col_range.min;
  int goto_col = cur_col + page_cols;
  goto_col = MIN(vis_cols.size-view_cols, goto_col);

  ViewCol_At(goto_col);
}
void GridTableView::ColFwdAll() {
  ViewCol_At(vis_cols.size - view_cols);
}

void GridTableView::SetViewCols(int count) {
  if (view_cols != count) {
    need_scale_update = true;
  }
  view_cols = count;
}

#ifndef TA_QT3D
// callback for view transformer dragger
void T3GridViewNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3GridViewNode* vnd = (T3GridViewNode*)userData;
  GridTableView* nv = static_cast<GridTableView*>(vnd->dataView());

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nv->main_xform.rotate.x, nv->main_xform.rotate.y,
                           nv->main_xform.rotate.z), nv->main_xform.rotate.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate translation by current rotation
  trans[0] *= nv->main_xform.scale.x;
  trans[1] *= nv->main_xform.scale.y;
  trans[2] *= nv->main_xform.scale.z;
  taVector3f tr(trans[0], trans[1], trans[2]);
  nv->main_xform.translate += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nv->main_xform.scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nv->main_xform.rotate.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

  vnd->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  vnd->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);

  float frmg = T3GridViewNode::frame_margin;
  float frmg2 = 2.0f * frmg;
  float wdth = vnd->getWidth();

  vnd->txfm_shape()->translation.setValue(.5f * wdth, .5f * (1.0f + frmg2), 0.0f);

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nv->UpdateDisplay();
}

// this callback is registered in GridTableView::Render_pre

void T3GridViewNode_MouseCB(void* userData, SoEventCallback* ecb) {
  GridTableView* gv = (GridTableView*)userData;
  T3Panel* fr = gv->GetFrame();
  SoMouseButtonEvent* mouseevent = (SoMouseButtonEvent*)ecb->getEvent();
  SoMouseButtonEvent::Button but = mouseevent->getButton();
  if(!SoMouseButtonEvent::isButtonReleaseEvent(mouseevent, but)) return; // only releases
  bool left_but = false;        // assume other button -- don't really care what it is
  if(but == SoMouseButtonEvent::BUTTON1)
    left_but = true;
  bool got_one = false;
  for(int i=0;i<fr->root_view.children.size;i++) {
    taDataView* dv = fr->root_view.children[i];
    if(dv->InheritsFrom(&TA_GridTableView)) {
      GridTableView* tgv = (GridTableView*)dv;
      T3ExaminerViewer* viewer = tgv->GetViewer();
      SoRayPickAction rp( viewer->getViewportRegion());
      rp.setPoint(mouseevent->getPosition());
#ifndef TA_QT3D
      rp.apply(viewer->quarter->getSoEventManager()->getSceneGraph()); // event mgr has full graph!
#endif
      SoPickedPoint* pp = rp.getPickedPoint(0);
      if(!pp) continue;
      SoNode* pobj = pp->getPath()->getNodeFromTail(1);
      if(!pobj) continue;
      if(!pobj->isOfType(SoMatrixGrid::getClassTypeId())) {
        continue;
      }
      SoMatrixGrid* mtxg = (SoMatrixGrid*)pobj;
      DataCol* dcol = (DataCol*)mtxg->user_data;
      taMatrixPtr matrix;
      matrix = mtxg->matrix->GetFrameSlice_(mtxg->slice_idx);
      SbVec3f pt = pp->getObjectPoint(pobj);

      int geom_x, geom_y;
      matrix->geom.Get2DGeomGui(geom_x, geom_y, mtxg->odd_y, 1);
//      float cl_x = 1.0f / (float)geom_x;      // how big each cell is
//      float cl_y = 1.0f / (float)geom_y;
      int xp = (int)((pt[0] * geom_x));
      int yp = (int)((pt[1] * geom_y));

      float val_to_set = 1.0f;
      float val_to_unset = 0.0f;
      if(tgv->click_vals) {
        if(left_but) {
          val_to_set = tgv->lmb_val;
          val_to_unset = tgv->mmb_val;
        }
        else {
          val_to_set = tgv->mmb_val;
          val_to_unset = tgv->lmb_val;
        }
        taProject* proj = (taProject*)tgv->GetOwner(&TA_taProject);
        // save undo state!
        if(proj && dcol) {
          proj->undo_mgr.SaveUndo(dcol, "GridView Click Vals", dcol);
        }
      }

      // note: all the bot zero stuff is backwards due to coord inversion -- coin is bot zero!
      if(matrix->dims() == 1) {
        int ymax = matrix->dim(0);      // assumes odd_y
        int yeff = yp;
        if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) yeff = ymax-1-yp;
        tgv->last_sel_val = matrix->SafeElAsFloat(yeff);
        if(tgv->click_vals) {
          if(tgv->last_sel_val == val_to_set)
            matrix->SetFmVar(val_to_unset, yeff);
          else
            matrix->SetFmVar(val_to_set, yeff);
        }
      }
      if(matrix->dims() == 2) {
        int yeff = yp;
        if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) yeff = geom_y-1-yp;
        tgv->last_sel_val = matrix->SafeElAsFloat(xp, yeff);
        if(tgv->click_vals) {
          if(tgv->last_sel_val == val_to_set)
            matrix->SetFmVar(val_to_unset, xp, yeff);
          else
            matrix->SetFmVar(val_to_set, xp, yeff);
        }
      }
      if(matrix->dims() == 4) {
        yp--;                   // yp has 1 extra in 4d by some reason.. spacing..
        int xmax = matrix->dim(0);      int ymax = matrix->dim(1);
        int xxmax = matrix->dim(2);     int yymax = matrix->dim(3);

        int oxp = xp / (xmax+1);        int oyp = yp / (ymax+1);
        int ixp = xp % (xmax+1);        int iyp = yp % (ymax+1);

        int iyeff = iyp;  int oyeff = oyp;
        if(mtxg->mat_layout != SoMatrixGrid::BOT_ZERO) {
          iyeff = ymax-1-iyp;     oyeff = yymax-1-oyp;
        }
        tgv->last_sel_val = matrix->SafeElAsFloat(ixp, iyeff, oxp, oyeff);
        if(tgv->click_vals) {
          if(tgv->last_sel_val == val_to_set)
            matrix->SetFmVar(val_to_unset, ixp, iyeff, oxp, oyeff);
          else
            matrix->SetFmVar(val_to_set, ixp, iyeff, oxp, oyeff);
        }
      }
      if(tgv->click_vals) {
        // this causes a crash
//      if(dcol)
//        dcol->SigEmitUpdated(); // col drives updating
      }
      else {
        tgv->last_sel_got = true;
        if(dcol) {
          tgv->last_sel_col_nm = dcol->name;
        }
      }
      tgv->UpdateDisplay();     // update to show last viewed val
      got_one = true;
      break;
    }
  }
  if(got_one)
    ecb->setHandled();
}

#endif // TA_QT3D

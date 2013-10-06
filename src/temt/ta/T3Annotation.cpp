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

#include "T3Annotation.h"
#include <T3DataViewMain>

#include <taMisc>

void T3Annotation::Initialize() {
  type = LINE;
  rend_type = (AnnoteType)-1;
  scale = 1.0f;
  start = 0.0f;
  end = 1.0f;
  bot_left = 0.0f;
  top_right = 1.0f;
  line_width = 1.0f;
  arrow_size = .02f;
  text = "Select, Context Menu to Edit";
  font_size = .05f;
  color.Set(0.0f, 0.0f, 0.0f);
}

void T3Annotation::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(taMisc::gui_active && rend_type >= 0 && type != rend_type) {
    UpdateDisplay();
  }
}

void T3Annotation::UpdateDisplay() {
  T3DataViewMain* t3m = GET_MY_OWNER(T3DataViewMain);
  if(t3m) {
    t3m->ReBuildAll();
  }
}

void T3Annotation::SetColor(const String& clr) {
  color.setColorName(clr);
}

void T3Annotation::SetLine(float st_x, float st_y, float st_z,
                           float ed_x, float ed_y, float ed_z,
                           float ln_width , const String& clr) {
  type = LINE;
  start.SetXYZ(st_x, st_y, st_z);
  end.SetXYZ(ed_x, ed_y, ed_z);
  line_width = ln_width;
  SetColor(clr);
  UpdateAfterEdit();            // update
}
void T3Annotation::SetArrow(float st_x, float st_y, float st_z,
                            float ed_x, float ed_y, float ed_z,
                            float ln_width , const String& clr,
                            float arrow_sz) {
  type = ARROW;
  start.SetXYZ(st_x, st_y, st_z);
  end.SetXYZ(ed_x, ed_y, ed_z);
  line_width = ln_width;
  arrow_size = arrow_sz;
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetDoubleArrow(float st_x, float st_y, float st_z,
                                  float ed_x, float ed_y, float ed_z,
                                  float ln_width , const String& clr,
                                  float arrow_sz) {
  type = DOUBLEARROW;
  start.SetXYZ(st_x, st_y, st_z);
  end.SetXYZ(ed_x, ed_y, ed_z);
  line_width = ln_width;
  arrow_size = arrow_sz;
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetRectangle(float bot_left_x, float bot_left_y, float bot_left_z,
                                float top_right_x, float top_right_y, float top_right_z,
                                float ln_width , const String& clr) {
  type = RECTANGLE;
  bot_left.SetXYZ(bot_left_x, bot_left_y, bot_left_z);
  top_right.SetXYZ(top_right_x, top_right_y, top_right_z);
  line_width = ln_width;
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetEllipse(float bot_left_x, float bot_left_y, float bot_left_z,
                              float top_right_x, float top_right_y, float top_right_z,
                              float ln_width , const String& clr) {
  type = ELLIPSE;
  bot_left.SetXYZ(bot_left_x, bot_left_y, bot_left_z);
  top_right.SetXYZ(top_right_x, top_right_y, top_right_z);
  line_width = ln_width;
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetCircle(float ctr_x, float ctr_y, float ctr_z,
                             float radius,
                             float ln_width , const String& clr) {
  type = ELLIPSE;
  bot_left.SetXYZ(ctr_x - radius, ctr_y - radius, ctr_z);
  top_right.SetXYZ(ctr_x + radius, ctr_y + radius, ctr_z);
  line_width = ln_width;
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetText(const String& txt,
                           float pos_x, float pos_y, float pos_z,
                           float font_sz, const String& clr) {
  type = TEXT;
  text = txt;
  font_size = font_sz;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  SetColor(clr);
  UpdateAfterEdit();            // update
}

void T3Annotation::SetObject(const String& obj_file_path,
                             float pos_x, float pos_y, float pos_z,
                             const String& clr) {
  type = OBJECT;
  obj_fname = obj_file_path;
  pos.SetXYZ(pos_x, pos_y, pos_z);
  SetColor(clr);
  UpdateAfterEdit();            // update
}

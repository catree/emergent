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

#include "ActrVisionModule.h"
#include <ActrModel>
#include <ActrSlot>

#include <Random>
#include <float_Array>
#include <taMath_float>
#include <taMisc>

void ActrVisParams::Initialize() {
  n_finst = 4;
  finst_span = 3.0f;
  onset_span = 0.5f;
  move_tol = 0.5f;
}

void ActrAttnParams::Initialize() {
  auto_attend = false;
  latency = 0.085f;
  scene_chg_thresh = 0.25f;
}

void ActrVisionModule::Initialize() {
}

void ActrVisionModule::InitModule() {
  if((bool)buffer) return;
  if(!Model()) return;
  ActrModel* mod = Model();
  bool made_new;
  buffer = mod->buffers.FindMakeNameType("visual", NULL, made_new);
  buffer->module = this;
  buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    buffer->act_total = 0.0f;   // visual-activation
  }

  location_buffer = mod->buffers.FindMakeNameType("visual_location", NULL, made_new);
  location_buffer->module = this;
  location_buffer->SetBufferFlag(ActrBuffer::STD_FLAGS); // harvest, merge
  if(made_new) {
    location_buffer->act_total = 0.0f;   // visual-location-activation
  }

  ActrChunkType* ck = NULL;
  mod->DefineChunkType("visual_object", "", "screen_pos", "value",
                       "status", "color", "height", "width");
  mod->DefineChunkType("abstract_object", "", "value", "line_pos", "bin_pos");
  mod->DefineChunkType("abstract_letter", "abstract_object");
  mod->DefineChunkType("abstract_number", "abstract_object");
  mod->DefineChunkType("text", "visual_object");
  mod->DefineChunkType("empty_space", "visual_object");
  mod->DefineChunkType("line", "visual_object");
  mod->DefineChunkType("oval", "visual_object");
  mod->DefineChunkType("cursor", "visual_object");
  mod->DefineChunkType("phrase", "visual_object", "objects", "words", "colors");
  ck = mod->DefineChunkType("visual_location", "", "screen_x", "screen_y", "distance",
                            "kind", "color", "value", "height", "width", "size", "object");
  ck->SetSlotChunkType("object", "visual_object"); // object is pointer to visual_object
  mod->DefineChunkType("set_visloc_default", "", "type", "screen_x", "screen_y",
                       "distance", "kind", "color", "value", "height", "width",
                       "size");
  mod->DefineChunkType("char_primitive", "visual_location", "left", "right");
  mod->DefineChunkType("vision_command");
  mod->DefineChunkType("pm_constant");
  mod->DefineChunkType("color");
  ck = mod->DefineChunkType("move_attention", "vision_command", "screen_pos", "scale");
  ck->SetSlotChunkType("screen_pos", "visual_location"); // screen_pos is pointer to vis loc
  
  mod->DefineChunkType("start_tracking", "vision_command");
  ck = mod->DefineChunkType("assign_finst", "vision_command", "object", "location");
  ck->SetSlotChunkType("object", "visual_object"); 
  ck->SetSlotChunkType("location", "visual_location");
  mod->DefineChunkType("clear_scene_change", "vision_command");
  mod->DefineChunkType("clear");

  mod->DefineChunk("lowest", "pm_constant");
  mod->DefineChunk("highest", "pm_constant");
  mod->DefineChunk("current", "pm_constant");
  mod->DefineChunk("current_x", "pm_constant");
  mod->DefineChunk("current_y", "pm_constant");
  mod->DefineChunk("clockwise", "pm_constant");
  mod->DefineChunk("counterclockwise", "pm_constant");
  mod->DefineChunk("external", "pm_constant");
  mod->DefineChunk("internal", "pm_constant");
  mod->DefineChunk("find_location", "vision_command");
  mod->DefineChunk("move_attention", "vision_command");
  mod->DefineChunk("assign_finst", "vision_command");
  mod->DefineChunk("start_tracking", "vision_command");
    
  mod->DefineChunk("black", "color");
  mod->DefineChunk("red", "color");
  mod->DefineChunk("blue", "color");
  mod->DefineChunk("green", "color");
  mod->DefineChunk("white", "color");
  mod->DefineChunk("magenta", "color");
  mod->DefineChunk("yellow", "color");
  mod->DefineChunk("cyan", "color");
  mod->DefineChunk("dark_green", "color");
  mod->DefineChunk("dark_red", "color");
  mod->DefineChunk("dark_cyan", "color");
  mod->DefineChunk("dark_blue", "color");
  mod->DefineChunk("dark_magenta", "color");
  mod->DefineChunk("dark_yellow", "color");
  mod->DefineChunk("light_gray", "color");
  mod->DefineChunk("dark_gray", "color");
        
  mod->DefineChunk("text", "chunk");
  mod->DefineChunk("box", "chunk");
  mod->DefineChunk("line", "chunk");
  mod->DefineChunk("oval", "chunk");
    
  mod->DefineChunk("new", "chunk");
  mod->DefineChunk("clear", "chunk");
}

void ActrVisionModule::Init() {
  inherited::Init();
  attended = NULL;
  found = NULL;
  finsts.Reset();
  eligible.Reset();
  visicon.Reset();
  last_cmd = "";
  buffer->UpdateState();
  location_buffer->Init();
  location_buffer->UpdateState();
}

void ActrVisionModule::ProcessEvent(ActrEvent& event) {
  if(event.action == "MODULE-REQUEST") {
    VisionRequest(event);
  }
  else if(event.action == "Encoding-complete") {
    EncodingComplete(event);
  }
  // else if(event.action == "CREATE-NEW-BUFFER-CHUNK") {
  //   CreateNewChunk(event);
  // }
  // else if(event.action == "SET-BUFFER-CHUNK") {
  //   SetBufferChunk(event);
  // }
  // else if(event.action == "MOD-VISION-CHUNK") {
  //   ModVisionChunk(event);
  // }
  else {
    ProcessEvent_std(event);   // respond to regular requests
  }
}

void ActrVisionModule::VisionRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  if(TestError(!ck || !ck->chunk_type, "VisionRequest", "chunk is NULL")) {
    return;
  }
  ActrModel* mod = Model();

  if(HasModuleFlag(BUSY)) {
    TestWarning(true, "VisionRequest",
                "a vision request was made while still busy activating previous request -- new request ignored");
    mod->LogEvent(-1.0f, "vision", "ABORT-IMAGINAL-REQ", "", "");
    return;
  }

  RemoveOldFinsts();            // update the list to current time

  if(event.dst_buffer == location_buffer) { // only supports find-location
    VisualLocationRequest(event);
  }
  else if(event.dst_buffer == buffer) { // supports many types
    if(ck->name == "move_attention" ||
       ck->chunk_type->InheritsFromCTName("move_attention")) {
      MoveAttentionRequest(event);
    }
    else if(ck->name == "start_tracking" ||
            ck->chunk_type->InheritsFromCTName("start_tracking")) {
      StartTrackingRequest(event);
    }
    else if(ck->name == "clear" || ck->chunk_type->InheritsFromCTName("clear")) {
      ClearRequest(event);
    }
    else if(ck->name == "clear_scene_change" ||
            ck->chunk_type->InheritsFromCTName("clear_scene_change")) {
      ClearSceneChangeRequest(event);
    }
    else if(ck->name == "assign_finst" ||
            ck->chunk_type->InheritsFromCTName("assign_finst")) {
      AssignFinstRequest(event);
    }
  }
}

void ActrVisionModule::VisualLocationRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "Find-location", "", "");
  last_cmd = "visual_location";

  SetModuleFlag(BUSY);
  ClearModuleFlag(ERROR);
  location_buffer->SetReq();
  location_buffer->ClearChunk();         // always clear before find

  Attended attd = NO_ATTENDED;
  Nearest nearest = NO_NEAREST;
  float nx = 0.0f;
  float ny = 0.0f;
  float ctrx = center.x;
  float ctry = center.y;
  // process parameters
  String_Array pary;
  pary.FmDelimString(event.params, ";");
  for(int i=0; i<pary.size; i++) {
    String ps = pary[i];
    if(ps.startsWith("nearest ")) {
      String spec = ps.after("nearest ");
      if(spec == "current" && attended) {
        nearest = NEAR_XY;
        nx = attended->GetSlotVal("screen_x").toFloat();
        ny = attended->GetSlotVal("screen_y").toFloat();
      }
      else if(spec == "current_x" && attended) {
        nearest = NEAR_X;
        nx = attended->GetSlotVal("screen_x").toFloat();
      }
      else if(spec == "current_y" && attended) {
        nearest = NEAR_Y;
        nx = attended->GetSlotVal("screen_y").toFloat();
      }
      else if(spec == "clockwise") {
        nearest = NEAR_CLOCKWISE;
      }
      else if(spec == "counterclockwise") {
        nearest = NEAR_COUNTERCLOCKWISE;
      }
      else {
        // must be name of a chunk in visicon
        ActrChunk* nck = visicon.FindName(spec);
        if(nck) {
          nearest = NEAR_XY;
          nx = nck->GetSlotVal("screen_x").toFloat();
          ny = nck->GetSlotVal("screen_y").toFloat();
        }
        else {
          TestWarning(true, "find_location", "nearest chunk name not found in visicon:",
                      spec);
        }
      }
    }
    else if(ps.startsWith("attended ")) {
      String spec = ps.after("attended ");
      if(spec == "nil" || spec == "false") {
        attd = NOT_ATTENDED;
      }
      else if(spec == "new") {
        attd = NEW_NOT_ATTENDED;
      }
      else {
        attd = ATTENDED;
      }
    }
    else if(ps.startsWith("center ")) {
      String spec = ps.after("center ");
      // must be name of a chunk in visicon
      ActrChunk* nck = visicon.FindName(spec);
      if(nck) {
        ctrx = nck->GetSlotVal("screen_x").toFloat();
        ctry = nck->GetSlotVal("screen_y").toFloat();
      }
      else {
        TestWarning(true, "find_location", "center chunk name not found in visicon:",
                    spec);
      }
    }
  }

  bool got_some = FindMatchingLocation(ck, attd, nearest, nx, ny, ctrx, ctry);

  if(!got_some) {
    ClearModuleFlag(BUSY);
    SetModuleFlag(ERROR);
    location_buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "FIND-LOC-FAILURE", "", "");
    return;
  }

  if(eligible.size == 1) {
    found = eligible.FastEl(0);
  }
  else {
    ChooseFromEligibleLocations();
  }

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, location_buffer,
                     "SET-BUFFER-CHUNK", found->name, event.act_arg,
                     found);

  ClearModuleFlag(BUSY);
  location_buffer->ClearReq();

  if(attn.auto_attend) {
    // todo: technically should be a separate event instead of just log
    mod->LogEvent(-1.0f, "vision", "automatically attending", "", "");

    ActrChunk* ma = mod->chunks.FindName("move_attention");

    mod->ScheduleEvent(0.05f, ActrEvent::max_pri, this, this, buffer,
                       "Move-attention", found->name, event.act_arg,
                       ma);     // ma type triggers move

    // from the vision.lisp module, line 2600:
    // ;; Hack to clear the module state just before the attention shift so as not to 
    // ;; jam things.
         
    // (schedule-event-relative .05 'change-state :params (list :exec 'free :proc 'free) :destination :vision :output nil
    //                          :module :vision :priority 1))
  }
}

bool ActrVisionModule::FindMatchingLocation(ActrChunk* ck, Attended attd,
                                            Nearest nearest,
                                            float nx, float ny,
                                            float ctrx, float ctry) {
  ActrChunk cpck;               // need a local copy
  cpck.CopyFrom(ck);
  float_Array lohivals;               // contains lowest and highest values

  bool low_hi = false;          // were low or hi used?
  for(int i=0; i<cpck.slots.size; i++) {
    ActrSlot* sl = cpck.slots.FastEl(i);
    if(sl->val == "lowest" || sl->val == "highest") {
      low_hi = true;
    }
    else if(sl->val == "current") {
      if(attended) {
        ActrSlot* csl = attended->FindSlot(sl->name);
        if(csl) {
          sl->CopyFrom(csl);
        }
      }
    }
  }
  
  if(low_hi) {
    lohivals.SetSize(cpck.slots.size);
  }

  ActrModel* mod = Model();
  eligible.Reset();

  ActrChunk_List* lst = &visicon;
  if(attd == ATTENDED) {
    lst = &finsts;
  }

  for(int i=0; i<lst->size; i++) {
    ActrChunk* oc = lst->FastEl(i);
    bool updt = false;
    if(mod->UpdateGui()) {
      if(oc->HasChunkFlag(ActrChunk::ELIGIBLE) ||
         oc->HasChunkFlag(ActrChunk::RETRIEVED))
        updt = true;
    }
    oc->ClearChunkFlag(ActrChunk::ELIGIBLE);
    oc->ClearChunkFlag(ActrChunk::RETRIEVED);

    if(attd == NOT_ATTENDED) {
      if(oc->HasChunkFlag(ActrChunk::RECENT)) continue; // skip
    }
    if(attd == NEW_NOT_ATTENDED) {
      if(oc->HasChunkFlag(ActrChunk::RECENT)) continue; // skip
      float tm = mod->cur_time - oc->time.t_new;
      if(tm > params.onset_span) { // not new
        continue;
      }
    }

    ActrChunkType* compar = cpck.chunk_type->CommonChunkType(oc->chunk_type);
    bool match = true;
    if(compar) {
      for(int j=0; j<compar->slots.size; j++) {
        ActrSlot* sl = cpck.slots.FastEl(j);
        ActrSlot* os = oc->slots.SafeEl(j);
        if(sl->val == "lowest" || sl->val == "highest") {
          continue;
        }
        if(!sl->MatchesMem(os, false, false)) { // false = exact, why_not
          match = false;
          break;
        }
      }
    }
    else {
      match = false;
    }
    if(match) {
      eligible.Link(oc);
      oc->SetChunkFlag(ActrChunk::ELIGIBLE);

      // get low-high values only for matches
      if(low_hi) {
        for(int j=0; j<compar->slots.size; j++) {
          ActrSlot* sl = cpck.slots.FastEl(j);
          ActrSlot* os = oc->slots.FastEl(j);
          if(sl->val == "lowest") {
            float oval = (float)os->val;
            if(eligible.size == 1) { // first one, set
              lohivals[j] = oval;
            }
            else {
              float& lv = lohivals[j];
              lv = MIN(lv, oval);
            }
          }
          else if(sl->val == "highest") {
            float oval = (float)os->val;
            if(eligible.size == 1) { // first one, set
              lohivals[j] = oval;
            }
            else {
              float& lv = lohivals[j];
              lv = MAX(lv, oval);
            }
          }
        }
      }
    }
    if(updt && mod->UpdateGui()) {
      oc->SigEmitUpdated();
    }
  }

  if(low_hi) {                  // go through eligible and eliminate 
    for(int i=eligible.size-1; i>=0; i--) {
      ActrChunk* oc = eligible.FastEl(i);
      ActrChunkType* compar = cpck.chunk_type->CommonChunkType(oc->chunk_type);

      bool remove = false;
      for(int j=0; j<compar->slots.size; j++) {
        ActrSlot* sl = cpck.slots.FastEl(j);
        ActrSlot* os = oc->slots.FastEl(j);
        if(sl->val == "lowest") {
          float oval = (float)os->val;
          if(oval > lohivals[j]) {
            remove = true;
            break;
          }
        }
        else if(sl->val == "lowest") {
          float oval = (float)os->val;
          if(oval < lohivals[j]) {
            remove = true;
            break;
          }
        }
      }
      if(remove) {
        eligible.RemoveIdx(i);
      }
    }
  }

  if(nearest == NO_NEAREST) {
    return (eligible.size > 0);
  }
  if(TestWarning(nearest >= NEAR_CLOCKWISE, "FindLocation",
                 "clockwise and counterclockwise nearest tests not yet supported")) {
    return (eligible.size > 0);
  }

  // now process nearest
  float_Array dsts;
  dsts.SetSize(eligible.size);
  float min_dist = 0.0f;
  for(int i=eligible.size-1; i>=0; i--) {
    ActrChunk* oc = eligible.FastEl(i);
    float cx = oc->GetSlotVal("screen_x").toFloat();
    float cy = oc->GetSlotVal("screen_y").toFloat();
    float dist = 0.0f;
    if(nearest == NEAR_XY) {
      dist = taMath_float::euc_dist(cx, cy, nx, ny);
    }
    else if(nearest == NEAR_X) {
      dist = taMath_float::fabs(cx - nx);
    }
    else if(nearest == NEAR_Y) {
      dist = taMath_float::fabs(cy - ny);
    }
    dsts[i] = dist;
    if(dist < min_dist) {
      min_dist = dist;
    }
  }
    
  for(int i=eligible.size-1; i>=0; i--) {
    ActrChunk* oc = eligible.FastEl(i);
    float dist = dsts[i];
    if(dist > min_dist) {
      eligible.RemoveIdx(i);
    }
  }

  return (eligible.size > 0);
}


void ActrVisionModule::ChooseFromEligibleLocations() {
  int best_idx = 0;
  float best_time = 1.0e6f;
  int_Array ties;
  ActrModel* mod = Model();
  for(int i=0; i<eligible.size; i++) {
    ActrChunk* pr = eligible.FastEl(i);
    float eff_t = mod->cur_time - pr->time.t_new; // how old
    if(eff_t < best_time) {
      ties.Reset();
      ties.Add(i);              // we always want us to be in the tie set
      best_idx = i;
      best_time = eff_t;
    }
    else if(eff_t == best_time) { // keep track of ties
      ties.Add(i);
    }
  }
  if(ties.size > 1) {
    if(mod->params.enable_rnd) {
      int choose = Random::IntZeroN(ties.size);
      found = eligible.FastEl(ties[choose]);
    }
    else {
      found = eligible.FastEl(best_idx); // just choose first
    }
  }
  else {
    found = eligible.FastEl(best_idx);
  }
}

void ActrVisionModule::RemoveOldFinsts() {
  ActrModel* mod = Model();
  for(int i=finsts.size-1; i >= 0; i--) {
    ActrChunk* oc = finsts.FastEl(i);
    if(mod->cur_time - oc->time.t_act > params.finst_span) {
      oc->ClearChunkFlag(ActrChunk::RECENT);
      finsts.RemoveIdx(i);
    }
  }
}

void ActrVisionModule::UpdateFinsts(ActrChunk* attend) {
  ActrModel* mod = Model();
  if(finsts.size < params.n_finst) {
    finsts.Link(attend);      // just add it
  }
  else {
    float oldest_t = 0.0f;
    int oldest_i = 0;
    for(int i=finsts.size-1; i >= 0; i--) {
      ActrChunk* oc = finsts.FastEl(i);
      float tm = mod->cur_time - oc->time.t_act;
      if(tm > oldest_t) {
        oldest_t = tm;
        oldest_i = i;
      }
    }
    finsts.FastEl(oldest_i)->ClearChunkFlag(ActrChunk::RECENT);
    finsts.ReplaceLinkIdx(oldest_i, attend); // replace the oldest
  }
  attend->SetChunkFlag(ActrChunk::RECENT);
}

void ActrVisionModule::MoveAttentionRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "Move-attention", "", "");

  SetModuleFlag(BUSY);
  SetModuleFlag(PROC);       // move-attention = processor
  SetModuleFlag(EXEC);       // move-attention and execution (also tracking, unreq re-eq)
  ClearModuleFlag(ERROR);
  buffer->SetReq();
  buffer->ClearChunk();         // always clear before..

  ActrChunk* moveto = NULL;
  String scale;
  if(ck->chunk_type->InheritsFromCTName("move_attention")) {
    moveto = ck->GetSlotValChunk("screen_pos");
    scale = ck->GetSlotValLiteral("scale");
  }
  else {                        // must be the command
    moveto = found;             // use prev
  }
  if(!moveto) {
    ClearModuleFlag(BUSY);
    ClearModuleFlag(PROC);
    ClearModuleFlag(EXEC);
    SetModuleFlag(ERROR);
    buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "MOVE-ATTN-FAILURE", "move_attention chunk not found",
                  "");
    return;
  }
  // todo: process the scale command -- not compatible with new model of visicon

  ActrChunk* obj = moveto->GetSlotValChunk("object");
  if(!obj) {
    ClearModuleFlag(BUSY);
    ClearModuleFlag(PROC);
    ClearModuleFlag(EXEC);
    SetModuleFlag(ERROR);
    buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "MOVE-ATTN-FAILURE", "object pointer not set in move_attention visual_location chunk",
                  "");
    return;
  }

  attended = moveto;            // this is now the new attended location
  UpdateFinsts(attended);       // and we mark it as such

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, buffer,
                     "Encoding-complete", obj->name, event.act_arg,
                     obj);

}

void ActrVisionModule::EncodingComplete(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->ScheduleEvent(0.0f, ActrEvent::max_pri, this, this, buffer,
                     "SET-BUFFER-CHUNK", ck->name, event.act_arg,
                     ck);

  ClearModuleFlag(BUSY);
  ClearModuleFlag(PROC);
  ClearModuleFlag(EXEC);
  buffer->ClearReq();
}

void ActrVisionModule::StartTrackingRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "Start-tracking", "", "");

  if(!attended) {
    ClearModuleFlag(BUSY);
    ClearModuleFlag(EXEC);
    SetModuleFlag(ERROR);
    buffer->ClearReq();
    mod->LogEvent(-1.0f, "vision", "Start-tracking-FAILURE", "no currently-attended item");
    return;
  }

  SetModuleFlag(BUSY);
  SetModuleFlag(EXEC);       // stays on during entire tracking time
  ClearModuleFlag(ERROR);

  ActrChunk* obj = attended->GetSlotValChunk("object"); // should exist!
  tracking = obj;          // just turn it on -- up to other code to keep it up-to-date
  
  ClearModuleFlag(BUSY);
  ClearModuleFlag(EXEC);
  buffer->ClearReq();
}

void ActrVisionModule::ClearRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "CLEAR", "", "");

  tracking = NULL;
  attended = NULL;
  ClearModuleFlag(EXEC);
  ClearModuleFlag(ERROR);

  // todo: should delay by .050 and send a subsequent event that records CHANGE_STATE
  // should clear last_cmd as well

  ClearModuleFlag(BUSY);
  buffer->ClearReq();
}

void ActrVisionModule::ClearSceneChangeRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  mod->LogEvent(-1.0f, "vision", "CLEAR_SCENE_CHANGE", "", "");

  // clears any pending scene change -- takes no time and does not cause any busy

}

void ActrVisionModule::AssignFinstRequest(ActrEvent& event) {
  ActrChunk* ck = event.chunk_arg;
  ActrModel* mod = Model();

  ActrChunk* loc = ck->GetSlotValChunk("location");
  if(loc) {
    UpdateFinsts(loc);          // mark it -- should check types
    mod->LogEvent(-1.0f, "vision", "ASSIGN_FINST", "", "");
  }
  else {
    TestWarning(true, "AssignFinstRequest",
                "object-based requests not current supported");
  }
}

bool ActrVisionModule::ProcessQuery(ActrBuffer* buf, const String& query, bool why_not) {
  String quer = query;
  bool neg = false;
  if(quer.endsWith('-')) {
    quer = quer.before('-',-1);
    neg = true;
  }
  bool rval = false;
  ActrModel* mod = Model();
  if(quer == "attended" || quer == "attended t") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      rval = ck->HasChunkFlag(ActrChunk::RECENT); // attended = full and finst
    }
    else {
      rval = false;
    }
  }
  else if(quer == "unattended" || quer == "attended nil") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      rval = !ck->HasChunkFlag(ActrChunk::RECENT);
    }
    else {
      rval = true;
    }
  }
  else if(quer == "newly_attended" || quer == "attended new") {
    if(location_buffer->IsFull()) {
      ActrChunk* ck = location_buffer->CurChunk();
      float tm = mod->cur_time - ck->time.t_new;
      rval = (tm <= params.onset_span);
    }
    else {
      rval = false;
    }
  }
  else if(quer == "scene_change" || quer == "scene_change t") {
    // todo: check if schene changed within past onset_span seconds
  }
  else if(quer == "no_scene_change" || quer == "scene_change not") {
    // todo: check if not schene changed within past onset_span seconds
  }
  else if(quer.startsWith("scene_change_value")) {
    String valstr = quer.after("scene_change_value");
    float val = (float)valstr;
    // todo: check if not schene changed within past onset_span seconds
  }
  // todo: last_command
  else {
    return ProcessQuery_std(buf, query, why_not);
  }
  if(neg) rval = !rval;
  if(!rval && why_not) {
    taMisc::Info("module:",GetDisplayName(), "buffer:", buf->GetDisplayName(),
                 "query:", query, "returned false");
  }
  return rval;
}


bool ActrVisionModule::SetParam(const String& param_nm, Variant par1, Variant par2) {
  bool got = false;
  if(param_nm == "visual_activation" && buffer) {
    buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_location_activation" && location_buffer) {
    location_buffer->act_total = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_num_finsts") {
    params.n_finst = par1.toInt();
    got = true;
  }
  else if(param_nm == "visual_finst_span") {
    params.finst_span = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_onset_span") {
    params.onset_span = par1.toFloat();
    got = true;
  }
  else if(param_nm == "visual_movement_tolerance") {
    params.move_tol = par1.toFloat();
    got = true;
  }
  else if(param_nm == "auto_attend") {
    attn.auto_attend = par1.toBool();
    got = true;
  }
  else if(param_nm == "visual_attention_latency") {
    attn.latency = par1.toFloat();
    got = true;
  }
  else if(param_nm == "scene_change_threshold") {
    attn.scene_chg_thresh = par1.toFloat();
    got = true;
  }

  return got;
}


void ActrVisionModule::AddToVisIcon(ActrChunk* ck) {
  ActrModel* mod = Model();
  ck->InitChunk(mod->cur_time);
  visicon.Add(ck);
}
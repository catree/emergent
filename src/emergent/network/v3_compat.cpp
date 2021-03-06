// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

// v3_compat.cpp -- version 3 compatability objects, only for converting

#include "v3_compat.h"

#include "css_ta.h"
#include "ta_program_els.h"
#include "netstru.h"
#include "emergent_project.h"


#ifdef TA_GUI
#include "ta_qtviewer.h"

TA_BASEFUNS_CTORS_DEFN(ClearLogProc);

TA_BASEFUNS_CTORS_DEFN(DispNetWeightsProc);

TA_BASEFUNS_CTORS_DEFN(DispDataEnvProc);

TA_BASEFUNS_CTORS_DEFN(TimeCounterStatResetProc);

TA_BASEFUNS_CTORS_DEFN(TimeCounterStat);

TA_BASEFUNS_CTORS_DEFN(UniquePatStat);

TA_BASEFUNS_CTORS_DEFN(UnitEventRFStat);

TA_BASEFUNS_CTORS_DEFN(UnitActRFStatResetProc);

TA_BASEFUNS_CTORS_DEFN(UnitActRFStat);

TA_BASEFUNS_CTORS_DEFN(MaxActTrgStat);

TA_BASEFUNS_CTORS_DEFN(ProcCounterStat);

TA_BASEFUNS_CTORS_DEFN(EpochCounterStat);

TA_BASEFUNS_CTORS_DEFN(CopyToEnvStat);

TA_BASEFUNS_CTORS_DEFN(ComputeStat);

TA_BASEFUNS_CTORS_DEFN(ProjectionStat);

TA_BASEFUNS_CTORS_DEFN(CompareStat);

TA_BASEFUNS_CTORS_DEFN(ScriptStat);

TA_BASEFUNS_CTORS_DEFN(ActThreshRTStat);

TA_BASEFUNS_CTORS_DEFN(CyclesToSettle);

TA_BASEFUNS_CTORS_DEFN(ClosestEventStat);

TA_BASEFUNS_CTORS_DEFN(PatternFlagProcess);

TA_BASEFUNS_CTORS_DEFN(MultiEnvProcess);

TA_BASEFUNS_CTORS_DEFN(BridgeProcess);

TA_BASEFUNS_CTORS_DEFN(ForkProcess);

TA_BASEFUNS_CTORS_DEFN(SameSeedBatch);

TA_BASEFUNS_CTORS_DEFN(GridSearchBatch);

TA_BASEFUNS_CTORS_DEFN(SyncEpochProc);

TA_BASEFUNS_CTORS_DEFN(InitWtsProc);

TA_BASEFUNS_CTORS_DEFN(LoadWtsProc);

TA_BASEFUNS_CTORS_DEFN(SaveWtsProc);

TA_BASEFUNS_CTORS_DEFN(SaveNetsProc);

TA_BASEFUNS_CTORS_DEFN(ScriptProcess);

TA_BASEFUNS_CTORS_DEFN(FromFileEnv);

TA_BASEFUNS_CTORS_DEFN(DurEvent);

TA_BASEFUNS_CTORS_DEFN(GroupPatternSpec);

TA_BASEFUNS_CTORS_DEFN(XYSubPattern);

TA_BASEFUNS_CTORS_DEFN(XYSubPatternSpec);

TA_BASEFUNS_CTORS_DEFN(XYPattern);

TA_BASEFUNS_CTORS_DEFN(XYPatternSpec);

TA_BASEFUNS_CTORS_DEFN(ProbEventSpec);

TA_BASEFUNS_CTORS_DEFN(ProbPatternSpec_Group);

TA_BASEFUNS_CTORS_DEFN(ProbPattern);

TA_BASEFUNS_CTORS_DEFN(FreqTimeEnv);

TA_BASEFUNS_CTORS_DEFN(FreqTimeEvent_Group);

TA_BASEFUNS_CTORS_DEFN(FreqTimeEvent);

TA_BASEFUNS_CTORS_DEFN(TimeEnvironment);

TA_BASEFUNS_CTORS_DEFN(TimeEvent_Group);

TA_BASEFUNS_CTORS_DEFN(TimeEvent);

TA_BASEFUNS_CTORS_DEFN(FreqEnv);

TA_BASEFUNS_CTORS_DEFN(FreqEvent_Group);

TA_BASEFUNS_CTORS_DEFN(FreqEvent);

TA_BASEFUNS_CTORS_DEFN(InteractiveScriptEnv);

TA_BASEFUNS_CTORS_DEFN(ScriptEnv);

TA_BASEFUNS_CTORS_DEFN(Environment_List);

TA_BASEFUNS_CTORS_DEFN(Environment_Group);

TA_BASEFUNS_CTORS_DEFN(Environment);

TA_BASEFUNS_CTORS_DEFN(Event_Group);

TA_BASEFUNS_CTORS_DEFN(Event);

TA_BASEFUNS_CTORS_DEFN(Pattern);

TA_BASEFUNS_CTORS_DEFN(EventSpec_SPtr);

TA_BASEFUNS_CTORS_DEFN(EventSpec);

TA_BASEFUNS_CTORS_DEFN(PatternSpec_Group);

TA_BASEFUNS_CTORS_DEFN(PatternSpec);

TA_BASEFUNS_CTORS_DEFN(BatchProcess);

TA_BASEFUNS_CTORS_DEFN(TrainProcess);

TA_BASEFUNS_CTORS_DEFN(NEpochProcess);

TA_BASEFUNS_CTORS_DEFN(InteractiveEpoch);

TA_BASEFUNS_CTORS_DEFN(SequenceEpoch);

TA_BASEFUNS_CTORS_DEFN(SequenceProcess);

TA_BASEFUNS_CTORS_DEFN(EpochProcess);

TA_BASEFUNS_CTORS_DEFN(TrialProcess);

TA_BASEFUNS_CTORS_DEFN(SettleProcess);

TA_BASEFUNS_CTORS_DEFN(CycleProcess);

TA_BASEFUNS_CTORS_DEFN(SchedProcess);

TA_BASEFUNS_CTORS_DEFN(StepParams);

TA_BASEFUNS_CTORS_DEFN(Counter);

TA_BASEFUNS_CTORS_DEFN(MonitorStat);

TA_BASEFUNS_CTORS_DEFN(SE_Stat);

TA_BASEFUNS_CTORS_DEFN(Stat_Group);

TA_BASEFUNS_CTORS_DEFN(Stat);

TA_BASEFUNS_CTORS_DEFN(AggStat);

TA_BASEFUNS_CTORS_DEFN(StatValAgg);

TA_BASEFUNS_CTORS_DEFN(StatVal_List);

TA_BASEFUNS_CTORS_DEFN(StatVal);

TA_BASEFUNS_CTORS_DEFN(DataItem_List);

TA_BASEFUNS_CTORS_DEFN(DataItem);

TA_BASEFUNS_CTORS_DEFN(Process_Group);

TA_BASEFUNS_CTORS_DEFN(Process);

TA_BASEFUNS_CTORS_DEFN(Script);

TA_BASEFUNS_CTORS_DEFN(V3ScriptFile);

TA_BASEFUNS_CTORS_DEFN(CtrlPanelData);

TA_BASEFUNS_CTORS_DEFN(V3ProjectBase);

TA_BASEFUNS_CTORS_DEFN(PDPLog_Group);

TA_BASEFUNS_CTORS_DEFN(PDPLog);

TA_BASEFUNS_CTORS_DEFN(TypeDefault_Group);

TA_BASEFUNS_CTORS_DEFN(Pattern_Group);

TA_BASEFUNS_CTORS_DEFN(Script_Group);

TA_BASEFUNS_CTORS_LITE_DEFN(CritParam);

TA_BASEFUNS_CTORS_DEFN(float_RArray);


/////////////////////////
//      float_RArray    //
//////////////////////////

void float_RArray::InitLinks() {
  float_Array::InitLinks();
  taBase::Own(range, this);
}

void float_RArray::Copy_(const float_RArray& cp) {
  range = cp.range;
}


//////////////////////////
//      CritParam       //
//////////////////////////

void CritParam::Initialize() {
  flag = false;
  rel = LESSTHANOREQUAL;
  val = 0.0f;
  n_met = 0;
  cnt = 1;
};

void CritParam::Copy_(const CritParam& cp) {
  flag = cp.flag;
  rel = cp.rel;
  val = cp.val;
  cnt = cp.cnt;
  n_met = cp.n_met;
}

bool CritParam::Evaluate(float cmp) {
  if(!flag) return false;
  bool met = false;
  switch(rel) {
  case EQUAL:
    if(cmp == val)      met = true;
    break;
  case NOTEQUAL:
    if(cmp != val)      met = true;
    break;
  case LESSTHAN:
    if(cmp < val)       met = true;
    break;
  case GREATERTHAN:
    if(cmp > val)       met = true;
    break;
  case LESSTHANOREQUAL:
    if(cmp <= val)      met = true;
    break;
  case GREATERTHANOREQUAL:
    if(cmp >= val)      met = true;
    break;
  }
  if(met) {
    n_met++;
    if(n_met >= cnt) return true;
  }
  else
    n_met = 0;          // reset the counter (has to be consecutive)
  return false;
}


//////////////////////////
//      CtrlPanelData   //
//////////////////////////

void CtrlPanelData::Initialize() {
  active = false;
  lft = 0.0f;
  top = 0.0f;
}

#endif


///////////////////////////
//      Script          //
//////////////////////////

void Script::Initialize() {
  recording = false;
  auto_run = false;
}

// void Script::Destroy() {
//   CutLinks();
// }

// void Script::InitLinks() {
//   taNBase::InitLinks();
// //filer not a taBase  taBase::Own(script_file, this);
//   taBase::Own(s_args, this);
//   if(script_file->fname.empty())     // initialize only on startup up, not transfer
//     SetScript("");
// }

// void Script::CutLinks() {
//   StopRecording();
//   taNBase::CutLinks();
// }

// void Script::Copy_(const Script& cp) {
//   ScriptBase::Copy_(cp);
//   auto_run = cp.auto_run;
//   s_args = cp.s_args;
//   script_compiled = false; // redo compile
// }

// void Script::UpdateAfterEdit() {
//   // have to assume user changed something
//   script_compiled = false;
//   // if user supplied a string, then nuke the filename
//   if (!script_string.empty()) {
//     script_file->file_selected = false;
//     script_file->fname = _nilString;
//   }
//   // if based on a script, we automatically update our name
//   else if (!script_file->fname.empty()) {
//     name = script_file->fname;
//     if(name.contains(".css"))
//       name = name.before(".css");
//     name = taPlatform::getFileName(name); // strip path
//   }
//   taNBase::UpdateAfterEdit();
//   if (HasScript())
//     CompileScript();
// }

// bool Script::Run() {
//   return ScriptBase::RunScript();
// }

// void Script::Record(const char* file_nm) {
// #ifdef DMEM_COMPILE
//   if ((taMisc::dmem_nprocs > 1) && (taMisc::record_script != NULL)) {
//     taMisc::Error("Record: Cannot record a script under DMEM with the gui -- record script is used to communicate between processes");
//     return;
//   }
// #endif
//   Script_Group* mg = GET_MY_OWNER(Script_Group);
//   if(mg != NULL)
//     mg->StopRecording();
//   if (file_nm != NULL)
//     SetScript(file_nm);
//   if (script_file->fname.empty()) {
//     taMisc::Error("Record: No script file selected.\n Open a Script file and press Apply");
//     return;
//   }

//   ostream* strm = script_file->open_append();
//   if((strm == NULL) || strm->bad()) {
//     taMisc::Error("Record: Script file could not be opened:", script_file->fname);
//     script_file->Close();
//     return;
//   }

// #ifdef TA_GUI
//   taMisc::StartRecording(strm);
// #endif
//   recording = true;
// }

// void Script::StopRecording() {
//   if(!recording)
//     return;
// #ifdef TA_GUI
//   taMisc::StopRecording();
// #endif
//   script_file->Close();
//   recording = false;
// }

// void Script::Clear() {
//   if(recording) {
//     script_file->Close();
//     taMisc::record_script = script_file->open_write();
//     return;
//   }
//   if(script_file->fname.empty()) {
//     taMisc::Error("Clear: No Script File Selected\n Open a Script file and press Apply");
//     return;
//   }

//   ostream* strm = script_file->open_write();
//   if((strm == NULL) || strm->bad()) {
//     taMisc::Error("Clear: Script file could not be opened:", script_file->fname);
//   }
//   script_file->Close();
// }

// void Script::Compile() {
//   CompileScript();
// }

// void Script::CmdShell() {
//   CmdShellScript();
// }

// void Script::ExitShell() {
//   ExitShellScript();
// }

// void Script::AutoRun() {
//   if(!auto_run)
//     return;
//   Run();
// }




//////////////////////////
//      Script_Group    //
//////////////////////////

void Script_Group::Initialize() {
  SetBaseType(&TA_Script);
//  SetAdapter(new Script_GroupAdapter(this));
}

// void Script_Group::StopRecording() {
//   FOREACH_ELEM_IN_GROUP(Script, sb, *this)
//     sb->StopRecording();
// }

// void Script_Group::AutoRun() {
//   FOREACH_ELEM_IN_GROUP(Script, sb, *this)
//     sb->AutoRun();
// }


//////////////////////////
//      Process         //
//////////////////////////

void Process::Initialize() {
  min_network = &TA_Network;
  min_layer = &TA_Layer;
  min_unit = &TA_Unit;
  min_con_group = &TA_ConGroup;
  min_con = &TA_Connection;
  type=C_CODE;
  network = NULL;
  environment = NULL;
}

void DataItem::Initialize() {
  is_string = false;
  vec_n = 0;
  disp_opts = " ";              // always pad with initial blank
}

void StatVal::Initialize() {
  val = 0.0f;
}

void AggStat::Initialize() {
  from = NULL;
  real_stat = NULL;
  type_safe = true;
}

void Stat::Initialize() {
  own_proc = NULL;
  has_stop_crit = false;
  n_copy_vals = 0;
  loop_init = INIT_IN_LOOP;
  log_stat = true;
  layer = NULL;
}

void SE_Stat::Initialize() {
  tolerance = 0.0f;
}

void MonitorStat::Initialize() {
  net_agg.op = Aggregate::MEAN;
  variable = "act";
}

void StepParams::Initialize() {
  owner = NULL;
  proc = NULL;
  n = 1;
}

void SchedProcess::Initialize() {
  re_init=true;
  sub_proc_type = NULL;
  cntr = NULL;
  super_proc = NULL;
  sub_proc = NULL;
  log_loop = false;
  log_counter = false;
  loop_stats.SetBaseType(&TA_Stat);
  final_stats.SetBaseType(&TA_Stat);
  init_procs.SetBaseType(&TA_Process);
  loop_procs.SetBaseType(&TA_Process);
  final_procs.SetBaseType(&TA_Process);
  logs.SetBaseType(&TA_PDPLog);
  cntr_items.SetBaseType(&TA_DataItem);
}

void CycleProcess::Initialize() {
  sub_proc_type = NULL;
}

void SettleProcess::Initialize() {
  sub_proc_type = &TA_CycleProcess;
  cycle.SetMax(100);
  log_counter = true;
}

void TrialProcess::Initialize() {
  sub_proc_type = NULL;
  cur_event = NULL;
  epoch_proc = NULL;
  enviro_group = NULL;
}

void EpochProcess::Initialize() {
  sub_proc_type = &TA_TrialProcess;
  trial.SetMax(100);
  order = PERMUTED;
  cur_event = NULL;
  wt_update = ON_LINE;
  batch_n = 10;
  batch_n_eff = 10;
  enviro_group = NULL;
  dmem_nprocs = 1024;           // take a large number if possible!
}

void SequenceProcess::Initialize() {
  sub_proc_type = &TA_TrialProcess;
  tick.SetMax(10);
  order = SEQUENTIAL;
  sequence_init = INIT_STATE;
  sequence_epoch = NULL;
  cur_event_gp = NULL;
  cur_event = NULL;
  enviro_group = NULL;
}

void SequenceEpoch::Initialize() {
  sub_proc_type = &TA_SequenceProcess;
  small_batch = SEQUENCE;       // prior default setting..
  cur_event_gp = NULL;
}

void InteractiveEpoch::Initialize() {
  order = SEQUENTIAL;
  last_trial_val = -1;
}

void NEpochProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epc_ctr.SetMax(50);
  epoch_proc = NULL;
  log_counter = true;
  epoch = 0;
}

void TrainProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  epoch.SetMax(1000);
  epoch_proc = NULL;
  log_counter = true;
}

void BatchProcess::Initialize() {
  sub_proc_type = &TA_TrainProcess;
  batch.SetMax(10);
}

//////////////////////////////////
//      Environment             //
//////////////////////////////////

void PatternSpec::Initialize() {
  type = INACTIVE;
  to_layer = LAY_NAME;
  layer_num = 0;
  layer = NULL;
  pattern_type = &TA_Pattern;
  layer_flags = DEFAULT;
  use_flags = USE_NO_FLAGS;
  n_vals = 0;
  geom.y = 1;
  initial_val = 0.0f;
  noise.type = Random::NONE;
  noise.mean = 0.0f;
  noise.var = 0.5f;
}

void PatternSpec_Group::Initialize() {
  pat_gp_type = &TA_Pattern_Group;
  SetBaseType(&TA_PatternSpec);
}

void EventSpec::Initialize() {
  last_net = NULL;
  pattern_layout = HORIZONTAL;
}

void Pattern::Initialize() {
}

void Event::Initialize() {
  index = -1;
  patterns.SetBaseType(&TA_Pattern);
}

void Environment::Initialize() {
  events.SetBaseType(&TA_Event);
  event_specs.SetBaseType(&TA_EventSpec);
//TODO  views.SetBaseType(&TA_EnviroView);
  event_ctr = 0;
}

void ScriptEnv::Initialize() {
}

void FreqEvent::Initialize() {
  frequency = 1.0f;
}

void FreqEvent_Group::Initialize() {
  fenv = NULL;
  frequency = 1.0f;
}

void FreqEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP_EVENT;
  sample_type = PERMUTED;
  events.SetBaseType(&TA_FreqEvent);
  events.gp.SetBaseType(&TA_FreqEvent_Group);
}

void TimeEvent_Group::Initialize() {
  end_time = 1.0f;
  interpolate = USE_ENVIRO;
  SetBaseType(&TA_TimeEvent);
}

void TimeEnvironment::Initialize() {
  events.SetBaseType(&TA_TimeEvent);
  events.gp.SetBaseType(&TA_TimeEvent_Group);
  interpolate = CONSTANT;
}

void FreqTimeEnv::Initialize() {
  n_sample = 1;
  freq_level = GROUP;           // group is typically used with time
  sample_type = RANDOM;
  events.SetBaseType(&TA_FreqTimeEvent);
  events.gp.SetBaseType(&TA_FreqTimeEvent_Group);
}

void ProbPattern::Initialize() {
  prob = 1.0f;
  applied = false;
}

void ProbPatternSpec_Group::Initialize() {
  last_pat = -1;
}

void ProbEventSpec::Initialize() {
  patterns.gp.el_typ = &TA_ProbPatternSpec_Group;
  default_prob = .25f;
}


void XYPatternSpec::Initialize() {
  pattern_type = &TA_XYPattern;
  wrap = false;
  apply_background = false;
  background_value = 0.0f;
}

void XYSubPatternSpec::Initialize() {
  pattern_type = &TA_XYSubPattern;
  wrap = false;
}

void GroupPatternSpec::Initialize() {
  sub_geom.SetGtEq(1);
  taVector3i& gg = gp_geom;
  gg = geom / sub_geom;
  trans_apply = false;
}

//////////////////////////////////
//      Basic Processes         //
//////////////////////////////////

void ScriptProcess::Initialize() {
}

void SyncEpochProc::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
}

void GridSearchBatch::Initialize() {
  start_val = .1f;
  inc_val = .1f;
  cur_val = .1f;
}

void SameSeedBatch::Initialize() {
  rnd_seeds.SetBaseType(&TA_RndSeed);
  in_goto_val = -1;
}

void ForkProcess::Initialize() {
  second_proc_type = &TA_TrialProcess;
  second_proc = NULL;
  second_network = NULL;
  second_environment = NULL;
}

void BridgeProcess::Initialize() {
  second_network = NULL;
  src_layer = NULL;
  trg_layer = NULL;
  direction = ONE_TO_TWO;
  src_variable = "act";
  trg_variable = "ext";
  trg_ext_flag = Unit::EXT;
}

void MultiEnvProcess::Initialize() {
  log_counter = true;
  env.SetMax(0);
  use_subctr_max = false;
}

void PatternFlagProcess::Initialize() {
  sub_proc_type = &TA_EpochProcess;
  log_counter = true;
  pattern_no = 0;
  flag = PatternSpec::NO_FLAGS;
  invert = false;
  val_idx.SetMax(0);
}

//////////////////////////////////
//      Stats Processes         //
//////////////////////////////////

void ClosestEventStat::Initialize() {
  trial_proc = NULL;
  cmp_type = float_RArray::SUM_SQUARES;
  dist_tol = 0.0f;
  norm = false;
  subgp_no = -1;
  net_agg.op = Aggregate::MIN;
}

void CyclesToSettle::Initialize() {
  settle = NULL;
  net_agg.op = Aggregate::GROUP;
}

void ActThreshRTStat::Initialize() {
  loop_init = INIT_START_ONLY;
  settle = NULL;
  net_agg.op = Aggregate::GROUP;
  act_thresh = .75f;
  // max_act.stopcrit.flag = true;  // don't actually stop..
  max_act.stopcrit.val = act_thresh;
  max_act.stopcrit.rel = CritParam::GREATERTHANOREQUAL;
  crossed_thresh = false;
}

void ScriptStat::Initialize() {
}

void CompareStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  net_agg.op = Aggregate::GROUP;
}

void ProjectionStat::Initialize() {
  stat = NULL;
  dist_metric = float_RArray::INNER_PROD;
  dist_tol = 0.0f;
  dist_norm = false;
  net_agg.op = Aggregate::GROUP;
}

void ComputeStat::Initialize() {
  stat_1 = NULL;
  stat_2 = NULL;
  net_agg.op = Aggregate::MEAN;
}

void CopyToEnvStat::Initialize() {
  stat = NULL;
  data_env = NULL;
  accum_scope = SUPER;
  net_agg.op = Aggregate::GROUP;
}

void EpochCounterStat::Initialize() {
  net_agg.op = Aggregate::GROUP;
}

void ProcCounterStat::Initialize() {
  proc = NULL;
  net_agg.op = Aggregate::GROUP;
}

void MaxActTrgStat::Initialize() {
}

void UnitActRFStat::Initialize() {
  data_env = NULL;
  norm_mode = NORM_UNIT;
  net_agg.op = Aggregate::MEAN;
}

void UnitActRFStatResetProc::Initialize() {
  unit_act_rf_stat = NULL;
}

void UnitEventRFStat::Initialize() {
  epoch_proc = NULL;
  data_env = NULL;
  net_agg.op = Aggregate::MEAN;
}

void UniquePatStat::Initialize() {
  loop_init = INIT_START_ONLY;
  pat_stat = NULL;
  data_env = NULL;
  cmp_type = float_RArray::CORREL;
  dist_tol = 0.0f;
  norm = false;
  uniq_tol = .8f;
  net_agg.op = Aggregate::MEAN;
}

void TimeCounterStat::Initialize() {
  net_agg.op = Aggregate::MEAN;
  loop_init = INIT_START_ONLY;
}

void TimeCounterStatResetProc::Initialize() {
  time_ctr_stat = NULL;
}

void DispDataEnvProc::Initialize() {
  data_env = NULL;
  pat_no = 0;
  disp_type = RAW_DATA_GRID;
  disp_log = NULL;
  dist_metric = float_RArray::EUCLIDIAN;
  dist_norm = false;
  dist_tol = 0.0f;
  x_axis_component = 0;
  y_axis_component = 1;
}

void DispNetWeightsProc::Initialize() {
  recv_layer = NULL;
  send_layer = NULL;
  grid_log = NULL;
}

void ClearLogProc::Initialize() {
  log_to_clear = NULL;
}

//////////////////////////////////
//      TypeDefault_Group       //
//////////////////////////////////

int TypeDefault_Group::Dump_Load_Value(istream& strm, taBase* par) {
  Reset(); // get rid of any existing defaults before loading
  return inherited::Dump_Load_Value(strm, par);
}

//////////////////////////
//      PDPLog          //
//////////////////////////

void PDPLog::Initialize() {
  log_file = NULL; // maybe this var is not needed for streaming???

  data.name = "data"; // override the default one
  data_bufsz = 10000;
  data_shift = .20f;
  record_proc_name = false;
  geom.SetXYZ(4, 1, 3);
  pos.SetXYZ(0, 0, 0);
}

void PDPLog::InitLinks() {
  inherited::InitLinks();
  taBase::Own(data, this);
  taBase::Own(display_labels, this);
  taBase::Own(pos, this);
  taBase::Own(geom, this);
}

void PDPLog::Destroy() {
  CutLinks();
  if (log_file) {
    taRefN::unRefDone(log_file);
    log_file = NULL;
  }
}

void PDPLog::CutLinks() {
  geom.CutLinks();
  pos.CutLinks();
  display_labels.CutLinks();
  data.CutLinks();
  inherited::CutLinks();
}

/*nn void PDPLog::Copy_(const PDPLog& cp) {
  if (log_file) *log_file = *(cp.log_file);
  data = cp.data;
  data_bufsz = cp.data_bufsz;
  data_shift = cp.data_shift;
  record_proc_name = cp.record_proc_name;

  display_labels = cp.display_labels;
  pos = cp.pos;
  geom = cp.geom;
} */



//////////////////////////////////
//       V3ProjectBase          //
//////////////////////////////////

void V3ProjectBase::Initialize() {
  specs.SetBaseType(&TA_BaseSpec);
  environments.SetBaseType(&TA_Environment);
  processes.SetBaseType(&TA_SchedProcess);
  scripts.SetBaseType(&TA_Script);
}

void V3ProjectBase::InitLinks_impl() {
  inherited::InitLinks_impl();
  taBase::Own(defaults, this);
  taBase::Own(specs, this);
  taBase::Own(environments, this);
  taBase::Own(processes, this);
  taBase::Own(logs, this);
  taBase::Own(scripts, this);
}

void V3ProjectBase::CutLinks_impl() {
  scripts.CutLinks();
  logs.CutLinks();
  processes.CutLinks();
  environments.CutLinks();
  //  specs.CutLinks();  // don't do this! leads to infinite loops.
  defaults.CutLinks();
  inherited::CutLinks_impl();
}

void V3ProjectBase::Copy_(const V3ProjectBase& cp) {
  defaults = cp.defaults;
  specs = cp.specs;
  environments = cp.environments;
  processes = cp.processes;
  logs = cp.logs;
  scripts = cp.scripts;
}

void V3ProjectBase::ConvertToV4() {
  int ch = taMisc::Choice("This will convert the legacy v3.x project to v4.x format.\nYou may see some error messages but pay more attention\n to final success or failure message.  Do you want to continue?", "Yes", "No");
  if (ch != 0) return;
  if (ConvertToV4_impl())
    taMisc::Choice("The conversion was successful", "Ok");
  else
    taMisc::Choice("The conversion failed -- see console for mesesages", "Ok");
}

bool V3ProjectBase::ConvertToV4_impl() {
  taMisc::Error("Unable to convert a basic project -- must be a specific algorithm type!");
  return false;
}

bool V3ProjectBase::ConvertToV4_Script_impl(Program_Group* pg, const String& objnm,
                                            const String& fname, SArg_Array& s_args) {
  Program* prog = (Program*)pg->NewEl(1, &TA_Program);
  prog->name = objnm;
  UserScript* us = (UserScript*)prog->prog_code.New(1, &TA_UserScript);
  us->ImportFromFileName(fname); // looks on paths etc too..
  us->desc = "script imported from: " + fname;
  for(int i=0;i<s_args.size; i++) {
    ProgVar* pv = (ProgVar*)prog->args.New(1, &TA_ProgVar);
    pv->name = s_args.labels[i];
    pv->SetString(s_args[i]);
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_Enviros(ProjectBase* nwproj) {
  DataTable_Group* dgp = (DataTable_Group*)nwproj->data.FindMakeGpName("InputData");
  for(int ei=0; ei < environments.leaves; ei++) {
    Environment* env = environments.Leaf(ei);
    DataTable* dt = dgp->NewEl(1,&TA_DataTable);
    dt->name = env->name;
    String_Data* gpcol = NULL;
    int st_col = 0;             // starting column for standard fields
    if(env->events.gp.size > 0) {
      gpcol = dt->NewColString("Group");
      st_col++;
    }
    //String_Data* nmcol =
    dt->NewColString("Name");
    if(env->event_specs.size == 0) continue;
    EventSpec* es = (EventSpec*)env->event_specs[0];
    for(int pi=0; pi < es->patterns.size; pi++) {
      PatternSpec* ps = (PatternSpec*)es->patterns[pi];
      dt->NewColMatrix(DataCol::VT_FLOAT, ps->name, 2, MAX(ps->geom.x,1), MAX(ps->geom.y,1));
    }
    // also convert special data fields -- spec_type is very hacky local code for type
    int spec_type = 0;
    if(env->InheritsFrom(&TA_FreqEnv)) {
      dt->NewColFloat("frequency");
      spec_type = 1;
    }
    if(env->InheritsFrom(&TA_TimeEnvironment)) {
      dt->NewColFloat("time");
      spec_type = 2;
    }
    if(env->InheritsFrom(&TA_FreqTimeEnv)) {
      dt->NewColFloat("frequency");
      dt->NewColFloat("time");
      spec_type = 3;
    }
    if(env->events.leaves > 0 && env->events.Leaf(0)->InheritsFrom(&TA_DurEvent)) {
      dt->NewColFloat("duration");
      spec_type = 4;
    }

    FOREACH_ELEM_IN_GROUP(Event, ev, env->events) {
      dt->AddBlankRow();
      if(gpcol) {
        Event_Group* eg = (Event_Group*)ev->GetOwner();
        if(!eg->name.empty())
          dt->SetValAsString(eg->name, 0, -1); // -1 = last row
      }
      dt->SetValAsString(ev->name, st_col, -1); // -1 = last row
      if(ev->spec.SPtr() && ev->spec.SPtr() != es) { // not default spec, and not child thereof
        if(ev->spec.spec->GetOwner(&TA_EventSpec) != es)
          continue;
      }
      for(int pi=0; pi < ev->patterns.size; pi++) {
        Pattern* pat = (Pattern*)ev->patterns[pi];
        taMatrix* mat = dt->GetValAsMatrix(st_col + 1 + pi, -1);
        if(mat) {
          taBase::Ref(mat);
          for(int vi=0; vi<pat->value.size; vi++) {
            mat->SetFmVar_Flat(pat->value[vi], vi);
          }
          taBase::unRefDone(mat);
        }
      }
      if(spec_type == 1) {
        dt->SetValColName(((FreqEvent*)ev)->frequency, "frequency", -1);
      }
      if(spec_type == 2) {
        dt->SetValColName(((TimeEvent*)ev)->time, "time", -1);
      }
      if(spec_type == 3) {
        dt->SetValColName(((FreqTimeEvent*)ev)->time, "time", -1);
        dt->SetValColName(((FreqTimeEvent*)ev)->frequency, "frequency", -1);
      }
      if(spec_type == 4) {
        dt->SetValColName(((DurEvent*)ev)->duration, "duration", -1);
      }
    }
    if(env->InheritsFrom(&TA_ScriptEnv)) {
      ScriptEnv* se = (ScriptEnv*)env;
      if(!se->script_file.fname.empty()) {
        ConvertToV4_Script_impl(&nwproj->programs, env->name + "_ScriptEnv",
                                se->script_file.fname, se->s_args);
      }
    }
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_Nets(ProjectBase* nwproj) {
  FOREACH_ELEM_IN_GROUP(Network, net, networks) {
    net->specs = specs;         // copy specs into network
    net->ReplaceSpecs_Gp(specs, net->specs); // replace pointers

    // convert layer types!
    FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
      lay->UpdateAfterEdit();
      String lnm = lay->name;  lnm.downcase();
      if(lnm.contains("in") || lnm.contains("stim"))
        lay->layer_type = Layer::INPUT;
      else if(lnm.contains("out") || lnm.contains("trg") || lnm.contains("targ")
              || lnm.contains("resp"))
        lay->layer_type = Layer::TARGET;
      else
        lay->layer_type = Layer::HIDDEN;
    }
  }

  networks.el_base = nwproj->networks.el_base; // make sure the copy goes through!
  nwproj->networks = networks;  // this should the do spec updating automatically!

  FOREACH_ELEM_IN_GROUP(Network, net, nwproj->networks) {
    net->RemoveUnits();
    net->Build();
    net->NewView();
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_Scripts(ProjectBase* nwproj) {
  for(int i=0; i < scripts.size; i++) {
    Script* scr = scripts[i];
    if(!scr->script_file.fname.empty()) {
      ConvertToV4_Script_impl(&nwproj->programs, scr->name,
                                scr->script_file.fname, scr->s_args);
    }
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_ProcScripts_impl(ProjectBase* nwproj, taBase_Group* gp,
                                                 const String& nm_extra) {
  SArg_Array null_args;
  for(int i=0; i < gp->size; i++) {
    Process* proc = (Process*)gp->FastEl(i);
    if(proc->type == Process::C_CODE) continue;
    if(proc->script_file.fname.empty()) continue;
    if(proc->InheritsFrom(&TA_ScriptProcess)) {
      ConvertToV4_Script_impl(&nwproj->programs, proc->name + nm_extra,
                              proc->script_file.fname, ((ScriptProcess*)proc)->s_args);
    }
    else if(proc->InheritsFrom(&TA_ScriptStat)) {
      ConvertToV4_Script_impl(&nwproj->programs, proc->name + nm_extra,
                              proc->script_file.fname, ((ScriptStat*)proc)->s_args);
    }
    else {
      ConvertToV4_Script_impl(&nwproj->programs, proc->name + nm_extra,
                              proc->script_file.fname, null_args);
    }
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_ProcScripts(ProjectBase* nwproj) {
  // todo: in principle we could actually walk the procs and create name-for-name
  // replacement programs that would get loaded from prog_lib in the same places, etc
  // not sure this is worth it..

  FOREACH_ELEM_IN_GROUP(SchedProcess, proc, processes) {
    ConvertToV4_ProcScripts_impl(nwproj, (taBase_Group*)&proc->loop_stats, "_" + proc->name + "_loop_stats");
    ConvertToV4_ProcScripts_impl(nwproj, (taBase_Group*)&proc->final_stats, "_" + proc->name + "_final_stats");
    ConvertToV4_ProcScripts_impl(nwproj, (taBase_Group*)&proc->init_procs, "_" + proc->name + "_init_procs");
    ConvertToV4_ProcScripts_impl(nwproj, (taBase_Group*)&proc->loop_procs, "_" + proc->name + "_loop_procs");
    ConvertToV4_ProcScripts_impl(nwproj, (taBase_Group*)&proc->final_procs, "_" + proc->name + "_final_procs");
  }
  return true;
}

bool V3ProjectBase::ConvertToV4_Edits(ProjectBase* nwproj) {
  FOREACH_ELEM_IN_GROUP(ControlPanel, se, edits) {
    ControlPanel* ned = (ControlPanel*)nwproj->edits.New(1, &TA_ControlPanel);
    ned->CopyFrom(se);
  }
  nwproj->edits.UpdatePointers_NewPar(this, nwproj); // update select edit pointers!
  return true;
}

bool V3ProjectBase::ConvertToV4_DefaultApplyInputs(ProjectBase* nwproj) {
  if(nwproj->programs.gp.size == 0) {
    taMisc::Error("Default Program group was not found in DefaultApplyInputs");
    return false;
  }

  Program* apply_ins = ((Program_Group*)nwproj->programs.gp[0])->FindName("ApplyInputs");
  if(!apply_ins) return false;

  LayerWriter* lw = (LayerWriter*)apply_ins->objs.FindType(&TA_LayerWriter);
  if(!lw) return false;

  if(environments.leaves <= 0) return false;
  Environment* env = (Environment*)environments.Leaf(0);
  if(env->event_specs.size <= 0) return false;
  EventSpec* es = (EventSpec*)env->event_specs[0];

  DataTable_Group* dgp = (DataTable_Group*)nwproj->data.FindMakeGpName("InputData");
  DataTable* dt = (DataTable*)dgp->FindName(env->name);
  if(!dt) return false;

  if(networks.leaves <= 0) return false;
  Network* net = (Network*)nwproj->networks.Leaf(0);

  return ConvertToV4_ApplyInputs(lw, es, net, dt);
}

bool V3ProjectBase::ConvertToV4_ApplyInputs(LayerWriter* lw, EventSpec* es,
                                            Network* net, DataTable* dt) {
  lw->layer_data.Reset();
  lw->data = dt;
  lw->network = net;
  bool made_new;
  for(int i=0; i<es->patterns.size; i++) {
    PatternSpec* ps = (PatternSpec*)es->patterns[i];
    if(ps->type == PatternSpec::INACTIVE) continue;
    //LayerWriterEl* le = (LayerWriterEl*)
    lw->layer_data.FindMakeLayerData(ps->name, ps->layer_name, made_new);
    Layer* lay = (Layer*)net->layers.FindLeafName(ps->layer_name);
    if(lay) {
      if(ps->type == PatternSpec::INPUT) lay->layer_type = Layer::INPUT;
      else if(ps->type == PatternSpec::TARGET) lay->layer_type = Layer::TARGET;
      else if(ps->type == PatternSpec::COMPARE) lay->layer_type = Layer::OUTPUT;
    }
  }
  { // trial_name
    LayerWriterEl* le = (LayerWriterEl*)lw->layer_data.FindMakeChanName("Name", made_new);
    le->net_target = LayerDataEl::TRIAL_NAME;
  }
  if(dt->data.FindName("Group"))
  { // groupl_name
    LayerWriterEl* le = (LayerWriterEl*)lw->layer_data.FindMakeChanName("Group", made_new);
    le->net_target = LayerDataEl::GROUP_NAME;
  }
  lw->UpdateAfterEdit();        // sets subguys
  return true;
}

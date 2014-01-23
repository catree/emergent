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

#ifndef ActrEvent_h
#define ActrEvent_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ActrModuleRef>
#include <ActrAction>
#include <ActrChunkRef>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrEvent);

class E_API ActrEvent : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR an ActR event -- how time is managed in the system
INHERITED(taOBase)
public:
  static int            max_pri; // #NO_SAVE #HIDDEN maximum scheduling priority
  static int            min_pri;  // #NO_SAVE #HIDDEN minimum scheduling priority

  float                 time;   // time at which the event is to occur
  int                   priority; // priority for scheduling 0..max_pri
  ActrModuleRef         src_module; // the module that created the event
  ActrModuleRef         dst_module; // the module to send the event to
  ActrBufferRef         dst_buffer; // the buffer to operate on, if relevant
  String                action;     // what to do -- parsed by dst_module except !Stop! which stops execution
  String                params;     // literal parameters for action, comma delimited usually
  ActrActionRef         act_arg;    // action, if relevant
  ActrChunkPtr          chunk_arg;  // chunk, if relevant

  static ActrEvent*     NewEvent(float tm, int pri,
                                 ActrModule* src_mod, ActrModule* dst_mod,
                                 ActrBuffer* dst_buf, const String& act,
                                 const String& pars,
                                 ActrAction* actn = NULL, ActrChunk* chnk = NULL,
                                 TypeDef* event_type = NULL);
  // #CAT_ActR create a new event with given params

  virtual void     LogEvent(ActrModel* mod, DataTable* dt);
  // #CAT_ActR log the event to a log_table configured by ActrModel with relevant columns

  String& Print(String& strm, int indent = 0) const CPP11_OVERRIDE;
  String  GetDisplayName() const CPP11_OVERRIDE;
  String  GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }
  String  GetDesc() const CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(ActrEvent);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrEvent_h

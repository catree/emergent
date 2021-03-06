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

#ifndef Projection_h
#define Projection_h 1

// parent includes:
#include <SpecUser>

// member includes:
#include <ConSpec>
#include <ProjectionRef>
#include <ProjectionSpec>
#include <LayerRef>
#include <taColor>
#include <Random>
#include <SimpleMathSpec>
#include <Relation>

// declare all other types mentioned but not required to include:
class DataTable; //
class NetMonitor; //

eTypeDef_Of(Projection);

class E_API Projection: public SpecUser {
  // #STEM_BASE ##CAT_Network ##SCOPE_Network Projection describes connectivity between layers (from receivers perspective)
INHERITED(SpecUser)
public:
  enum PrjnSource {
    NEXT,               // Recv from the next layer in network
    PREV,               // Recv from the previous layer in network
    SELF,               // Recv from the same layer
    CUSTOM,             // Recv from the layer spec'd in the projection
    INIT                // #IGNORE used during create to skip err msgs
  };

  enum PrjnDirection {  // which direction does this projection come from
    FM_INPUT,           // from layer is closer to input signals than recv layer
    FM_OUTPUT,          // from layer is closer to output signals than recv layer
    LATERAL,            // from layer is same distance from input and output signals as this layer
    DIR_UNKNOWN,        // direction not set
  };

#ifdef __MAKETA__
  String                name;           // #READ_ONLY #SHOW name of the projection -- this is generated automatically based on the from name
#endif

  bool                  off;            // #DEF_false turn this projection off -- useful for experimenting with projections while being able to keep the specifications in place
  bool                  disp;           // display this projection in the network view display
  Layer*                layer;          // #READ_ONLY #NO_SAVE #HIDDEN #NO_SET_POINTER layer this prjn is in
  PrjnSource            from_type;      // #CAT_Structure Source of the projections
  LayerRef              from;           // #CAT_Structure #CONDEDIT_ON_from_type:CUSTOM layer receiving from (set this for custom)
  ProjectionSpec_SPtr   spec;           // #CAT_Structure spec for this item
  TypeDef*              con_type;       // #TYPE_Connection #CAT_Structure Type of connection
  ConSpec_SPtr          con_spec;       // #CAT_Structure conspec to use for creating connections

  int                   recv_idx;       // #READ_ONLY #CAT_Structure receiving con_group index
  int                   send_idx;       // #READ_ONLY #CAT_Structure sending con_group index
  int                   recv_n;         // #READ_ONLY #CAT_Structure #DEF_1 number of receiving con_groups allocated to this projection: almost always 1 -- some things won't work right if > 1 (e.g., copying)
  int                   send_n;         // #READ_ONLY #CAT_Structure number of sending con_groups: almost always 1 -- some things won't work right if > 1 (e.g., copying)

  bool                  projected;       // #HIDDEN #CAT_Structure t/f if connected

  bool                  dir_fixed;      // fix the direction setting to specific value shown below (cannot fix DIR_UNKNOWN) -- otherwise it is automatically computed based on the layer_type settings
  PrjnDirection         direction;      // #CAT_Structure #CONDEDIT_ON_dir_fixed which direction does this projection go (in terms of distance from input and output layers) -- auto computed by Compute_PrjnDirection when network is built, or you can manually set, but be sure to set dir_fixed to keep that setting; optionally used by only some algorithms
  taColor               prjn_clr;       // #CAT_Structure Default color for the projection line and arrow (subservient to the Type-defined color, if applicable)

  inline ConSpec*       GetConSpec()    { return con_spec.spec.ptr(); }
  // #CAT_Structure get the connection spec for this projection
  inline ProjectionSpec* GetPrjnSpec()  { return spec.spec.ptr(); }
  // #CAT_Structure get the projection spec for this projection

  // note: following defined in Layer.h
  inline bool   IsActive();
  // #CAT_Access is this projection active and valid?

  inline bool   NotActive() 
  { return !IsActive(); }
  // #CAT_Access is this projection NOT active and valid?

  inline bool   ValidIdxs() 
  { return ((recv_idx >= 0) && (send_idx >= 0)); }
  // #CAT_Access are the recv_idx and send_idx valid for this projection?

  virtual void  UpdateName();
  // #CAT_Structure update the name of projection to reflect any changes in where it receives from

  virtual void  ToggleOff();
  // #MENU #MENU_ON_Object #DYN1 #CAT_Structure toggle the off status of this projection -- if on, turn off, if off, turn on
  
  virtual void  SetFrom();
  // #CAT_Structure set where to receive from based on selections
  virtual void  SetCustomFrom(Layer* from_lay);
  // #MENU #MENU_ON_Actions #MENU_CONTEXT #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_from set a CUSTOM projection from given layer (if from_lay == layer, turns into SELF)

  virtual void  RemoveCons();
  // #MENU #MENU_ON_Actions #CONFIRM #CAT_Structure Reset all connections for this projection

  virtual void  Copy_Weights(const Projection* src);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_Weights copies weights from other projection

 virtual void  CheckSpecs();
  // #CAT_Structure check to make sure that specs are not null and set to the right type, and update with new specs etc to fix any errors (with notify), so that at least network operations will not crash -- called in Build and CheckConfig

  // convenience functions for those defined in the spec
  void  Connect_Sizes()         { spec->Connect_Sizes(this); }
  // #IGNORE first pass of connecting -- sets up all the Cons objects within units, and computes all the target allocation size information
  void  Connect_Cons()         { spec->Connect_Cons(this); }
  // #IGNORE third pass of connecting -- actually make the connections

  int   ProbAddCons(float p_add_con, float init_wt = 0.0)
  { return spec->ProbAddCons(this, p_add_con, init_wt); }
  // #MENU #MENU_ON_Actions #USE_RVAL #CAT_Structure probabilistically add a proportion of new connections to replace those pruned previously, init_wt = initial weight value of new connection

  void  Init_Weights_Prjn(ConGroup* cg, Network* net, int thr_no)
  { spec->Init_Weights_Prjn(this, cg, net, thr_no); }
  // #CAT_Weights custom initialize weights in this con group for given receiving unit ru
  void  Init_Weights_renorm(ConGroup* cg, Network* net, int thr_no)
  { spec->Init_Weights_renorm(this, cg, net, thr_no); }
  // #CAT_Weights #IGNORE renormalize weights -- done as a second pass after Init_Weights and before Init_Weights_post

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Weights apply given transformation to weights -- must call Init_Weights_post at network level after running this!
  virtual void  RenormWeights(bool mult_norm, float avg_wt);
  // #MENU #CAT_Weights renormalize the weight values using either multiplicative (for positive-only weight values such as Leabra) or subtractive normalization (for pos/neg weight values, such as backprop) to hit the given average weight value -- must call Init_Weights_post at network level after running this!
  virtual void  RescaleWeights(const float rescale_factor);
  // #MENU #CAT_Weights rescale weights by multiplying by given factor -- must call Init_Weights_post at network level after running this!
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Weights add noise to weights using given noise specification -- must call Init_Weights_post at network level after running this!
  virtual int   PruneCons(const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Weights remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)

  virtual bool  UpdateConSpecs(bool force = false);
  // #CAT_Structure update con specs for all connection groups for this projection in the network to use con_spec (only if changed from last update -- force = do regardless); returns true if changed and all cons can use given spec

  bool  ApplySpecToMe(BaseSpec* spec) override;
  
  virtual bool  SetPrjnSpec(ProjectionSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_spec.spec set the projection spec (connectivity pattern) for this projection
  virtual bool  SetConSpec(ConSpec* sp);
  // #BUTTON #DROP1 #DYN1 #CAT_Structure #INIT_ARGVAL_ON_con_spec.spec set the con spec for all connections in this prjn
  virtual bool  CheckConnect(bool quiet=false) { return spec->CheckConnect(this, quiet); }
  // #CAT_Structure check if projection is connected

  virtual int   ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp);
  // #CAT_Structure switch any connections/projections using old_sp to using new_sp
  virtual int   ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp);
  // #CAT_Structure switch any projections using old_sp to using new_sp

  virtual bool  SetConType(TypeDef* td);
  // #BUTTON #DYN1 #CAT_Structure #TYPE_Connection #INIT_ARGVAL_ON_con_type set the connection type for all connections in this prjn

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #DYN1 #CAT_Statistic monitor (record in a datatable) the given variable on this projection

  virtual void  SaveWeights(const String& fname="");
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (leave fname empty to pull up file chooser)
  virtual bool  LoadWeights(const String& fname="", bool quiet = false);
  // #BUTTON #MENU #EXT_wts #GHOST_OFF_flags:BUILT,INTACT #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (fmt is read from file) (leave fname empty to pull up file chooser)

  virtual DataTable*    WeightsToTable(DataTable* dt, const String& col_nm = "",
                                       bool recv_wts = true);
  // #MENU #NULL_OK  #NULL_TEXT_0_NewTable #CAT_Structure copy entire set of projection weights to given table (e.g., for analysis), with one row per receiving unit, and one column (name is layer name if not otherwise specified) that has a float matrix cell of the geometry of the sending layer -- recv prjn if recv_wts is true, else sending weights
  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "");
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var"

  String       GetTypeDecoKey() const override { return "Projection"; }
  int    GetEnabled() const    override { return !off; }
  void   SetEnabled(bool value) override { off = !value; }

  bool ChangeMyType(TypeDef* new_type) override;

  void  InitLinks() override;
  void  CutLinks() override;
  void  Copy_(const Projection& cp);
  TA_BASEFUNS(Projection);
protected:
  ConSpec*      m_prv_con_spec; // previous con spec set for cons

  void UpdateAfterEdit_impl() override;
  void UpdateAfterMove_impl(taBase* old_owner) override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
private:
  void  Initialize();
  void  Destroy();
};

// needs the actual layer include to compile TA file
#ifdef __TA_COMPILE__
#include <Layer>
#endif

#endif // Projection_h

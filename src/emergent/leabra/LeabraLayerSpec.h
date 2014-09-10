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

#ifndef LeabraLayerSpec_h
#define LeabraLayerSpec_h 1

// parent includes:
#include <LayerSpec>
#include <SpecMemberBase>

// member includes:
#include <Layer>
#include <AvgMaxVals>

// declare all other types mentioned but not required to include:
class LeabraLayer; // 
class LeabraNetwork; // 
class LeabraInhib; // 
class KwtaSortBuff; // 
class LeabraUnit; //
eTypeDef_Of(LeabraUnGpData);

eTypeDef_Of(LeabraInhibSpec);

class E_API LeabraInhibSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is computed in Leabra system (kwta, unit inhib, etc)
INHERITED(SpecMemberBase)
public:
  enum InhibType {		// how to compute the inhibition
    FF_FB_INHIB,                // simulated interneuron-like feedforward (proportional to avg netin) and feedback (proportional to avg act) inhibition
    KWTA_INHIB,			// between thresholds of k and k+1th most activated units (sets precise k value, should use i_kwta_pt = .2 std )
    KWTA_AVG_INHIB,		// average of top k vs avg of rest (provides more flexibility in actual k value, should use i_kwta_pt = .5 std)
    UNIT_INHIB,			// unit-based inhibition (g_i from netinput -- requires connections with inhib flag set to provide inhibition)
  };

  InhibType	type;		// #DEF_FF_FB_INHIB how to compute inhibition (resulting in inhibitory conductance g_i in units) -- FF_FB feedforward and feedback is the default and should be used except for legacy models using KWTA variants, or UNIT_INHIB for more biologically detailed models
  float		kwta_pt;	// #CONDSHOW_OFF_type:FF_FB_INHIB #DEF_0.2;0.5 [Defaults: .2 for KWTA_INHIB, .5 for KWTA_AVG] 
  float         gi;             // #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 [1.5-2.3 typical, can go much lower or higher as needed] overall gain on ff & fb inhibition -- this is main paramter to adjust to change overall activation levels -- FF_FB does NOT use kwta.pct parameter to set inhibition, so you must adjust it here
  float		ff;		// #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 #DEF_1 overall inhibitory contribution from feedforward inhibition -- computed from average netinput (i.e., synaptic drive into layer)
  float		fb;		// #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 #DEF_0.5;1 overall inhibitory contribution from feedback inhibition -- computed from average activation
  float		self_fb;	// #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 #DEF_0.5;0.02;0;1 individual unit self feedback inhibition -- can produce proportional activation behavior in individual units for specialized cases (e.g., scalar val or BG units), but not good for typical hidden layers (use .02 max)
  float         prv_trl_ff;     // #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 amount of previous trial's final ff inhibition to carry forward to the current trial -- this produces a temporal-derivative-like effect to suppress responses to sustained inputs, at the trial level
  float         prv_phs_ff;     // #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 amount of previous phase's final ff inhibition to carry forward to the current phase -- this produces a temporal-derivative-like effect to suppress responses to sustained inputs, at the phase level
  float         dt;             // #CONDSHOW_ON_type:FF_FB_INHIB #MIN_0 #DEF_0.7 time constant for integrating inhibitory values -- prevents oscillations that otherwise occur -- relatively rapid .7 typically works, but may need to go lower if oscillations are a problem
  bool          up_immed;       // #CONDSHOW_ON_type:FF_FB_INHIB inhibition rises immediately, and dt only applies to decay -- this is important for spiking units, but should otherwise generally be off for rate-coded units
  float         ff0;            // #CONDSHOW_ON_type:FF_FB_INHIB #DEF_0.1 feedforward zero point in terms of average netinput -- below this level, no FF inhibition is computed -- the 0.1 default should be good for most cases (and helps FF_FB match kwta dynamics more closely), but if FF inhib is not strong enough, you may need to lower it
  float		min_i;		// #CONDSHOW_OFF_type:FF_FB_INHIB #DEF_0 minimum inhibition value -- set this higher than zero to prevent units from getting active even if there is not much overall excitation

  inline float    FFInhib(const float netin) {
    float ffi = 0.0f;
    if(netin > ff0) ffi = ff * (netin - ff0);
    return ffi;
  }
  // feedforward inhibition value as function of netinput

  inline float    FBInhib(const float act) {
    float fbi = fb * act;
    return fbi;
  }
  // feedback inhibition value as function of netinput

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(LeabraInhibSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(KWTASpec);

class E_API KWTASpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies k-winner-take-all parameters
INHERITED(SpecMemberBase)
public:
  enum K_From {
    USE_K,			// use the k specified directly
    USE_PCT,			// use the percentage pct to compute the k as a function of layer size
    USE_PAT_K			// use the activity level of the current event pattern (k = # of units > pat_q)
  };

  K_From	k_from;		// how is the active_k determined: directly by k, by pct, or by no. of units where ext > pat_q
  int		k;		// #CONDSHOW_ON_k_from:USE_K desired number of active units in the layer
  float		pct;		// #CONDSHOW_ON_k_from:USE_PCT desired proportion of activity (used to compute a k value based on layer size, .25 std)
  float		pat_q;		// #CONDSHOW_ON_k_from:USE_PAT_K #DEF_0.2;0.5 threshold for pat_k based activity level: add to k if ext > pat_q
  float         avg_dt;         // #DEF_0.005 #MIN_0 time constant for integrating time-average values at the layer level -- e.g., acts_m_avg, which is useful for providing an estimate of actual expected activity levels in a layer, to compare against pct
  bool		diff_act_pct;	// #DEF_false if true, use different actual percent activity for expected overall layer activation -- the expected layer activation contributes to the normalization of net input scaling -- lower activity = stronger connections and vice-versa, so that different inputs with different activity levels are equated in their relative contribution to net input, by default.  Read the Leabra NetinScaling section of the online wiki docs for full details
  float		act_pct;	// #CONDSHOW_ON_diff_act_pct actual percent activity to put in kwta.pct field of layer -- see diff_act_pct for implications for netinput scaling, and read the Leabra NetinScaling section of the online wiki docs for full details

  // following are obsolete legacy parameters that have moved to GpInhibSpec
  bool		gp_i;		// #NO_SAVE #HIDDEN #READ_ONLY obsolete legacy parameter that has moved to GpInhibSpec
  float		gp_g;		// #NO_SAVE #HIDDEN #READ_ONLY obsolete legacy parameter that has moved to GpInhibSpec

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(KWTASpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(GpInhibSpec);

class E_API GpInhibSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specifies how inhibition is communicated among groups (either layers within a layer group or unit groups within a layer, both of which can operate according to different settings) -- each item's computed inhib vals contribute with a factor of gp_g (0-1) to a pooled inhibition value, which is the MAX over all these individual scaled inhibition terms -- the final inhibition value is then a MAX of the individual original (unscaled) inhibition and this pooled value -- depending on the gp_g factor, this can cause more weak items (layers or unit groups) to drop out
INHERITED(SpecMemberBase)
public:
  bool		on;		// compute grouped inhibition at the level specified (layers or unit groups within layers)
  bool          fffb;          // #CONDSHOW_ON_on (only valid when using FF_FB_INHIB inhibition) -- compute layer-level FFFB inhibition and apply that as a MAX relative to the unit-group level inhibition -- lay_gi is the layer-level inhibitory gain values -- typically you can use the same value used at the unit group level, and it just works..
  float         lay_gi;         // #CONDSHOW_ON_on&&fffb gain value for layer-level FF and FB inhibition -- typically same value as the group-level inhib.gi parameter
  float		gp_g;		// #CONDSHOW_ON_on&&!fffb #MIN_0 how much this item (layer or unit group) contributes to the pooled group-level inhibition values -- the higher the value (closer to 1) the stronger the overall pooled inhibition effect within the group, with 1 being a maximal amount of pooled inhibition
  float		self_g;	        // #CONDSHOW_ON_on #DEF_1 self inhibition gain factor -- typically 1, but can be set lower to cause this unit group to inhibit others, but not itself as strongly
  bool		diff_act_pct;	// #CONDSHOW_ON_on if true, adjust the expected overall layer activation by m-- the expected layer activation contributes to the normalization of net input scaling -- lower activity = stronger connections and vice-versa, so that different inputs with different activity levels are equated in their relative contribution to net input, by default.  Read the Leabra NetinScaling section of the online wiki docs for full details
  float		act_pct_mult;	// #CONDSHOW_ON_on&&diff_act_pct #MIN_0 #MAX_1 multiplier for expected percent activity in the layer -- multiplies value set by kwta spec (including its own diff_act_pct setting if set), to take into account the effects of group-level inhibition as set by this spec -- for unit group inhibition, should be roughly <expected groups active> / <total number of groups> -- the expected layer activation contributes to the normalization of net input scaling -- lower activity = stronger connections and vice-versa, so that different inputs with different activity levels are equated in their relative contribution to net input, by default.  Read the Leabra NetinScaling section of the online wiki docs for full details.
  bool		pct_fm_frac;	// #CONDSHOW_ON_on&&diff_act_pct get the act_pct_mult from 1/act_denom -- often easier to express in terms of denominator of fraction rather than straight percent
  float		act_denom;	// #CONDSHOW_ON_on&&diff_act_pct&&pct_fm_frac #MIN_1 1 over this value goes to act_pct_mult

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(GpInhibSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(ClampSpec);

class E_API ClampSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for clamping 
INHERITED(SpecMemberBase)
public:
  bool		hard;		// #DEF_true whether to hard clamp inputs where activation is directly set to external input value (act = ext, computed once at start of settle) or do soft clamping where ext is added into net input (net += gain * ext)
  float		gain;		// #CONDSHOW_OFF_hard #DEF_0.02:0.5 soft clamp gain factor (net += gain * ext)
  bool		max_plus;	// #CONDSHOW_ON_hard when hard clamping target activation values, the clamped activations are set to the maximum activation in the minus phase plus some fixed offset
  float		plus;		// #CONDSHOW_ON_hard&&max_plus #DEF_0.01 the amount to add to max minus phase activation in clamping the plus phase
  float		min_clamp;	// #CONDSHOW_ON_hard&&max_plus #DEF_0.5 the minimum clamp value allowed in the max_plus clamping system
  float         minus_targ_gain; // For TI models -- how much of the targ target value to add to the netinput during the minus phase

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(ClampSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(DecaySpec);

class E_API DecaySpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra holds decay values
INHERITED(SpecMemberBase)
public:
  float		event;		// #MIN_0 #MAX_1 [1 to clear] proportion decay of state vars between events
  float		phase;		// #MIN_0 #MAX_1 [1 for Leabra_CHL, 0 for CtLeabra_X/CAL] proportion decay of state vars between minus and plus phases 

  String       GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(DecaySpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };  // note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(CosDiffLrateSpec);

class E_API CosDiffLrateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra modulation of learning rate by the recv layer cos_diff between act_p and act_m (0..1 measure of how similar plus and minus phase are across layer) -- maximum learning in the zone of proximal development with mid-range cos_diff values, relative to a running-average cos_diff value, which gets a lrate multiplier of 1
INHERITED(SpecMemberBase)
public:
  bool          on;             // #DEF_true enable (but don't necessarily engage -- settings determined in LeabraConSpec) cos_diff modulated learning as function of  normalized inner product (cosine) between act_p and act_m -- turning this on automatically turns on a network flag to call Compute_CosDiff at end of plus phase -- only avg_dt is used for l_mix = X_COS_DIFF in LeabraConSpec, rest is used if cos_diff_lrate is selected
  float		lo_diff;	// #CONDSHOW_ON_on #MIN_0 #MAX_1 low end of the learning rate modulation function -- below this cos_diff level, use the constant lo_lrate value
  float		lo_lrate;	// #CONDSHOW_ON_on #MIN_0 learning rate multiplier in effect below the lo_diff cos_diff value
  float         hi_diff;        // #CONDSHOW_ON_on #MIN_0 #MAX_1 high end of the learning rate modulation function -- below this cos_diff level, use the constant hi_lrate value
  float		hi_lrate;	// #CONDSHOW_ON_on #MIN_0 #MAX_1 learning rate mulitiplier in effect above the hi_diff cos_diff value
  float		avg_dt;	        // #DEF_0.01 #CONDSHOW_ON_on #MIN_0 time constant for computing running average cos_diff value

  float	        LrateMod(const float diff_avg, const float cos_diff);
  // get the learning rate modulation factor based on running-average diff and current cos_diff
  void	        UpdtDiffAvg(float& diff_avg, const float cos_diff);
  // update the running average diff value -- guarantees lo_diff < diff_avg < hi_diff

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(CosDiffLrateSpec);
protected:
  SPEC_DEFAULTS;
  // void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};


eTypeDef_Of(LeabraLayerSpec);

class E_API LeabraLayerSpec : public LayerSpec {
  // #STEM_BASE ##CAT_Leabra Leabra layer specs, computes inhibitory input for all units in layer
INHERITED(LayerSpec)
public:
  enum InhibGroup {
    ENTIRE_LAYER,		// treat entire layer as one inhibitory group (even if subgroups exist)
    UNIT_GROUPS,		// treat sub unit groups as separate inhibitory groups -- only works if layer actually has unit groups -- layer-level inhibition contributes according to the unit_gp_inhib parameters if set
  };

  InhibGroup	inhib_group;	// #CAT_Activation what to consider the inhibitory group (layer or unit subgroups, or both)
  LeabraInhibSpec inhib;	// #CAT_Activation how to compute inhibition -- for kwta modes, a single global inhibition value is computed for the entire layer
  KWTASpec	kwta;		// #CONDSHOW_OFF_inhib_group:UNIT_GROUPS #CAT_Activation desired activity level over entire layer (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  KWTASpec	gp_kwta;	// #CONDSHOW_OFF_inhib_group:ENTIRE_LAYER #CAT_Activation desired activity level for units within unit groups (not for ENTIRE_LAYER) (NOTE: used to set target activity for UNIT_INHIB, AVG_MAX_PT_INHIB, but not used for actually computing inhib for these cases)
  GpInhibSpec	lay_gp_inhib;	// #CAT_Activation pooling of inhibition across layers within layer groups -- only applicable if the layer actually lives in a subgroup with other layers (and only in a first-level subgroup, not a sub-sub-group) -- each layer's computed inhib vals contribute with a factor of gp_g (0-1) to a pooled inhibition value, which is the MAX over all these individual scaled inhibition terms -- the final inhibition value for a given layer is then a MAX of the individual layer's original (unscaled) inhibition and this pooled value -- depending on the gp_g factor, this can cause more weak layers to drop out
  GpInhibSpec	unit_gp_inhib;	// #CAT_Activation #CONDSHOW_ON_inhib_group:UNIT_GROUPS pooling of inhibition across unit groups within layers -- only applicable if the layer actually has unit groups -- each unit group's computed inhib vals contribute with a factor of gp_g (0-1) to a pooled inhibition value, which is the MAX over all these individual scaled inhibition terms -- the final inhibition value for a given unit group is then a MAX of the individual unit group's original (unscaled) inhibition and this pooled value -- depending on the gp_g factor, this can cause more weak unit groups to drop out
  ClampSpec	clamp;		// #CAT_Activation how to clamp external inputs to units (hard vs. soft)
  DecaySpec	decay;		// #CAT_Activation decay of activity state vars between events, -/+ phase, and 2nd set of phases (if appl)
  CosDiffLrateSpec cos_diff_lrate;  // #CAT_Learning modulation of learning rate by the recv layer cos_diff between act_p and act_m -- maximum learning in the zone of proximal development

  ///////////////////////////////////////////////////////////////////////
  //	Access, status functions

  inline bool   HasUnitGpInhib(Layer* lay)
  { return (inhib_group == UNIT_GROUPS) && lay->unit_groups; }
  // does this layer have unit-group level inhibition?
  inline bool   HasLayerInhib(Layer* lay)
  { return !HasUnitGpInhib(lay); }
  // does this layer have unit-group level inhibition


  ///////////////////////////////////////////////////////////////////////
  //	General Init functions

  virtual void BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net);
  // #IGNORE build unit-level thread information: flat list of units, etc -- this is called by network BuildUnits_Threads so that layers (and layerspecs) can potentially modify which units get added to the compute lists, and thus which are subject to standard computations -- default is all units in the layer

  virtual void	SetLearnRule(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning set current learning rule from the network

  virtual void	Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning layer-level initialization taking place after Init_Weights on units
    virtual void Init_Inhib(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation called in Init_Weights_Layer initialize the inhibitory state values
    virtual void Init_Stats(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic called in Init_Weights_Layer intialize statistic variables

  virtual void	Init_Acts(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize unit-level dynamic state variables (activations, etc)
  virtual void	Init_ActAvg(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize act_avg values
  virtual void	Init_Netins(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)

  virtual void 	DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)

  ///////////////////////////////////////////////////////////////////////
  //	TrialInit -- at start of trial

  virtual void	Trial_Init_Specs(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags
  virtual void	Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning layer level trial init -- overload where needed

    virtual void Trial_DecayState(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation NOT CALLED DURING STD PROCESSING decay activations and other state between events
    virtual void Trial_NoiseInit(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation NOT CALLED DURING STD PROCESSING initialize various noise factors at start of trial
    virtual void Trial_NoiseInit_KPos_ugp(LeabraLayer* lay, 
						 Layer::AccessMode acc_md, int gpidx,
						 LeabraInhib* thr, LeabraNetwork* net);
    // #CAT_Activation NOT CALLED DURING STD PROCESSING initialize various noise factors at start of trial
    virtual void Trial_Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Learning NOT CALLED DURING STD PROCESSING reset the sender-receiver coproduct average (CtLeabra_X/CAL) -- calls unit-level function of same name

  ///////////////////////////////////////////////////////////////////////
  //	SettleInit -- at start of settling

  virtual void	Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: compute actual activity levels based on spec, inputs, etc
    virtual void Compute_Active_K_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
				      LeabraInhib* thr, KWTASpec& kwtspec);
    // #IGNORE unit gp version
    virtual int	Compute_Pat_K(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
			      LeabraInhib* thr);
    // #IGNORE PAT_K compute

  virtual void	Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase: all layer-level misc init takes place here (calls TargFlags_Layer) -- other stuff all done directly in Settle_Init_Units call

  virtual void	Settle_Init_TargFlags(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation initialize start of a setting phase, set input flags appropriately, etc
    virtual void Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net);
    // #IGNORE layer-level initialize start of a setting phase, set input flags appropriately, etc
  virtual void	Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation prior to settling: hard-clamp inputs

  virtual void	ExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void	TargExtToComp(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 1: Netinput 

  // main computation is direct Send_NetinDelta call on units through threading mechanism
  // followed by Compute_NetinInteg on units


  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 2: Inhibition

  virtual void	Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute AvgMax stats on netin values computed during netin computation -- used for FF_FB inhibition -- called automatically by Compute_Inhib
    virtual void Compute_NetinStats_ugp(LeabraLayer* lay,
					Layer::AccessMode acc_md, int gpidx,
					LeabraInhib* thr,  LeabraNetwork* net);
    // #IGNORE compute AvgMax stats on netin and i_thr values computed during netin computation -- per unit group

  virtual void	Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Activation compute the inhibition for layer -- this is the main call point into this stage of processing
    virtual void Compute_Inhib_impl(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx, 
			 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of inhibition computation for either layer or unit group

    virtual void Compute_Inhib_kWTA_Sort(LeabraLayer* lay, Layer::AccessMode acc_md,
					 int gpidx, int nunits,  LeabraInhib* thr,
					 KwtaSortBuff& act_buff, KwtaSortBuff& inact_buff,
					 int& k_eff, float& k_net, int& k_idx);
    // #CAT_Activation implementation of sort into active and inactive unit buffers -- basic to various kwta functions: eff_k = effective k to use, k_net = net of kth unit (lowest unit in act_buf), k_idx = index of kth unit

    virtual void Compute_Inhib_kWTA(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			   LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of basic kwta inhibition computation
    virtual void Compute_Inhib_kWTA_Avg(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			   LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of kwta avg-based inhibition computation
    virtual void Compute_Inhib_FfFb(LeabraLayer* lay,
			 Layer::AccessMode acc_md, int gpidx,
			   LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec);
    // #IGNORE implementation of feed-forward, feed-back inhibition computation

  virtual void	Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation Stage 2.2: for layer groups, need to propagate inhib out to unit groups

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Step 3: Activation

  // main function is basic Compute_Act which calls a bunch of sub-functions on the unitspec
  // called directly on units through threading mechanism

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats

  virtual void	Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no=-1);
  // #CAT_Statistic compute cycle-level stats -- acts AvgMax, OutputName, etc
  // this does all the indented functions below

    virtual int  LayerStatsStartUnitIdx() { return 0; }
    // #IGNORE unit index to start on for computing layer statistics -- ScalarValLayers set this to 1

    virtual void Compute_AvgMaxVals_ugp(LeabraLayer* lay, 
					Layer::AccessMode acc_md, int gpidx,
					AvgMaxVals& vals, ta_memb_ptr mb_off);
    // #IGNORE utility to compute avg max vals for units in group, with member offset mb_off from unit
    virtual void Compute_AvgMaxActs_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
					LeabraInhib* thr);
    // #IGNORE unit group compute AvgMaxVals for acts -- also does acts_top_k
    virtual void Compute_Acts_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute activation AvgMaxVals (acts)

    virtual void Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)
      virtual void Compute_OutputName_ugp(LeabraLayer* lay, 
					  Layer::AccessMode acc_md, int gpidx,
					  LeabraInhib* thr, LeabraNetwork* net);
      // #IGNORE compute the output_name field from the layer acts.max_i (only for OUTPUT or TARGET layers)

    virtual void Compute_UnitInhib_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Statistic compute unit inhibition AvgMaxVals (un_g_i)

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Stats -- optional non-default guys

  virtual float	Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average activation of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current act_eq values
    virtual float Compute_TopKAvgAct_ugp(LeabraLayer* lay,
					 Layer::AccessMode acc_md, int gpidx,
					 LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE ugp version

  virtual float	Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the average net input of the top k most active units (useful as a measure of recognition) -- requires a kwta inhibition function to be in use, and operates on current net values
    virtual float Compute_TopKAvgNetin_ugp(LeabraLayer* lay,
					   Layer::AccessMode acc_md, int gpidx,
					   LeabraInhib* thr, LeabraNetwork* net);
    // #IGNORE ugp version

  ///////////////////////////////////////////////////////////////////////
  //	Cycle Optional Misc

  virtual void	Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation do special processing midway through the minus phase, as determined by the mid_minus_cycle parameter, if > 0 -- currently used for the PBWM and Hippocampus algorithms -- stores act_mid

  ///////////////////////////////////////////////////////////////////////
  //	SettleFinal

  virtual void	PostSettle_Pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Activation perform computations in layers at end of settling -- this is a pre-stage that occurs prior to final PostSettle -- use this for anything that needs to happen prior to the standard PostSettle across layers (called by Settle_Final)
  virtual void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation after settling, keep track of phase variables, etc.
    virtual void PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get minus phase act stats
    virtual void PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net);
    // #CAT_Activation get plus phase act stats

  virtual void	Compute_ActM_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_m AvgMaxVals from act_m -- not currently used
  virtual void	Compute_ActP_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_p AvgMaxVals from act_p -- not currently used
  virtual void	Compute_ActCtxt_AvgMax(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation compute acts_ctxt AvgMaxVals from act_ctxt

  ///////////////////////////////////////////////////////////////////////
  //	LeabraTI

  virtual bool  TI_UpdateContextTest(LeabraLayer* lay, LeabraNetwork* net)
  { return true; }
  // #CAT_TI test whether TI context should be updated for this layer or not -- for gated layers (e.g., PFCLayerSpec), this is modulated by gating signal
  virtual void  TI_Compute_Deep5bAct(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TI compute CIFER deep 5b activation from thal and act_p
  virtual void  TI_Compute_CtxtAct(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TI compute TI context activation -- act_ctxt from net_ctxt
  virtual void  TI_ClearContext(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_TI clear the act_ctxt and net_ctxt context variables -- can be useful to do at clear discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //	TrialFinal

  virtual void	EncodeState(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning encode final state information at end of trial for time-based learning across trials
  virtual void	Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Activation update self-regulation (accommodation, hysteresis) at end of trial

  ///////////////////////////////////////////////////////////////////////
  //	Learning

  virtual void	Compute_SRAvg_State(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute state flag setting for sending-receiving activation averages (CtLeabra_X/CAL) -- called at the Cycle_Run level by network Compute_SRAvg_State -- unless manual_sravg flag is on, it just copies the network-level sravg_vals.state setting
  virtual void	Compute_SRAvg_Layer(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning compute layer-level sravg values -- called in advance of the unit level call -- updates the sravg_vals for this layer based on the sravg_vals.state flag
  virtual bool	Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning test whether to compute sravg values at the connection level (expensive) -- default is true, but some layers might opt out to save time

  virtual void	Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) { };
  // #CAT_Learning do special computations at layer level prior to standard unit-level thread dwt computation -- not used in base class but is in various derived classes

  virtual bool	Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning test whether to compute weight changes -- layers can opt out of learning entirely if they automatically don't learn

  virtual void	SetUnitLearnFlags(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Learning set the LEARN flags for all units in the layer

  ///////////////////////////////////////////////////////////////////////
  //	Trial-level Stats

  virtual float	Compute_SSE(LeabraLayer* lay, LeabraNetwork* net,
			    int& n_vals, bool unit_avg = false, bool sqrt = false);
  // #CAT_Statistic compute sum squared error of activation vs target over the entire layer -- always returns the actual sse, but unit_avg and sqrt flags determine averaging and sqrt of layer's own sse value

  virtual float	Compute_NormErr_ugp(LeabraLayer* lay, 
				    Layer::AccessMode acc_md, int gpidx,
				    LeabraInhib* thr, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error for given unit group -- just gets the raw sum over unit group -- sum += per unit: if (net->on_errs && act_m > .5 && targ < .5) return 1; if (net->off_errs && act_m < .5 && targ > .5) return 1; else return 0
  virtual float	Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute normalized binary error of unit targ vs. act_m -- layer-level value is already normalized, and network just averages across the layers (each layer contributes equally to overal normalized value, instead of contributing in proportion to number of units) -- returns -1 if not an err target defined in same way as sse -- per unit: if (net->on_errs && act_m > .5 && targ < .5) return 1; if (net->off_errs && act_m < .5 && targ > .5) return 1; else return 0; normalization is based on k value per layer: total possible err for both on and off errs is 2 * k (on or off alone is just k)

  virtual float  Compute_CosErr(LeabraLayer* lay, LeabraNetwork* net, int& n_vals);
  // #CAT_Statistic compute cosine (normalized dot product) of target compared to act_m over the layer -- n_vals is number of units contributing

  virtual float  Compute_CosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of phase activation difference in this layer: act_p compared to act_m -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values
  virtual float  Compute_AvgActDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute average act_diff (act_p - act_m) for this layer -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values -- this is an important statistic to track overall 'main effect' differences across phases 
  virtual float  Compute_TrialCosDiff(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute cosine (normalized dot product) of trail activation difference in this layer: act_p compared to p_act_p -- must be called after PostSettle (SettleFinal) for plus phase to get the act_p values

  virtual void	Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute the absolute layer-level and relative netinput from different projections into this layer
  virtual void	Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net);
  // #CAT_Statistic compute time-average relative netinput from different projections into this layer (e.g., every epoch)


  ////////////////////////////////////////////
  //	Misc structural routines

  virtual LeabraLayer* FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec
  virtual LeabraLayer* FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec);
  // #CAT_Structure find a layer that given layer receives from based on the type of layer spec: uses exact type match, not inherits!
  static  LeabraLayer* FindLayerFmSpecNet(Network* net, TypeDef* layer_spec);
  // #CAT_Structure find a layer in network based on the type of layer spec

  virtual void	HelpConfig();	// #BUTTON #CAT_Structure get help message for configuring this spec
  bool CheckConfig_Layer(Layer* lay, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms, including settings on the processes NOTE: this routine augments the default layer checks, it doesn't replace them

  virtual TypeDef* 	UnGpDataType()  { return &TA_LeabraUnGpData; }
  // #CAT_Structure type of unit group data object to create for the layers associated with this layer spec

  void	InitLinks();
  SIMPLE_COPY(LeabraLayerSpec);
  TA_BASEFUNS(LeabraLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ CutLinks(); }
  void	Defaults_init();
};

SPECPTR_OF(LeabraLayerSpec);

#endif // LeabraLayerSpec_h

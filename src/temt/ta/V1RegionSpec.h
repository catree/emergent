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

#ifndef V1RegionSpec_h
#define V1RegionSpec_h 1

// parent includes:
#include <VisRegionSpecBase>

// member includes:
#include <V1GaborSpec>
#include <V1KwtaSpec>
#include <V1sNeighInhib>
#include <XYNGeom>
#include <V1MotionSpec>
#include <V1BinocularSpec>
#include <V1ComplexSpec>
#include <V2BordOwnSpec>
#include <V2BordOwnStencilSpec>
#include <VisSpatIntegSpec>
#include <float_Matrix>
#include <int_Matrix>
#include <CircMatrix>

// declare all other types mentioned but not required to include:
class DataTable; // 
class V1RetinaProc; // 


class TA_API V1RegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of V1 simple and complex filters -- used as part of overall V1Proc processing object -- produces Gabor and more complex filter activation outputs directly from image bitmap input -- each region is a separate matrix column in a data table (and network layer), and has a specified spatial resolution
INHERITED(VisRegionSpecBase)
public:
  enum BinocularFilters { // #BITS flags for V1 binocular (V1B) filters to run, for computing disparity information across two visual inputs
    BF_NONE	= 0, // #NO_BIT
    V1B_DSP	= 0x0001, // basic disparity computation -- MIN(L,R) matching on v1pi_out features
    V1B_AVGSUM	= 0x0002, // compute weighted average summary of the disparity signals over entire field -- result is a single scalar value that can be fed into a ScalarValLayerSpec layer to provide an input representation to a network, for example -- output is a single 1x1 data table cell
#ifndef __MAKETA__
    BF_DEFAULT  = V1B_DSP,	     // #IGNORE #NO_BIT this is the default setup
#endif
  };

  enum ComplexFilters { // #BITS flags for specifying which complex filters to include
    CF_NONE	= 0, // #NO_BIT
    LEN_SUM	= 0x0001, // length summing cells -- just average along oriented line
    END_STOP	= 0x0002, // end stop cells -- len sum minus single same orientation point after a gap -- requires LEN_SUM
#ifndef __MAKETA__
    CF_DEFAULT  = LEN_SUM | END_STOP,  // #IGNORE #NO_BIT this is the default setup
#endif
  };

  enum V2Filters { // #BITS flags for specifying which v2 filters to include
    V2_NONE	= 0, // #NO_BIT
    V2_TL	= 0x0001, // compute V2 T and L junction detectors -- prereq for V2_BO too
    V2_BO	= 0x0002, // compute V2 border ownership output, integrating length sum and TL junction signals
#ifndef __MAKETA__
    V2_DEFAULT  = V2_NONE,  // #IGNORE #NO_BIT this is the default setup
#endif
  };

  enum DspHoriz {	// for storing disparity match information
    DHZ_LEN,		// length of current horizontal line structure -- how many horiz orients in a row is this one part of
    DHZ_START,		// starting x-axis index where the horiz line starts
    DHZ_ORIG_OFF,	// original offset value for this line, which was replaced by computed value
    DHZ_N,		// number of disparity match values to record
  };

  enum OptionalFilters { // #BITS flags for specifying additional output filters -- misc grab bag of outputs
    OF_NONE	= 0, // #NO_BIT
    ENERGY	= 0x0001, // overall energy of V1S feature detectors -- a single unit activation per spatial location in same resolution map as V1S -- output map is just 2D, not 4D -- and is always saved to a separate output column -- suitable for basic spatial mapping etc
  };

  enum SpatIntegFilters { // #BITS flags for specifying what is subject to additional spatial integration -- multiple can be selected
    SI_NONE	= 0, // #NO_BIT
    SI_V1S	= 0x0001, // V1 simple cell, with all selected polarities and color contrasts (but not motion or binocular depth)
    SI_V1PI	= 0x0002, // V1 polarity invariance, which just has angles and does a max over polarities and color contrasts -- lower dimensionality
    SI_V1PI_SG	= 0x0004, // V1 square grouped polarity invariance -- lower resolution grouped version of polarity invariant reps
    SI_V1S_SG	= 0x0008, // V1 square grouped raw V1S full polarity/color values -- lower resolution grouped version of V1S reps -- note that the SG version of V1S is not otherwise needed so this is what triggers its computation -- also unless SEP_MATRIX is flagged, this is output in one table with SI_V1C or SI_V2BO if those are also selected
    SI_V1C	= 0x0010, // V1 complex, which is length sum and end stop operating on top of V1SG square grouped (if sg4 option selected)
    SI_V2BO	= 0x0020, // V2 border output cells, integrating length sum and T, L junction detectors that inform border ownership
  };

  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X = 0,
    Y = 1,
  };
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE = 0,	   // along the direction of the line
    ORTHO = 1,	   // orthogonal to the line
  };
  enum OnOff {   // on (excitatory) vs. off (inhibitory)
    ON = 0,
    OFF = 1,
  };
  enum LeftRight {   // left vs. right -- directionality
    LEFT = 0,
    RIGHT = 1,
  };
  enum Angles {   // angles, 4 total
    ANG_0 = 0,
    ANG_45 = 1,
    ANG_90 = 2,
    ANG_135 = 3,
  };
  enum AngDir {   // ang = angle, dir = direction
    ANG = 0,
    DIR = 1,
  };

  /////////// Simple
  V1GaborSpec	v1s_specs;	// specs for V1 simple filters, computed using gabor filters directly onto the incoming image
  RenormMode	v1s_renorm;	// #DEF_LIN_RENORM how to renormalize the output of v1s static filters -- applied prior to kwta
  V1KwtaSpec	v1s_kwta;	// k-winner-take-all inhibitory dynamics for the v1 simple stage -- important for cleaning up these representations for subsequent stages, especially binocluar disparity and motion processing, which require correspondence matching, and end stop detection
  V1sNeighInhib	v1s_neigh_inhib; // specs for V1 simple neighborhood-feature inhibition -- inhibition spreads in directions orthogonal to the orientation of the features, to prevent ghosting effects around edges
  DataSave	v1s_save;	// how to save the V1 simple outputs for the current time step in the data table
  XYNGeom	v1s_img_geom; 	// #READ_ONLY #SHOW size of v1 simple filtered image output -- number of hypercolumns in each axis to cover entire output
  XYNGeom	v1s_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 simple filtering -- n_angles (x) * 2 or 6 polarities (y; monochrome|color) -- configured automatically

  /////////// Motion
  V1MotionSpec	v1s_motion;	// #CONDSHOW_OFF_motion_frames:0||motion_frames:1 specs for V1 motion filters within the simple processing layer
  RenormMode	v1m_renorm;	// #CONDSHOW_OFF_motion_frames:0||motion_frames:1 #DEF_LIN_RENORM how to renormalize the output of v1s motion filters
  XYNGeom	v1m_feat_geom; 	// #READ_ONLY size of one 'hypercolumn' of features for V1 motion filtering -- always x = angles; y = 2 * speeds

  /////////// Binocular
  BinocularFilters v1b_filters; // #CONDSHOW_ON_region.ocularity:BINOCULAR which binocular (V1B) filtering to perform to compute disparity information across the two eyes
  V1BinocularSpec  v1b_specs;	// #CONDSHOW_ON_region.ocularity:BINOCULAR specs for V1 binocular filters -- comes after V1 simple processing in binocular case
  RenormMode	v1b_renorm;	 // #CONDSHOW_ON_region.ocularity:BINOCULAR #DEF_LIN_RENORM how to renormalize the output of v1b filters -- applies ONLY to basic v1b_dsp_out -- is generally a good idea because disparity computation results in reduced activations overall due to MIN operation
  DataSave	v1b_save;	// #CONDSHOW_ON_region.ocularity:BINOCULAR how to save the V1 binocular outputs for the current time step in the data table
  XYNGeom	v1b_feat_geom; // #CONDSHOW_ON_region.ocularity:BINOCULAR #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 binocular disparity output -- disp order: near, focus, far -- [v1s_feats.n or x][tot_disps]

  /////////// Complex
  ComplexFilters v1c_filters; 	// which complex cell filtering to perform
  V1ComplexSpec v1c_specs;	// specs for V1 complex filters -- comes after V1 binocular processing 
  RenormMode	v1c_renorm;	// #DEF_LIN_RENORM how to renormalize the output of v1c filters, prior to kwta -- currently only applies to length sum
  V1KwtaSpec	v1ls_kwta;	// k-winner-take-all inhibitory dynamics for length sum level
  V1sNeighInhib	v1ls_neigh_inhib; // specs for V1 length-sum neighborhood-feature inhibition -- inhibition spreads in directions orthogonal to the orientation of the features, to prevent ghosting effects around edges
  DataSave	v1c_save;	// how to save the V1 complex outputs for the current time step in the data table

  XYNGeom	v1sg_img_geom; 	// #READ_ONLY size of v1 square grouping output image geometry -- input is v1s_img_geom, with either 2x2 or 4x4 spacing of square grouping operations reducing size by that amount
  XYNGeom	v1c_img_geom; 	// #READ_ONLY #SHOW size of v1 complex filtered image output -- number of hypercolumns in each axis to cover entire output -- this is equal to v1sq_img_geom if sq_gp4 is on, or v1s_img_geom if not
  XYNGeom	v1c_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 complex filtering -- includes length sum and end stop in combined output -- configured automatically with x = n_angles

  ////////// V2
  V2Filters	v2_filters;	// which V2 filtering to perform
  V2BordOwnSpec	v2_specs;	// specs for V2 filtering
  V2BordOwnStencilSpec	v2_ffbo; // specs for V2 feed-forward border-ownership computation
  DataSave	v2_save;	// how to save the V2 complex outputs for the current time step in the data table

  SpatIntegFilters spat_integ;	// what to perform spatial integration over
  VisSpatIntegSpec si_specs;	// spatial integration output specs
  RenormMode	si_renorm;	// how to renormalize spat integ output prior to performing kwta
  V1KwtaSpec	si_kwta;	// k-winner-take-all inhibitory dynamics for spatial integration output -- 
  DataSave	si_save;	// how to save the spatial integration outputs for the current time step in the data table
  XYNGeom	si_v1c_geom; 	// #READ_ONLY #SHOW size of spat integ v1c image output

  OptionalFilters opt_filters; 	// optional filter outputs -- always rendered to separate tables in data table
  DataSave	opt_save;	// how to save the optional outputs for the current time step in the data table

  //////////////////////////////////////////////////////////////
  //	Geometry and Stencils

  ///////////////////  V1S Geom/Stencils ////////////////////////
  int		n_colors;	// #READ_ONLY number of color channels to be processed (1 = monochrome, 4 = full color)
  int		n_polarities;	// #READ_ONLY #DEF_2 number of polarities per color -- always 2
  int		n_polclr;	// #READ_ONLY number of polarities * number of colors -- y dimension of simple features for example
  float_Matrix	v1s_gabor_filters; // #READ_ONLY #NO_SAVE gabor filters for v1s processing [filter_size][filter_size][n_angles]
  float_Matrix	v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles
  float_Matrix	v1s_ang_slopes_raw; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles -- non-normalized
  int_Matrix	v1s_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]

  ////////////////// V1M Motion Geom/Stencils
  int		v1m_in_polarities; // #READ_ONLY number of polarities used in v1 motion input processing (for history, etc) -- always set to 1 -- using polarity invariant inputs
  float_Matrix	v1m_weights;  	// #READ_ONLY #NO_SAVE v1 simple motion weighting factors (1d)
  int_Matrix	v1m_stencils; 	// #READ_ONLY #NO_SAVE stencils for motion detectors, in terms of v1s location offsets through time [x,y][1+2*tuning_width][motion_frames][directions:2][angles][speeds] (6d)
  int_Matrix	v1m_still_stencils; // #READ_ONLY #NO_SAVE stencils for motion detectors -- detecting stillness, in terms of v1s location offsets through time [x,y][1+2*tuning_width][motion_frames][angles] (4d)

  ///////////////////  V1B Geom/Stencils ////////////////////////
  int_Matrix	v1b_widths; 	// #READ_ONLY #NO_SAVE width of stencils for binocularity detectors 1d: [tot_disps]
  float_Matrix	v1b_weights;	// #READ_ONLY #NO_SAVE v1 binocular gaussian weighting factors for integrating disparity values into v1b unit activations -- for each tuning disparity [max_width][tot_disps] -- only v1b_widths[disp] are used per disparity
  int_Matrix	v1b_stencils; 	// #READ_ONLY #NO_SAVE stencils for binocularity detectors, in terms of v1s location offsets per image: 2d: [XY][max_width][tot_disps]

  ///////////////////  V1C Geom/Stencils ////////////////////////
  int_Matrix	v1sg_stencils; 	// #READ_ONLY #NO_SAVE stencils for v1 square grouping -- represents center points of the lines for each angle [x,y,len][5][angles] -- there are 5 points for the 2 diagonal lines with 4 angles -- only works if n_angles = 4 and line_len = 4 or 5
  int_Matrix	v1ls_stencils;  // #READ_ONLY #NO_SAVE stencils for complex length sum cells [x,y][len_sum_width][angles]
  int_Matrix	v1ls_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]
  int_Matrix	v1es_stencils;  // #READ_ONLY #NO_SAVE stencils for complex end stop cells [x,y][pts=3(only stop)][len_sum,stop=2][dirs=2][angles] -- new version

  ///////////////////  V2 Stencils
  int_Matrix	v2tl_stencils; 	// #READ_ONLY #NO_SAVE stencils for V2 T & L-junction detectors
  int_Matrix	v2ffbo_stencils;  // #READ_ONLY #NO_SAVE stencils for V2 feedforward border ownership inputs from length sum
  int_Matrix	v2ffbo_stencil_n; // #READ_ONLY #NO_SAVE number of points per stencil
  float_Matrix	v2ffbo_weights;  // #READ_ONLY #NO_SAVE weights for V2 feedforward border ownership inputs from length sum
  float_Matrix	v2ffbo_norms;  // #READ_ONLY #NO_SAVE normalization constant for V2 feedforward border ownership inputs from length sum

  ///////////////////  Spat Integ Stencils / Geom
  float_Matrix	si_weights;	// #READ_ONLY #NO_SAVE spatial integration weights for weighting across rf
  XYNGeom	si_v1s_geom; 	// #READ_ONLY size of spat integ v1s image output
  XYNGeom	si_v1sg_geom; 	// #READ_ONLY size of spat integ v1sg image output

  //////////////////////////////////////////////////////////////
  //	Outputs

  ///////////////////  V1S Output ////////////////////////
  float_Matrix	v1s_out_r_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_l_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_gci;	 // #READ_ONLY #NO_SAVE v1 simple cell inhibitory conductances, for computing kwta
  float_Matrix	v1s_ithr;	 // #READ_ONLY #NO_SAVE v1 simple cell inhibitory threshold values -- intermediate vals used in computing kwta
  float_Matrix	v1s_out_r;	 // #READ_ONLY #NO_SAVE v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_l;	 // #READ_ONLY #NO_SAVE v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1pi_out_r;  	 // #READ_ONLY #NO_SAVE polarity invariance over v1 simple cell output -- max over polarities, right eye [feat.x][1][img.x][img.y]
  float_Matrix	v1pi_out_l;  	 // #READ_ONLY #NO_SAVE polarity invariance over v1 simple cell output -- max over polarities, left eye [feat.x][1][img.x][img.y]

  ///////////////////  V1M Motion Output ////////////////////////
  float_Matrix	v1m_out_r;	 // #READ_ONLY #NO_SAVE v1 motion cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = speed * dir * [0=on,1=off -- luminance only]
  float_Matrix	v1m_out_l;	 // #READ_ONLY #NO_SAVE v1 motion cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = speed * dir * [0=on,1=off -- luminance only] 
  float_Matrix	v1m_maxout_r;	 // #READ_ONLY #NO_SAVE v1 motion cell output -- max over directions, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = v1m_in_polarities
  float_Matrix	v1m_maxout_l;	 // #READ_ONLY #NO_SAVE v1 motion cell output -- max over directions, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = v1m_in_polarities
  float_Matrix	v1m_hist_r;	 // #READ_ONLY #NO_SAVE history of v1 simple cell output for motion computation, right eye [feat.x][feat.y][img.x][img.y][time] -- feat.y = [0=on,1=off -- luminance only]
  float_Matrix	v1m_hist_l;	 // #READ_ONLY #NO_SAVE history of v1 simple cell output for motion computation, left eye [feat.x][feat.y][img.x][img.y][time] -- feat.y = [0=on,1=off -- luminance only]
  CircMatrix	v1m_circ_r;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time -- attached to v1m_hist_r
  CircMatrix	v1m_circ_l;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time -- attached to v1m_hist_l
  float_Matrix	v1m_still_r; // #READ_ONLY #NO_SAVE places with stable features across motion window (no motion), for each v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off -- luminance only]
  float_Matrix	v1m_still_l; // #READ_ONLY #NO_SAVE places with stable features across motion window (no motion), for each v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off -- luminance only]

  ///////////////////  V1B Binocular Output ////////////////////////
  int_Matrix	v1b_dsp_horiz;	 // #READ_ONLY #NO_SAVE horizontal line ambiguity resolution data -- length and start of horizontal line structures [DHZ_N][img.x][img.y]
  float_Matrix	v1b_dsp_out;  	 // #READ_ONLY #NO_SAVE disparity output -- [v1b_feat.x][v1b_feat.y][img.x][img.y]  (v1b_feat.x = angles, v1b_feat.y = tot_disps disparities)
  float_Matrix	v1b_dsp_out_tmp; // #READ_ONLY #NO_SAVE temp buffer for disparity output -- used for cross-res computation -- [v1b_feat.x][v1b_feat.y][img.x][img.y]  (v1b_feat.x = angles or total v1s features, v1b_feat.y = tot_disps disparities)
  float		v1b_avgsum_out;	 // #READ_ONLY #NO_SAVE v1b avgsum output (single scalar value which is average summary of disparity values)

  ///////////////////  V1C Complex Output ////////////////////////
  float_Matrix	v1sg_out;	 // #READ_ONLY #NO_SAVE square 4x4 grouping of polarity invariant V1 reps -- reduces dimensionality and introduces robustness -- operates on v1pi inputs [v1pi_feat.x][1][v1sq_img.x][v1sq_img.y]
  float_Matrix	v1ls_out_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) length sum output -- operates on v1pi or v1sg inputs -- [feat.x][1][v1c_img.x][v1c_img.y]
  float_Matrix	v1ls_out;	 // #READ_ONLY #NO_SAVE length sum output after kwta [feat.x][1][v1c_img.x][v1c_img.y]
  float_Matrix	v1ls_gci;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory conductances, for computing kwta
  float_Matrix	v1ls_ithr;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory threshold values -- intermediate vals used in computing kwta
  float_Matrix	v1es_out;	 // #READ_ONLY #NO_SAVE end stopping output -- operates on length sum and raw v1s/v1pi input [feat.x][2][v1c_img.x][v1c_img.y]
  float_Matrix	v1es_gci;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory conductances, for computing kwta

  ///////////////////  V2 Output ////////////////////////
  float_Matrix	v2tl_out;	 // #READ_ONLY #NO_SAVE V2 T and L junction detector output [feat.x][4][v1c_img.x][v1c_img.y]
  float_Matrix	v2tl_max;	 // #READ_ONLY #NO_SAVE max activation over v2tl_out -- [v1c_img.x][v1c_img.y]
  float_Matrix	v2bos_out;	 // #READ_ONLY #NO_SAVE V2 border ownership on simple inputs output -- integrates T and L with ambiguous edge signals, to provide input suitable for network settling dynamics [feat.x][2][v1c_img.x][v1c_img.y]
  float_Matrix	v2bo_out;	 // #READ_ONLY #NO_SAVE V2 border ownership output -- integrates T and L with ambiguous edge signals, to provide input suitable for network settling dynamics [feat.x][2][v1c_img.x][v1c_img.y]
  float_Matrix	v2bo_lat;	 // #READ_ONLY #NO_SAVE V2 border ownership lateral integration

  ///////////////////  SI Spatial Integration Output ////////////////////////
  float_Matrix	si_gci;	 	// #READ_ONLY #NO_SAVE inhibitory conductances, for computing kwta
  float_Matrix	si_v1s_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_out_raw;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_sg_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_sg_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_sg_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_sg_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	v1s_sg_out;	 // #READ_ONLY #NO_SAVE square grouping of v1s 
  float_Matrix	v1s_sg_out_raw;	 // #READ_ONLY #NO_SAVE square grouping of v1s 
  float_Matrix	si_v1c_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1c_out_raw;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v2bo_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v2bo_out_raw; // #READ_ONLY #NO_SAVE spatial integration

  ///////////////////  OPT optional Output ////////////////////////
  float_Matrix	energy_out;	 // #READ_ONLY #NO_SAVE energy at each location: max activation over features for each image location -- [img.x][img.y]

  int		AngleDeg(int ang_no);
  // get angle value in degress based on angle number
  virtual void	GridGaborFilters(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 Gabor Filters into the data table
  virtual void	GridV1Stencils(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 stencils into data table and generate a grid view -- these are the effective receptive fields at each level of processing
  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view -- one row for each type of filter (scroll to see each in turn) -- light squares show bounding box of rf, skipping every other

  virtual void 	V1bDspCrossResMin(float extra_width=0.0f, int max_extra=4, float pct_to_min=0.5f);
  // #CAT_V1B integrate v1b_dsp_out values across different resolutions within the same parent V1RetinaProc object -- call this after first pass processing, before applying results -- extra_width is multiplier on rf size needed to map between layer sizes that is added to the lower resolution side, to deal with extra blurring at lower res relative to higher res -- max_extra is maximum such extra to use, in actual pixel values -- pct_to_min is how far proportionally to move toward the minimum value -- 1 = full MIN, 0 = ignore cross res constraints entirely

  virtual void 	V2BoDepthFmFg(V1RetinaProc* all_flat, float fg_thr=0.1f);
  // #CAT_V2BO use figure-only image coding in v2bo_out from this retina proc as a mask to allocate a flat image with figure and background elements (all_flat) into a depth-coded output column called _v2bo_fgbg in data_table -- operates across all resolutions present in current V1RetinaProc -- just call on first one -- result is output directly to the v2bo_out with depth_out > 1 
  virtual void 	V2BoDepthFmImgMask(DataTable* img_mask, V1RetinaProc* all_flat, float fg_thr=0.1f);
  // #CAT_V2BO figure-only image coding in v2bo_out from this retina proc AND image mask in datatable from input image mask as a mask to allocate a flat image with figure and background elements (all_flat) into a depth-coded output column called _v2bo_fgbg in data_table -- operates across all resolutions present in current V1RetinaProc -- just call on first one -- result is output directly to the v2bo_out with depth_out > 1 -- img_mask must have sequential columns starting at 0 with correct sizes of image mask values

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1RegionSpec);
protected:
  float_Matrix* cur_out_acts;	// cur output activations -- for kwta thing
  float_Matrix* cur_still;	// cur still for motion
  float_Matrix* cur_maxout;	// cur maxout for motion
  float_Matrix* cur_hist;	// cur hist for motion
  float_Matrix* cur_v1b_in_r;	// current v1b input, r
  float_Matrix* cur_v1b_in_l;	// current v1b input, l

  override void	UpdateAfterEdit_impl();

  override void	UpdateGeom();

  override bool NeedsInit();
  override bool InitFilters();
  override bool InitOutMatrix();
  override bool InitDataTable();

  virtual bool	InitFilters_V1Simple();
  virtual bool	InitFilters_V1Motion();
  virtual bool	InitFilters_V1Binocular();
  virtual bool	InitFilters_V1Complex();
  virtual bool	InitFilters_V2();
  virtual bool	InitFilters_SpatInteg();

  virtual float V2FFBoWt(taVector2i& suc, int rang_dx, int sang_dx, int rdir, int sdir);
  // compute V2 Feed-forward border-ownership weights for stencils

  override bool	FilterImage_impl(bool motion_only = false);
  override void IncrTime();

  virtual bool	V1SimpleFilter();
  // do simple filters -- main wrapper
  virtual bool	V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
				      float_Matrix* out);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Static_thread(int v1s_idx, int thread_no);
  // do simple filters, static only on current inputs -- do it
  virtual void 	V1SimpleFilter_Static_neighinhib_thread(int v1s_idx, int thread_no);
  // do neighborhood inhibition on simple filters
  virtual bool 	V1SimpleFilter_PolInvar(float_Matrix* v1s_out_in, float_Matrix* v1pi_out);
  // polarity invariance: max polarities of v1s_out 
  virtual void 	V1SimpleFilter_PolInvar_thread(int v1s_idx, int thread_no);
  // polarity invariance: max polarities of v1s_out 

  virtual bool	V1SimpleFilter_Motion(float_Matrix* in, float_Matrix* out, float_Matrix* maxout, 
		      float_Matrix* still, float_Matrix* hist, CircMatrix* circ);
  // do simple filters, motion on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no);
  // do simple filters, motion on current inputs -- do it
  virtual void 	V1SimpleFilter_Motion_CpHist_thread(int v1s_idx, int thread_no);
  // do simple motion filters, copy v1s to history
  virtual void 	V1SimpleFilter_Motion_Still_thread(int v1s_idx, int thread_no);
  // do simple motion filters, compute non-moving (still) background

  virtual bool	V1BinocularFilter();
  // do binocular filters -- dispatch threads
  virtual void 	V1BinocularFilter_MinLr_thread(int v1s_idx, int thread_no);
  // do binocular filters -- compute MIN(left,right) version of matches between eyes
  virtual void 	V1BinocularFilter_HorizTag_thread(int v1s_idx, int thread_no);
  // do binocular filters -- initial tag of horizontal line structures
  virtual void 	V1BinocularFilter_HorizAgg();
  // do binocular filters -- aggregation of initial tags and ambiguity resolution
  virtual void 	V1BinocularFilter_AvgSum();
  // v1 binocular weighted-average summary

  virtual bool	V1ComplexFilter();
  // do complex filters -- dispatch threads
  virtual void 	V1ComplexFilter_SqGp4_thread(int v1sg_idx, int thread_no);
  // square-group4 if selected
  virtual void 	V1ComplexFilter_LenSum_thread(int v1c_idx, int thread_no);
  // length-sum
  virtual void 	V1ComplexFilter_LenSum_neighinhib_thread(int v1c_idx, int thread_no);
  // do neighborhood inhibition on ls filters
  virtual void 	V1ComplexFilter_EndStop_thread(int v1c_idx, int thread_no);
  // end stop

  virtual bool	V2Filter();
  // do V2 filters -- dispatch threads
  virtual void 	V2Filter_TL_thread(int v1c_idx, int thread_no);
  // t&l junctions
  virtual void 	V2Filter_FFBO_thread(int v1c_idx, int thread_no);
  // feedforward border ownership computation
  virtual void 	V2Filter_LatBO_thread(int v1s_idx, int thread_no);
  // lateral interactions for BO
  virtual void 	V2Filter_LatBOinteg_thread(int v1s_idx, int thread_no);
  // lateral integration of BO
  virtual void 	V2Filter_BOfinal_thread(int v1s_idx, int thread_no);
  // final cleanup of BO -- renormalize ambiguous to sum to full activation value

  virtual bool	SpatIntegFilter();
  // do spatial integration filters -- dispatch threads
  virtual void 	SpatIntegFilter_V1S_thread(int v1s_idx, int thread_no);
  virtual void 	SpatIntegFilter_V1PI_thread(int v1s_idx, int thread_no);
  virtual void 	SpatIntegFilter_V1PI_SG_thread(int v1sg_idx, int thread_no);
  virtual void 	SpatIntegFilter_V1S_SqGp4_thread(int v1sg_idx, int thread_no);
  virtual void 	SpatIntegFilter_V1S_SG_thread(int v1sg_idx, int thread_no);
  virtual void 	SpatIntegFilter_V1C_thread(int v1c_idx, int thread_no);
  virtual void 	SpatIntegFilter_V2BO_thread(int v1c_idx, int thread_no);

  virtual bool	V1OptionalFilter();
  // do optional filters -- dispatch threads
  virtual void 	V1OptionalFilter_Energy_thread(int v1s_idx, int thread_no);
  // energy as max activation output of v1pi_out_r

  virtual bool V1SOutputToTable(DataTable* dtab, bool fmt_only = false);
  // simple to output table
  virtual bool V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out, const String& col_sufx,
				     bool fmt_only = false);
  // simple to output table impl
  virtual bool V1MOutputToTable_impl(DataTable* dtab, float_Matrix* out, float_Matrix* maxout,
     float_Matrix* still, float_Matrix* hist, CircMatrix* circ, const String& col_sufx,
     bool fmt_only = false);
  // motion to output table impl
  virtual bool V1BOutputToTable(DataTable* dtab, bool fmt_only = false);
  // binocular to output table
  virtual bool V1COutputToTable(DataTable* dtab, bool fmt_only = false);
  // complex to output table
  virtual bool V2OutputToTable(DataTable* dtab, bool fmt_only = false);
  // V2 to output table
  virtual bool SIOutputToTable(DataTable* dtab, bool fmt_only = false);
  // Spat Invar to output table
  virtual bool OptOutputToTable(DataTable* dtab, bool fmt_only = false);
  // optional to output table
};

#endif // V1RegionSpec_h
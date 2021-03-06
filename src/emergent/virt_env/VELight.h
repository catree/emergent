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

#ifndef VELight_h
#define VELight_h 1

// parent includes:
#include <VEBody>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector3f>
#include <VELightParams>

// declare all other types mentioned but not required to include:
class SoLight; // #IGNORE

eTypeDef_Of(VELight);

class E_API VELight : public VEBody {
  // virtual environment light -- a body that contains a light source -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- light only affects items after it in the list of objects!
INHERITED(VEBody)
public:
  enum LightType {
    DIRECTIONAL_LIGHT,          // shines in a given direction, rotation is used for the direction, but position is not -- fastest rendering speed
    POINT_LIGHT,                // radiates in all directions, uses position for location
    SPOT_LIGHT,                 // shines in a given direction from a given position
  };

#ifdef __MAKETA__
  LongAxis      long_axis;      // #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a light
#endif

  LightType     light_type;     // type of light
  VELightParams light;          // light parameters
  float         drop_off_rate;  // #CONDSHOW_ON_light_type:SPOT_LIGHT (0-1) how fast light drops off with increasing angle from the direction angle -- this only really has an effect when shadows are turned on
  float         cut_off_angle;  // #CONDSHOW_ON_light_type:SPOT_LIGHT (30 default) angle in degrees from the direction vector where there will be no light -- the smaller the value, the stronger the shadow effect when shadows are on -- with 45 shadows are very light
  taVector3f    dir_norm;       // #READ_ONLY #SHOW normal vector for where the camera is facing

#ifdef TA_QT3D
  // virtual SoLight*      CreateLight();
  // #IGNORE create the So light of correct type
  // virtual void          ConfigLight(SoLight* lgt);
  // #IGNORE config So light parameters
#else // TA_QT3D
  virtual SoLight*      CreateLight();
  // #IGNORE create the So light of correct type
  virtual void          ConfigLight(SoLight* lgt);
  // #IGNORE config So light parameters
#endif // TA_QT3D

  virtual bool          UpdateLight();
  // #BUTTON if environment is already initialized and viewed, this will update the light in the display based on current settings
  virtual void          UpdtDirNorm();
  // update dir_norm from cur_quat rotation

  void Init() override;
  void CurFromODE(bool updt_disp = false) override;
  void CurToODE() override;

  TA_SIMPLE_BASEFUNS(VELight);
private:
  void  Initialize();
  void  Destroy() { };
};

SMARTREF_OF(E_API, VELight); // VELightRef

#endif // VELight_h

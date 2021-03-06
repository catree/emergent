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

#ifndef VEBody_h
#define VEBody_h 1

// parent includes:
#include <taNBase>
#include "network_def.h"

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector3f>
#include <taAxisAngle>
#include <taQuaternion>
#include <taTransform>
#include <taColor>
#include <VETexture>
#include <VESurface>
#include <ODEIntParams>

#ifndef __MAKETA__
#include <ode/ode.h>
#endif

// declare all other types mentioned but not required to include:
class VEWorld; // 
class VEObject; // 

eTypeDef_Of(ODEDamping);

class E_API ODEDamping : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE damping parameters
INHERITED(taOBase)
public:
  bool          on;             // #DEF_false whether to use any of these damping parameters
  float         lin;            // #DEF_0 #CONDSHOW_ON_on The body's linear damping scale.
  float         lin_thr;        // #DEF_0 #CONDSHOW_ON_on The body's linear damping threshold. Damping will be applied only if the linear speed is above the threshold limit
  float         ang;            // #DEF_0 #CONDSHOW_ON_on The body's angular damping scale.
  float         ang_thr;        // #DEF_0 #CONDSHOW_ON_on The body's angular damping threshold. Damping will be applied only if the angular speed is above the threshold limit
  float         ang_speed;      // #DEF_0 #CONDSHOW_ON_on You can also limit the maximum angular speed. In contrast to the damping functions, the angular velocity is affected before the body is moved. This means that it will introduce errors in joints that are forcing the body to rotate too fast. Some bodies have naturally high angular velocities (like cars' wheels), so you may want to give them a very high (like the default, infinity) limit.

  TA_SIMPLE_BASEFUNS(ODEDamping);
private:
  void  Initialize();
  void  Destroy()       { };
};

eTypeDef_Of(ODEFiniteRotation);

class E_API ODEFiniteRotation : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv finite rotation mode settings
INHERITED(taOBase)
public:
  bool          on;             // #DEF_false whether to enable finite rotation mode -- False: An "infinitesimal" orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. True: A finite orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
  taVector3f  axis; // #CONDSHOW_ON_on This sets the finite rotation axis for a body, if finite rotation mode is being used. If this axis is zero (0,0,0), full finite rotations are performed on the body. If this axis is nonzero, the body is rotated by performing a partial finite rotation along the axis direction followed by an infinitesimal rotation along an orthogonal direction. This can be useful to alleviate certain sources of error caused by quickly spinning bodies. For example, if a car wheel is rotating at high speed you can set the wheel's hinge axis here to try and improve its behavior.

  TA_SIMPLE_BASEFUNS(ODEFiniteRotation);
private:
  void  Initialize();
  void  Destroy()       { };
};

class VEBody; //
SMARTREF_OF(E_API, VEBody); // VEBodyRef

eTypeDef_Of(VEBody);

class E_API VEBody : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vebod virtual environment body (rigid structural element), subject to physics dynamics
INHERITED(taNBase)
public:
  enum BodyFlags { // #BITS flags for bodies
    BF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // turn this object off -- do not include in the virtual world
    FIXED               = 0x0002, // body cannot move at all
    EULER_ROT           = 0x0004, // use euler rotation angles instead of axis and angle
    PLANE2D             = 0x0008, // body is constrained to the Z=0 plane
    FM_FILE             = 0x0010, // load object image features from Inventor (iv) object file
    NO_COLLIDE          = 0x0020, // this body is not part of the collision detection system -- useful for light beams and other ephemera
    CUR_FM_FILE         = 0x0040, // #NO_SHOW #READ_ONLY current flag setting load object image features from Inventor (iv) object file
    GRAVITY_ON          = 0x0080, // does gravitational force affect this body?
    VERBOSE             = 0x0100, // where applicable output verbose messages for this body (e.g., in carousel, lists names of objects being loaded)
    INIT_WAS_ABS        = 0x1000, // #NO_BIT init vals were specified in abs coords last time (not rel)
  };
    //    COLLIDE_FM_FILE       = 0x0008, // use object shape from file for collision detection (NOTE: this is more computationally expensive and requires trimesh feature to be enabled in ode)

  enum Shape {                  // shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,                    // a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
    NO_SHAPE,                   // no shape at all -- only for special classes like lights
  };
  enum LongAxis {
    LONG_X=1,                   // long axis is in X direction
    LONG_Y,                     // long axis is in Y direction
    LONG_Z,                     // long axis is in Z direction
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*         body_id;        // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the body (cast to a dBodyID which is dxbody*)
  void*         geom_id;        // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the body (cast to a dGeomID which is dxgeom*)
  BodyFlags     flags;          // flags for various body properties

  bool          init_rel;       // initial values are all set relative to another body (rel_body) -- when true the init_ vals are all read-only and set automatically in update after edit and during Init
  VEBodyRef     rel_body;       // #CONDSHOW_ON_init_rel other body that our positions, rotation, and velocities are computed relative to -- in general better if rel_body is before this one in list of objects, especially if it is also relative to something else, so everything gets updated in the proper order.  definitely avoid loops!
  taVector3f    rel_pos;       // #CONDSHOW_ON_init_rel initial position of body relative to rel_body -- set to current during Init() call
  taAxisAngle   rel_rot;       // #CONDSHOW_OFF_flags:EULER_ROT||!init_rel initial rotation of body in terms of an axis and angle (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- relative to rel_body -- set to current during Init() call
  taVector3f    rel_euler;     // #CONDSHOW_ON_flags:EULER_ROT&&init_rel initial rotation of body in Euler angles in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854 -- set to current during Init() call
  taVector3f    rel_lin_vel;   // #CONDSHOW_ON_init_rel initial linear velocity relative to rel_body -- set to current during Init() call
  taVector3f    rel_ang_vel;   // #CONDSHOW_ON_init_rel initial angular velocity relative to rel_body -- set to current during Init() call

  taVector3f    init_pos;       // #CONDEDIT_OFF_init_rel initial position of body -- set to current during Init() call -- if init_rel then auto-computed
  taAxisAngle   init_rot;       // #CONDEDIT_OFF_flags:EULER_ROT||init_rel initial rotation of body in terms of an axis and angle (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- set to current during Init() call -- if init_rel then auto-computed
  taVector3f    init_euler;     // #CONDEDIT_ON_flags:EULER_ROT&&!init_rel initial rotation of body in Euler angles in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854 -- set to current during Init() call -- if init_rel then auto-computed
  taQuaternion  init_quat;      // #READ_ONLY initial rotation of body in Quaternion form -- automatically converted from init_rot or init_euler depending on EULER_ROT flag
  taVector3f    init_lin_vel;   // #CONDEDIT_OFF_init_rel initial linear velocity -- set to current during Init() call -- if init_rel then auto-computed
  taVector3f    init_ang_vel;   // #CONDEDIT_OFF_init_rel initial angular velocity -- set to current during Init() call -- if init_rel then auto-computed

  taVector3f    cur_pos;        // current position of body
  taAxisAngle   cur_rot;        // #CONDSHOW_OFF_flags:EULER_ROT current rotation of body (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  taVector3f    cur_euler;      // #CONDSHOW_ON_flags:EULER_ROT current rotation of body in Euler angles in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854
  taQuaternion  cur_quat;       // #READ_ONLY current rotation of body in Quaternion form
  taQuaternion  cur_quat_raw;   // #READ_ONLY current rotation of body in Quaternion form -- raw version without corrective rotation for capsules or cylinders
  taVector3f    cur_lin_vel;    // current linear velocity
  taVector3f    cur_ang_vel;    // current angular velocity

  Shape         shape;          // shape of body for purposes of mass/inertia and collision (and visual rendering if not FM_FILE)
  float         mass;           // total mass of body (in kg)
  float         radius;         // #CONDSHOW_OFF_shape:BOX radius of body, for all but box
  float         length;         // #CONDSHOW_OFF_shape:BOX,SPHERE length of body, for all but box
  LongAxis      long_axis;      // #CONDSHOW_OFF_shape:BOX,SPHERE direction of the long axis of the body (where length is oriented) -- only for CAPSULE and CYLINDER -- this creates an additional rotation between init_ rotation and cur_ rotation, only applied at time of init
  taVector3f    box;            // #CONDSHOW_ON_shape:BOX length of box in each axis for BOX-shaped body

  taTransform obj_xform;        // #CONDSHOW_ON_flags:FM_FILE full transform to apply to body file to align/size/etc with body
  String        obj_fname;      // #CONDSHOW_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes body appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool          set_color;      // if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor       color;          // #CONDSHOW_ON_set_color default color of body if not otherwise defined ('a' alpha value used for transparency -- 1 = fully opaque (transparency = 0), 0 = fully transparent (transparency = 1)) -- this is the diffuse color, which reflects light sources in a diffuse way
  taColorPhong  phong_color;    // #CONDSHOW_ON_set_color phong-style color parameters, which give a more 3D structure to the shading of the object
  VETextureRef  texture;        // #SCOPE_VEWorld #NULL_OK texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface     surface;        // physics surface properties of the body (softness, bounciness)
  ODEIntParams  softness;       // set the cfm and erp values higher here to make the surface softer
  ODEDamping    damp;           // Linear and angular damping, to remove energy and generally provide greater robustness
  ODEFiniteRotation finite_rotation; // controls the way a body's orientation is updated at each time step

  //////////////////////////////
  //    Internal-ish stuff
#ifndef __MAKETA__
  dMass         mass_ode;       // #IGNORE full ode mass of body
#endif
  void*         fixed_joint_id; // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of joint used to fix a FIXED body

  int          GetEnabled() const override {  return !HasBodyFlag(OFF); }
  void         SetEnabled(bool value) override { SetBodyFlagState(OFF, !value); }
  String       GetDesc() const override { return desc; }
  inline void           SetBodyFlag(BodyFlags flg)   { flags = (BodyFlags)(flags | flg); }
  // set body flag state on
  inline void           ClearBodyFlag(BodyFlags flg) { flags = (BodyFlags)(flags & ~flg); }
  // clear body flag state (set off)
  inline bool           HasBodyFlag(BodyFlags flg) const { return (flags & flg); }
  // check if body flag is set
  inline void           SetBodyFlagState(BodyFlags flg, bool on)
  { if(on) SetBodyFlag(flg); else ClearBodyFlag(flg); }
  // set body flag state according to on bool (if true, set flag, if false, clear it)


  virtual VEWorld* GetWorld();  // #CAT_ODE get world object (parent of this guy)
  virtual void* GetWorldID();   // #CAT_ODE get world id value
  virtual VEObject* GetObject();// #CAT_ODE get parent object (parent of this guy)
  virtual void* GetObjSpaceID(); // #CAT_ODE get object space id value

  virtual bool  CreateODE();    // #CAT_ODE create body in ode (if not already created) -- returns false if unable to create
  virtual void  DestroyODE();   // #CAT_ODE destroy body in ode (if created)

  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void  Init_Shape();   // #CAT_ODE #EXPERT set shape information
  virtual void  Init_Pos();     // #CAT_ODE #EXPERT set initial position
  virtual void  Init_Rotation();        // #CAT_ODE #EXPERT set initial rotation
  virtual void  Init_Velocity();        // #CAT_ODE #EXPERT set initial velocity (linear and angular)
  virtual void  Init_Mass();    // #CAT_ODE #EXPERT set the mass of body in ODE
  virtual void  Init_FiniteRotation(); // #CAT_ODE #EXPERT set the finite rotation mode of body in ODE
  virtual void  Init_Gravity(); // #CAT_ODE #EXPERT set the gravity mode
  virtual void  Init_Damping(); // #CAT_ODE #EXPERT set the damping parameters

  virtual void Step_pre();
  // #EXPERT #CAT_ODE do computations prior to taking a step of the ode physics solver -- called automatically at the start of Step() to allow special objects (e.g., VEArm) to update prior to stepping
  virtual void  CurToODE();
  // #CAT_ODE #BUTTON #GHOST_ON_body_id:NULL set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call

  virtual void  CurToODE_Pos();      // #CAT_ODE #EXPERT set current position
  virtual void  CurToODE_Rotation();    // #CAT_ODE #EXPERT set current rotation
  virtual void  CurToODE_Velocity();    // #CAT_ODE #EXPERT set current velocity (linear and angular)

  virtual void  CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

  virtual void  GetInitFromRel();
  // #CAT_ODE #EXPERT if init_rel is on, this will compute init values from relative values -- called automatically during Init() and UAE
  virtual void  UpdateCurFromRel();
  // #CAT_ODE #EXPERT if init_rel is on, this will compute current position and rotation from relative offsets compared to the rel_body current values -- see also VEObject::UpdateCurToRels
  virtual void  SaveCurAsPrv();
  // #IGNORE save current vals as prv_* -- needed for UpdateCurToRels
  virtual void  UpdateCurRotFmQuat();
  // #CAT_ODE #EXPERT update current rotation parameters from cur_quat read from ODE or whenever cur_quat might be set externally (e.g., gui dragging)
  virtual void  InitRotFromCur();
  // #CAT_ODE #EXPERT set init rotation parameters from current rotation
  virtual void  CurRotFromInit();
  // #EXPERT set current rotation from initial

  virtual void  Translate(float dx, float dy, float dz, bool init, bool abs_pos = false);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos -- if abs_pos then set directly to coordinates instead of adding them to current values
  virtual void  MoveDirDistAxis(float dist, float x_ax=0.0, float y_ax=1.0, float z_ax=0.0, float rot=0.0);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once) along given direction (specified by axis and angle) RELATIVE TO CURRENT rotation, compared to 0 rotation direction which is along positive X axis -- if you want to go straight forward, just leave direction values at initial defaults (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) 
  virtual void  MoveDirDistEuler(float dist, float euler_x=0.0, float euler_y=0.0,
                                 float euler_z=0.0);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once) along given direction (specified by Euler angles) RELATIVE TO CURRENT rotation, compared to 0 rotation direction which is along positive X axis -- if you want to go straight forward, just leave direction values at initial defaults (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) 
  virtual void  Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void  RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init, bool abs_rot = false);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though -- if abs_rot then set directly to coordinates instead of adding them to current values (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  virtual void  RotateEuler(float euler_x, float euler_y, float euler_z, bool init, bool abs_rot = false);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation in Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- if abs_rot then set directly to coordinates instead of adding them to current values (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  virtual void  RotateEulerZXZ(float alpha, float beta, float gamma, bool init, bool abs_rot = false);
    // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation in Euler ZXZ angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- if abs_rot then set directly to coordinates instead of adding them to current values (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  
  virtual void  RotateEulerYXY(float alpha, float beta, float gamma, bool init, bool abs_rot = false);
    // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation in Euler ZXZ angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- if abs_rot then set directly to coordinates instead of adding them to current values (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)

  virtual void  AddForce(float fx, float fy, float fz, bool torque=false, bool rel=false);
  // #BUTTON #CAT_ODE add given force vector to object at its center of mass -- if torque then it is a torque (angular force), otherwise linear -- if rel then force is relative to the objects own frame of reference (orientation) -- otherwise it is in the global reference frame
  virtual void  AddForceAtPos(float fx, float fy, float fz, float px, float py, float pz,
                              bool rel_force=false, bool rel_pos=false);
  // #BUTTON #CAT_ODE add given force vector to object at given position on object -- rel_force and rel_pos specify values relative to the reference frame (orientation, position) of the body, in contrast to global reference frame coordinates
  virtual void  CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init
  virtual void  SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of body to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void  CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  //////////////////////////////
  //    Set Damping

  virtual void SetLinearDamping(float ldamp);
  // #CAT_ODE Set the body's linear damping scale. After setting a damping scale, the body will ignore the world's damping scale until dBodySetDampingDefaults() is called.
  virtual void SetAngularDamping(float adamp);
  // #CAT_ODE Set the body's angular damping scale. After setting a damping scale, the body will ignore the world's damping scale until dBodySetDampingDefaults() is called.
  virtual void SetLinearDampingThreshold(float lthresh);
  // #CAT_ODE Set the body's linear damping threshold. The damping will be applied only if the linear speed is above the threshold limit
  virtual void SetAngularDampingThreshold(float athresh);
  // #CAT_ODE Set the body's angular damping threshold. The damping will be applied only if the angular speed is above the threshold limit
  virtual void SetMaxAngularSpeed(float maxaspeed);
  // #CAT_ODE You can also limit the maximum angular speed. In contrast to the damping functions, the angular velocity is affected before the body is moved. This means that it will introduce errors in joints that are forcing the body to rotate too fast. Some bodies have naturally high angular velocities (like cars' wheels), so you may want to give them a very high (like the default, dInfinity) limit.

  //////////////////////////////
  //    Finite rotation mode - important for wheels

  virtual void SetFiniteRotationMode(int rotmode);
  // #CAT_ODE This function controls the way a body's orientation is updated at each time step. The mode argument can be 0 or 1.0: An "infinitesimal" orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. This is the default for every new body that is created. 1: A finite orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
  virtual void SetFiniteRotationAxis(float xr, float yr, float zr);
  //#CAT_ODE This sets the finite rotation axis for a body. This is axis only has meaning when the finite rotation mode is set (see dBodySetFiniteRotationMode). If this axis is zero (0,0,0), full finite rotations are performed on the body. If this axis is nonzero, the body is rotated by performing a partial finite rotation along the axis direction followed by an infinitesimal rotation along an orthogonal direction. This can be useful to alleviate certain sources of error caused by quickly spinning bodies. For example, if a car wheel is rotating at high speed you can call this function with the wheel's hinge axis as the argument to try and improve its behavior.

  //////////////////////////////
  //    Gravity mode

  virtual void SetGravityMode(int mode);
  // #CAT_ODE Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity.

  bool  IsCurShape()  { return ((shape == cur_shape) &&
                                (HasBodyFlag(FM_FILE) == HasBodyFlag(CUR_FM_FILE))); }
  // #CAT_ODE is the ODE guy actually configured for the current shape or not?

  SIMPLE_COPY(VEBody);
  SIMPLE_INITLINKS(VEBody);
  void CutLinks() override;
  TA_BASEFUNS(VEBody);
protected:
  Shape         cur_shape;      // #IGNORE current shape that was previously set -- for detecting updates
  LongAxis      cur_long_axis;  // #IGNORE long axis that was previously set -- for detecting updates
  taVector3f    prv_pos;      // #IGNORE previous cur_pos value -- set prior to a Translate function move, for use by UpdateCurFromRel
  taQuaternion  prv_quat;      // #IGNORE previous cur_quat rotation value -- set prior to a Rotate function rotation, for use by UpdateCurFromRel


  void         UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy();
};

#endif // VEBody_h

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

#ifndef taQuaternion_h
#define taQuaternion_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class taVector3f; // 
class taAxisAngle; // 
class float_Matrix; // 

// note: following is informed by http://willperone.net/Code/quaternion.php and ODE code

class TA_API taQuaternion: public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a quaternion representation of 3D rotation 
  INHERITED(taBase)
public:
  float         s; // scalar, real component
  float         x; // imaginary x component
  float         y; // imaginary y component
  float         z; // imaginary z component

  inline void   SetSXYZ(float ss, float xx, float yy, float zz)
  { s = ss; x = xx; y = yy; z = zz; }
  // #CAT_Quaternion set scalar and xyz imaginary values
  inline void   GetSXYZ(float& ss, float& xx, float& yy, float& zz)
  { ss = s; xx = x; yy = y; zz = z; }
  // #CAT_Quaternion get scalar and xyz imaginary values

  inline void	ToMatrix(taMatrix& mat) const
  { mat.SetGeom(1,4); mat.SetFmVar(s,0); mat.SetFmVar(x,1); mat.SetFmVar(y,2);
    mat.SetFmVar(z,3); }
  // set values to a 1x4 matrix object (can be any type of matrix object) -- in order: s,x,y,z
  inline void	FromMatrix(taMatrix& mat) 
  { s = mat.SafeElAsVar(0).toFloat(); x = mat.SafeElAsVar(1).toFloat();
    y = mat.SafeElAsVar(2).toFloat(); z = mat.SafeElAsVar(3).toFloat(); }
  // set values from a matrix object (can be any type of matrix object) -- in order: s,x,y,z

  inline void	FromEuler(float theta_x, float theta_y, float theta_z) {
    float cos_z_2 = cosf(0.5f*theta_z);
    float cos_y_2 = cosf(0.5f*theta_y);
    float cos_x_2 = cosf(0.5f*theta_x);

    float sin_z_2 = sinf(0.5f*theta_z);
    float sin_y_2 = sinf(0.5f*theta_y);
    float sin_x_2 = sinf(0.5f*theta_x);

    s = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
    x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
    y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
    z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
  }
  // #CAT_Quaternion #BUTTON set quaternion from three Euler angles
  inline void	FromEulerVec(const taVector3f& euler) {
    FromEuler(euler.x, euler.y, euler.z);
  }
  // #CAT_Quaternion #BUTTON set quaternion from three Euler angles in vector form

  inline void	ToEuler(float& theta_z, float& theta_y, float& theta_x, bool homogenous=true) const {
    float sqs = s*s;    
    float sqx = x*x;    
    float sqy = y*y;    
    float sqz = z*z;    
    if(homogenous) {
      theta_x = atan2f(2.0f * (x*y + z*s), sqx - sqy - sqz + sqs);    		
      theta_y = asinf(-2.0f * (x*z - y*s));
      theta_z = atan2f(2.0f * (y*z + x*s), -sqx - sqy + sqz + sqs);    
    }
    else {
      theta_x = atan2f(2.0f * (z*y + x*s), 1.0f - 2.0f*(sqx + sqy));
      theta_y = asinf(-2.0f * (x*z - y*s));
      theta_z = atan2f(2.0f * (x*y + z*s), 1.0f - 2.0f*(sqy + sqz));
    }
  }
  // #CAT_Quaternion return three Euler angles from quaternion
  inline void	ToEulerVec(taVector3f& euler, bool homogenous=true) const {
    ToEuler(euler.x, euler.y, euler.z);
  }
  // #CAT_Quaternion return three Euler angles from quaternion into a 3D vector

  inline void	FromAxisAngle(const taAxisAngle& axa) {
    float ang2 = axa.rot*0.5f; float sinang2 = sinf(ang2);
    s = cosf(ang2); x = axa.x * sinang2; y = axa.y * sinang2; z = axa.z * sinang2;
  }
  // #CAT_Quaternion set quaternion from taAxisAngle (axis + angle) value
  inline void	FromAxisAngle(float x_axis, float y_axis, float z_axis, float rot_ang) {
    float ang2 = rot_ang*0.5f; float sinang2 = sinf(ang2);
    s = cosf(ang2); x = x_axis * sinang2; y = y_axis * sinang2; z = z_axis * sinang2;
  }
  // #CAT_Quaternion set quaternion from taAxisAngle (axis + angle) value
  inline void	ToAxisAngle(taAxisAngle& axa) const {
    axa.rot = acosf(s);
    float sinangi = sinf(axa.rot);
    if(sinangi == 0.0f) {	// can't convert
      axa.rot = 0.0f;
      axa.x = 0.0f; axa.y = 0.0f; axa.z = 1.0f;
      return;
    }
    sinangi = 1.0f / sinangi;
    axa.x = x * sinangi; axa.y = y * sinangi; axa.z = z * sinangi;  axa.rot *= 2.0f;
  }
  // #CAT_Quaternion set taAxisAngle from this quaternion

  inline void	ToRotMatrix(float_Matrix& mat) const {
    float mag = Mag();
    if(mag < 0.9999 || mag > 1.0001) { taMisc::Error("taQuaternion::ToMatrix -- must be normalized (Mag == 1.0), mag is:", 
						     String(mag)); return; }
    mat.SetGeom(2,3,3);

    mat.Set(1.0f - 2.0f*(y*y+z*z), 0,0);
    mat.Set(2.0f*(x*y-s*z),1,0);
    mat.Set(2.0f*(x*z+s*y),2,0);

    mat.Set(2.0f*(x*y+s*z),0,1);
    mat.Set(1.0f-2.0f*(x*x+z*z), 1,1);
    mat.Set(2.0f*(y*z-s*x),2,1);
    
    mat.Set(2.0f*(x*z-s*y),0,2);
    mat.Set(2.0f*(y*z+s*x),1,2);
    mat.Set(1.0f-2.0f*(x*x+y*y),2,2);
  }
  // #CAT_Quaternion create a 3x3 rotation matrix from quaternion

  void	RotateAxis(float x_axis, float y_axis, float z_axis, float rot_ang) {
    taQuaternion q; q.FromAxisAngle(x_axis, y_axis, z_axis, rot_ang);
    q.Normalize();
    *this *= q;
  }
  // #CAT_Quaternion #BUTTON rotate this rotation by given axis rotation parameters
  void	RotateEuler(float theta_x, float theta_y, float theta_z) {
    taQuaternion q; q.FromEuler(theta_x, theta_y, theta_z);
    q.Normalize();
    *this *= q;
  }
  // #CAT_Quaternion #BUTTON rotate this rotation by given rotation parameters

  void	RotateXYZ(float& x, float& y, float& z) {
    taQuaternion vecq(0, x, y, z); // convert vec to quat
    taQuaternion conj = this->Conjugated();
    taQuaternion rotv = *this * vecq * conj;
    x = rotv.x; y = rotv.y; z = rotv.z;
  }
  // #CAT_Quaternion #BUTTON rotate x,y,z vector according to current rotation parameters
  void	RotateVec(taVector3f& vec) {
    RotateXYZ(vec.x, vec.y, vec.z);
  }
  // #CAT_Quaternion #BUTTON rotate vector according to current rotation parameters

  inline float SqMag() const {
    return s*s + x*x + y*y + z*z;
  }
  // #CAT_Quaternion squared magnitude (length) of quaternion -- sum of squared components
  inline float Mag() const {
    return sqrtf(SqMag());
  }
  // #CAT_Quaternion magnitude (length) of quaternion -- square root of sum of squared components
  inline void Conjugate() {
    x = -x; y = -y; z = -z;
  }
  // #CAT_Quaternion convert this quaternion into its conjugate (imaginary x,y,z values flipped to their negatives)
  inline taQuaternion Conjugated() const {
    taQuaternion rv(*this); rv.Conjugate(); return rv;
  }
  // #CAT_Quaternion return conjugate version of this quaternion (imaginary x,y,z values flipped to their negatives)
  inline void Invert() {
    Conjugate(); *this /= SqMag(); 
  }
  // #CAT_Quaternion invert this quaternion (conjugate and divide by squared magnitude)
  inline taQuaternion Inverted() const {
    taQuaternion rv(*this); rv.Invert(); return rv;
  }
  // #CAT_Quaternion convert this quaternion into its conjugate (imaginary x,y,z values flipped to their negatives)
  inline void Normalize() {
    *this /= Mag(); 
  }
  // #CAT_Quaternion normalize this quaternion (divide by magnitude or length)
  inline taQuaternion Normalized() const {
    taQuaternion rv(*this); rv.Normalize(); return rv;
  }
  // #CAT_Quaternion return normalized version of this quaternion (divide by magnitude or length)

  inline taQuaternion operator + (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s + q.s; rv.x = x + q.x; rv.y = y + q.y; rv.z = z + q.z; return rv;
  }
  inline taQuaternion operator - (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s - q.s; rv.x = x - q.x; rv.y = y - q.y; rv.z = z - q.z; return rv;
  }
  inline taQuaternion operator * (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s * q.s - (x * q.x + y * q.y + z * q.z);
    rv.x = y*q.z - z*q.y + s*q.x + x*q.s;
    rv.y = z*q.x - x*q.z + s*q.y + y*q.s;
    rv.z = x*q.y - y*q.x + s*q.z + z*q.s; return rv;
  }
  inline taQuaternion operator / (const taQuaternion& q) const {
    taQuaternion inv(q);
    inv.Invert(); // 1/q
    return *this * inv;
  }

  inline taQuaternion operator * (float scale) const {
    taQuaternion rv; rv.s = s * scale; rv.x = x * scale; rv.y = y * scale; rv.z = z * scale; return rv;
  }
  inline taQuaternion operator / (float scale) const {
    taQuaternion rv;
    if(scale != 0.0f) { rv.s = s / scale; rv.x = x / scale; rv.y = y / scale; rv.z = z / scale; }
    else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
    return rv;
  }

  inline taQuaternion operator - () const {
    taQuaternion rv; rv.s = -s; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }

  inline taQuaternion& operator += (const taQuaternion& q) {
    s += q.s; x += q.x; y += q.y; z += q.z; return *this;
  }
  inline taQuaternion& operator -= (const taQuaternion& q) {
    s -= q.s; x -= q.x; y -= q.y; z -= q.z; return *this;
  }
  inline taQuaternion& operator *= (const taQuaternion& q) {
    float ox = x; float oy = y; float oz = z;
    x = oy*q.z - oz*q.y + s*q.x + ox*q.s;
    y = oz*q.x - ox*q.z + s*q.y + oy*q.s;
    z = ox*q.y - oy*q.x + s*q.z + oz*q.s;
    s = s*q.s - (ox * q.x + oy * q.y + oz * q.z); return *this;
  }
  inline taQuaternion& operator /= (const taQuaternion& q) {
    taQuaternion inv(q);
    inv.Invert(); // 1/q
    return *this *= inv;
  }

  inline taQuaternion& operator *= (float scale) {
    s *= scale; x *= scale; y *= scale; z *= scale; return *this;
  }
  inline taQuaternion& operator /= (float scale) {
    if(scale != 0.0f) { s /= scale; x /= scale; y /= scale; z /= scale; }
    else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
    return *this;
  }

#ifndef __MAKETA__
  void		ToODE(dQuaternion dq) const
  { dq[0] = s; dq[1] = x; dq[2] = y; dq[3] = z; }
  void		FromODE(const dQuaternion dq)
  { s = dq[0]; x = dq[1]; y = dq[2]; z = dq[3]; }
#endif

  String        GetStr() const { return String(s) + ", " + String(x) + ", " + String(y) + ", " + String(z);  }

  taQuaternion(float ss, float xx, float yy, float zz)
  { SetSXYZ(ss, xx, yy, zz); }
  taQuaternion(float theta_x, float theta_y, float theta_z)
  { FromEuler(theta_x, theta_y, theta_z); }
  taQuaternion(const taVector3f& euler)
  { FromEulerVec(euler); }

  taQuaternion& operator=(const taAxisAngle& cp)
  { FromAxisAngle(cp); return *this; }
  taQuaternion& operator=(const taVector3f& cp)
  { FromEulerVec(cp); return *this; }

  TA_BASEFUNS_LITE(taQuaternion);
private:
  void                  Copy_(const taQuaternion& cp)  { s = cp.s; x = cp.x; y = cp.y; z = cp.z; }
  void                  Initialize() { s = 1.0f; x = y = z = 0.0f; }
  void                  Destroy() {}
};

#endif // taQuaternion_h
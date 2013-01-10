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

#ifndef VELinearMuscle_h
#define VELinearMuscle_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taVector3f>

// declare all other types mentioned but not required to include:


class TA_API VELinearMuscle : public taNBase {
  // A muscle that exerts force proportional to its input
  INHERITED(taNBase)
public:

  taVector3f 	IPprox;    // proximal/medial insertion point
  taVector3f 	IPdist;    // distal/lateral insertion point
  taVector3f 	p3;        // point of intersection with the bending line
  float 	gain;
  bool 		bend;      // true if the muscle is currently bending
  float 	old_length1;
  float 	old_length2; // past lengths, used to obtain contraction speed with the 3 point method. UpdateIPs keeps them actualized. old_length2 is the length 2 timesteps ago.

  taVector3f 	Contract(float stim);
  // Returns the force vector (pointing towards the proximal insertion point) resulting from a given stimulation of the muscle;

  float Length();       // Returns current length of muscle
  float Speed();  	// Returns muscle's contraction speed one world stepsize ago

  TA_SIMPLE_BASEFUNS(VELinearMuscle);
private:

  void Initialize();
  void Init(taVector3f prox, taVector3f dist, float MrG);
  void Init(taVector3f prox, taVector3f dist, float MrG, taVector3f pp3, bool bending);
  void Destroy();
  virtual VEArm* GetArm(); // Get pointer to VEArm containing muscle
};

#endif // VELinearMuscle_h
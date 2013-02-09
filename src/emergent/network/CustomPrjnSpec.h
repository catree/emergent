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

#ifndef CustomPrjnSpec_h
#define CustomPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(CustomPrjnSpec);

class E_API CustomPrjnSpec : public ProjectionSpec {
  // connectivity is defined manually (i.e. unit-by-unit)
INHERITED(ProjectionSpec)
public:

  override void	Connect(Projection* prjn);	// do nothing

  TA_BASEFUNS_NOCOPY(CustomPrjnSpec);
private:
  void	Initialize()	{ };
  void	Destroy()	{ };
};

#endif // CustomPrjnSpec_h
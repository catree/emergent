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

#ifndef ResetDataRows_h
#define ResetDataRows_h 1

// parent includes:
#include <DataOneProg>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ResetDataRows);

class TA_API ResetDataRows: public DataOneProg { 
  // clear out all existing rows of data in data table
INHERITED(DataOneProg)
public:
  String	GetDisplayName() const CPP11_OVERRIDE;
  String	GetToolbarName() const CPP11_OVERRIDE { return "reset rows"; }

  TA_BASEFUNS_NOCOPY(ResetDataRows);
protected:
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

#endif // ResetDataRows_h

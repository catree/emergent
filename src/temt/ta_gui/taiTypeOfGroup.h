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

#ifndef taiTypeOfGroup_h
#define taiTypeOfGroup_h 1

// parent includes:
#include <taiTypeOfList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiTypeOfGroup);

class TA_API taiTypeOfGroup : public taiTypeOfList {
  TAI_TYPEBASE_SUBCLASS(taiTypeOfGroup, taiTypeOfList);
public:
  int           BidForType(TypeDef* td) override;
protected:
  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr) override;
};

#endif // taiTypeOfGroup_h

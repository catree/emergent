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

#ifndef T3Panel_List_h
#define T3Panel_List_h 1

// parent includes:
#include <taViewer_List>

// member includes:
#include <T3Panel>


// declare all other types mentioned but not required to include:

taTypeDef_Of(T3Panel_List);

class TA_API T3Panel_List: public taViewer_List { // #NO_TOKENS
INHERITED(taViewer_List)
public:
  TA_DATAVIEWLISTFUNS(T3Panel_List, taViewer_List, T3Panel)
private:
  NOCOPY(T3Panel_List)
  void  Initialize() { SetBaseType(&TA_T3Panel);}
  void  Destroy() {}
};


#endif // T3Panel_List_h

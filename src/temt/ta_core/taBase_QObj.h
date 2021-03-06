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

#ifndef taBase_QObj_h
#define taBase_QObj_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:
#include <taOABase>

// declare all other types mentioned but not required to include:

/*
 * taBase_QObj enables a taOBase object to handle Qt events, via a
 * proxy(taBase_QObj)/stub(taBase) approach. Note that dual-parenting a taBase object
 * with QObject is not workable, because QObject must come first, and then all the (taBase*)(void*)
 * casts in the system break...

 * To use, subclass taBase_QObj when events need to be handled. Create the instance in
 * the Initialize() call and set with SetAdapter. The adapter object does not participate
 * in copying/cloning/etc. (it has no state information).

 * Since classes can have subclasses, there may be successive calls to Initialize with subclasses
 * of an adapter. The adapters will be chained, with the new adapter becoming the QObject parent of
 * the previous adapter. Therefore, when the current adapter is destroyed, child adapters will also
 * get destroyed. It will not matter whether a base class hooks to its own adapter, or to a subclass
 * or to a combination.
*/

class TA_API taBase_QObj: public QObject {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class taOABase;
public:
  taBase_QObj(taOABase* owner_): QObject(NULL) {owner = owner_;}
  ~taBase_QObj();
protected:
  taOABase* owner; // #IGNORE
};

#endif // taBase_QObj_h

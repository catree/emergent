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

#ifndef taiEditorOfSelectEditBase_h
#define taiEditorOfSelectEditBase_h 1

// parent includes:
#include <taiEditorOfClass>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfSelectEditBase : public taiEditorOfClass {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorOfClass)
  Q_OBJECT
public:
  SelectEdit*   sele;

  override void         Constr_Body();
  override taBase*      GetMembBase_Flat(int idx); // these are overridden by seledit
  override taBase*      GetMethBase_Flat(int idx);

  taiEditorOfSelectEditBase(void* base, TypeDef* td, bool read_only_ = false,
        QObject* parent = 0);
  taiEditorOfSelectEditBase()     { Initialize();};
  ~taiEditorOfSelectEditBase();

protected:
  override void         Constr_Methods_impl();

protected slots:
  virtual void          DoRemoveSelEdit() = 0;
  virtual void          mnuRemoveMember_select(int idx); // #IGNORE removes the indicated member
  virtual void          mnuRemoveMethod_select(int idx); // #IGNORE removes the indicated method
private:
  void  Initialize();
};

#endif // taiEditorOfSelectEditBase_h
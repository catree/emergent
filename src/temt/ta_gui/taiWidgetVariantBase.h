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

#ifndef taiWidgetVariantBase_h
#define taiWidgetVariantBase_h 1

// parent includes:
#include <taiWidgetComposite>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetComboBox; //
class taiWidgetToggle; //
class taiWidgetFieldIncr; //
class taiWidgetField; //
class taiWidgetTokenChooser; //
class QStackedWidget; //



class TA_API taiWidgetVariantBase: public taiWidgetComposite {
  // common code/members for complex types that use a variant
  Q_OBJECT
  INHERITED(taiWidgetComposite)
public:
  enum CustomFlags { // #BITS
    flgNoInvalid        = 0x010000, // don't let user choose Invalid
    flgNoAtomics        = 0x020000, // don't let user choose any atomics
    flgIntOnly          = 0x040000, // Int only (used for Enum)
    flgNoPtr            = 0x080000, // don't let user choose raw ptr
    flgNoBase           = 0x100000, // don't let user choose taBase or taMatrix
    flgNoTypeItem       = 0x200000, // don't let user choose TypeItem
    flgFixedType        = 0x400000, // type is fixed to what it already is -- cannot be chosen
  };

  bool                  fillHor() override {return true;} 
  taiWidgetVariantBase(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags = 0);
  ~taiWidgetVariantBase();

protected:
  enum StackControls { // #IGNORE indexes of controls in the stack
    scInvalid,
    scBool,
    scInt,
    scField, // includes string and char
    scPtr,
    scBase,
    scTypeItem // for TypeItem token
  };

  mutable int           m_updating;

  taiWidgetComboBox*          cmbVarType;
  QStackedWidget*       stack; // holds the subfields for different types

  taiWidgetToggle*            togVal; // for: bool
  taiWidgetFieldIncr*         incVal; // for: ints
  taiWidgetField*             fldVal; // for: char, string, most numbers
  taiWidgetTokenChooser*    tabVal; // for taBase & Matrix token
  iLabel*               tiVal; // for TypeItem token -- display-only for now

  void                  Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void          Constr_impl(QWidget* gui_parent_, bool read_only_);
    // (possibly) extend, and called in your constructor
  void                  GetImage_Variant(const Variant& var);
  void                  GetValue_Variant(Variant& var) const;


protected slots:
  void                  cmbVarType_itemChanged(int itm);

};

#endif // taiWidgetVariantBase_h

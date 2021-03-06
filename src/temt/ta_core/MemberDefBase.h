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

#ifndef MemberDefBase_h
#define MemberDefBase_h 1

// parent includes:
#include <TypeItem>

// member includes:

// declare all other types mentioned but not required to include:
class MemberDefBase_List; //
class taiMember; //

taTypeDef_Of(MemberDefBase);

class TA_API MemberDefBase : public TypeItem { // #VIRT_BASE #NO_INSTANCE common subclass of MemberDef and PropertyDef
INHERITED(TypeItem)
public:
  MemberDefBase_List*   owner;
  TypeDef*              type;     // of this item
  String_PArray         inh_opts; // inherited options ##xxx
  bool                  is_static; // true if this member is static
#ifdef TA_GUI
  taiMember*            im;             // gui structure for edit representation -- if this is a memberdef that is the storage for a property, then the im is assigned to the property
#endif

  virtual bool          isReadOnly() const = 0; // absolutely read-only
  virtual bool          isGuiReadOnly() const = 0; // read-only in the gui

  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_MemberDefBase;}
  virtual bool          ValIsDefault(const void* base,
                                     int for_show) const = 0; // = taMisc::IS_EXPERT);
    // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void          Copy(const MemberDefBase& cp);
  void          Copy(const MemberDefBase* cp); // this is a "pseudo-virtual" type guy, that will copy a like source (Member or Property)
  TypeDef*     GetOwnerType() const override;
  MemberDefBase();
  MemberDefBase(const String& nm);
  MemberDefBase(TypeDef* ty, const String& nm, const String& dsc, const String& op,
    const String& lis, bool is_stat = false);
  MemberDefBase(const MemberDefBase& cp);
  ~MemberDefBase();

  virtual const Variant GetValVar(const void* base) const = 0;
  virtual void  SetValVar(const Variant& val, void* base,
    void* par = NULL) = 0;

  bool          CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  bool          ShowMember(int show_forbidden = USE_SHOW_GUI_DEF, 
                           TypeItem::ShowContext show_context = SC_ANY, 
                           int show_allowed = SHOW_CHECK_MASK) const; 
  // decide whether to output or not based on options (READ_ONLY, HIDDEN, etc)

protected:
  // note: bits in the show* vars are set to indicate the value, ie READ_ONLY has that bit set
  mutable int   show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  mutable int   show_edit;
  mutable int   show_tree;

  void          ShowMember_CalcCache() const; // called when show_any=0, ie, not configured yet
  void          ShowMember_CalcCache_impl(int& show, const String& suff) const;
private:
  void          Initialize();
  void          Copy_(const MemberDefBase& cp);
};

#endif // MemberDefBase_h

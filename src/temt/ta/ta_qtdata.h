// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_qtdata.h: Qt-based graphical data representations

#ifndef TA_QTDATA_H
#define TA_QTDATA_H

#include "ta_variant.h"
#include "ta_base.h"
#include "ta_qtdata_def.h"

#ifndef __MAKETA__
# include <QAction>
# include <QList>
# include <QMenu>
# include <QObject>
# include <QWidget>
# include "ibutton.h" 
# include "icheckbox.h"
# include "ilineedit.h"
# include "idialog.h"
#endif

#include "igeometry.h"

//nn? #include <unistd.h>

// externals
class QKeySequence; // #IGNORE
class MatrixGeom;

// forwards
class taiField;
class taiToggle;
class taiActions;
class taiMenu;
class taiAction;
class taiItemChooser; 
class taiItemPtrBase; 
class taiTokenPtrButton; //


class TA_API taiCompData : public taiData {
  //  #NO_INSTANCE base class for composite data elements
INHERITED(taiData)
  Q_OBJECT
public:
  enum LayoutType { // the type of layout being used
    LT_HBox,
    LT_Flow,
    LT_Stacked
  };
  
  taiCompData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags = 0);
  override ~taiCompData();

  taiDataList		data_el;

  QLayout*		GetLayout() {return (QLayout*)lay;} // override
  inline LayoutType	layType() const {return lay_type;}
  QWidget*		widgets(int index);
  int			widgetCount();

  override taBase*	ChildBase() const 
   {if (m_child_base) return m_child_base; return inherited::ChildBase();}
   // child base, typically obtained from parent or host, except ex. PolyData
  virtual void		InitLayout(); // default creates a QHBoxLayout in the Rep
  void			AddChildWidget(QWidget* child_widget, int space_after = -1,
    int stretch = 0);
    // s_a=-1 for default taiM->hspc_c
  virtual void 		AddChildMember(MemberDef* md); // adds label and control for the member
  virtual void		EndLayout(); // default adds a stretch

#ifndef __MAKETA__
signals:
  void			ChildDataChangedNotify(taiData* sender);
  // emitted whenever a child does DataChangedNotify
#endif
protected:
  QLayout*		lay;	// may be ignored/unused by subclasses
  int			last_spc;	// space after last widget, -1 = none
  LayoutType		lay_type;
  mutable taBase*	m_child_base; // typically set in PolyData GetImage
  inline QHBoxLayout*	layHBox() const {return (QHBoxLayout*)lay;} // only if !hasFlow
  inline iFlowLayout*	layFlow() const {return (iFlowLayout*)lay;} // only if hasFlow
  inline QStackedLayout* layStacked() const {return (QStackedLayout*)lay;} // only if hasFlow
  override void		ChildAdd(taiData* child);
  override void		ChildRemove(taiData* child);
  virtual void		AddChildWidget_impl(QWidget* child_widget, int spacing,
    int stretch);// default does an add to layout

protected slots:
  void 			ChildDataChanged(taiData* sender);
  // connected to DataChanged of all our children

private:
  QObjectList*		mwidgets; // list of child widgets
};

class iTextEdit; // #IGNORE

class TA_API iFieldEditDialog : public iDialog {
  Q_OBJECT
INHERITED(iDialog)
public:
  iTextEdit*	txtText;
  QPushButton*	btnOk; // read/write only
  QPushButton* 	btnCancel; // or close, if read only
  QPushButton*	btnApply; // writes it back to field
  QPushButton* 	btnRevert; // gets back from field
  
  bool		isReadOnly() {return m_read_only;}
  virtual void	setText(const QString& value);
  
  iFieldEditDialog(bool modal_, bool read_only, const String& desc, taiField* parent);
  ~iFieldEditDialog();
  
public slots:
  override void accept();
  override void reject();

protected:
  taiField*	field;
  bool		m_read_only;
  
  void		setApplyEnabled(bool val); // set apply/revert enabled or not
  
protected slots:
  void		btnApply_clicked();
  void		btnRevert_clicked();
  void		repChanged();
  
private:
  void 		init(bool readOnly, const String& desc);
};


class TA_API taiField : public taiData {
  Q_OBJECT
friend class iFieldEditDialog;
public:
  iLineEdit*		rep() const { return leText; }
  bool			fillHor() {return true;} // override 
  void 			setMinCharWidth(int num); // hint for min chars, 0=no min

  MemberDef*		lookupfun_md;	// for lookup function, member def 
  void*			lookupfun_base; // for lookup function, base of owner
  
  taiField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiField();
  
  void 	GetImage(const String& val);
  String GetValue() const;

protected slots:
  void			selectionChanged();
  void			btnEdit_clicked(bool);
  void			lookupKeyPressed();
  void			lookupKeyPressed_dialog();

protected:
#ifndef __MAKETA__
  QPointer<iLineEdit>	leText;
  QPointer<QToolButton>	btnEdit; // if requested, button to invoke dialog editor
#endif
  iFieldEditDialog*	edit; // an edit dialog, if created
  override void		GetImage_impl(const void* base) {GetImage(*((String*)base));}
  override void		GetValue_impl(void* base) const {*((String*)base) = GetValue();} 
  override void		GetImageVar_impl(const Variant& val) {GetImage(val.toString());}
  override void		GetValueVar_impl(Variant& val) const {val.updateFromString(GetValue());} 
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};

class TA_API taiFileDialogField : public taiData {
  // for FILE_DIALOG_xxx strings
  Q_OBJECT
public:
  enum FileActionType {		
    FA_LOAD,
    FA_SAVE,
    FA_APPEND,
  };

  FileActionType	file_act;	// file action to take for file dialog
  String		file_ext;	// file extention list (from EXT_xxx)
  String		file_type;	// file type information (from FILETYPE_xxx)
  int			file_cmprs;	// compress?  from #COMPRESS -1 = default, 0 = no, 1 = yes
  taBase*		base_obj;	// taBase object for saving/loading 
  
  iLineEdit*		rep() const { return leText; }
  bool			fillHor() {return true;} // override 
  void 			setMinCharWidth(int num); // hint for min chars, 0=no min

  taiFileDialogField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_,
		     int flags = 0, FileActionType fact=FA_LOAD, const String& fext = "",
		     const String& ftyp = "", int fcmprs = -1);
  ~taiFileDialogField();

  void 	GetImage(const String& val);
  String GetValue() const;
  
protected slots:
  void			selectionChanged();
  void			filebtn_clicked(bool);
  void			lookupKeyPressed(); // remap to file dialog

protected:
#ifndef __MAKETA__
  QPointer<iLineEdit>	leText;
  QPointer<QToolButton>	btnEdit; // if requested, button to invoke file dialog
#endif
  override void		GetImage_impl(const void* base) {GetImage(*((String*)base));}
  override void		GetValue_impl(void* base) const {*((String*)base) = GetValue();} 
  override void		GetImageVar_impl(const Variant& val) {GetImage(val.toString());}
  override void		GetValueVar_impl(Variant& val) const {val.updateFromString(GetValue());} 
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};

// this is for integers -- includes up and down arrow buttons
class TA_API taiIncrField : public taiData { // increment/decrement field
  Q_OBJECT
public:
  inline iSpinBox*	rep() const { return (iSpinBox*)(QWidget*)m_rep; }
  taiIncrField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: flgPosOnly

  virtual void		setMinimum(const Variant& min); //NOTE: for iSpinBox, this must be an int
  virtual void		setMaximum(const Variant& max); //NOTE: for iSpinBox, this must be an int

  void 			GetImage(int val);
  int 			GetValue() const;

protected slots:
  void			selectionChanged();

protected:
  override void		GetImageVar_impl(const Variant& val) {GetImage(val.toInt());}
  override void		GetValueVar_impl(Variant& val) const {val = GetValue();} 
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};


class TA_API taiToggle : public taiData {
public:
  inline iCheckBox*	rep() const { return (iCheckBox*)(QWidget*)m_rep; }

  taiToggle(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
//  ~taiToggle() {}

  void 	GetImage(bool val);
  bool	GetValue() const;
protected:
  override void		GetImageVar_impl(const Variant& val) {GetImage(val.toBool());}
  override void		GetValueVar_impl(Variant& val) const {val = GetValue();} 
};


class TA_API taiPlusToggle : public taiCompData {
// a regular field plus a toggle..
  Q_OBJECT
INHERITED(taiCompData)
public:
  taiData*		data; // set by caller after creating contained class
  iCheckBox* 		but_rep;
  inline QWidget* 	rep() {return (QWidget*)m_rep;} //parent of contained controls

  taiPlusToggle(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiPlusToggle();

  override void	InitLayout();

  virtual int	GetValue();
  virtual void 	GetImage(bool chk);
public slots:
  virtual void	Toggle_Callback();

protected:
  override void	DataChanged_impl(taiData* chld);
  override void	applyNow();
};

//////////////////////////
//     taiComboBox	//
//////////////////////////

class TA_API taiComboBox : public taiData {
  Q_OBJECT
public:
  inline iComboBox*	rep() const { return (iComboBox*)(QWidget*)m_rep; }

  taiComboBox(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiComboBox(bool is_enum, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  void		AddItem(const String& val, const QVariant& userData = QVariant()); // add an item to the list
  void		Clear(); //clears all items (only needed if refilling list)
  void 		RemoveItemByData(const QVariant& userData); // remove item with indicated data
  void 		RemoveItemByText(const String& val); // remove item with indicated text

  void 		GetImage(int itm);  // set to this item number
  void		GetValue(int& itm) const;

  void		SetEnumType(TypeDef* enum_typ, bool force = false); // sets a new enum type
  void 		GetEnumImage(int enum_val);  // set to item number corresponding to this enum value, enum mode only
  void		GetEnumValue(int& enum_val) const; // set to enum value corresponding to item number, enum mode only
  
#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-IDataHost users, forwards chkbox signal
#endif

protected:
  override void		GetImageVar_impl(const Variant& val) 
    {if (m_is_enum) GetEnumImage(val.toInt()); else GetImage(val.toInt());}
  override void		GetValueVar_impl(Variant& val) const 
    {int i; if (m_is_enum) GetEnumValue(i); else GetValue(i); val = i;} 

private:
  bool		m_is_enum;
  void		Initialize(QWidget* gui_parent_, bool is_enum = false);
};

//////////////////////////
//     taiBitBox	//
//////////////////////////

class TA_API iBitCheckBox: public iCheckBox { // #IGNORE specialized checkbox for the taiBitBox class
INHERITED(iCheckBox)
  Q_OBJECT
public:
  int		val;
  bool		auto_apply;
  iBitCheckBox(bool auto_apply, int val_, String label, QWidget* parent);

public slots:
  void		this_clicked(bool on);

#ifndef __MAKETA__
signals:
  void		clickedEx(iBitCheckBox* sender, bool on);
#endif
};



class TA_API taiBitBox : public taiData { // supports enums that are bit fields 
  Q_OBJECT
public:
  int			no_show; // any bits that should be temporarily hidden
  int			no_edit; // any bits that should be set readonly
  
  inline QWidget*	rep() const { return (QWidget*)(QWidget*)m_rep; }
  bool			fillHor() {return true;} // override 

  taiBitBox(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiBitBox(bool is_enum, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  void		AddBoolItem(bool auto_apply, String name, int val, const String& desc = _nilString); // add an item to the list

  void 		GetImage(int val);  // set to this value, according to bit fields
  void		GetValue(int& val) const;

  void*			m_par_obj_base;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base

public slots:
  void		bitCheck_clicked(iBitCheckBox* sender, bool on); // #IGNORE

#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-IDataHost users, forwards chkbox signal
#endif

protected:
  QHBoxLayout* 	lay; //#IGNORE
  int		m_val; //#IGNORE
  override void		GetImageVar_impl(const Variant& val)  {GetImage(val.toInt());}
  override void		GetValueVar_impl(Variant& val) const 
    {int i; GetValue(i); val = i;} 
private:
  void		Initialize(QWidget* gui_parent_);
};

class TA_API taiDimEdit : public taiData { // specify number of dims and each dim value, ex. for Matrix dims
INHERITED(taiData)
  Q_OBJECT
public:
  inline iDimEdit*		rep() const {return (iDimEdit*)(QWidget*)m_rep;}
  bool			fillHor() {return true;} // override 
  taiDimEdit(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: 

  void 			GetImage(const MatrixGeom* arr);
  void 			GetValue(MatrixGeom* arr) const;

protected:
  override void		GetImage_impl(const void* base) {GetImage((const MatrixGeom*)base);}
  override void		GetValue_impl(void* base) const {GetValue((MatrixGeom*)base);} 
private:
  void 			Initialize(QWidget* gui_parent_);

};


//////////////////////////
//     taiPolyData	//
//////////////////////////

// this class supports the use of hierarchical sub-data within a data item
// its default behavior is to put everything in an hbox with labels

class TA_API taiPolyData : public taiCompData {
INHERITED(taiCompData)
  Q_OBJECT
public:
  static taiPolyData*	New(bool add_members, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags = 0); // set add_members false to manually add members, otherwise all eligible typ members added
  static bool 		ShowMemberStat(MemberDef* md, int show); // primarily for ProgEdit so we can use the exact same algorithm
  
  Member_List		memb_el;	// member elements (1:1 with data_el)
  int			show;
  
  inline QWidget*	rep() const { return (QWidget*)m_rep; } //note: actual class may be subclass of QFrame
  bool			fillHor() {return true;} // override 

  override void 	AddChildMember(MemberDef* md);
  
  ~taiPolyData();

protected:
  void			Constr(QWidget* gui_parent_);
  void			AddTypeMembers(); // called to add all typ members
  override void		ChildRemove(taiData* child); // remove from memb_el too
  override void		GetImage_impl(const void* base);
  override void		GetValue_impl(void* base) const; 
  virtual bool		ShowMember(MemberDef* md) const;
  taiPolyData(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


//////////////////////////
//     taiColor		//
//////////////////////////

class TA_API taiColor : public taiData {
INHERITED(taiData)
public:
  iColorButton*		rep() const {return (iColorButton*)(QWidget*)m_rep;}
  
  void 			GetImage(const iColor& val);
  iColor		GetValue() const;
  
  taiColor(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_,
    int flags = 0);

};


//////////////////////////
//     taiDataDeck	//
//////////////////////////

// contains sub-data's within a deck (aka Qt Stack) -- can toggle between them

class TA_API taiDataDeck : public taiCompData {
  Q_OBJECT
public:
  inline QWidget* rep() {return (QWidget*)m_rep;}

  taiDataDeck(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  virtual void  GetImage(int i);
};


class TA_API taiVariantBase: public taiCompData {
  // common code/members for complex types that use a variant
INHERITED(taiCompData)
  Q_OBJECT
public:
  enum CustomFlags { // #BITS
    flgNoInvalid	= 0x010000, // don't let user choose Invalid
    flgNoAtomics	= 0x020000, // don't let user choose any atomics
    flgIntOnly		= 0x040000, // Int only (used for Enum)
    flgNoPtr		= 0x080000, // don't let user choose raw ptr
    flgNoBase		= 0x100000 // don't let user choose taBase or taMatrix
  };
  
  bool			fillHor() {return true;} // override 
  taiVariantBase(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiVariantBase();

protected:
  enum StackControls { // #IGNORE indexes of controls in the stack
    scInvalid,
    scBool,
    scInt,
    scField, // includes string and char
    scPtr,
    scBase
  };
  
  mutable int		m_updating;
  
  taiComboBox*		cmbVarType;
  QStackedWidget*	stack; // holds the subfields for different types
  
  taiToggle*		togVal; // for: bool
  taiIncrField*		incVal; // for: ints
  taiField*		fldVal; // for: char, string, most numbers
  taiTokenPtrButton*	tabVal; // for taBase & Matrix token
  
  void			Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void		Constr_impl(QWidget* gui_parent_, bool read_only_); 
    // (possibly) extend, and called in your constructor
  void  		GetImage_Variant(const Variant& var);
  void	 		GetValue_Variant(Variant& var) const;

  
protected slots:
  void			cmbVarType_itemChanged(int itm);

};


class TA_API taiVariant: public taiVariantBase {
INHERITED(taiVariantBase)
  Q_OBJECT
public:
  
  inline QWidget*	rep() const { return (QWidget*)m_rep; } //note: actual class may be subclass of QFrame

  void  	GetImage(const Variant& var) {GetImage_Variant(var);}
  void	 	GetValue(Variant& var) const {GetValue_Variant(var);}
  
  taiVariant(IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiVariant();
protected:
  override void		GetImageVar_impl(const Variant& val) {GetImage(val);}
  override void		GetValueVar_impl(Variant& val) const {GetValue(val);} 
};


//////////////////////////////////
//   Menus and Toolbars		//
//////////////////////////////////


class TA_API taiAction: public QAction {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS holds menu and/or toolbar item data -- can be the root item of a submenu
  Q_OBJECT
friend class taiActions;
friend class taiAction_List;
public:
  enum CallbackType {
    none,		// callback parameters ignored
    action,		// corresponds to IV "action" type of callback -- slot for signal is parameterless
    men_act,		// corresponds to IV "men_act" type of callback -- slot for signal takes taiAction* as a sender
    int_act,		// corresponds to Qt menu/action signals -- slot for signal takes an int parameter
    ptr_act,		// slot for signal takes a void* parameter
    var_act		// slot for signal takes a "const Variant&" parameter
  };

  int			sel_type;
  String		label() {return String(text());} 
  Variant		usr_data; // default is Invalid, can also be int or void* 

  bool			canSelect(); // true if item can be taiMenu::curSel value
  virtual bool		isSubMenu() 	{ return false; }
  bool			isGrouped(); // indicates if item in a group, ie is a radio group action
#ifndef QT3_SUPPORT
  bool			addTo(QWidget* w) 
    {if (w) w->addAction(this); else return false; return true;}
#endif
  void			AddTo(taiActions* targ); // convenience function
  
  taiAction(int sel_type_, const String& label_); // used by taiMenu etc.
  taiAction(const QString& label_, const QKeySequence& accel, const char* name); // used by viewer/browser
  taiAction(const QString& label_, QObject* receiver, const char* member, const QKeySequence& accel); 
    // used by viewer/browser
  taiAction(const Variant& usr_data_, const QString& label_, const QKeySequence& accel, const char* name = NULL); // used by viewer/browser
  virtual ~taiAction();

  void connect(CallbackType ct_, const QObject *receiver, const char* member); // connect callback to given
  void connect(const taiMenuAction* mact); // convenience function

  virtual QMenu*	SubMenu() 	{ return NULL; }

#ifndef __MAKETA__
signals:
  void Action();			// for a direct call-back
  void MenuAction(taiAction* sender);	// for a menu-action call-back (returns item selected)
  void IntParamAction(int param);	// for Qt-style parameter ((int)usr_data)
  void PtrParamAction(void* ptr);	// for ptr parameter ((void*)usr_data)
  void VarParamAction(const Variant& var);	// for variant parameter ((void*)usr_data)
#endif // ndef __MAKETA__

protected:
  int			nref; // ref'ed when put onto a taiAction_List, deleted when ref=0
  int			m_changing; // for supressing recursive signals
  void			init(int sel_type_); 
  void 			emitActions();
  
protected slots:
  virtual void		this_triggered_toggled(bool checked); // annoying, but self connect, so we can re-raise
  
private:
  taiAction(const taiAction&); // no copying
  void operator=(const taiAction&); 
};

class TA_API taiSubMenuEl: public taiAction { // an action used exclusively to hold a submenu
  Q_OBJECT
friend class taiActions;
public:
  taiMenu* 		sub_menu_data; // the taiMenu for this submenu

  QMenu*		SubMenu() 	{ return menu(); } // override
  bool			isSubMenu() 	{ return true; } // override
  
protected: // only allowed to be used internally when creating submenus
  void		this_triggered_toggled(bool checked) {} // override submenu items don't signal anything
  
  taiSubMenuEl(const String& label_, taiMenu* sub_menu_data); //
  ~taiSubMenuEl();

};

class TA_API taiAction_List : public taPtrList<taiAction> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  int			count() {return size;} // for Qt-api compat
  
  taiAction*		PeekNonSep(); // returns last non-separator item, if any
  
  ~taiAction_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiAction*)it)->text()); }
  
protected:
  override void*	El_Ref_(void* it)  { ((taiAction*)it)->nref++; return it; }	// when pushed
  override void* 	El_unRef_(void* it)  { ((taiAction*)it)->nref--; return it; }	// when popped
  override void		El_Done_(void* it);
};


//////////////////////////////////
//   taiActions		//
//////////////////////////////////

class TA_API taiActions : public taiData {
  // #VIRT_BASE common subtype for menus and menubars
  Q_OBJECT
INHERITED(taiData)
friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  enum RepType { // for parameterized creation of a compatible subtype
    popupmenu, 	// for a taiMenu
    buttonmenu // for a taiButtonMenu
  };
  
  enum SelType {
    st_none =		0x00,	// no action on selection -- primarily for SubItem entries
    normal =		0x01,	// selecting the item causes an action; no persistent check mark by menu item
    normal_update =	0x81,
    radio = 		0x02,	// only 1 of n items on the menu can be selected; selecting one deselects any other
    radio_update =	0x82,
    toggle =		0x04,	// item can be checked or unchecked, independent of any other item
    toggle_update =	0x84,

    use_default =	0x10,	// only for taiActions::AddAction -- inherits from the taiActions

    update = 		0x80	// flag added to normal selection types, causes parent item to indicate Changed when item selected
  };
  
  static taiActions* New(RepType rt, int sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL); // note: this is primarily for compatibility with Qt3 version code

  SelType		sel_type;
  int			font_spec; //taiMisc::FontSpec
  taiAction_List	items;
  QWidget*		gui_parent; // needed for submenus
  
  int			count() const {return items.size;} // qt compatability -- note that count won't have separators
  String		label() const; //#GET_Label
  QMenu*		menu()	{ return m_menu; }	 // for when actions stored in a menu, otherwise NULL
  virtual void		setLabel(const String& val); // #SET_Label replaces the SetMLabel call in IV
  taiAction*		curSel() const;
  void			setCurSel(taiAction* value);
  QWidget*		actionsRep(); // where actions are stored, in menu if a menu, else in Rep
  virtual void		AddSep(bool new_radio_grp = false); // add menu separator -- can also be used to create new radio groups --  won't add initial sep, or 2 separators in a row; seps don't count as taiActions
  virtual void		AddAction(taiAction* act); // add the already created action
  taiAction* 		AddItem(const String& val, SelType st = use_default, 
    taiAction::CallbackType ct = taiAction::none, const QObject *receiver = NULL, const char* member = NULL,
    const Variant& usr = _nilVariant);
  taiAction* 		AddItem(const String& val, taiAction::CallbackType ct, 
    const QObject *receiver, const char* member,
    const Variant& usr = _nilVariant, const QKeySequence& shortcut = 0);
  taiAction* 		AddItem(const String& val, SelType st, const taiMenuAction* men_act, 
    const Variant& usr = _nilVariant);
  taiAction* 		AddItem(const String& val, const Variant& usr);
	
  virtual taiMenu*	AddSubMenu(const String& val, TypeDef* typ_ = NULL); // add a submenu -- this also works for toolbars, and will create a toolbar menu button
  
//obs  virtual taiAction*	GetValue()	{ return cur_sel; }
  virtual bool 		GetImageByData(const Variant& usr); // for radio menus, set to item with this data; recursive
  void 			GetImageByIndex(int item);
  taiAction*		FindActionByData(const Variant& usr); // find 1st action with this data, NULL if not found
  taiMenu*		FindSubMenu(const String& nm); // find specified submenu, or NULL if not found

  void			DeleteItem(uint index); // deletes the indicated item -- deletes the gui representation as well
  virtual void		NewRadioGroup();	// start a new radio group (must also preceed first group)
  virtual void		Reset();
  
  taiAction*		operator[](int index) const {return items.SafeEl(index);}
  taiActions(int sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL,
      bool has_menu = false, QMenu* exist_menu = NULL);
  ~taiActions();
#ifndef __MAKETA__
signals:
  void labelChanged(const char* val); //
#endif

  
protected:  
  QActionGroup*		cur_grp; // for radio groups, current group, if any
  taiAction*		cur_sel;  // selection for getting value of menu -- only used by top-level menu
  String		mlabel; // string contents of current menu label
#ifndef __MAKETA__
  QPointer<QMenu>	m_menu; // for when items add to a menu
#endif
  taiActions*	par_menu; // parent menu, if any -- many methods delegate their calls upward if there is a parent
  taiSubMenuEl*		par_menu_el; // parent submenu element, if any
  void 			emitLabelChanged(const String& val); // #IGNORE
  virtual bool 		GetImage_impl(const Variant& usr);  // #IGNORE set to this usr item, returns false if not found -- recursive for hiermenus
  virtual void 		ActionAdded(taiAction* it); // add to rep, def adds to mrep, but overridden for menubutton type
  virtual void 		ActionRemoving(taiAction* it); // remove from rep, def removes from mrep, but overridden for menubutton type
  
protected slots:
  virtual void 		child_triggered_toggled(taiAction* act);
};

//////////////////////////////////
// 	    taiMenu		//
//////////////////////////////////

class TA_API taiMenu : public taiActions {
  // (possibly) hierarchical menu for selecting a single item
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
friend class taiMenu_List; // hack because lists return refs to strings, not values
friend class taiActions;
public:
  taiMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL);
//nbg  taiMenu(int rt, int st, int ft, QWidget* gui_parent_); // constructor for WinBase and other non-taiDialog uses
  taiMenu(QWidget* gui_parent_, int sel_type_= normal, int font_spec_ = 0, QMenu* exist_menu = NULL);
    // constructor for Browser and context menus ft=0 means default font size; 
  ~taiMenu();

  void			exec(const iPoint& pos);
  taiAction*		insertItem(const char* val, const QObject *receiver = NULL, const char* member = NULL,
    const QKeySequence* accel = NULL); // OBS compatability routine with QMenu
  void			insertSeparator() {AddSep();} // Qt-convenience

private:
  void			init(); // #IGNORE
};


//////////////////////////////////
//  taiButtonMenu		//
//////////////////////////////////

class TA_API taiButtonMenu: public taiActions { // a button, in which the actions appear as a popup menu; can also just be an Edit button, with no menu (pass flgEditOnly)
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
public:
  inline QAbstractButton* rep() {return (QAbstractButton*)(QWidget*)m_rep;}
  
  override void		Delete(); // should delete the menu if we didn't use it
  taiButtonMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL);
private:
  void 			init();

};

//////////////////////////////////
// 	    taiMenuBar		//
//////////////////////////////////

class TA_API taiMenuBar : public taiActions {
  // top level menu bar
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  inline QMenuBar*	rep_bar() {return (QMenuBar*)(QWidget*)m_rep;}
  
  override void		AddSep(bool new_radio_grp = false) {} // no seps or groups allowed in a menubar
  
  taiMenuBar(int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // used by taiEditDataHost
  taiMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu); // used by iDataViewer
  ~taiMenuBar();
protected:
  void 			init(QMenuBar* exist_menu); // #IGNORE
  void 			ConstrBar(QWidget* gui_parent_, QMenuBar* exist_bar = NULL); // #IGNORE
};

class TA_API taiToolBar: public taiActions { // a toolbar, in which the actions appear as toolbuttons or button menus
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
public:
  inline QToolBar*	rep() {return (QToolBar*)(QWidget*)m_rep;}
  
  taiToolBar(QWidget* gui_parent_, int ft, QToolBar* exist_bar); // used by iDataViewer
protected:
  void 			init(QToolBar* exist_bar);

};

//////////////////////////////////
// 	taiMenu_List		//
//////////////////////////////////

//NOTE: this class provided for winbase.h and others

class TA_API taiMenu_List : public taPtrList<taiActions> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ if (own_items) delete (taiMenu*)it; }

public:
  bool own_items; //generally false
  taiMenu_List()            { own_items = false; }
  ~taiMenu_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiActions*)it)->mlabel); }
};


//////////////////////////////////
// 	taiEditButton		//
//////////////////////////////////

class TA_API taiEditButton : public taiButtonMenu {
  // actually an edit menu... -- flgReadOnly creates menu which only allows for #EDIT_READ_ONLY members
  Q_OBJECT
public:
  static taiEditButton*	New(void* base, taiEdit *taie, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgReadOnly, flgEditOnly -- internally sets flgEditOnly if appropriate
        
  ~taiEditButton();

public slots:
  virtual void	Edit();		// edit callback
  void setRepLabel(const char* label);
protected:
  void*		cur_base;
  taiEdit*	ie;
  taiDataList 	meth_el;	// method elements

  override void		GetImage_impl(const void* base); 
  virtual void		GetMethMenus();
  virtual void		SetLabel();
  
  taiEditButton(void* base, taiEdit *taie, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); 
};


//////////////////////////////////
// 	taiObjChooser		//
//////////////////////////////////

class TA_API taiObjChooser: iDialog {
// ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select objects from a list, much like a file chooser.  can be tokens from typedef or items on a list
  INHERITED(iDialog)
  Q_OBJECT
public:
  static taiObjChooser* createInstance(TAPtr parob, const char* captn, bool selonly = true, QWidget* par_window_ = NULL);
    // create method for lists/groups
  static taiObjChooser* createInstance(TypeDef* tpdf, const char* captn, TAPtr scope_ref_ = NULL, QWidget* par_window_ = NULL);
    // create method for tokens

  bool	        	select_only;	// if true, only for selecting objects from current parent object
  String		caption; 	// current caption at top of chooser
  String		path_str;	// current path string
  TABLPtr		lst_par_obj;	// parent object that is a list object
  TAPtr			reg_par_obj; 	// parent object that is *not* a list object
  TypeDef*		typ_par_obj; 	// parent object that is a typedef (get tokens)
  TAPtr			scope_ref;	// reference object for scoping

  TAPtr			sel_obj() const {return msel_obj;}// current selected object
  void			setSel_obj(const TAPtr value);	// 
  String		sel_str() const {return msel_str;} 	// string rep of current selection
  String_Array		items;		// the items in the list

  QGridLayout*		layOuter;
  QListWidget* 		browser; 	// list of items
  QLineEdit* 		editor;
  QHBoxLayout*		layButtons;
  QPushButton*		btnOk;
  QPushButton*		btnCancel;

  taiObjChooser(TAPtr parob, const char* captn, bool selonly, QWidget* par_window_); // USE createInstance instead!!!
  taiObjChooser(TypeDef* tpdf, const char* captn, TAPtr scope_ref_, QWidget* par_window_); // USE createInstance instead!!!

  virtual bool	Choose();
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void 	Build();	// called as constructed
  virtual void 	Clear();	// reset data
  virtual void 	Load();		// reload data
  virtual void 	ReRead();	// update browser for new parent

  virtual void	GetPathStr();	// get current path string
  virtual void  AddItem(const char* itm, const void* data_ = NULL); // add one item to dialog
  virtual void	UpdateFmSelStr(); // update selection based on sel_str

protected:
  TAPtr			msel_obj;	// current selected object
  String		msel_str; 	// string rep of current selection

  void 		init(const char* captn, bool selonly, QWidget* par_window_); // called by constructors
  virtual void 	AcceptEditor_impl(QLineEdit* e);
  void 		AddObjects(TAPtr obj);
  void		AddTokens(TypeDef* td); // add all tokens of given type

protected slots:
  void accept(); // override
  void reject(); // override
  // callbacks
  void 		browser_currentItemChanged(QListWidgetItem* itm, QListWidgetItem* prv);
  void 		browser_itemDoubleClicked(QListWidgetItem* itm);
  virtual void 	DescendBrowser();
  virtual void 	AcceptEditor();
};

//////////////////////////////////
// 	taiItemChooser		//
//////////////////////////////////

class TA_API taiItemChooser: iDialog {
// ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select items from a list, much like a file chooser; can be tokens from typedef or items on a list
INHERITED(iDialog)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjDataRole = Qt::UserRole + 1,
    ObjCatRole  // for object category string, whether shown or not
  };
#endif
  static const String	cat_none; // "none" category
  static int		filt_delay; // delay, in msec, to invoke filter after typing

  static taiItemChooser* New(const String& caption, taiItemPtrBase* client = NULL, 
    int ft = 0, QWidget* par_window_ = NULL);

  String		caption; 	// current caption at top of chooser
  bool			multi_cats;	// each item may have multiple categories, separated by commas

  inline taiItemPtrBase* client() const {return m_client;} // only valid in Constr and between Choose...accept/reject
  void*			selObj() const {return m_selObj;} // current selected object
  void			setSelObj(void* value, bool force = false);	// 

  int			catFilter() const {return m_cat_filter;}
  void			setCatFilter(int value, bool force = false);
  int			view() const {return m_view;}
  void			setView(int value, bool force = false);
  
  QVBoxLayout*		layOuter;
  QComboBox*		  cmbView;
  QComboBox*		  cmbCat; //note: item 0 is "all" (i.e., no filtering)
  QTreeWidget* 		  items; 	// list of items
//QHBoxLayout*		  layButtons;
  QPushButton*		    btnOk;
  QPushButton*		    btnCancel;
//QHBoxLayout*		  layFilter;
  QLineEdit*		  filter;

  virtual bool		Choose(taiItemPtrBase* client);
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void 		Clear();	// reset data
  void			SetFilter(const QString& filt); // apply a filter
  void			ClearFilter(); // remove filtering
  
  virtual QTreeWidgetItem* AddItem(const QString& itm_txt, QTreeWidgetItem* parent, 
    const void* data_ = NULL); // add one item to dialog, optionally with data
  virtual QTreeWidgetItem* AddItem(const QString& itm_cat, const QString& itm_txt,
    QTreeWidgetItem* parent, const void* data_ = NULL); 
    // add one categorized item to dialog, optionally with data

protected:
  int			m_changing;
  void*			m_selObj;	// current selected object
  QTreeWidgetItem*	m_selItem; // cached for showEvent
  int			m_view;
  int			m_cat_filter;
  taiItemPtrBase* 	m_client; // NOTE: only valid in Constr and between Choose...accept/reject
  QString		last_filter; // for checking if anything changed
  QTimer*		timFilter; // timer for filter changes
  
  void 			showEvent(QShowEvent* event); //override
  
  void 			ApplyFiltering();
  virtual void 		Refresh();	// rebuild current view
  bool 			SetCurrentItemByData(void* value); 
  bool 			ShowItem(const QTreeWidgetItem* item) const;
  virtual void		Constr(taiItemPtrBase* client_); 
   // does constr, called in static, so can extend

  taiItemChooser(const String& caption, QWidget* par_window_);
protected slots:
  void 			accept(); // override
  void 			reject(); // override
  void 			items_itemDoubleClicked(QTreeWidgetItem* itm, int col);
  void 			cmbView_currentIndexChanged(int index);
  void 			cmbCat_currentIndexChanged(int index);
  void 			filter_textChanged(const QString& text);
  void 			timFilter_timeout();
  void			show_timeout(); // for scrolling to item
private:
  void 		init(const String& captn); // called by constructors
  taiItemChooser(const taiItemChooser&); //no
  taiItemChooser& operator=(const taiItemChooser&); //no
};

typedef bool (*item_filter_fun)(void*, void*); // optional filter, spec'ed in ITEM_FILTER_xxx

class TA_API taiItemPtrBase : public taiData {
// common base for MemberDefs, MethodDefs, TypeDefs, Enums, and tokens, that use the ItemChooser
  Q_OBJECT
public:
  item_filter_fun	item_filter; // #IGNORE optional filter, in ITEM_FILTER_xxx
  String		filter_start_txt; // if nonempty, item name must start with this text to be included
  
  virtual const String	labelText(); // "tag: name" for button
  virtual const String	titleText(); // title of overall chooser;
  virtual int		columnCount(int view) const = 0; 
    // number of header columns in the view
  virtual const String	headerText(int index, int view) const = 0;
  inline const String   nullText() { return null_text; }
  inline QAbstractButton* rep() {return m_but;}
  inline void*		sel() const {return m_sel;}
  virtual bool		isValid() const {return (targ_typ);} // if all required params have been set
  virtual int		catCount() const; 
  // number of categories, where supported; 0=nocat, 1+=cats
  virtual const String	catText(int index) const; 
  // number of different kinds of views, ex flat vs. 
  virtual int		viewCount() const {return 1;} 
  // number of different kinds of views, ex flat vs. tree
  virtual const String	viewText(int index) const = 0; 
  // number of different kinds of views, ex flat vs. 
  virtual bool		hasNoItems() { return false; }
  // if it is possible to quickly determine that there are no items on the list, return true -- makes the chooser read-only with "no items available to choose" text label -- if nullOK, then this must always be false.
  virtual bool		hasOnlyOneItem() { return false; }
  // if it is possible to quickly determine that there is one and only one item in the list, return true AND set the selection to that one item -- if true, it makes the chooser read-only --  first check should be if nullOK and there are no items -- then there is one item! -- set to null -- if not, then multiple items and should return false

  void			setNullText(const String& nt) { null_text = " " + nt; }
  // set text to display instead of NULL for a null item
  
  bool			ShowItemFilter(void* base, void* item, const String& itnm) const;
  // apply optional item_filter and filter_start_txt, else true
  
  virtual void 		GetImage(void* cur_sel, TypeDef* targ_typ);
  
  virtual void		BuildCategories(); // for types that support categories
  virtual void		BuildChooser(taiItemChooser* ic, int view = 0) {}
    // builds the tree

  ~taiItemPtrBase();
  
public slots:
  bool			OpenChooser(); // make and then open chooser dialog
  virtual void		EditPanel() {} // used by tokens to edit -- opens panel on guy (in new tab)
  virtual void		EditDialog() {} // used by tokens to edit -- opens edit dialog

protected:
  void*			m_sel; // current value
  QAbstractButton*	m_but;
  QToolButton*		btnEdit; // only for tokens
  TypeDef*		targ_typ; 
  String_Array*		cats; // categories -- only created if needed
  String		null_text;	
  
  virtual const String	itemTag() const {return _nilString;} // for "N: label" on button, is "N: "
  virtual const String	labelNameNonNull() const = 0; // name part of label, when obj non-null
  
  virtual void		BuildCategories_impl() {} // for types that support categories
  virtual void 		UpdateImage(void* cur_sel);
  
  taiItemPtrBase(TypeDef* typ_, IDataHost* host,
		 taiData* par, QWidget* gui_parent_, int flags_ = 0,
		 const String& flt_start_txt = ""); // typ_ 
};


class TA_API taiMemberDefButton : public taiItemPtrBase {
// for MemberDefs
INHERITED(taiItemPtrBase)
public:
  inline MemberDef*	md() const {return (MemberDef*)m_sel;}
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  int			viewCount() const {return 1;} // override
  const String		viewText(int index) const; // override

  void			GetImage(MemberDef* cur_sel, TypeDef* targ_typ) 
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  MemberDef*		GetValue() {return md();}

  override void		BuildChooser(taiItemChooser* ic, int view = 0); // 
  
  virtual bool		ShowMember(MemberDef* mbr);

  taiMemberDefButton(TypeDef* typ_, IDataHost* host,
		     taiData* par, QWidget* gui_parent_, int flags_ = 0,
		     const String& flt_start_txt = "");
protected:
  const String		itemTag() const {return "Member: ";}
  const String		labelNameNonNull() const;

  override void		BuildCategories_impl();
  void 			BuildChooser_0(taiItemChooser* ic);
};


class TA_API taiMethodDefButton : public taiItemPtrBase {
// for MethodDefs
INHERITED(taiItemPtrBase)
public:
  inline MethodDef*	md() const {return (MethodDef*)m_sel;}
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  int			viewCount() const {return 2;} // override
  const String		viewText(int index) const; // override

  void			GetImage(MethodDef* cur_sel, TypeDef* targ_typ) 
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  MethodDef*		GetValue() {return md();}

  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  taiMethodDefButton(TypeDef* typ_, IDataHost* host,
		     taiData* par, QWidget* gui_parent_, int flags_ = 0,
		     const String& flt_start_txt = "");
protected:
  const String		itemTag() const {return "Method: ";}
  const String		labelNameNonNull() const;

  override void		BuildCategories_impl();
  void 			BuildChooser_0(taiItemChooser* ic);
  int 			BuildChooser_1(taiItemChooser* ic, TypeDef* top_typ, 
    QTreeWidgetItem* top_item); // we use this recursively
    
  virtual bool		ShowMethod(MethodDef* mth);
};


class TA_API taiMemberMethodDefButton : public taiItemPtrBase {
// for MemberDefs AND MethodDefs -- useful for path completion lookup for example
INHERITED(taiItemPtrBase)
public:
  inline TypeItem*	md() const {return (TypeItem*)m_sel;}

  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  int			viewCount() const {return 3;} // override
  const String		viewText(int index) const; // override

  void			GetImage(MemberDef* cur_sel, TypeDef* targ_typ) 
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  TypeItem*		GetValue() {return md();}

  override void		BuildChooser(taiItemChooser* ic, int view = 0); // 
  
  virtual bool		ShowMember(MemberDef* mbr);
  virtual bool		ShowMethod(MethodDef* mth);

  taiMemberMethodDefButton(TypeDef* typ_, IDataHost* host,
			   taiData* par, QWidget* gui_parent_, int flags_ = 0,
			   const String& flt_start_txt = "");
protected:
  const String		itemTag() const {return "Member/Method: ";}
  const String		labelNameNonNull() const;

  override void		BuildCategories_impl();
  void 			BuildChooser_0(taiItemChooser* ic); // all
  void 			BuildChooser_1(taiItemChooser* ic); // just mbr
  void 			BuildChooser_2(taiItemChooser* ic); // just mth
};


class TA_API taiEnumStaticButton : public taiItemPtrBase {
// for enums AND static members, methods -- useful for path completion lookup for example
INHERITED(taiItemPtrBase)
public:
  inline TypeItem*	md() const {return (TypeItem*)m_sel;}

  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  int			viewCount() const {return 4;} // override
  const String		viewText(int index) const; // override

  void			GetImage(MemberDef* cur_sel, TypeDef* targ_typ) 
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  TypeItem*		GetValue() {return md();}

  override void		BuildChooser(taiItemChooser* ic, int view = 0); // 
  
  virtual bool		ShowEnum(EnumDef* enm);
  virtual bool		ShowMember(MemberDef* mbr);
  virtual bool		ShowMethod(MethodDef* mth);

  taiEnumStaticButton(TypeDef* typ_, IDataHost* host,
		      taiData* par, QWidget* gui_parent_, int flags_ = 0,
		      const String& flt_start_txt = "");
protected:
  const String		itemTag() const {return "Enum/static: ";}
  const String		labelNameNonNull() const;

  override void		BuildCategories_impl();
  void 			BuildChooser_0(taiItemChooser* ic); // all
  void 			BuildChooser_1(taiItemChooser* ic); // just enum
  void 			BuildChooser_2(taiItemChooser* ic); // just static mbr
  void 			BuildChooser_3(taiItemChooser* ic); // just static meth
};


class TA_API taiTypeDefButton : public taiItemPtrBase {
// for TypeDefs
INHERITED(taiItemPtrBase)
public:
  inline TypeDef*	td() const {return (TypeDef*)m_sel;}
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  const String		viewText(int index) const; // override
  override bool		hasNoItems();
  override bool		hasOnlyOneItem();

  void			GetImage(TypeDef* cur_sel, TypeDef* targ_typ) 
    {taiItemPtrBase::GetImage((void*)cur_sel, targ_typ);}
  TypeDef*		GetValue() {return td();}

  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  taiTypeDefButton(TypeDef* typ_, IDataHost* host,
		   taiData* par, QWidget* gui_parent_, int flags_ = 0,
		   const String& flt_start_txt = "");
protected:
  enum TypeCat {
    TC_NoAdd, // for ptrs, non classes, etc.
    TC_NoAddCheckChildren, // typically for templates
    TC_Add // "normal" class types
  };
  const String		itemTag() const {return "Type: ";}
  const String		labelNameNonNull() const;

  TypeCat		AddType_Class(TypeDef* typ); // true if should be shown to user
  override void 	BuildCategories_impl();
  void 			BuildCategoriesR_impl(TypeDef* top_typ);
  int 			BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
    QTreeWidgetItem* top_item); // we use this recursively
  int 			CountChildren(TypeDef* td);
};

class TA_API taiEnumTypeDefButton : public taiTypeDefButton {
// 
INHERITED(taiTypeDefButton)
public:
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  
  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  taiEnumTypeDefButton(TypeDef* typ_, IDataHost* host,
		       taiData* par, QWidget* gui_parent_, int flags_ = 0,
		       const String& flt_start_txt = "");
protected:
  const String		itemTag() const {return "Enum Type: ";}

  bool			AddType_Enum(TypeDef* typ_, TypeDef* par_typ); // true if should be shown to user
  int 			BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
    QTreeWidgetItem* top_item); // we use this recursively
};


class TA_API taiTokenPtrButton : public taiItemPtrBase {
// for tokens of taBase objects
INHERITED(taiItemPtrBase)
public:
  inline TAPtr		token() const {return (TAPtr)m_sel;}
  override int		columnCount(int view) const;
  override const String	headerText(int index, int view) const;
  override int		viewCount() const {return 1;}
  override const String	viewText(int index) const;
  override bool		hasNoItems();
  override bool		hasOnlyOneItem();
  virtual bool		countTokensToN(int& cnt, TypeDef* td, int n, void*& last_itm);
  // recursively count valid (in scope etc) tokens of type until reaching n, at which point a true is returned -- if n is not reached, return false -- used for above two routines

  virtual void		GetImage(TAPtr ths, TypeDef* targ_typ, TAPtr scope = NULL,
				 TypeDef* scope_type = NULL);
  // get image, using the new type and scope params supplied
  virtual taBase*	GetValue() {return token();}
  
  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  override void		EditPanel();
  override void		EditDialog();
  
  taiTokenPtrButton(TypeDef* typ_, IDataHost* host,
		    taiData* par, QWidget* gui_parent_, int flags_ = 0,
		    const String& flt_start_txt = "");
protected:
  taSmartRef		scope_ref;	// reference object for scoping, default is none
  TypeDef*		scope_typ;	// type of scope to use (NULL = default)
  
  const String		itemTag() const {return "Token: ";}
  const String		labelNameNonNull() const;

  int 			BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
    QTreeWidgetItem* top_item); // we use this recursively
  virtual bool		ShowToken(void* tk) const;
};


class TA_API taiTokenPtrMultiTypeButton : public taiItemPtrBase {
// for tokens of taBase objects of multiple types
INHERITED(taiItemPtrBase)
public:
  TypeSpace		type_list; // #LINK_GROUP set of types to generate tokens for -- must be set manually after construction and before GetImage etc -- be sure to only do Link here..

  inline TAPtr		token() const {return (TAPtr)m_sel;}
  override int		columnCount(int view) const;
  override const String	headerText(int index, int view) const;
  override int		viewCount() const; // n = size of type_list + 1
  override const String	viewText(int index) const;

  virtual void		GetImage(TAPtr ths, TypeDef* targ_typ, TAPtr scope = NULL,
				 TypeDef* scope_type = NULL);
  // get image, using the new type and scope params supplied
  virtual taBase*	GetValue() {return token();}
  
  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  override void		EditPanel();
  override void		EditDialog();
  
  taiTokenPtrMultiTypeButton(TypeDef* typ_, IDataHost* host,
			     taiData* par, QWidget* gui_parent_, int flags_ = 0,
			     const String& flt_start_txt = "");
protected:
  taSmartRef		scope_ref;	// reference object for scoping, default is none
  TypeDef*		scope_typ;	// type of scope to use (NULL = default)
  
  const String		itemTag() const {return "Token: ";}
  const String		labelNameNonNull() const;

  int 			BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
    QTreeWidgetItem* top_item); // we use this recursively
  virtual bool		ShowToken(void* tk) const;
};


//////////////////////////////////
// 	taiFileButton		//
//////////////////////////////////

class TA_API taiFileButton : public taiButtonMenu {
  Q_OBJECT
public:
  bool			read_only;	// only reading streams is an option
  bool			write_only;	// only writing streams are available

  virtual void		SetFiler(taFiler* gf_); // you can use your own filer if you want; ref counted
  virtual void  	GetImage();
  taFiler* 		GetFiler() {return gf;} //NOTE: make sure you ref it properly!

  virtual void		GetGetFile();	// make sure we have a getfile..

  taiFileButton(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_,
      int flags_, bool rd_only = false, bool wrt_only = false);
  ~taiFileButton();

public slots:
  virtual void		Open();		// callback
  virtual void		Save();		// callback
  virtual void		SaveAs();	// callback
  virtual void		Append();	// callback
  virtual void		Close();	// callback
  virtual void		Edit();		// callback
protected:
  taFiler*		gf;
};


//////////////////////////////////
// 	taiElBase		//
//////////////////////////////////

// base class for sundry taiData items that use a menu, and have a taBase-derived current item

class TA_API taiElBase: public taiData {
INHERITED(taiData)
public:
  TAPtr		cur_obj;
  override void		DataChanged(taiData* chld = NULL); // do autoapply
//  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  virtual void  GetMenu(taiActions* actions, taiMenuAction* actn = NULL) {} // variant provided for MenuGroup_impl in winbase
  void 		setCur_obj(TAPtr value, bool do_chng = true); // set cur_obj and notifies change if different
  taiElBase(taiActions* actions_, TypeDef* tp, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiElBase();
protected:
  taiActions*	ta_actions;
  bool		ownflag;
};

//////////////////////////////////
// 	taiToken,Sub		//
//////////////////////////////////

class TA_API taiToken : public taiElBase {
  // for making menus of tokens
  Q_OBJECT
public:
//  bool		over_max;	// over max_menu

  override QWidget*	GetRep() { return (ta_actions) ? ta_actions->GetRep() : NULL; }
  
  virtual void		GetImage(TAPtr ths);
    // get image, using the current type and scope
  virtual void		GetImage(TAPtr ths, TypeDef* new_typ, TAPtr new_scope);
    // get image, using the new type and scope supplied
  virtual TAPtr		GetValue(); //
//nn  virtual void		SetTypeScope(TypeDef* new_typ, TAPtr new_scope); 
    // set a new base type and scope; doesn't update menu
  
  virtual void		GetUpdateMenu(const taiMenuAction* actn = NULL); 
    // gets or updates the menu

private: //test for usage
  taiToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = (flgNullOk | flgEditOk)); // uses flags flgNullOk, flgEditOk,
//            bool nul_not=false, bool edt_not=false);
protected:
  TAPtr		scope_ref;	// reference object for scoping, default is none
  
  virtual void	GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* actn = NULL);

protected slots:
  virtual void	Edit();		// for edit callback
  virtual void	Chooser();	// for chooser callback
  void 		ItemChosen(taiAction* menu_el); // when user chooses from menu
};

class TA_API taiSubToken : public taiElBase {
  // Menu for sub tokens of a giventype
  Q_OBJECT
public:
  void*		menubase;

  QWidget*	GetRep();

  virtual void	GetMenu(taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);
  virtual void	GetMenuImpl(void* base, taiMenuAction* actn = NULL);

  virtual void	GetImage(const void* ths, void* sel=NULL);
  virtual void*	GetValue();

  taiSubToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk
  taiSubToken(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk

public slots:
  virtual void	Edit();		// for edit callback
};


class TA_API taiTypeInfoBase : public taiData {
// common base for MemberDefs, MethodDefs, TypeDefs, and Enums of a typedef in the object with a MDTYPE_xxx option
public:
  MemberDef*	memb_md; // MemberDef of the member that will get the target pointer
  TypeDef*	targ_typ;	// target type from which to get list of items -- may be same as typ, but could differ
  void*		menubase;	// the address of the object
  taiActions*	ta_actions;

  override QWidget* GetRep();


  virtual void		GetMenu() = 0;
  virtual void 		GetImage(const void* base, bool get_menu = true, void* cur_sel = NULL);

  taiTypeInfoBase(taiActions::RepType rt, int ft, MemberDef* memb_md_, 
    TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags_ = 0); // if targ_type is null, it will be determined
  ~taiTypeInfoBase();
protected:
  virtual void 		GetTarget(); // determines the target type for the lookup menu
};

/*nuke
//////////////////////////////////
// 	taiMemberDef		//
//////////////////////////////////

class TA_API taiMemberDefMenu : public taiTypeInfoBase {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
INHERITED(taiTypeInfoBase)
public:
  MemberDef*	md;

  override void		GetMenu();
  virtual MemberDef*	GetValue();

  taiMemberDefMenu(taiActions::RepType rt, int ft, MemberDef* md_,
    MemberDef* memb_md_, TypeDef* typ_, IDataHost* host, 
    taiData* par, QWidget* gui_parent_, int flags_ = 0);
};
*/
//////////////////////////////////
// 	taiMethodDef		//
//////////////////////////////////

class TA_API taiMethodDefMenu : public taiTypeInfoBase {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
INHERITED(taiTypeInfoBase)
public:
  MethodDef*	md;
  MethodSpace*  sp;

  override void		GetMenu() {GetMenu(NULL);}
  virtual void		GetMenu(const taiMenuAction* actn);
  virtual void		UpdateMenu(const taiMenuAction* actn = NULL);

  virtual MethodDef*	GetValue();

  taiMethodDefMenu(taiActions::RepType rt, int ft, MethodDef* md_, 
    MemberDef* memb_md_, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
};


//////////////////////////////////
// 	taiTypeHier		//
//////////////////////////////////

class TA_API taiTypeHier : public taiData {
// for menus of type hierarchy
public:
  taiActions* 	ta_actions;
  bool		ownflag;
  bool		enum_mode; // when set, creates a hier of enum TypeDefs, for picking an enum type

  virtual void	GetMenu() {GetMenu(NULL);}
  virtual void	GetMenu(const taiMenuAction* acn);
  virtual void	UpdateMenu() {UpdateMenu(NULL);}
  virtual void	UpdateMenu(const taiMenuAction* acn);

  override QWidget* GetRep();
  virtual void		GetImage(TypeDef* ths);
  virtual TypeDef*	GetValue();

  taiTypeHier(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags_ = 0);
  taiTypeHier(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags_ = 0);
  ~taiTypeHier(); //
protected:
  bool		AddType_Enum(TypeDef* typ_); 
  bool		AddType_Class(TypeDef* typ_); 
  int		CountChildren(TypeDef* typ_);
  int		CountEnums(TypeDef* typ_);
  virtual void	GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn); 
  void	GetMenu_Enum_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn); //
};

//////////////////////////////////
//	taiMethMenu,Button	//
//////////////////////////////////

// unlike real taiData, functions are not subject to updating
// so the constructor is the one that does all the work..

class cssClass;			// #IGNORE
class cssiArgDialog;		// #IGNORE

class TA_API taiMethodData : public taiData {
  // all representations of member functions must inherit from this one
  Q_OBJECT
public:
  void*		base;		// base is of parent type
  MethodDef* 	meth;
  bool		is_menu_item;   // true if this is supposed to be a menu item (else QWidget*)
  cssClass*	args;
  cssiArgDialog* arg_dlg;
  int		use_argc;

  taiMethodData(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  virtual QAbstractButton*	GetButtonRep() {return buttonRep;}
    // button connected to method -- only created on demand by subclasses

  virtual bool	hasButtonRep() { return (buttonRep != NULL); }

  virtual void	GenerateScript(); // output script code equivalent if recording

  virtual void 	AddToMenu(taiActions* mnu);
  
public slots:
  virtual void 	CallFun() {CallFun_impl();} // call the function (button callback)

protected:
  static void  	ShowReturnVal(cssEl* rval, IDataHost* host,
     const String& meth_name); // show return value after menu call
  
  QAbstractButton*	buttonRep;
  QWidget*	gui_parent;
  QAbstractButton*	MakeButton(); // makes the button if necessary, and returns a reference
  
  bool 		CallFun_impl();		// impl -- we could delete partway through, returns true if we still exist!
// note that some situations can cause this guy to delete during processing
  void		ApplyBefore();	// apply changes before performing menu call
  void		UpdateAfter();	// update display after performing menu call

};

class TA_API taiMethMenu : public taiMethodData {
  // all representations of member functions must inherit from this one
  // NOTE: gui_parent is the parent of the Button, since the menu item is already parented in menu
public:
  taiMethMenu(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  override QAbstractButton* GetButtonRep() {return MakeButton();}
  QWidget*	GetRep()		{ return (QWidget*)buttonRep; }
};

class TA_API taiMethButton : public taiMethodData {
  // button representation of a method -- uses the QAbstractButton, and sets it as the Rep as well
public:
  override QAbstractButton* GetButtonRep();
  
  taiMethButton(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);
};


class TA_API taiMethToggle : public taiMethodData {
  // toggle representation of a method (does not call directly, but checks flag)
  Q_OBJECT
public:
  inline QCheckBox* 	rep() {return (QCheckBox*)(QWidget*)m_rep;}

  taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

public slots:
  void			CallFun(); // replaces
}; 

#endif // TA_QTDATA_H

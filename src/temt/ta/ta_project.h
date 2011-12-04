// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

// ta_project.h

#ifndef TA_PROJECT_H
#define TA_PROJECT_H

#include "ta_group.h"
#include "ta_defaults.h"
#include "ta_datatable.h"
#include "ta_viewspec.h"
#include "ta_viewer.h"
#include "ta_program.h"
#include "ta_plugin_p.h"
#include "ta_qtclipdata.h"
#include "colorscale.h"
#include "ta_seledit.h"
#include "ta_time.h"
#include "ta_thread.h"

class taDoc;
class taWizard;
class taProject;
class Project_Group;
class taRootBase;
class iPluginEditor;//

////////////////////////////////////////////////////////////////////////
//		taDoc -- documents

class TA_API taDoc : public taNBase {
  // ##CAT_Docs document for providing information on projects and other objects
INHERITED(taNBase)
public:
  String	desc;	   	// #EDIT_DIALOG #CAT_taBase description of this documentation: what is this doc about
  bool		auto_open;	// open this document upon startup
  bool		web_doc; 	// #READ_ONLY this document lives on the web, at the following URL, instead of being local text saved in the project -- the most recently viewed version of the document is cached into the local text, and is rendered if it is not possible to connect to the internet -- this flag is automatically updated based on the url field
  String	wiki;		// name of a wiki, as specified in global preferences, where this object should be stored -- this is used to lookup the wiki name -- if blank then url must be a full URL path
  String	url;		// a URL location for this document -- if blank or "local" then text field is used as document text -- otherwise if wiki name is set, then this is relative to that wiki, as wiki_url/index.php/Projects/url, otherwise it is a full URL path to a valid location
  String	full_url;	// #HIDDEN full URL location for this document -- always updated and used as a backup if loaded in a context where the wiki name is not recognized
  float		text_size;	// #DEF_1 multiplier for text size on page (zoom factor) -- 1.0 is default -- this multiplies the global preferences/options font_size / 12.0 factor (i.e., 12.0 is assumed to be a normal baseline font size), to arrive at the overall font size scaling factor

  String	text; 		// #HIDDEN #EDIT_DIALOG the text of the document for local docs (in html/mediawiki format)
  String	html_text; 	// #READ_ONLY #HIDDEN #EDIT_DIALOG wiki conversion of html text -- use this for actual display

  static String		WikiParse(const String& in_str);
  // convert very basic wiki syntax to html format -- == headers ==, * bulleted lists, [[ ]] links, etc

  virtual void		UpdateText();
  // update the html_text from the user-entered text with wiki syntax by calling WikiParse

  virtual void		SetURL(const String& new_url);
  // set a new url -- will parse into wiki and url if possible
  virtual String	GetURL();
  // get a full url based on url specified plus any absolute refs needed

  override String 	GetTypeDecoKey() const { return "Doc"; }

  TA_BASEFUNS(taDoc);
protected:
  override void		UpdateAfterEdit_impl();
  static const String	init_text; // ##IGNORE initial text

private:
  SIMPLE_COPY(taDoc);
  void 	Initialize();
  void 	Destroy() { }
};
TA_SMART_PTRS(taDoc);

class TA_API UserData_DocLink: public UserDataItemBase {
  // a link to a doc -- when added to the userdata of an obj, will auto display the doc
INHERITED(UserDataItemBase)
public:
  taDocRef		doc; // the doc
  
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  TA_BASEFUNS(UserData_DocLink)
private:
  void Copy_(const UserData_DocLink& cp){doc = cp.doc;}
  void Initialize();
  void Destroy() {}
};

class TA_API Doc_Group : public taGroup<taDoc> {
  // ##CAT_Docs ##EXPAND_DEF_1 group of doc objects
INHERITED(taGroup<taDoc>)
public:
  virtual void		AutoEdit();

  override String 	GetTypeDecoKey() const { return "Doc"; }
  TA_BASEFUNS(Doc_Group);
private:
  NOCOPY(Doc_Group)
  void	Initialize() 		{ SetBaseType(&TA_taDoc); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
//		taWizard -- wizards

class TA_API taWizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 ##CAT_Wizard wizard for automating construction of simulation objects
INHERITED(taNBase)
public:
  bool		auto_open;	// open this wizard upon startup
  taDoc		wiz_doc;	// #HIDDEN #NO_SAVE wizard doc object

  virtual void		RenderWizDoc();
  // #IGNORE render the wizard doc, providing the interface that the user sees to select wizard elements -- called in InitLinks -- calls header, impl, footer -- use impl to override raw text render

  override String 	GetTypeDecoKey() const { return "Wizard"; }

  void	InitLinks();
  TA_BASEFUNS(taWizard);
protected:
  void	UpdateAfterEdit_impl();
  virtual void	RenderWizDoc_header(); // the doc header (very start) text
  virtual void	RenderWizDoc_footer(); // the doc footer (very end) text
  virtual void	RenderWizDoc_impl();   // main render of content
private:
  SIMPLE_COPY(taWizard);
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class TA_API Wizard_Group : public taGroup<taWizard> {
  // ##CAT_Wizard ##EXPAND_DEF_1 group of wizard objects
INHERITED(taGroup<taWizard>)
public:
  virtual void		AutoEdit();

  override String 	GetTypeDecoKey() const { return "Wizard"; }

  TA_BASEFUNS(Wizard_Group);
private:
  NOCOPY(Wizard_Group)
  void	Initialize() 		{ SetBaseType(&TA_taWizard); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
//		SelectEdit -- control panels

class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // ##CAT_Display ##EXPAND_DEF_1 group of select edit dialog objects
INHERITED(taGroup<SelectEdit>)
public:
  virtual void	AutoEdit();

  override String 	GetTypeDecoKey() const { return "SelectEdit"; }

  TA_BASEFUNS(SelectEdit_Group);
private:
  NOCOPY(SelectEdit_Group)
  void	Initialize() { SetBaseType(&TA_SelectEdit);}
  void 	Destroy()		{ };
};

////////////////////////////////////////////////////////////////////////
//		Undo/Redo System

class taUndoRec;
class taUndoDiffSrc;
class taUndoMgr;
//

class TA_API taUndoDiffSrc : public taOBase {
  // ##CAT_Undo full source record for diff-based saving of undo save data -- diffs are computed against this guy
INHERITED(taOBase)
public:
  taDateTime	mod_time;	// time (to seconds level of resolution) when obj was modified
  taBaseRef	save_top;	// top-level object under which the data was saved
  String	save_top_path;	// path to the save_top -- in case it disappears -- again relative to mgr owner
  String	save_data;	// dump-file save from the save_top object (save_top->Save_String)
  taStringDiff	diff;		// diff for this data -- save_data is string_a for this case
  int		last_diff_n;	// raw number of diff records -- when this gets to be too large, then it is time to move on to a new src
  float		last_diff_pct;	// percent diff records are of total save_data lines -- when this gets to be too large, then it is time to move on to a new src

  virtual void	InitFmRec(taUndoRec* rec);
  // initialize our vals from given record
  virtual void	EncodeDiff(taUndoRec* rec);
  // encode a new diff against given record, set rec to point to us
  virtual int	UseCount();
  // report how many undo recs are using this guy -- if none, then remove..

  TA_SIMPLE_BASEFUNS(taUndoDiffSrc);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

TA_SMART_PTRS(taUndoDiffSrc);

class TA_API taUndoDiffSrc_List : public taList<taUndoDiffSrc> {
  // ##CAT_Undo list of full source records for diff-based saving of undo save data -- managed using circular buffer logic per new functions
INHERITED(taList<taUndoDiffSrc>)
public:
  int		st_idx;		// #READ_ONLY index in underlying array where the list starts (i.e., the position of the logical 0 index) -- updated by functions and should not be set manually
  int		length;		// #READ_ONLY logical length of the list -- is controlled by adding and shifting, and should NOT be set manually

  int	CircIdx(int cidx) const
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index

  bool 	CircIdxInRange(int cidx) const { return InRange(CircIdx(cidx)); }
  // #CAT_CircAccess check if logical circular index is in range
  
  taUndoDiffSrc*	CircSafeEl(int cidx) const { return SafeEl(CircIdx(cidx)); }
  // #CAT_CircAccess returns element at given logical circular index, or NULL if out of range

  taUndoDiffSrc*	CircPeek() const {return SafeEl(CircIdx(length-1));}
  // #CAT_CircAccess returns element at end of circular buffer

  /////////////////////////////////////////////////////////
  //	Special Modify Routines

  void		CircShiftLeft(int nshift)
  { st_idx = CircIdx(nshift); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  void		CircAddExpand(taUndoDiffSrc* item) {
    if((st_idx == 0) && (length >= size)) {
      inherited::Add(item); length++; 	// must be building up the list, so add it
    }
    else {
      ReplaceIdx(CircIdx(length++), item);	// expand the buffer length and set to the element at the end
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, expanding the length of the list by 1 under all circumstances

  void		CircAddLimit(taUndoDiffSrc* item, int max_length) {
    if(length >= max_length) {
      CircShiftLeft(1 + length - max_length); // make room
      ReplaceIdx(CircIdx(length++), item);	// set to the element at the end
    }
    else {
      CircAddExpand(item);
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, shifting it left if length is at or above max_length to ensure a fixed overall length list (otherwise expanding list up to max_length)

  override void	Reset();

  void 	Copy_(const taUndoDiffSrc_List& cp);
  TA_BASEFUNS(taUndoDiffSrc_List);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

class TA_API taUndoRec : public taOBase {
  // ##CAT_Undo one undo record -- saves all necessary state information
INHERITED(taOBase)
public:
  String	mod_obj_path;	// path to the object that was just about to be modified, after this record was saved -- relative to owner of taUndoMgr (typically the project)
  String 	mod_obj_name;	// name of modified object
  String	action;		// a brief description of the action performed
  taDateTime	mod_time;	// time (to seconds level of resolution) when obj was modified
  taBaseRef	save_top;	// top-level object under which the data was saved
  String	save_top_path;	// path to the save_top -- in case it disappears -- again relative to mgr owner
  String	save_data;	// dump-file save from the save_top object (save_top->Save_String)
  taUndoDiffSrcRef diff_src;	// if this is non-null, use it as the source for saving a diff against
  taStringDiffEdits diff_edits;	// edit list for reconstructing original data from diff against diff_src

  void		EncodeMyDiff();

  String	GetData();	// get the data for this save, either by diff or straight data

  TA_SIMPLE_BASEFUNS(taUndoRec);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

class TA_API taUndoRec_List : public taList<taUndoRec> {
  // ##CAT_Undo list of undo records -- managed using circular buffer logic per new functions
INHERITED(taList<taUndoRec>)
public:
  int		st_idx;		// #READ_ONLY index in underlying array where the list starts (i.e., the position of the logical 0 index) -- updated by functions and should not be set manually
  int		length;		// #READ_ONLY logical length of the list -- is controlled by adding and shifting, and should NOT be set manually

  int	CircIdx(int cidx) const
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index

  bool 	CircIdxInRange(int cidx) const { return InRange(CircIdx(cidx)); }
  // #CAT_CircAccess check if logical circular index is in range
  
  taUndoRec*	CircSafeEl(int cidx) const { return SafeEl(CircIdx(cidx)); }
  // #CAT_CircAccess returns element at given logical circular index, or NULL if out of range

  taUndoRec*	CircPeek() const {return SafeEl(CircIdx(length-1));}
  // #CAT_CircAccess returns element at end of circular buffer

  /////////////////////////////////////////////////////////
  //	Special Modify Routines

  void		CircShiftLeft(int nshift)
  { st_idx = CircIdx(nshift); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  void		CircAddExpand(taUndoRec* item) {
    if((st_idx == 0) && (length >= size)) {
      inherited::Add(item); length++; 	// must be building up the list, so add it
    }
    else {
      ReplaceIdx(CircIdx(length++), item);	// expand the buffer length and set to the element at the end
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, expanding the length of the list by 1 under all circumstances

  void		CircAddLimit(taUndoRec* item, int max_length) {
    if(length >= max_length) {
      CircShiftLeft(1 + length - max_length); // make room
      ReplaceIdx(CircIdx(length++), item);	// set to the element at the end
    }
    else {
      CircAddExpand(item);
    }
  }
  // #CAT_CircModify add a new item to the circular buffer, shifting it left if length is at or above max_length to ensure a fixed overall length list (otherwise expanding list up to max_length)

  override void	Reset();

  void 	Copy_(const taUndoRec_List& cp);
  TA_BASEFUNS(taUndoRec_List);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

////////////////////////////////
//	Undo Threading

class UndoDiffThreadMgr;

class TA_API UndoDiffTask : public taTask {
  // threading task for computing the diff on undo save data -- fork this off to save time
INHERITED(taTask)
public:
  override void run();
  // runs specified chunk of computation (encode diff)

  UndoDiffThreadMgr* mgr() { return (UndoDiffThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(UndoDiffTask);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API UndoDiffThreadMgr : public taThreadMgr {
  // #INLINE thread manager for UndoDiff tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  taUndoMgr*	undo_mgr() 	{ return (taUndoMgr*)owner; }

  override void	Run();
  
  TA_BASEFUNS_NOCOPY(UndoDiffThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};

////////////////////////////////
//	Undo Manager

class TA_API taUndoMgr : public taOBase {
  // ##CAT_Undo undo manager -- handles the basic undo functionality
INHERITED(taOBase)
public:
  UndoDiffThreadMgr	diff_threads; // #NO_SAVE #HIDDEN threading support for computing diffs
  taUndoDiffSrc_List	undo_srcs;    // #SHOW_TREE #NO_SAVE diff source records
  taUndoRec_List	undo_recs;    // #SHOW_TREE #NO_SAVE the undo records
  int			cur_undo_idx;	// #READ_ONLY #NO_SAVE logical index into undo record list where the next undo/redo will operate -- actually +1 relative to index to undo -- 0 = no more undos -- goes to the end for each SaveUndo, moves back/forward for Undo/Redo
  int			undo_depth;	// #NO_SAVE how many undo's to keep around
  float			new_src_thr; 	// #NO_SAVE threshold for how big (as a proportion of total file size) the diff's need to get before a new undo source record is created
  bool			save_load_file; // #NO_SAVE save a copy of the file that is loaded during an undo or redo -- file name is "undo_load_file.txt" in cur directory -- useful for debugging issues
  taUndoRec*		rec_to_diff;	// #IGNORE for threading system, rec for diffing


  void		Nest(bool nest);
  // call in pairs, to indicate nested contexts
  virtual bool	SaveUndo(taBase* mod_obj, const String& action, taBase* save_top = NULL,
			 bool force_proj_save = false, taBase* undo_save_owner = NULL);
  // save data for purposes of later being able to undo it -- takes a pointer to object that is being modified, a brief description of the action being performed (e.g., "Edit", "Cut", etc), and the top-level object below which current state information will be saved -- this must be *known to encapsulate all changes* that result from the modification, and also be sufficiently persistent so as to be around when undoing and redoing might be requested -- it defaults to the GetUndoBarrier object or, if that is NULL, the owner of this mgr, which is typically the project (unless force_proj_save is set, in which case it always uses the project).  undo_save_owner is the list/group object where some kind of structural modification is taking place (add, remove, move) that could affect other objects
  virtual void	PurgeUnusedSrcs();
  // remove any undo_srcs that are not currently being used

  virtual bool	Undo();
  // undo the most recent action
  virtual bool	Redo();
  // redo the most recent action that was undone

  virtual int	UndosAvail();
  // return the number of undo actions currently available
  virtual int	RedosAvail();
  // return the number of redo actions currently available

  virtual void	ReportStats(bool show_list = false, bool show_diffs = false);
  // #BUTTON #EDIT_READ_ONLY report (on cout) the current undo statistics in terms of # of records and total amount of ram taken, etc -- if show_list, show full list of current undo info, if show_diffs, then show full diffs of changes from orig source data (requires show_list too)

  TA_SIMPLE_BASEFUNS(taUndoMgr);
protected:
  int			nest_count; // #IGNORE -- +/- before after nesting contexts
  int			loop_count; // 
  virtual bool	LoadFromRec_impl(taUndoRec* urec);

private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};


/////////////////////////////////////////////////
// 		Project proper

class TA_API taProjVersion : public taOBase {
  // #EDIT_INLINE project version numbering information
INHERITED(taOBase)
public:
  int	major;			// use for incompatible or other major changes
  int	minor;			// use for compatible or minor incremental changes
  int	step;			// use for incremental steps -- automatically incremented during SaveNoteChanges
  
  void		Set(int mj, int mn, int st = 0) { major = mj; minor = mn; step = st; }
  void		SetFromString(String ver); // parse, mj.mn.st
  const String	GetString() 
  { return String(major).cat(".").cat(String(minor)).cat(".").cat(String(step)).cat(".");}

  void		Clear() {major = minor = step = 0;} // reset version info to 0
  bool		GtEq(int mj, int mn, int st = 0); // true if the version is greater than or equal to the indicated version

  TA_SIMPLE_BASEFUNS(taProjVersion);
private:
  void	Initialize() { Clear(); }
  void 	Destroy()    { };
};


class TA_API taLicense : public taOBase {
  // #EDIT_INLINE license information -- attach a license to an object
INHERITED(taOBase)
public:
  taMisc::StdLicense license;	// select a standard license to use, or CUSTOM_LIC to enter custom license text
  String	owner;		// legal name of copyright owner (e.g., Regents of University of xyz) -- will be filled in by license_owner in global preferences/options setting by default for new items
  String	org;		// name of organization that actually created the content (e.g., MyLab at University of xyz) -- defaults to owner if left blank  -- will be filled in by license_org in global preferences/options setting by default for new items
  String	year;		// copyright year (can be a range)
  String	custom;		// #CONDSHOW_ON_license:CUSTOM_LIC custom license text
  String	lic_text;	// #HIDDEN #NO_SAVE full license text -- for view license

  void		ViewLicense();	// #BUTTON view the license text based on current selection
  static String	GetLicenseText(taMisc::StdLicense lic, const String& owner, const String& year, const String& org=""); // get text for given license, given parameters
  
  TA_SIMPLE_BASEFUNS(taLicense);
private:
  void	Initialize();
  void 	Destroy()    { };
};

//////////////////////////////////////
//		taProject

class TA_API taProject : public taFBase {
  // ##FILETYPE_Project ##EXT_proj ##COMPRESS #VIRT_BASE ##DUMP_LOAD_POST ##DEF_NAME_ROOT_Project ##CAT_Project ##UNDO_BARRIER ##EXPAND_DEF_2 Base class for a project object containing all relevant info for a given instance -- all ta GUI-based systems should have one..
INHERITED(taFBase)
public:
  enum	StdLicense {
    GPL2,			// GNU General Public License (GPL), version 2
    GPL3,			// GNU General Public License (GPL), version 3
    BSD,			// Berkeley Software Distribution License
    OSL3,			// Open Software License, version 3
    AFL3,			// Academic Free License, version 3
    MIT,			// MIT License
    APACHE,			// Apache License, version 2.0
    MOZILLA,			// Mozilla Public License 1.1 (MPL)
    RESEARCH,			// Research License based on CU Boulder template
  };

  enum ProjLibs {
    USER_LIB,                   // user's personal library
    SYSTEM_LIB,                 // local system library
    WEB_LIB,                    // web-based library
    SEARCH_LIBS,                // search through the libraries (for loading)
  };
  
  String 		tags;	   // #EDIT_DIALOG list of comma separated tags that indicate the basic function of this project -- should be listed in hierarchical order, with most important/general tags first -- these are used for searching the online project library if this project is uploaded
  taProjVersion 	version; 
  // project version numbering information -- useful for keeping track of changes over time (recorded in change log automatically with SaveNoteChanges)
  taLicense		license; // license for this project -- to be determined by the original creator of the project -- if this is not you, then you must abide by the constraints of the original license, if specified -- do ViewLicense button to see the applicable license information for this project
  taWikiURL		wiki_url; // url to synchronize project to/from wiki -- see taMisc::wiki_url for base url in case of relative location
  taBase_Group		templates; // #HIDDEN templates for new objects -- copy new objects from here
  Doc_Group		docs; // documents, typically linked to other objects
  Wizard_Group    	wizards; // Wizards for automatically configuring simulation objects
  SelectEdit_Group	edits;	// special edit dialogs for selected elements
  DataTable_Group	data;	// data, such as patterns for network input
  taBase_Group		data_proc; // objects that perform data processing operations (functions collected on objects for different kinds of operations)
  Program_Group		programs; // Gui-based programs to run simulations and other processing
  DataViewer_List	viewers; // a list of the viewers that have been made and saved in the project; choose one, right click, and OpenViewer to view if not open
  DataViewer_List	viewers_tmp; // #READ_ONLY #HIDDEN #NO_SAVE temporary viewers (edit dialogs etc -- not saved)
  taUndoMgr		undo_mgr; // #READ_ONLY #HIDDEN #NO_SAVE undo manager

  bool			m_dirty; // #HIDDEN #READ_ONLY #NO_SAVE
  bool			m_no_save; // #HIDDEN #READ_ONLY #NO_SAVE -- flag to prevent double user query on exiting; cleared when undirtying
  String		last_change_desc; // #EXPERT description of the last change made to the project -- used for change log
  String                view_plog;
  // #READ_ONLY #NO_SAVE current view of project log data

  override bool		isDirty() const {return m_dirty;}
  override void 	setDirty(bool value);  //

  static String         GetProjTemplatePath(ProjLibs library);
  // #CAT_ProjTemplates get path to given project template library

  virtual void         	SaveAsTemplate(const String& template_name, const String& desc,
		          const String& tags, ProjLibs library = USER_LIB);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #CAT_Project save the project to given project library -- please add a useful description and set of comma separated tags (especially if this is a new template) so others will know what it is for

  ///////////////////////////////////////////////////////////////////
  //	View/Browser Stuff

  MainWindowViewer*	GetDefaultProjectBrowser();
  // #CAT_Display gets one if there is, else NULL
  MainWindowViewer*	GetDefaultProjectViewer();
  // #CAT_Display gets one if there is, else NULL
  virtual MainWindowViewer* AssertDefaultProjectBrowser(bool auto_opn); 
    // #CAT_Display make sure the default project browser is made, and optionally open it
  virtual void		OpenNewProjectBrowser(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a new browser, either 3-pane or 2-2-pane (tree + viewer)
  virtual void		OpenNewProjectViewer(String proj_browser_name = "(default name)");
    // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_Display open a new 3D viewer (with panels)
  virtual MainWindowViewer* NewProjectBrowser(); 
    // create a new, empty viewer -- note: window not opened yet
  virtual void		RefreshAllViews();
  // #CAT_Display manual refresh of all view information in the project -- equivalent to the View/Refresh (F5 key) menu -- should not be necessary but sometimes comes in handy..
  virtual void		UpdateUi();
  // #CAT_Display manual call to update user interface enabled/disabled settings -- usually done through signals and slots, but this can be useful for non-gui driven changes that might affect enabling
  virtual void		SelectT3ViewTabNo(int tab_idx);
  // activate given 3D view tab, specified by number (0..n-1, left to right) -- see also SelectT3ViewTabName
  virtual void		SelectT3ViewTabName(const String& tab_nm);
  // activate given 3D view tab, specified by the name of the tab -- see also SelectT3ViewTabNo

  ///////////////////////////////////////////////////////////////////
  //	Get new proj objects

  virtual DataTable*	GetNewInputDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.InputData (used for data generation functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*	GetNewOutputDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.OutputData (used for monitoring and logging functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual DataTable*	GetNewAnalysisDataTable(const String& nw_nm="", bool msg=false);
  // #CAT_Data create a new data table in data.AnalysisData (used for various data processing and graphing functions).  nw_nm = name for new table, msg = issue a warning message about the creation of this table
  virtual taBase*	FindMakeNewDataProc(TypeDef* typ, const String& nm);
  // #CAT_Data find existing data processing object of given type, or else make one and give it nm
  virtual SelectEdit* 	FindMakeSelectEdit(const String& seledit_name);
  // #CAT_Edit get select edit object of given name, or make one if not found
  virtual taDoc* 	FindMakeDoc(const String& doc_name, const String& wiki_nm = "",
				    const String& web_url = "");
  // #CAT_Doc get doc document object of given name, or make one if not found -- also set the wiki name and web url if specified

  ///////////////////////////////////////////////////////////////////
  //	misc

  virtual void		UpdateChangeLog();
  // #BUTTON #CAT_File update change log for this project, stored as a ChangeLog item in docs on the project -- you will be prompted to enter a description of recent changes, and the date, user, and file names will be recorded
  virtual void		UndoStats(bool show_list = false, bool show_diffs = false);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_File report to css Console the current undo statistics in terms of # of records and total amount of RAM taken, etc -- if show_list, show full list of current undo info, if show_diffs, then show full diffs of changes from orig source data (requires show_list too)
  virtual void		ReplaceString(const String& srch, const String& repl);
  // #BUTTON #CAT_File replace string in this project -- WARNING: this is potentially highly destructive and dangerous -- it just does a literal string replace for ANYTHING that matches srch in the ENTIRE project file (types, paths, values everything) -- this could end up rendering your project completely brain dead, which you will soon see due to error messages: it converts entire project to a string object, does the replace on that, and then attempts to load the new project string back over the top of the current one -- errors will typically be evident during this load process.  If you know what you're doing, this can be a very convenient way to update types or other such things globally.

  virtual void		SaveRecoverFile();
  // #CAT_File Save a recover file of this project, usually called when a signal is received indicating a crash condition
  virtual void		SaveRecoverFile_strm(ostream& strm);
  // #IGNORE underlying save function to use when saving a recover file -- might want to do something special here
  virtual String	GetAutoFileName(const String& suffix, const String& ftype_ext = ".proj");
  // #CAT_File get a file name to save project to, with suffix but file extension *removed*, based on any existing file name, project name, and type
  virtual bool		AutoSave(bool force = false);
  // #CAT_File called automatically by the wait process -- if enough time has passed or force is true, save current project to an auto save backup file (file name + _autosave)

  override bool		SetFileName(const String& val);
  override int 		Save_strm(ostream& strm, taBase* par=NULL, int indent=0);
  override int		Save(); 
  override int		SaveAs(const String& fname = ""); 
  override int	 	Load(const String& fname="", taBase** loaded_obj_ptr = NULL);
  virtual void		OpenProjectLog();
  // #CAT_File set the system event logging to record to the project name + .plog -- called for SaveAs and Load -- uses project file_name value

  virtual bool		CleanFiles();
  // #BUTTON #CAT_File remove autosave and recover files based on current file_name -- this is done by default after saving a non-recover or autosave version of the project

  virtual void		ViewLicense();
  // #BUTTON view the applicable license information for this project

  virtual int		SaveNoteChanges(); 
  // #CAT_File saves the project to a file using current file name, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving
  virtual int		SaveAsNoteChanges(const String& fname = ""); 
  // #CAT_File Saves object data to a new file -- if fname is empty, it prompts the user, but first prompts for a text note of changes that have been made, which are registered in the ChangeLog document within the project prior to saving

  virtual void		ViewProjLog();
  // #MENU #MENU_ON_View #MENU_CONTEXT #MENU_SEP_BEFORE #BUTTON #CAT_File view the current project log file in internal text viewer display
  virtual void		ViewProjLog_Editor();
  // #MENU #MENU_ON_View #MENU_CONTEXT #CAT_File view the current project log file in an external editor as specified in the preferences

  override void		Dump_Load_post();
  void			OpenViewers(); // open any yet unopen viewers

  void	UpdateAfterEdit();
  virtual void		InitLinks_impl(); // #IGNORE use this instead of InitLinks in subclasses
  virtual void		CutLinks_impl(); // #IGNORE use this instead of CutLinks in subclasses -- you can call this first to nuke the viewers etc. before your own stuff
  TA_BASEFUNS(taProject);
  
protected:
  TimeUsed		auto_save_timer; // #IGNORE timer used for auto saving

  virtual void 		InitLinks_post(); // #IGNORE called after all _impls (not called for undo_loading): assert things like default wizards in here
  void 	CutLinks(); // don't override this -- use _impl instead
  virtual MainWindowViewer* MakeProjectBrowser_impl(); // make a standard viewer for this project type
  override int		GetOwnerEditableState_impl(int mask) const
    {return 0;} // the readonly stops here!
  void 			DoView();
  
private:
  void	Copy_(const taProject& cp);
  void 	InitLinks(); // don't try to use this -- use _impl instead
  void	Initialize();
  void 	Destroy();
};
TA_SMART_PTRS(taProject)

class TA_API CssProject : public taProject {
  // generic project -- can be used in css (has no network dependencies)
INHERITED(taProject)
public:
  TA_BASEFUNS_NOCOPY(CssProject);
private:
  void	Initialize() {}
  void 	Destroy() {}
};

//////////////////////////////////
//      Project Template        //
//////////////////////////////////

class TA_API ProjTemplateEl: public taNBase {
  // #INSTANCE #INLINE #CAT_Project an element in the project template listing
INHERITED(taNBase)
public:
  String        desc;           // #EDIT_DIALOG description of what this project does and when it should be used
  String        tags;           // #EDIT_DIALOG list of comma-separated tags describing function of project
  String_Array  tags_array;     // #HIDDEN #READ_ONLY #NO_SAVE parsed version of the tags list, for internal use to actually operate on the tags
  String        lib_name;       // #EDIT_DIALOG name of template that contains this project
  String        URL;            // #EDIT_DIALOG full URL to find this project -- file: for files
  String        filename;       // #EDIT_DIALOG file name given to this project
  String        date;           // #EDIT_DIALOG last modify date for this project

  virtual taProject* NewProject(Project_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new project based on this template
  virtual bool  LoadProject(taProject* prog);
  // load into given project; true if loaded, false if not

  virtual bool  ParseProjFile(const String& fnm, const String& path);
  // get project information from project template info file name and associated meta data

  virtual void  ParseTags(); // parse list of tags into tags_array

  override String GetDesc() const { return desc; }
  override String GetTypeName() const { return lib_name; }
  // This shows up in chooser instead of ProjTemplateEl!

  TA_SIMPLE_BASEFUNS(ProjTemplateEl);
protected:

private:
  void  Initialize();
  void  Destroy();
};

class TA_API ProjTemplateEl_List : public taList<ProjTemplateEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Project list of project template elements
INHERITED(taList<ProjTemplateEl>)
public:
  TA_SIMPLE_BASEFUNS(ProjTemplateEl_List);
protected:

private:
  void  Initialize();
  void  Destroy() { Reset(); CutLinks(); }
};

class TA_API ProjTemplates: public ProjTemplateEl_List {
  // #INSTANCE #INLINE #CAT_Project the set of project templates available
INHERITED(ProjTemplateEl_List)
public:
  bool                  not_init; // list has not been initialized yet

  void  	FindProjects();         // search paths to find all available projects
  taProject* 	NewProject(ProjTemplateEl* proj_type, Project_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT #NO_SAVE_ARG_VAL create a new project in new_owner of given type -- new_owner is group where project will be created
  taProject* 	NewProjectFmName(const String& proj_nm, Project_Group* new_owner);
  // create a new project (lookup by name) (NULL if not found); new_owner is group where project will be created

  TA_SIMPLE_BASEFUNS(ProjTemplates);
protected:

private:
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

class TA_API Project_Group : public taGroup<taProject> {
  //  ##CAT_Project group of projects
INHERITED(taGroup<taProject>)
friend class taProject;
public:

  static ProjTemplates  proj_templates; // #HIDDEN_TREE library of available projects

  taProject* 	NewFromTemplate(ProjTemplateEl* proj_type);
  // #BUTTON #MENU_CONTEXT #FROM_GROUP_proj_templates #NO_SAVE_ARG_VAL #CAT_Project create a new project from a library of existing project templates
  taProject* 	NewFromTemplateByName(const String& proj_nm);
  // #CAT_Project create a new project from a library of existing project templates, looking up by name (NULL if name not found)

  override int	 	Load_strm(istream& strm, taBase* par=NULL, taBase** loaded_obj_ptr = NULL);
  override int	 	Load(const String& fname="", taBase** loaded_obj_ptr = NULL);

  void	InitLinks();
  TA_BASEFUNS(Project_Group);
private:
  NOCOPY(Project_Group)
  void	Initialize() 		{SetBaseType(&TA_taProject);} // upclassed in pdp
  void 	Destroy()		{ };
};

#ifndef __MAKETA__
// this is currently only needed for Mac applications to get open events from finder
// but could potentially be extended in the future..

# include <QApplication>
class taApplication : public QApplication {
  Q_OBJECT
public:
  taApplication ( int & argc, char ** argv );
protected:
  bool event(QEvent *);
};
#endif

class TA_API taRootBaseAdapter: public QObject {
  // ##IGNORE QObject for dispatching startup routines in event loop
INHERITED(QObject)
friend class taRootBase;
  Q_OBJECT
public:
  taRootBaseAdapter(): QObject(NULL) {}
  ~taRootBaseAdapter() {}
  
protected slots:
  void 	Startup_ProcessArgs();
  void 	Startup_RunStartupScript();
  void 	ConsoleNewStdin(int n_lines); // console got new standard input
#ifdef DMEM_COMPILE
  void 	DMem_SubEventLoop();
#endif
};

class TA_API taRootBase: public taFBase {
  // ##CAT_Project ##EXPAND_DEF_2 base class for the root of the structural hierarchy (root. or . in css / paths)
INHERITED(taFBase)
public:
  static TypeDef*	root_type; // set in Startup_Main
  static taRootBase*	instance();
  
  String		version; 	// #READ_ONLY #SHOW current version number
  taBase_List		templates;	// #NO_SAVE #READ_ONLY objects used as templates -- do not use or mess with these!
  Doc_Group		docs; 		// #NO_SAVE documents, typically linked to other objects
  Wizard_Group		wizards; 	// #NO_SAVE global wizards -- see each project for project-specific wizards
  Project_Group		projects; 	// #NO_SAVE The projects
  DataViewer_List	viewers;	// #NO_SAVE viewer display settings for the root object only
  DataViewer_List	viewers_tmp; 	// #READ_ONLY #HIDDEN #NO_SAVE temporary viewers
  taPlugin_List		plugins; 	// available plugins
  taBase_List		plugin_state; 	// #NO_SAVE #HIDDEN #HIDDEN_TREE state objs of plugins -- created/managed by plugin system; state saved as {name}.state in user data
  taPluginBase_List	plugin_deps; 	// #SHOW_TREE #EXPERT_TREE #NO_SAVE  dynamic list, populated in presave
  taiMimeFactory_List	mime_factories; // #NO_SAVE #HIDDEN_TREE extensible list of mime factories
  ColorScaleSpec_Group 	colorspecs;	// global list of color specs for displaying values in terms of colors
  taBase_List		objs;  		// #SHOW_TREE #EXPERT_TREE #NO_SAVE misc place for app-global objs, usually for system use, ex. tcp server
  String_Array		recent_files; 	// #NO_SHOW recently loaded files
  String_Array		recent_paths; 	// #NO_SHOW recently used paths
  String_Array		sidebar_paths; 	// #NO_SHOW for the file chooser dialog -- sidebar paths that are saved and reloaded
  
  void		OpenRemoteServer(ushort port = 5360);
  // #MENU #MENU_ON_Server Open a Server for remote TCP-based control of this application
  void		CloseRemoteServer();
  // #MENU #MENU_ON_Server Close an open Server, if any
  
  virtual void  Options();
  // edit global settings/parameters (taMisc)
  virtual void	About();
  // #MENU #MENU_ON_Object get information/copyright notice
  virtual void	SaveAll();
  // saves all the projects
  override void	WindowShowHook();
  
  void		AddRecentFile(const String& value, bool no_save = false); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  void		AddRecentPath(const String& value, bool no_save = false); // #IGNORE add this path to the recent list
  
  taBase*	FindGlobalObject(TypeDef* base_type = &TA_taBase, 
    const String& name = _nilString);
    // find an object deriving from base_type, with given name, or any name if blank

  virtual taDoc* FindMakeDoc(const String& doc_name, const String& wiki_nm = "",
			     const String& web_url = "");
  // #CAT_Doc get doc document object of given name, or make one if not found -- also set the wiki name and web url if specified

  virtual void  MonControl(bool on);
  // #MENU #MENU_ON_Object set profile monitoring: on=true = on, else off.  starts out off..
  
  taBase*	GetTemplateInstance(TypeDef* typ);
  // get an instance of the indicated tab type, or NULL if not found
  
  ///////////////////////////////////////////////////////////////////////////////
  //		Startup Code	(in order of calling by Startup_Main)

  static bool	Startup_Main(int& argc, const char* argv[], ta_void_fun ta_init_fun = NULL,
			     TypeDef* root_typ = &TA_taRootBase);
  // #IGNORE this is the main function call to startup the software -- creates a root object of given type, processes args, sets global state vars, starts up css, and opens a browser window on the root object if in gui mode

  static bool	Startup_InitDMem(int& argc, const char* argv[]);
  // #IGNORE init distributed memory (MPI) stuff
  static bool	Startup_ProcessGuiArg(int argc, const char* argv[]);
  // #IGNORE process the -gui/-nogui arg
  static bool	Startup_InitApp(int& argc, const char* argv[]);
  // #IGNORE init application stuff (qapp etc) -- note: twiddles cmdline args
  static bool	Startup_InitArgs(int& argc, const char* argv[]);
  // #IGNORE process args into more usable form
  static bool	Startup_InitTA(ta_void_fun ta_init_fun);
  // #IGNORE basic type-access system intializaton
  static bool	Startup_InitTypes();
  // #IGNORE final init of typedefs
  static bool	Startup_EnumeratePlugins();
  // #IGNORE enumeration of plugins 
  static bool	Startup_LoadPlugins();
  // #IGNORE load and init types of plugins 
  static bool	Startup_InitCss();
  // #IGNORE initialize css script system
  static bool	Startup_InitGui();
  // #IGNORE initialize gui system
  static bool	Startup_InitViewColors();
  // #IGNORE initialize default view colors
  static bool	Startup_ConsoleType();
  // #IGNORE arbitrate type of console, based on user options, and app context
  static bool	Startup_MakeWizards();
  // #IGNORE make the global wizards
  static bool	Startup_InitPlugins();
  // #IGNORE create and restore plugin state, and final initialize
  static bool	Startup_MakeMainWin();
  // #IGNORE open the main window (browser of root object) (returns success)
  static bool	Startup_Console();
  // #IGNORE start the console shell (returns success)
  static bool	Startup_RegisterSigHandler();
  // #IGNORE register signal handler routine (i.e., cleanup routine to save recover file upon crashing)

  static bool	Startup_Run();
  // #IGNORE go ahead and run the main event loop

  // these following two guys are run in the event loop, after Startup_Run
  static bool	Startup_ProcessArgs();
  // #IGNORE process general args
  static bool	Startup_RunStartupScript();
  // #IGNORE process general args

  static void	Cleanup_Main();
  // #IGNORE after init, or running, do final cleanups (called by StartupInit on fail, or Startup_Run)

  virtual void	ConsoleNewStdin(int n_lines);
  // notification that the console has received new input lines

#ifdef DMEM_COMPILE
  static bool 	Run_GuiDMem();
  // #IGNORE run the gui under dmem: requires special code.. 
  //protected:
  static int 	DMem_SubEventLoop();
  // #IGNORE for dmem sub-process (dmem_proc > 0), event-processing loop -- note, called from event loop
  static void	DMem_WaitProc(bool send_stop_to_subs = false);
  // #IGNORE waiting process for dmem_proc = 0, if send_stop_to_subs, sends a stop command to sub procs so they bail out of sub loop
public:
#endif
  	
  bool		CheckAddPluginDep(TypeDef* td); // add a plugin dependency, if this type is a  type defined in a plugin; true if it was
  bool		VerifyHasPlugins(); // check the current plugin_deps w/ loaded plugins, return true if all needed plugins loaded OR user says to continue loading anyway
  void		MakeWizards();
  static void 	SaveRecoverFileHandler(int err = 1);
  // error handling function that saves a recover file when system crashes

  int		SavePluginState(); // save current state for plugins in user data
  int		LoadPluginState(); // load state for plugins from user data
  int	Save();
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(taRootBase)
protected:
  enum StartupMilestones { // #BIT successively marked, so we can shutdown cleanly
    SM_MPI_INIT		= 0x002,
    SM_QAPP_OBJ		= 0x004,
    SM_SOQT_INIT	= 0x008,
    SM_TYPES_INIT	= 0x020,
    SM_ROOT_CREATE	= 0x040,
    SM_APP_OBJ		= 0x080,
    SM_REG_SIG		= 0x100
  };
  
  static int		milestone; // StartupMilestones
  static taMisc::ConsoleType console_type; // #IGNORE 
  static int 		console_options; //#IGNORE taMisc::ConsoleOptions 
  static ContextFlag	in_init; // suppresses spurious Saves
  
  static bool 		Startup_InitTA_AppFolders();  // #IGNORE Share, Plugins
  static bool 		Startup_InitTA_InitUserAppDir();  // #IGNORE once found
  static void		Startup_EnumerateEnginesR(TypeDef* typ);
  override void		UpdateAfterEdit_impl();
  // #IGNORE recursively enumerate from typ 
  bool			AddRecentFile_impl(const String& value); // #IGNORE add this file to the recent list (also adds the path to recent paths)
  bool			AddRecentPath_impl(const String& value); // #IGNORE add this path to the recent list;
  virtual void		AddTemplates(); // called in InitLinks -- extend to add new templates
  virtual void		AddDocs(); // called in InitLinks -- extend to add new docs
  virtual taBase* 	GetTemplateInstance_impl(TypeDef* typ, taBase* base);
  virtual void		MakeWizards_impl();

#ifdef TA_OS_LINUX 
public: // debuggy stuff
  enum FPExceptFlags { // #BITS floating point exception masks -- helps to debug nan issues etc.
   FPE_0		= 0, // #IGNORE
   FPE_INEXACT		= 0x001, // inexact result
   FPE_DIVBYZERO	= 0x002, // division by zero
   FPE_UNDERFLOW	= 0x004, // result not representable due to underflow
   FPE_OVERFLOW		= 0x008, // result not representable due to overflow
   FPE_INVALID		= 0x010, // invalid operation
  };
  
  FPExceptFlags		fpe_enable; // #EXPERT #NO_SAVE (DEBUG builds only) enable floating point exceptions; useful to help locate nan and similar issues
protected:
  int			GetFEFlags(FPExceptFlags fpef); // convert FPE flags to FE flags
  FPExceptFlags		GetFPEFlags(int fef); // convert FE flags to FPE flags
#endif

private:
  SIMPLE_COPY(taRootBase)
  void	Initialize();
  void	Destroy();
};


class TA_API PluginWizard : public taWizard {
  // #STEM_BASE ##CAT_Wizard ##EXT_wiz wizard to create a new Plugin
INHERITED(taWizard)
public:
  enum PluginType { // the type of plugin
    UserPlugin,		// created in your emergent_user/plugins folder and only available for your login on your computer
    SystemPlugin, 	// created in your computer's emergent/plugins folder -- makes plugin available to everyone, but may require Administrator/root access on your system
  };
  
  String		plugin_name;
  // the name, which must be a valid C identifier, and must not cause name clashes with existing classes or loaded plugins (this will be checked during Validate) -- lower case by convention
  String		class_name_prefix;
  // #READ_ONLY #SHOW capitalized version of plugin_name, for classes and similar contexts
  PluginType		plugin_type;
  // the type -- this controls the visibility of the plugin (just you, or everyone on your system) -- on Unix and some Windows systems, you will need administrator rights to install a system plugin	
  bool			validated;
  // #NO_SHOW passed validation test
  bool			created;
  // #NO_SHOW create plugin was run
  String		plugin_location;
  // folder where to create the plugin (folder name should usually be same as plugin_name)
  String		desc;
  // description of the plugin -- what does it do?  the more you can provide here, the better it will be for others who might want to use your plugin..
  String		uniqueId;
  // IMPORTANT: this MUST actually be a unique identifier string -- if two plugins have the same uniqueId, only the last one encountered will be loaded
  String		url;
  // can be a website where the plugin is described, or avail for download, etc
  taVersion		version;
  // current version number of the plugin -- can later be updated by editing the CMakeLists.txt file in the plugin directory
  
  virtual bool		Validate();
  // #BUTTON validate all the provided parameters, prior to making the Plugin
  virtual bool		Create();
  // #BUTTON #CONDEDIT_ON_validated  create the plugin -- must be validated first
  virtual bool		Compile();
  // #BUTTON #CONDEDIT_ON_created  compile the plugin from the existing source code -- must be created first -- does a make and a make install
  virtual bool		Clean();
  // #BUTTON #CONDEDIT_ON_created #CONFIRM remove (clean) the plugin, e.g., to prevent it from being loaded if it is causing problems -- this just removes the compiled library object file and does not remove the source code from the install directory, so it can be regenerated from that later if desired
  virtual bool		Editor();
  // #BUTTON open the plugin file editor to edit plugin source files right here..
  virtual bool		LoadWiz(const String& wiz_file);
  // #BUTTON #FILE_DIALOG_LOAD #FILETYPE_PluginWizard #EXT_wiz load an existing wizard configuration file saved from a prior wizard create step -- do this first to enable the Editor and Compile functions to work if you haven't just Created a new plugin
  virtual bool		SaveWiz();
  // #BUTTON save current wizard configuration info to the PluginWizard.wiz file into the current plugin_location directory (only valid after directory created)

  virtual bool		MakeAllPlugins(bool user_only = true);
  // #BUTTON re-make all the currently-installed plugins (useful e.g., when main source is updated) -- user_only = only make the User plugins, not the System ones.  Can also do this by running emergent --make_all_plugins or --make_all_user_plugins at command line

  virtual bool		ShowWiz();
  // show the wizard

  TA_BASEFUNS_NOCOPY(PluginWizard);
protected:
  String 		src_dir;
  String_PArray 	files;
#ifndef __MAKETA__
  QPointer<iPluginEditor> editor;
#endif

  override void		UpdateAfterEdit_impl();
  override void		CheckThisConfig_impl(bool quiet, bool& ok);

  virtual void		AddTemplatedFiles(bool upgrade_only);
  // populate the file list, for files to have their content templatized
  virtual void		AddCopyFiles(bool upgrade_only);
  // populate the file list, for files merely to copy
  virtual void		CreateDestFile(const String& src_file, 
				       const String& dst_file, bool& ok);	
  virtual void		TemplatizeFile(const String& src_file, 
				       const String& src, String& dst, bool& ok);

private:
  void 	Initialize();
  void 	Destroy()	{ };
};

class TestWizard:  public taWizard {
  // test wizard -- during devel, all Wizards must inherit from this
INHERITED(taWizard)
public:
  int		inty; // #PAGE_0_Start
  float		floaty;
  String	outputy; // #PAGE_1_Output
  
  void		LaunchWizard(); // #BUTTON launch the wizard

  TA_BASEFUNS(TestWizard);
private:
  SIMPLE_COPY(TestWizard);
  void 	Initialize() {inty=0; floaty=1;}
  void 	Destroy()	{ CutLinks(); }
};


#endif

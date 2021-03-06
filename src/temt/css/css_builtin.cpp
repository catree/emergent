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


// builtin functions, constants, etc.

#include <sstream>
#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_builtin.h"
#include "css_ta.h"

#include <int_Matrix>
#include <slice_Matrix>
#include <double_Matrix>
#include <String_Matrix>
#include <Variant_Matrix>

#include <taMisc>
#include <taBaseItr>
#include <MTRnd>
#include <cssMath>
#include <EnumDef>
#include <tabMisc>
#include <taRootBase>

#ifdef TA_GUI
#include "css_qt.h"
//#include "ta_qt.h" // maybe need qtdata and/or qtdialog
#endif

#ifndef __USE_XOPEN
#define __USE_XOPEN	// [I.Noda] added to avoid compiler error
#endif

//nn? #include <unistd.h>
#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#ifdef TA_OS_LINUX
#include <sys/time.h>
#endif
#ifdef TA_OS_MAC
#include <sys/time.h>
#endif

#if (defined(TA_OS_WIN))
#include <complex>
#endif

#include <css_misc_funs.h>

#ifndef CSS_NUMBER
#include "css_parse.h"
#endif


using namespace std;

cssElCFun*	cssBI::asgn=NULL;
cssElCFun*	cssBI::asgn_add=NULL;
cssElCFun*	cssBI::asgn_sub=NULL;
cssElCFun*	cssBI::asgn_mult=NULL;
cssElCFun*	cssBI::asgn_div=NULL;
cssElCFun*	cssBI::asgn_mod=NULL;
cssElCFun*	cssBI::asgn_lshift=NULL;
cssElCFun*	cssBI::asgn_rshift=NULL;
cssElCFun*	cssBI::asgn_and=NULL;
cssElCFun*	cssBI::asgn_xor=NULL;
cssElCFun*	cssBI::asgn_or=NULL;
cssElCFun*	cssBI::init_asgn=NULL;
cssElCFun*	cssBI::asgn_post_pp=NULL;
cssElCFun*	cssBI::asgn_pre_pp=NULL;
cssElCFun*	cssBI::asgn_post_mm=NULL;
cssElCFun*	cssBI::asgn_pre_mm=NULL;
cssElCFun*	cssBI::new_opr=NULL;
cssElCFun*	cssBI::del_opr=NULL;
cssElCFun*	cssBI::constr=NULL;
cssElCFun*	cssBI::add=NULL;
cssElCFun*	cssBI::sub=NULL;
cssElCFun*	cssBI::mul=NULL;
cssElCFun*	cssBI::div=NULL;
cssElCFun*	cssBI::modulo=NULL;
cssElCFun*	cssBI::lshift=NULL;
cssElCFun*	cssBI::rshift=NULL;
cssElCFun*	cssBI::bit_and=NULL;
cssElCFun*	cssBI::bit_xor=NULL;
cssElCFun*	cssBI::bit_or=NULL;
cssElCFun*	cssBI::neg=NULL;
cssElCFun*	cssBI::addr_of=NULL;
cssElCFun*	cssBI::de_ptr=NULL;
cssElCFun*	cssBI::de_array=NULL;
cssElCFun*	cssBI::make_matrix=NULL;
cssElCFun*	cssBI::points_at=NULL;
cssElCFun*	cssBI::member_fun=NULL;
cssElCFun*	cssBI::member_call=NULL;
cssElCFun*	cssBI::scoper=NULL;
cssElCFun*	cssBI::pop=NULL;
cssElCFun*	cssBI::cast=NULL;
cssElCFun*	cssBI::cond=NULL;
cssElCFun*	cssBI::switch_jump=NULL;
cssElCFun*	cssBI::doloop=NULL;
cssElCFun*	cssBI::foreach_cond=NULL;

cssElCFun*	cssBI::push_root=NULL;	// pushes a root value on stack
cssElCFun*	cssBI::push_next=NULL; // pushes next program item on stack
cssElCFun*	cssBI::push_cur_this=NULL; // push current this pointer on stack
cssElCFun*	cssBI::arg_swap=NULL; // swaps args
cssElCFun*	cssBI::fun_done=NULL; // swaps args
cssElCFun*	cssBI::array_alloc=NULL;
cssElCFun*	cssBI::sstream_rewind=NULL;

cssElCFun*	cssBI::gt=NULL;
cssElCFun*	cssBI::lt=NULL;
cssElCFun*	cssBI::eq=NULL;
cssElCFun*	cssBI::ge=NULL;
cssElCFun*	cssBI::le=NULL;
cssElCFun*	cssBI::ne=NULL;
cssElCFun*	cssBI::land=NULL;
cssElCFun*	cssBI::lor=NULL;
cssElCFun*	cssBI::lnot=NULL;
cssElCFun*	cssBI::bitneg=NULL;
cssElCFun*	cssBI::power=NULL;

cssInt*		cssBI::true_int=NULL;
cssInt*		cssBI::false_int=NULL;
cssConstBool*	cssBI::semicolon_mark=NULL;
cssConstBool*	cssBI::colon_mark=NULL;
cssConstBool*	cssBI::comma_mark=NULL;

cssTA_Base*	cssBI::root;		// root script element
cssTA_Matrix*	cssBI::matrix_inst;


//////////////////////////////////
// 	Internal Functions	//
//////////////////////////////////

// assign statements
static cssEl* cssElCFun_asgn_stub(int, cssEl* arg[]) {
  *(arg[1]) = *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_init_asgn_stub(int, cssEl* arg[]) {
  (arg[2])->InitAssign(*(arg[1]));
  return arg[2];
}
static cssEl* cssElCFun_asgn_add_stub(int, cssEl* arg[]) {
  *(arg[1]) += *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_sub_stub(int, cssEl* arg[]) {
  *(arg[1]) -= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_mult_stub(int, cssEl* arg[]) {
  *(arg[1]) *= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_div_stub(int, cssEl* arg[]) {
  if(!(arg[2]->IsTaMatrix())) { // let ta matrix guys do their own thing
    if((double)*(arg[2]) == 0.0) { // check at highest res -- new conversion rules in place
      cssProg* cp = arg[0]->prog;
      cssMisc::Warning(cp, "Floating Point Exception: Division by Zero");
      return arg[1];
    }
  }
  *(arg[1]) /= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_mod_stub(int, cssEl* arg[]) {
  if((double)*(arg[2]) == 0.0) {
    cssProg* cp = arg[0]->prog;
    cssMisc::Warning(cp, "Floating Point Exception: Division by Zero");
    return arg[1];
  }
  *(arg[1]) %= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_lshift_stub(int, cssEl* arg[]) {
  *(arg[1]) <<= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_rshift_stub(int, cssEl* arg[]) {
  *(arg[1]) >>= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_and_stub(int, cssEl* arg[]) {
  *(arg[1]) &= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_xor_stub(int, cssEl* arg[]) {
  *(arg[1]) ^= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_or_stub(int, cssEl* arg[]) {
  *(arg[1]) |= *(arg[2]);
  return arg[1];
}
static cssEl* cssElCFun_asgn_post_pp_stub(int, cssEl* arg[]) {
  cssEl* tmp;
  if(arg[1]->IsRef())
    tmp = arg[1]->GetActualObj()->AnonClone(); // save previous value
  else
    tmp = arg[1]->AnonClone(); // save previous value
  *(arg[1]) += *cssBI::true_int;	// an int value of 1
  return tmp;
}
static cssEl* cssElCFun_asgn_pre_pp_stub(int, cssEl* arg[]) {
  *(arg[1]) += *cssBI::true_int;	// an int value of 1
  return arg[1];
}
static cssEl* cssElCFun_asgn_post_mm_stub(int, cssEl* arg[]) {
  cssEl* tmp;
  if(arg[1]->IsRef())
    tmp = arg[1]->GetActualObj()->AnonClone(); // save previous value
  else
    tmp = arg[1]->AnonClone(); // save previous value
  *(arg[1]) -= *cssBI::true_int;	// an int value of 1
  return tmp;
}
static cssEl* cssElCFun_asgn_pre_mm_stub(int, cssEl* arg[]) {
  *(arg[1]) -= *cssBI::true_int;	// an int value of 1
  return arg[1];
}
// arg 1 is conditional, 2 is true, 3 is false
static cssEl* cssElCFun_cond_stub(int, cssEl* arg[]) {
  if((int)*arg[1])
    return arg[2];
  else
    return arg[3];
}

// if na=2 arg 1 is size, arg 2 is type, else arg 1 is type
static cssEl* cssElCFun_new_opr_stub(int na, cssEl* arg[]) {
  if (na == 2) {
    // allocate an array
    cssArray* tmp = new cssArray((int)(*arg[1]), arg[2]);
    return tmp;
  }
  else return arg[1]->NewOpr();
}
static cssEl* cssElCFun_del_opr_stub(int, cssEl* arg[]) {
  arg[1]->DelOpr();
  return &cssMisc::Void;
}

static cssEl* cssElCFun_constr_stub(int, cssEl* arg[]) {
  if(arg[1]->GetType() == cssEl::T_Class)
    ((cssClassInst*)arg[1]->GetNonRefObj())->ConstructToken();
  return &cssMisc::Void;
}

// cast operator
static cssEl* cssElCFun_cast_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  (arg[2])->MakeTempToken(cp);		// make the thing
  cssEl* rval = cp->Stack()->Pop(); 	// result is ptr to token
  rval->CastFm(*(arg[1]));
  return rval;
}

// push_next
static cssEl* cssElCFun_push_next_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cp->Frame()->pc++;
  if(cp->PC() < cp->size)
    return cp->insts[cp->PC()]->inst.El();
  else
    return &cssMisc::Void;
}

// pointer operators
static cssEl* cssElCFun_addr_of_stub(int, cssEl* arg[]) {
  return new cssPtr(arg[1]->GetAddr()); // pointers inside css are always handled by cssPtr
}
static cssEl* cssElCFun_de_ptr_stub(int, cssEl* arg[]) {
  return *(*arg[1]);		// calls operator* on object
}

static cssEl* cssElCFun_de_array_stub(int, cssEl* arg[]) {
  return (*arg[1])[*arg[2]];
}

static cssEl* cssElCFun_make_matrix_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(na == 0) {			// null = empty int matrix -- for coords
    return new cssTA_Matrix(new int_Matrix);
  }

  // first see what we've got and do some basic optimizations
  int n_int = 0;
  int n_real = 0;
  int n_string = 0;
  int n_matrix = 0;
  int n_semicolons = 0;
  int sc_ctr = 0;	// count up since semicolon
  int sc_geom = 0;	// semicolon geometry -- number of items per ;
  int n_colons = 0;
  int n_commas = 0;
  for(int i=1; i<=na; i++) {
    cssEl* aobj = arg[i]->GetActualObj();
    if(aobj == cssBI::semicolon_mark) {
      if(sc_geom > 0 && sc_geom != sc_ctr+1) {
	cssMisc::Error(cp, "make_matrix: Error in constructing matrix -- number of elements per semicolon is variable -- must be constant!");
	return &cssMisc::Void;
      }
      sc_geom = sc_ctr+1;	// include the ; itself
      sc_ctr = 0;
      n_semicolons++;
      continue;
    }
    if(aobj == cssBI::comma_mark) {
      sc_ctr++;
      n_commas++;	
    }
    else if(aobj == cssBI::colon_mark) {
      n_colons++;	
    }
    else if(aobj->GetPtrType() == cssEl::T_Int || 
	    aobj->GetPtrType() == cssEl::T_Int64 ||
	    aobj->GetPtrType() == cssEl::T_Short ||
	    aobj->GetPtrType() == cssEl::T_Long ||
	    aobj->GetPtrType() == cssEl::T_LongLong ||
	    aobj->GetPtrType() == cssEl::T_Char ||
	    aobj->GetPtrType() == cssEl::T_Enum ||
	    aobj->GetPtrType() == cssEl::T_DynEnum) n_int++;
    else if(aobj->GetPtrType() == cssEl::T_Real ||
	    aobj->GetPtrType() == cssEl::T_Float) n_real++;
    else if(aobj->GetPtrType() == cssEl::T_String) n_string++;
    else if(aobj->IsTaMatrix()) n_matrix++;
  }

  cssTA_Matrix* rval = NULL;
  int mat_dims = n_semicolons + n_commas+1;
  int eff_na = na - n_semicolons - n_colons - n_commas;

  MatrixGeom mg;
  if(n_colons > 0) {
    mg.SetGeom(2, 3, mat_dims);	// always 2d with 3 inner = start:end:step and outer = number of any kind of spec
  }
  else {
    if(sc_geom > 0) {
      int md = mat_dims / sc_geom;	// could double-check divisibilty but not critical
      mg.SetGeom(2, sc_geom, md);
    }
    else {
      mg.SetGeom(1, mat_dims);
    }
  }

  if(n_colons > 0) {
    slice_Matrix* imat = new slice_Matrix;
    rval = new cssTA_Matrix(imat);
    imat->SetGeomN(mg);
    int ses[3] = {0,-1,1};
    int ses_dx = 0;		// 0 = start, 1 = end, 2 = step
    int dim = 0;
    int n_cols = 0;
    for(int i=1; i<=na+1; i++) {
      if(i==na+1 || arg[i] == cssBI::comma_mark) {
	if(n_cols == 0) {
	  ses[1] = ses[0] + 1;	// just make it end one later
	}
	for(int j=0; j<3; j++)
	  imat->FastEl2d(j,dim) = ses[j];
	if(i==na+1) break;	// all done
	ses[0] = 0; ses[1] = -1; ses[2] = 1; ses_dx = 0;
	dim++; n_cols = 0;
	continue;
      }
      if(arg[i] == cssBI::colon_mark) {
	ses_dx++; n_cols++;
      }
      else {
	ses[ses_dx] = (int)*(arg[i]);
      }
    }
  }
  else if(n_real > 0 && n_real + n_int == eff_na) {	// any real promotes all to real
    double_Matrix* imat = new double_Matrix(mg);
    rval = new cssTA_Matrix(imat);
    int c=0;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      imat->Set_Flat((double)*arg[i], c++);
    }
  }
  else if(n_int == eff_na) {	// complete int array
    int_Matrix* imat = new int_Matrix(mg);
    rval = new cssTA_Matrix(imat);
    int c=0;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      imat->Set_Flat((int)*arg[i], c++);
    }
  }
  else if(n_string == eff_na) {	// complete string array
    String_Matrix* imat = new String_Matrix(mg);
    rval = new cssTA_Matrix(imat);
    int c=0;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      imat->Set_Flat(arg[i]->GetStr(), c++);
    }
  }
  else if(n_matrix == 0) {	// variant array
    Variant_Matrix* imat = new Variant_Matrix(mg);
    rval = new cssTA_Matrix(imat);
    int c=0;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      // check for type name and intercept as typedef
      if(arg[i]->name.nonempty()) {
	TypeDef* td = taMisc::FindTypeName(arg[i]->name);
	if(td) {
	  imat->Set_Flat((Variant)td, c++);
	}
	else {
	  imat->Set_Flat(arg[i]->GetVar(), c++);
	}
      }
      else {
	imat->Set_Flat(arg[i]->GetVar(), c++);
      }
    }
  }
  else if(n_matrix == eff_na) {	// complete sub-arrays -- ignores ;'s
    // special case of just one matrix: just return it directly --
    // otherwise adds unnec extra dimension
    if(eff_na == 1) {
      return arg[1];
    }
    TypeDef* td = NULL;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      taMatrix* mat = ((cssTA_Matrix*)arg[i]->GetNonRefObj())->GetMatrixPtr();
      if(!mat) {
	cssMisc::Error(cp, "make_matrix: Error in constructing matrix from sub-matricies: sub matrix is null!");
	return &cssMisc::Void;
      }
      if(i == 1) {
	td = mat->GetTypeDef();
	mg = mat->geom;
      }
      else {
	if(mg != mat->geom) {
	  cssMisc::Error(cp, "make_matrix: Error in constructing matrix from sub-matricies: sub matrix at position:", String(i-1), "has a different geometry:",
	   mat->geom.ToString(),
	   "than previous submatricies in list:", mg.ToString());
	  return &cssMisc::Void;
	}
	if(td != mat->GetTypeDef()) {
	  // todo: alternatively make variant matrix anyway
	  cssMisc::Error(cp, "make_matrix: Error in constructing matrix from sub-matricies: sub matrix at position:", String(i-1), "has a different type:",
			 mat->GetTypeDef()->name,
			 "than previous submatricies in the list:",
			 td->name,
			 "-- all must be same data type");
	  return &cssMisc::Void;
	}
      }
    }
    mg.AddDim(mat_dims);	// add another dimension with number of items
    taMatrix* tmat = ((cssTA_Matrix*)arg[1]->GetNonRefObj())->GetMatrixPtr();
    taMatrix* rmat = (taMatrix*)tmat->MakeToken();
    rval = new cssTA_Matrix(rmat);
    rmat->SetGeomN(mg);
    int c=0;
    for(int i=1; i<=na; i++) {
      if(arg[i] == cssBI::semicolon_mark || arg[i] == cssBI::comma_mark) continue;
      taMatrix* mat = ((cssTA_Matrix*)arg[i]->GetNonRefObj())->GetMatrixPtr();
      rmat->CopyFrame(*mat, c++);
    }
  }

  return rval;
}

static cssEl* cssElCFun_points_at_stub(int, cssEl* arg[]) {
  if(arg[2]->GetType() == cssEl::T_String)
    return (arg[1])->GetMemberFmName(arg[2]->GetStr());
  else
    return (arg[1])->GetMemberFmNo((int)*arg[2]);
}
static cssEl* cssElCFun_scoper_stub(int, cssEl* arg[]) {
  return (arg[1])->GetScoped(*arg[2]);
}

static cssEl* cssElCFun_member_fun_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cssEl* mbfun;
  if(arg[2]->GetType() == cssEl::T_String)
    mbfun = (arg[1])->GetMethodFmName(arg[2]->GetStr());
  else
    mbfun = (arg[1])->GetMethodFmNo((int)*arg[2]);
  if((mbfun->GetType() != cssEl::T_MbrCFun) && (mbfun->GetType() != cssEl::T_MbrScriptFun)) {
    if(mbfun != &cssMisc::Void)
      cssEl::Done((cssEl*)mbfun);
    cssMisc::Error(cp, "Member is not a function", mbfun->name);
    return &cssMisc::Void;
  }
  // find the member_call, pointed to by the jump statement just after this guy.
  cssInst* mbr_jmp = cp->insts[cp->PC()];
  if(!mbr_jmp->IsJump()) {
    cssMisc::Error(cp, "Internal error: member_fun didn't find jump stmt to find member_call");
    return &cssMisc::Void;
  }
  css_progdx mbr_call_dx = mbr_jmp->GetJump();
  if(cp->size <= mbr_call_dx) {
    cssMisc::Error(cp, "Internal error: member_fun didn't find member_call instruction");
    return &cssMisc::Void;
  }

  // code the new instruction
  cp->ReplaceCode(mbr_call_dx, mbfun);

  cp->Frame()->pc++;		// skip over jump!

  // now deal with args
  cp->Stack()->Push(&cssMisc::Void); // push the arg stop
  cssEl* ths = arg[1]->GetActualObj(); // get past references and pointers
  if(mbfun->GetType() == cssEl::T_MbrScriptFun) {	// push the 'this' pointer
    if(ths->GetType() == cssEl::T_Class)
      cp->Stack()->Push(ths);
    else {
      if(cp->CurThis() == NULL) {
	cssMisc::Error(cp, "No 'this' pointer for member function call");
	return &cssMisc::Void;
      }
      cp->Stack()->Push(cp->CurThis());
    }
  }
  // push the 'this' pointer for builtin functions
  else if(mbfun->GetType() == cssEl::T_MbrCFun) {
    // just always push on stack because otherwise the this gets nuked!!!
//     if(ths->GetType() == cssEl::T_Class)
    cp->Stack()->Push(ths);
  }
  if(cp->top->debug >= 2) {
    taMisc::Info("\nCalling member function:", mbfun->name);
  }
  return &cssMisc::Void;
}

static cssEl* cssElCFun_member_call_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cssMisc::Error(cp, "member_call: This function should have been replaced by actual member function!");
  return &cssMisc::Void;
}

static cssEl* cssElCFun_call_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(na < 2) {
    cssMisc::Error(cp, "call: at least 2 arguments are required: object and method name");
    return &cssMisc::Void;
  }
  taBase* obj = (taBase*)arg[1]->GetVoidPtrOfType(&TA_taBase);
  if(!obj) {
    cssMisc::Error(cp, "call: object argument is NULL");
    return &cssMisc::Void;
  }
  String fun_name = arg[2]->GetStr();
  cssEl** param = (cssEl**)calloc(na, sizeof(cssEl*));
  param[0] = &cssMisc::Void; // method*
  param[1] = &cssMisc::Void; // this*
  for(int i = 3; i<= na; i++) {
    param[i-1] = arg[i];
  }

  MethodDef* md = obj->GetTypeDef()->methods.FindName(fun_name);
  if(!md || !md->stubp) {
    cssMisc::Error(cp, "call: function:", fun_name,
		   "not found or stub function is NULL on object of type:",
		   obj->GetTypeDef()->name);
    free(param);
    return &cssMisc::Void;
  }

  if(md->fun_argd < 0) {	// with default args it does right thing
    if(na-2 != md->fun_argc) {
      cssMisc::Error(cp, "call: incorrect number of arguments provided to function:", 
		     fun_name,
		     "should have:", String(md->fun_argc), "received:", String(na-2));
      free(param);
      return &cssMisc::Void;
    }
  }

  cssEl* rval = (*(md->stubp))(obj, na-1, param);
  free(param);
  return rval;
}

static cssEl* cssElCFun_set_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  taBase* obj = (taBase*)arg[1]->GetVoidPtrOfType(&TA_taBase);
  if(!obj) {
    cssMisc::Error(cp, "call: object argument is NULL");
    return &cssMisc::Void;
  }
  String memb_name = arg[2]->GetStr();
  Variant val = arg[3]->GetVar();
  taBase::SetMemberVar(obj, memb_name, val);
  return arg[1];		// return val is always obj itself
}

// arg[1]..[na-1] = sizes,   arg[na] = array
static cssEl* cssElCFun_array_alloc_stub(int na, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  if(arg[na]->GetType() != cssEl::T_Array &&
     arg[na]->GetType() != cssEl::T_ArrayType) {
    cssMisc::Error(arg[0]->prog, "Array alloc did not get an array");
    rval->val = 0;
    return rval;
  }
  cssArray* tmp = (cssArray*) arg[na];
  rval->val = tmp->AllocAll(na-2, arg+1);
  return rval;
}

static cssEl* cssElCFun_add_stub(int, cssEl* arg[]) {
  return *(arg[1]) + *(arg[2]);
}
static cssEl* cssElCFun_sub_stub(int, cssEl* arg[]) {
  return *(arg[1]) - *(arg[2]);
}
static cssEl* cssElCFun_mul_stub(int, cssEl* arg[]) {
  return *(arg[1]) * *(arg[2]);
}
static cssEl* cssElCFun_div_stub(int, cssEl* arg[]) {
  if(!arg[2]->IsTaMatrix()) {
    if((double)*(arg[2]) == 0.0) {
      cssProg* cp = arg[0]->prog;
      cssMisc::Warning(cp, "Floating Point Exception: Division by Zero");
      return arg[2];
    }
  }
  return *(arg[1]) / *(arg[2]);
}
static cssEl* cssElCFun_modulo_stub(int, cssEl* arg[]) {
  if((double)*(arg[2]) == 0.0) {
    cssProg* cp = arg[0]->prog;
    cssMisc::Warning(cp, "Floating Point Exception: Division by Zero");
    return arg[2];
  }
  return *(arg[1]) % *(arg[2]);
}
static cssEl* cssElCFun_lshift_stub(int, cssEl* arg[]) {
  if((arg[1]->GetType() == cssEl::T_TA) && (arg[2]->GetType() == cssEl::T_Bool)) {
    cssTA* ta = (cssTA*)arg[1]->GetNonRefObj();
    if(ta->type_def->InheritsFrom(TA_ostream)) {
      ostream* strm = (ostream*)ta->GetVoidPtrOfType(&TA_ostream);
      if(strm != NULL) {
	bool conout = false;
	if(ta->name == "cout" || ta->name == "cerr") {
	  conout = true;
	}
	if(arg[2]->name == "flush") {
	  if(conout)
	    taMisc::ConsoleOutputChars("\n");
	  else
	    *strm << flush;
	  return arg[1];
	}
	if(arg[2]->name == "endl") {
	  if(conout)
	    taMisc::ConsoleOutputChars("\n");
	  else
	    *strm << endl;
	  return arg[1];
	}
	if(arg[2]->name == "ends") {
	  if(!conout)
	    *strm << ends;  return arg[1];
	}
	if(arg[2]->name == "dec") {
	  *strm << dec;  return arg[1];
	}
	if(arg[2]->name == "hex") {
	  *strm << hex;  return arg[1];
	}
	if(arg[2]->name == "oct") {
	  *strm << oct;	  return arg[1];
	}
      }
    }
  }
  return *(arg[1]) << *(arg[2]);
}
static cssEl* cssElCFun_rshift_stub(int, cssEl* arg[]) {
  if((arg[1]->GetType() == cssEl::T_TA) && (arg[2]->GetType() == cssEl::T_Bool)) {
    cssTA* ta = (cssTA*)arg[1]->GetNonRefObj();
    if(ta->type_def->InheritsFrom(TA_istream)) {
      istream* strm = (istream*)ta->GetVoidPtrOfType(&TA_istream);
      if(strm != NULL) {
	if(arg[2]->name == "ws") {
	  *strm >> ws;	  return arg[1];
	}
	if(arg[2]->name == "dec") {
	  *strm >> dec;	  return arg[1];
	}
	if(arg[2]->name == "hex") {
	  *strm >> hex;	  return arg[1];
	}
	if(arg[2]->name == "oct") {
	  *strm >> oct;	  return arg[1];
	}
      }
    }
  }
  return *(arg[1]) >> *(arg[2]);
}
static cssEl* cssElCFun_bit_and_stub(int, cssEl* arg[]) {
  return *(arg[1]) & *(arg[2]);
}
static cssEl* cssElCFun_bit_xor_stub(int, cssEl* arg[]) {
  return *(arg[1]) ^ *(arg[2]);
}
static cssEl* cssElCFun_bit_or_stub(int, cssEl* arg[]) {
  return *(arg[1]) | *(arg[2]);
}
static cssEl* cssElCFun_neg_stub(int, cssEl* arg[]) {
  return -(*(arg[1]));
}
static cssEl* cssElCFun_bitneg_stub(int, cssEl* arg[]) {
  return ~(*(arg[1]));
}

static cssEl* cssElCFun_sstream_rewind_stub(int, cssEl* arg[]) {
  std::stringstream* strm = (std::stringstream*)(void*)*(arg[1]);
  strm->clear();
  strm->seekg(0, ios::beg);
  return arg[1];
}

static cssEl* cssElCFun_pop_stub(int, cssEl**) {
  return &cssMisc::Void;		// the args have the stack contents, will be doneArgs
}
static cssEl* cssElCFun_nop_stub(int, cssEl**) {
  return &cssMisc::Void;
}
static cssEl* cssElCFun_arg_swap_stub(int na, cssEl* arg[]) {
  if(na != 2)
    return &cssMisc::Void;
  cssProg* cp = arg[0]->prog;
  cp->Stack()->Push(arg[2]);	// push this on the stack
  return arg[1];		// and this goes after it
}
static cssEl* cssElCFun_fun_done_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cssEl* fun_el = cp->insts[cp->PC()-2]->inst.El(); // 
  taMisc::Info("fun_done: pc: ", String(cp->PC()-2), " el: ", fun_el->name);
  fun_el->FunDone(cp);		// call the fun done
  return &cssMisc::Void;	// todo: not sure if this is correct
}

static cssEl* cssElCFun_switch_jump_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;

  cssElPtr ptr = cp->literals.FindName(cssSwitchJump_Name);
  cssArray* val_ary = (cssArray*)ptr.El();

  cssEl* sval = arg[1];		// first arg

  if((val_ary == &cssMisc::Void) || (sval == &cssMisc::Void)) {
    cssMisc::Error(cp, "case statement not in a valid switch block");
    ((cssElFun*)arg[0])->dostat = cssEl::Breaking; // bail
    return &cssMisc::Void;
  }

  ptr = val_ary->items->FindName(sval->GetStr());
  cssEl* jmp_to = ptr.El();
  if(jmp_to == &cssMisc::Void) { // not found
    ptr = val_ary->items->FindName(cssSwitchDefault_Name);
    jmp_to = ptr.El();
    if(jmp_to == &cssMisc::Void) {
      ((cssElFun*)arg[0])->dostat = cssEl::Breaking; // bail, no other case found
      return &cssMisc::Void;
    }
  }

  cp->SetPC((int)*jmp_to);	// go to that line
  return &cssMisc::Void;
}

static cssEl* cssElCFun_doloop_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cssEl* cond = cp->top->Prog()->Stack()->Pop();
  if((int)*cond != 0)
    cp->SetPC(0);			// restart at top
  return &cssMisc::Void;
}

static cssEl* cssElCFun_foreach_cond_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  taBase* list = NULL;
  bool tmp_list = false;        // delete list at end
  if(arg[1]->GetPtrType() == cssEl::T_String) {
    String val = arg[1]->GetStr();
    list = val.to_array();
    tmp_list = true;
  }
  else {
    list = (taBase*)arg[1]->GetVoidPtrOfType(&TA_taBase);
  }
  if(!list) {
    cssMisc::Error(cp, "foreach cond: did not get a list object to iterate over");
    return cssBI::false_int;	// no more to go
  }
  cssEl* itre = arg[2];
  if(itre->GetType() != cssEl::T_TA) {
    cssMisc::Error(cp, "foreach cond: iterator is not a taBase*");
    return cssBI::false_int;	// no more to go
  }
  cssTA_Base* itrc = (cssTA_Base*)itre;
  cssEl* var = arg[3];
  taBaseItr* FOREACH_itr = NULL;
  Variant val;
  if(!itrc->ptr) {
    if(!list->IsContainer()) {	// we got one single item
      // todo: problem here: this prevents detecting incorrect foreach exprs
      val = list;		// it is the value
      FOREACH_itr = new taBaseItr; // create a tmp dummy!
      *itrc = FOREACH_itr;		// set pointer
    }
    else {
      if(list->InheritsFrom(&TA_slice_Matrix)) { // must expand each time!
	list = ((slice_Matrix*)list)->Expand();
        tmp_list = true;        // is now tmp!
      }
      FOREACH_itr = new taBaseItr; // create a tmp dummy!
      *itrc = FOREACH_itr;		// set pointer
      val = list->IterFirst(*FOREACH_itr); // variant assign
    }
  }
  else {
    FOREACH_itr = (taBaseItr*)itrc->GetVoidPtrOfType(&TA_taBaseItr);
    if(!list->IsContainer()) {	// we got one single item
      FOREACH_itr = NULL;
      *itrc = FOREACH_itr;             // free ptr
    }
    else {
      if(list->InheritsFrom(&TA_slice_Matrix)) { // must expand each time!
	list = ((slice_Matrix*)list)->Expand();
        tmp_list = true;        // is tmp now!
      }
      val = list->IterNext(*FOREACH_itr); // variant assign
    }
  }
  if(FOREACH_itr) {
    if(FOREACH_itr->Done()) {
      FOREACH_itr = NULL;
      *itrc = FOREACH_itr;             // free ptr
    }
    else {
      *var = val;
      if(tmp_list)
        delete list;
      return cssBI::true_int;	// more to go
    }
  }
  if(tmp_list)
    delete list;
  return cssBI::false_int;	// no more to go
}

static cssEl* cssElCFun_return_stub(int na, cssEl* arg[]) {
  if(na > 0) {
    cssProg* cp = arg[0]->prog;
    cssScriptFun* sf = cp->top->GetCurrentFun();
    if(sf == NULL) {
      cssMisc::Error(cp, "Could not find current function scope for return value!");
      return &cssMisc::Void;
    }
    else {
      cssEl* rval = sf->argv[0].El();
      if(rval == &cssMisc::Void)
	cssMisc::Error(cp, "Attempt to return a value in a void function");
      rval->ArgCopy(*(arg[1]));	// set ref
    }
  }
  ((cssElFun*)arg[0])->dostat = cssEl::Returning;
  return &cssMisc::Void;
}
static cssEl* cssElCFun_break_stub(int, cssEl* arg[]) {
  ((cssElFun*)arg[0])->dostat = cssEl::Breaking;
  return &cssMisc::Void;
}
static cssEl* cssElCFun_continue_stub(int, cssEl* arg[]) {
  ((cssElFun*)arg[0])->dostat = cssEl::Continuing;
  return &cssMisc::Void;
}

static cssEl* cssElCFun_gt_stub(int, cssEl* arg[]) {
  return (*(arg[1]) > *(arg[2]));
}
static cssEl* cssElCFun_lt_stub(int, cssEl* arg[]) {
  return (*(arg[1]) < *(arg[2]));
}
static cssEl* cssElCFun_eq_stub(int, cssEl* arg[]) {
  return (*(arg[1]) == *(arg[2]));
}
static cssEl* cssElCFun_ge_stub(int, cssEl* arg[]) {
  return (*(arg[1]) >= *(arg[2]));
}
static cssEl* cssElCFun_le_stub(int, cssEl* arg[]) {
  return (*(arg[1]) <= *(arg[2]));
}
static cssEl* cssElCFun_ne_stub(int, cssEl* arg[]) {
  return (*(arg[1]) != *(arg[2]));
}
static cssEl* cssElCFun_land_stub(int, cssEl* arg[]) {
  return (*(arg[1]) && *(arg[2]));
}
static cssEl* cssElCFun_lor_stub(int, cssEl* arg[]) {
  return (*(arg[1]) || *(arg[2]));
}
static cssEl* cssElCFun_lnot_stub(int, cssEl* arg[]) {
  return !(*(arg[1]));
}

static cssEl* cssElCFun_push_root_stub(int, cssEl**) {
  return cssBI::root;
}

static cssEl* cssElCFun_push_cur_this_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->CurThis() == NULL) {
    // find this as hard-coded variable:
    cssElPtr ths = cp->top->hard_vars.FindName("this");
    if(ths.NotNull()) {
      return ths.El();
    }
    else {
      cssMisc::Error(cp, "No current 'this' pointer");
      return &cssMisc::Void;
    }
  }
  return cp->CurThis();
}

// pre-processor

static cssEl* cssElCFun_include_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  cp->top->SetInclude(arg[1]->GetStr());
  return &cssMisc::Void;
}

//////////////////////////////////
//	Install_Internals	//
//////////////////////////////////

static void Install_Internals() {
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, init_asgn, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_add, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_sub, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_mult, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_div, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_mod, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_lshift, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_rshift, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_and, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_xor, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_or, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_post_pp, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_pre_pp, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_post_mm, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, asgn_pre_mm, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, cond, 3, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, cast, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, add, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, sub, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, mul, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, div, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, modulo, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, lshift, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, rshift, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, bit_and, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, bit_xor, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, bit_or, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, neg, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, bitneg, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, addr_of, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, de_ptr, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, de_array, 2, CSS_FUN);
  cssElCFun_inst_ptr(cssMisc::Internal, make_matrix, cssEl::VarArg, CSS_FUN, " ");
  cssElInCFun_inst_ptr(cssMisc::Internal, points_at, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, scoper, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, member_fun, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, member_call, 0, CSS_FUN);
  cssElCFun_inst_ptr(cssMisc::Internal, pop, cssEl::VarArg, CSS_FUN, " ");
  cssElCFun_inst_ptr(cssMisc::Internal, array_alloc, cssEl::VarArg, CSS_FUN, " ");
  cssElInCFun_inst_ptr(cssMisc::Internal, push_root, cssEl::NoArg, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, push_next, cssEl::NoArg, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, push_cur_this, cssEl::NoArg, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, arg_swap, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, fun_done, cssEl::NoArg, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, sstream_rewind, 1, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, switch_jump, 1, CSS_FUN);

  cssElInCFun_inst_ptr(cssMisc::Internal, gt, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, lt, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, eq, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, ge, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, le, 2, CSS_FUN);
  cssElInCFun_inst_ptr(cssMisc::Internal, ne, 2, CSS_FUN);
  cssElInCFun_inst_ptr_nm(cssMisc::Internal, land, 2, "and", CSS_FUN);
  cssElInCFun_inst_ptr_nm(cssMisc::Internal, lor, 2, "or", CSS_FUN);
  cssElInCFun_inst_ptr_nm(cssMisc::Internal, lnot, 1, "not", CSS_FUN);

  // internal functions
  cssElCFun_inst	(cssMisc::Parse, return, cssEl::VarArg, CSS_RETURN, " ");
  cssElInCFun_inst	(cssMisc::Parse, break, cssEl::NoArg, CSS_BREAK);
  cssElInCFun_inst	(cssMisc::Parse, continue, cssEl::NoArg, CSS_CONTINUE);
  cssElCFun_inst_nm     (cssMisc::Parse, new_opr, cssEl::VarArg, "new", CSS_NEW, " ");
  cssElCFun_inst_nm     (cssMisc::Parse, del_opr, 1, "delete", CSS_DELETE, " ");
  cssElInCFun_inst_ptr	(cssMisc::Parse, constr, 1, CSS_FUN);

  cssElCFun_inst_nm     (cssMisc::Parse, new_opr, cssEl::VarArg, "new", CSS_NEW, " ");

  cssElCFun_inst_flg(cssMisc::Functions, call, cssEl::VarArg, CSS_FUN,
		     "call(obj, \"meth_name\", [args]): call the given method on given object(s), with given arguments to that function -- advantage over a direct method call is that it works even if obj is a list of objects, in which case it calls method on each in turn, and return value is a Variant_Matrix of all the return values.",
		     cssElFun::FUN_ITR_LIST, 1);
  cssElCFun_inst_flg(cssMisc::Functions, set, 3, CSS_FUN,
		     "set(obj, \"member_name\", value): set the given member on given object(s), with given value (can be any kind of expression resulting in value) -- advantage over a directly setting the member is that it works even if obj is a list of objects, in which case it sets member value on each in turn.",
		     cssElFun::FUN_ITR_LIST, 1);

  cssElInCFun_inst_ptr	(cssMisc::Internal, doloop, cssEl::NoArg, CSS_FUN);
  cssElInCFun_inst_ptr	(cssMisc::Internal, foreach_cond, 3, CSS_FUN);

  // stuff just for parsing
  cssElCFun_inst_nm	(cssMisc::Parse, nop,   cssEl::NoArg, "switch", CSS_SWITCH, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "if", CSS_IF, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "else", CSS_ELSE, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "while", CSS_WHILE, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "do", CSS_DO, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "for", CSS_FOR, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "foreach", CSS_FOREACH, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "in", CSS_IN, " ");

  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "class", CSS_CLASS, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "struct", CSS_CLASS, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "union", CSS_CLASS, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "enum", CSS_ENUM, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "extern", CSS_EXTERN, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "static", CSS_STATIC, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "private", CSS_PRIVATE, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "public", CSS_PUBLIC, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "protected", CSS_PROTECTED, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "virtual", CSS_VIRTUAL, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "const", CSS_CONST, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "inline", CSS_INLINE, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "case", CSS_CASE, " ");
  cssElCFun_inst_nm	(cssMisc::Parse, nop, 	cssEl::NoArg, "default", CSS_DEFAULT, " ");

  // pre-processor
  cssElInCFun_inst(cssMisc::PreProcessor, include,  	1, CSS_PP_INCLUDE);
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "define", CSS_PP_DEFINE, " ");
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "undef", CSS_PP_UNDEF, " ");
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "ifdef", CSS_PP_IFDEF, " ");
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "ifndef", CSS_PP_IFNDEF, " ");
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "else", CSS_PP_ELSE, " ");
  cssElCFun_inst_nm(cssMisc::PreProcessor, nop,  	cssEl::NoArg, "endif", CSS_PP_ENDIF, " ");

  cssMisc::Defines.Push(new cssDef(0, "__CSS__"));
}


//////////////////////////
// 	Commands	//
//////////////////////////

static cssEl* cssElCFun_alias_stub(int, cssEl* arg[]) {
  cssElCFun* tmp = (cssElCFun *)((cssRef*)arg[2])->ptr.El();
  cssMisc::Commands.Push(new cssElCFun(tmp->argc, tmp->funp, arg[1]->GetStr(),
				       tmp->parse, ""));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_backtrace_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  String fh;
  if(na > 0)
    csh->src_prog->BackTrace(fh, (int)*arg[1]);
  else
    csh->src_prog->BackTrace(fh);
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}
static cssEl* cssElCFun_chsh_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(na > 0) {
    if(arg[1]->GetType() == cssEl::T_SubShell) {
      csh->PushSrcProg(&(((cssSubShell*)arg[1]->GetNonRefObj())->prog_space));
      return &cssMisc::Void;
    }
    if(arg[1]->GetType() != cssEl::T_TA) {
      cssMisc::Error(cp, "chsh argument is not a pointer to a cssProgSpace");
      return &cssMisc::Void;
    }
    cssTA* ta = (cssTA*)arg[1];
    if(ta->type_def->InheritsFromName("cssProgSpace") && (ta->ptr != NULL)) {
      cssProgSpace* ps = (cssProgSpace*)ta->GetVoidPtr();
      csh->PushSrcProg(ps);
    }
    else {
      cssMisc::Error(cp, "chsh argument is not a valid pointer to a cssProgSpace");
    }
  }
  else {
    csh->PushSrcProg(cp->top);	// shell itself (probably a no-op but anyway)
  }
  return &cssMisc::Void;
}
static cssEl* cssElCFun_clearall_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->ClearAll();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_cont_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  if(na > 0)
    csh->src_prog->Cont((int)*(arg[1]));
  else
    csh->src_prog->Cont();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_goto_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->Cont((int)*(arg[1]));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_debug_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->SetDebug((int)*(arg[1]));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_cmdebug_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  csh->cmd_prog->SetDebug((int)*(arg[1]));
  return &cssMisc::Void;
}
// static cssEl* cssElCFun_define_stub(int, cssEl* arg[]) {
//   cssProg* cp = arg[0]->prog;
//   // todo: this needs to switch the state of the shell to take input into src instead of cmd
// //   cp->top->SetDefn();
//   return &cssMisc::Void;
// }
static cssEl* cssElCFun_delbp_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->DelBreak((int)*(arg[1]));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_delwp_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->DelWatchIdx((int)*(arg[1]));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_edit_stub(int na, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  if(na > 1)
    rval->val = arg[1]->Edit((int)*(arg[2]));
  else if(na == 1)
    rval->val = arg[1]->Edit();
  else
    cssMisc::Error(arg[0]->prog, "Edit requires an argument of the object to be edited");
  return rval;
}

static cssEl* cssElCFun_exit_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->PopSrcProg() == NULL) {
    csh->Exit();
  }
  return &cssMisc::Void;
}

static cssEl* cssElCFun_info_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  String fh;
  if(na >= 2) {
    csh->src_prog->Info(fh, arg[1]->GetStr(), arg[2]);
  }
  else if(na == 1) {
    csh->src_prog->Info(fh, arg[1]->GetStr());
  }
  else {
    csh->src_prog->Info(fh);
  }
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}

static cssEl* cssElCFun_help_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  String fh;
  if(na > 0) {
    cp->top->Help(fh, arg[1]);
  }
  else {
    cp->top->Help(fh);
  }
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}

static cssEl* cssElCFun_list_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  String fh;
  if(na > 0) {
    if(na == 2) {
      int nl = (int)*(arg[2]);
      if(nl > 0) csh->src_prog->list_n = nl;
    }
    if(arg[1]->GetPtrType() == cssEl::T_String) {
      csh->src_prog->ListFun(fh, arg[1]->GetStr());
    }
    else if(arg[1]->GetPtrType() == cssEl::T_Int) {
      csh->src_prog->ListSrc(fh, (int)*(arg[1]));
    }
    else if((arg[1]->GetType() == cssEl::T_ScriptFun) ||
       (arg[1]->GetType() == cssEl::T_MbrScriptFun))
    {
      cssScriptFun* fe = (cssScriptFun*)arg[1]->GetNonRefObj();
      fe->fun->ListSrc(fh);
    }
    else {
      csh->src_prog->ListSrc(fh);
    }
  }
  else {
    csh->src_prog->ListSrc(fh);
  }
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}
static cssEl* cssElCFun_load_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->ClearAll();
  csh->src_prog->Compile(arg[1]->GetStr());
  return &cssMisc::Void;
}

static cssEl* cssElCFun_print_stub(int argc, cssEl* arg[]) {
  if(argc == 0) return &cssMisc::Void;
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
//  cssCmdShell* csh = cp->top->cmd_shell;
  String fh = arg[1]->PrintStr();
  if(cp->top->debug > 1)
    fh << "\trefn: " << arg[1]->refn;
  fh << "\n";
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}

static cssEl* cssElCFun_remove_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  if(!csh->src_prog->types.Replace(arg[1], &cssMisc::Void)) {
    if(!cssMisc::TypesSpace.Replace(arg[1], &cssMisc::Void)) {
      if(!csh->src_prog->DelVar(arg[1]))
	cssMisc::Error(cp, "Could not delete type/variable:", (char*)arg[1]->GetName());
    }
  }
  return &cssMisc::Void;
}
static cssEl* cssElCFun_reload_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->reCompile();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_reset_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->Reset();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_restart_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->Restart();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_run_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->Run();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_setbp_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->SetBreak((int)*(arg[1]));
  return &cssMisc::Void;
}
static cssEl* cssElCFun_setwp_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  csh->src_prog->SetWatch(arg[1]);
  return &cssMisc::Void;
}
static cssEl* cssElCFun_setout_stub(int, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  ostream* strm = (ostream*)*(arg[1]);
  csh->fout = strm;
  return &cssMisc::Void;
}
static cssEl* cssElCFun_source_stub(int, cssEl* arg[]) {
//   cssProg* cp = arg[0]->prog;
  // todo: not clear what to do about this one!  it requires self-modification.  the one
  // case of a do-after flag required..
//   if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
//   cssCmdShell* csh = cp->top->cmd_shell;
//   if(csh->src_prog == NULL) return &cssMisc::Void;
//   csh->src_prog->Source(arg[1]->GetStr());
  return &cssMisc::Void;
}
static cssEl* cssElCFun_step_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  if(na > 0)
    csh->src_prog->step_mode = (int)*(arg[1]);
  else
    csh->src_prog->step_mode = 1;
  csh->src_prog->Cont();
  return &cssMisc::Void;
}
static cssEl* cssElCFun_system_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = system(arg[1]->GetStr()); // rval is compiler food
  return rval;
}
static cssEl* cssElCFun_type_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  String fh;
  if(na > 0) {
    arg[1]->PrintType(fh);
  }
  else {
    csh->src_prog->PrintTypes(fh);
  }
  cp->top->DisplayOutput(fh);	// paging default
  return &cssMisc::Void;
}
static cssEl* cssElCFun_undo_stub(int na, cssEl* arg[]) {
  cssProg* cp = arg[0]->prog;
  if(cp->top->cmd_shell == NULL) return &cssMisc::Void;
  cssCmdShell* csh = cp->top->cmd_shell;
  if(csh->src_prog == NULL) return &cssMisc::Void;
  if(na > 0)
    csh->src_prog->Undo((Int)*(arg[1]));
  else
    csh->src_prog->Undo();
  return &cssMisc::Void;
}

//////////////////////////////////
//	Install_Commands	//
//////////////////////////////////

static void Install_Commands() {
  cssElCFun_inst(cssMisc::Commands, alias, 		2, CSS_ALIAS,
"<cmd> <new_nm> Gives a new name to an existing command.  This is useful for defining\
 shortcuts (e.g., alias list ls)");
  cssElCFun_inst(cssMisc::Commands, backtrace, 		cssEl::VarArg, CSS_COMMAND,
"[<levels_back>] Displays a backtrace of the functions called up to the current one (i.e., as\
 called from within a breakpoint). Optional arg specifies the number of levels back to\
 go.  Level of detail displayed depends on the debug level.");
  cssElCFun_inst_nm(cssMisc::Commands, backtrace,   cssEl::VarArg, "bt", CSS_COMMAND,
"[<levels_back>] Displays a backtrace of the functions called up to the current one (i.e., as\
 called from within a breakpoint). Optional arg specifies the number of levels back to\
 go.  Level of detail displayed depends on the debug level.");
  cssElCFun_inst(cssMisc::Commands, chsh, 		cssEl::VarArg, CSS_COMMAND,
"<script_path> Switches the CSS interface to access the CSS script object pointed to by\
 the given path.  This is for hard-coded objects that have CSS script\
 objects in them (of type cssProgSpace)");
  cssElCFun_inst(cssMisc::Commands, clearall, 		0, CSS_COMMAND,
"Clears out everything from the current program space.  This is like\
 restarting the CSS shell, compared to reset which does not remove\
 any variables defined at the top-level.");
  cssElCFun_inst(cssMisc::Commands, cmdebug,   		1, CSS_COMMAND,
"<level> Sets the debug level for the command shell.  Level 1 provides a trace of the source lines\
 executed.  Level 2 provides a more detailed, machine-level trace, and\
 causes list to show the program at the machine level instead of\
 at the usual source level. Levels greater than 2 provide increasing\
 amounts of detail about the inner workings of CSS, which should not be\
 relevant to most users.");
  cssElCFun_inst(cssMisc::Commands, cont, 		0, CSS_COMMAND,
"Continues the execution of a program that was stopped either by a\
 breakpoint or by single-stepping.  To continue at a particular line in\
 the code, use the goto command.");
  cssElCFun_inst(cssMisc::Commands, debug,   		1, CSS_COMMAND,
"<level> Sets the debug level.  Level 1 provides a trace of the source lines\
 executed.  Level 2 provides a more detailed, machine-level trace, and\
 causes list to show the program at the machine level instead of\
 at the usual source level. Levels greater than 2 provide increasing\
 amounts of detail about the inner workings of CSS, which should not be\
 relevant to most users.");
  /*
  cssElCFun_inst(cssMisc::Commands, define, 		0, CSS_COMMAND,
"Toggles the mode where statements that are typed in become part of the\
 current program to be executed later (define mode), as opposed the\
 default (run mode) where statements are executed immediately after entering\
 them."); */
  cssElCFun_inst(cssMisc::Commands, edit,   		cssEl::VarArg, CSS_COMMAND,
"If the TA_GUI (graphical user interface) is active (i.e., by using\
 -gui to start up CSS), edit will bring up a graphical edit\
 dialog for the given object, which must be either a script-defined or\
 hard-coded class object.  The optional second argument, if\
 true, will cause the system to wait for the user to close the\
 edit dialog before continuing execution of the script.");
  cssElCFun_inst_nm(cssMisc::Commands, exit, 		0, "exit", CSS_COMMAND,
"Exits from the program (CSS), or from another program space if\
 chsh (or its TA_GUI equivalent) was called.");
  cssElCFun_inst(cssMisc::Commands, goto, 		1, CSS_COMMAND,
"<src_ln> Continues execution at the given source line.  ");
  cssElCFun_inst(cssMisc::Commands, help, 		cssEl::VarArg, CSS_HELP,
"[<expr>] Shows a short help message, including lists of commands and functions\
 available.    When passed argument (command, function, class, etc),\
 provides help information for it.");
  cssElCFun_inst(cssMisc::Commands, info, 		cssEl::VarArg, CSS_COMMAND,
"topic [<arg>] Provides info on a number of different topics -- type info without\
 any args to get a detailed list of the info topics available.");
  cssElCFun_inst(cssMisc::Commands, list, 		cssEl::VarArg, CSS_COMMAND,
"[<start_ln> [<n_lns>]] [<\"function\">] Lists the program source (and\
 machine code, if debug is 2 or greater), optionally starting at the\
 given source line number, and continuing for either 20 lines (the\
 initial default) or the number given by the second argument (which\
 then becomes the new default).  Alternatively, a function name can be\
 given (in quotes), which will start the listing at the beginning of that function\
 (even if the function is extern-al and does not appear in a\
 line-number based list).  list with no arguments will resume where the\
 last one left off.");
  cssElCFun_inst(cssMisc::Commands, load, 		1, CSS_COMMAND,
"<prog_file> Loads and compiles a new program from the given file."); //
  cssElCFun_inst(cssMisc::Commands, print, 		1, CSS_COMMAND, // "
"<expr> Prints the results of the given expression (which can be any valid CSS\
 expression), giving some type information and following with a new line\
 (\\n).  This is useful for debugging, but not for printing values\
 as part of an executing program.");
  cssElCFun_inst(cssMisc::Commands, reload, 		0, CSS_COMMAND,
"Reloads the current program from the last file that was load-ed.\
 This is useful because you do not have to specify the program file when\
 making a series of changes to a program.");
  cssElCFun_inst(cssMisc::Commands, remove,		1, CSS_COMMAND,
"<var_name> Removes given variable from whatever space it was defined in.  This can\
 be useful if a variable was defined accidentally or given the wrong name\
 during interactive use.");
  cssElCFun_inst(cssMisc::Commands, reset, 		0, CSS_COMMAND,
"Reset is like clearall, except that it does not remove any\
 top-level variables that might have been defined.  Neither of these\
 commands will remove anything declared extern.");
  cssElCFun_inst(cssMisc::Commands, restart,   		0, CSS_COMMAND,
"Resets the script to start at the beginning.  This is useful if you want\
 to stop execution of the program after a break point.");
  cssElCFun_inst(cssMisc::Commands, run,  		0, CSS_COMMAND,
"Runs the script from the start (as opposed to cont which\
 continues execution from the current location).");
  cssElCFun_inst(cssMisc::Commands, setbp, 		1, CSS_COMMAND,
"<src_ln> Sets a breakpoint at the given source-code line.  Execution of the\
 program will break when it gets to that line, and you will be able to\
 examine variables, etc.");
  cssElCFun_inst(cssMisc::Commands, setwp, 		1, CSS_COMMAND,
"<expr> Sets a watchpoint for the given expression.  Execution of the\
 program will break when the value of the expression changes");
  cssElCFun_inst(cssMisc::Commands, setout, 		1, CSS_COMMAND,
"<ostream> Sets the default output of CSS commands to the given stream.  This can\
 be used to redirect listings or program tracing output to a file.");
  cssElCFun_inst_nm(cssMisc::Commands, system, 		1, "shell", CSS_COMMAND,
"<shell_cmd> Executes the given Unix shell command (i.e., shell \"ls -lt\").");
  cssElCFun_inst(cssMisc::Commands, source, 		1, CSS_COMMAND,
"<cmd_file> Loads a file which contains a series of commands or statements, which\
 are executed exactly as if they were entered from the keyboard.  Note\
 that this is different than load-ing a program, which merely\
 compiles the program but does not execute it immediately thereafter.\
 source uses run mode, while load uses define mode.");
  cssElCFun_inst(cssMisc::Commands, step, 		cssEl::VarArg, CSS_COMMAND,
"<step_n> Sets the single-step mode for program execution.  The parameter is the\
 number of lines to step through before pausing.  A value of 0 turns off\
 single stepping.");
  cssElCFun_inst(cssMisc::Commands, type, 		cssEl::VarArg, CSS_COMMAND,
"<type_name> Gives type information about the given type.  This includes full\
 information about classes (both hard-coded and script-defined),\
 including members, functions, scoped types (enums), etc.");
  cssElCFun_inst(cssMisc::Commands, undo, 		cssEl::VarArg, CSS_COMMAND,
"This undoes the previous statement, when in define mode.");
  cssElCFun_inst(cssMisc::Commands, delbp, 		1, CSS_COMMAND,
"<src_ln> Deletes a breakpoint associated with the given source-code line number.");
  cssElCFun_inst(cssMisc::Commands, delwp, 		1, CSS_COMMAND,
"<watch_no> Deletes given watchpoint (see info watch for list of watchpoints)");
}


//////////////////////////
//    Math Functions	//
//////////////////////////

static cssEl* cssRealFun_drand48_stub(int, cssEl**) {
  return new cssReal((Real)MTRnd::GenRandRes53());
}
static cssEl* cssElCFun_lrand48_stub(int, cssEl**) {
  return new cssInt((int)MTRnd::GenRandInt32());
}
static cssEl* cssElCFun_random_stub(int, cssEl**) {
  return new cssInt((int)MTRnd::GenRandInt32());
}
static cssEl* cssElCFun_srand48_stub(int, cssEl* arg[]) {
  MTRnd::InitSeeds((int)*(arg[1]));
  return &cssMisc::Void;
}

static cssEl* cssElCFun_max_stub(int na, cssEl* arg[]) {
  if(arg[1]->IsTaMatrix()) {
    taMatrix* mat1 = cssTA_Matrix::MatrixPtr(*arg[1]);
    if(!mat1) {
      cssMisc::Error(arg[0]->prog, "max: first matrix arg is NULL");
      return &cssMisc::Void;
    }
    if(na == 1) {
      return cssEl::GetElFromVar(mat1->Max(), _nilString);
    }
    if(arg[2]->IsTaMatrix()) {
      taMatrix* mat2 = cssTA_Matrix::MatrixPtr(*arg[2]);
      if(!mat2) {
	cssMisc::Error(arg[0]->prog, "max: second matrix arg is NULL");
	return &cssMisc::Void;
      }
      return new cssTA_Matrix(mat1->Max(*mat2));
    }
    else {
      return new cssTA_Matrix(mat1->Max(arg[2]->GetVar()));
    }
  }
  else {
    if(na != 2) {
      cssMisc::Error(arg[0]->prog, "max: requires 2 arguments");
      return &cssMisc::Void;
    }
    return *(*(arg[1]) > *(arg[2])) ? arg[1] : arg[2];
  }
}

static cssEl* cssElCFun_min_stub(int na, cssEl* arg[]) {
  if(arg[1]->IsTaMatrix()) {
    taMatrix* mat1 = cssTA_Matrix::MatrixPtr(*arg[1]);
    if(!mat1) {
      cssMisc::Error(arg[0]->prog, "min: first matrix arg is NULL");
      return &cssMisc::Void;
    }
    if(na == 1) {
      return cssEl::GetElFromVar(mat1->Min(), _nilString);
    }
    if(arg[2]->IsTaMatrix()) {
      taMatrix* mat2 = cssTA_Matrix::MatrixPtr(*arg[2]);
      if(!mat2) {
	cssMisc::Error(arg[0]->prog, "min: second matrix arg is NULL");
	return &cssMisc::Void;
      }
      return new cssTA_Matrix(mat1->Min(*mat2));
    }
    else {
      return new cssTA_Matrix(mat1->Min(arg[2]->GetVar()));
    }
  }
  else {
    if(na != 2) {
      cssMisc::Error(arg[0]->prog, "min: requires 2 arguments");
      return &cssMisc::Void;
    }
    // returns cssBool* -- need to de-ref
    return (*(*(arg[1]) < *(arg[2]))) ? arg[1] : arg[2];
  }
}

static cssEl* cssElCFun_abs_stub(int na, cssEl* arg[]) {
  if(arg[1]->IsTaMatrix()) {
    taMatrix* mat1 = cssTA_Matrix::MatrixPtr(*arg[1]);
    if(!mat1) {
      cssMisc::Error(arg[0]->prog, "min: first matrix arg is NULL");
      return &cssMisc::Void;
    }
    return new cssTA_Matrix(mat1->Abs());
  }
  else {
    // returns cssBool* -- need to de-ref
    return (*(*(arg[1]) < *cssBI::false_int)) ? -(*arg[1]) : arg[1];
  }
}

static cssEl* cssElCFun_rc_stub(int na, cssEl* arg[]) {
  if(na == 1 && arg[1]->IsTaMatrix()) {
    taMatrix* cr = cssTA_Matrix::MatrixPtr(*arg[1]);
    if(!cr || cr->size < 2) {
      cssMisc::Error(arg[0]->prog, "rc: input matrix NULL or does not have at least 2 indicies");
      return &cssMisc::Void;
    }
    int_Matrix* rval = (int_Matrix*)cr->Clone();
    rval->FastEl_Flat(0) = cr->FastElAsVar_Flat(1).toInt(); // swap'em
    rval->FastEl_Flat(1) = cr->FastElAsVar_Flat(0).toInt();
    return new cssTA_Matrix(rval);
  }
  else if(na == 1) {
    cssMisc::Error(arg[0]->prog, "rc: single argument must be a matrix");
    return &cssMisc::Void;
  }
  int_Matrix* rval = new int_Matrix(1, na);
  rval->FastEl_Flat(0) = (int)*arg[2];
  rval->FastEl_Flat(1) = (int)*arg[1];
  for(int i=2;i<na; i++) 
    rval->FastEl_Flat(i) = (int)*arg[i+1];
  return new cssTA_Matrix(rval);
}

//////////////////////////////////
//	Install_Math		//
//////////////////////////////////

static void Install_Math() {
  // global real functions

  TypeDef* mathtd = &TA_cssMath;
  cssMath* mdobj = (cssMath*)mathtd->GetInstance();

  for(int i=0; i< mathtd->methods.size; i++) {
    MethodDef* md = mathtd->methods[i];
    if(!md->is_static) continue;
    if(md->HasOption("NO_CSS_MATH")) continue; // skip entirely
    String labl = md->OptionAfter("LABEL_");
    if(labl.empty())
      labl = md->name;
    if(md->fun_argc == 1) {
      if((md->arg_types[0] == &TA_double) && md->type && md->type == &TA_double) {
	// simple single arg double function -- use matrix encoding by default
	if(md->HasOption("NO_CSS_MATRIX")) {
	  cssMisc::Functions.Push
	    (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			    cssElFun::MBR_NO_THIS));
	}
	else {
	  cssMisc::Functions.Push
	    (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			    cssElFun::FUN_ITR_MATRIX | cssElFun::MBR_NO_THIS, 
			    md->fun_argc+1)); // arg will be shifted over for 'this'
	}
      }
      else {
	if(md->HasOption("CSS_MATRIX")) {
	  cssMisc::Functions.Push
	    (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			    cssElFun::FUN_ITR_MATRIX | cssElFun::MBR_NO_THIS, 
			    md->fun_argc+1)); // arg will be shifted over for 'this'
	}
	else {
	    cssMisc::Functions.Push
	      (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			      cssElFun::MBR_NO_THIS));
	}
      }
    }
    else {
      String mtxargs = md->OptionAfter("CSS_MATRIX_ARG_");
      int mtxarg = (int)mtxargs;
      if(mtxargs.nonempty() && mtxarg > 0) {
	cssMisc::Functions.Push
	  (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			  cssElFun::FUN_ITR_MATRIX | cssElFun::MBR_NO_THIS, mtxarg+1));
	// arg wil be shifted over for 'this'
	
      }
      else {
	cssMisc::Functions.Push
	  (new cssMbrCFun(md->fun_argc, mdobj, md->stubp, labl, md,
			  cssElFun::MBR_NO_THIS));
      }
    }
  }

  cssRealFun_inst(cssMisc::Functions, drand48, 0,
"Returns a uniformly-distributed random number between 0 and 1 (actually uses MT -- Mersenne Twister).");
  cssElCFun_inst(cssMisc::Functions, lrand48, 0, CSS_FUN,
"Returns a uniformly-distributed random number on the range of the integers (actually uses MT -- Mersenne Twister).");
  cssElCFun_inst(cssMisc::Functions, random, 0, CSS_FUN,
"Returns a uniformly-distributed random number on the range of the\
 integers.  CSS actually uses the MT (Mersenne Twister) function to generate the\
 number given the limitations of the standard random generators.");
  cssElCFun_inst(cssMisc::Functions, srand48, 1, CSS_FUN,
"(Int x) Provides a new random seed for the random number generator based on the argument.");

//   cssMisc::Functions.Push(cssBI::power = new cssElCFun(2, cssRealFun_pow_stub, "pow", CSS_FUN,
// "(Real x, y) Returns x to the y power.  This can also be expressed in CSS as x ^ y."));

  cssElCFun_inst_nm(cssMisc::Functions, max, cssEl::VarArg, "max", CSS_FUN,
		    "(x[,y]) Returns the maximum value -- if one arg that is a matrix, returns maximum value in matrix.  if 1 matrix and a scalar, or 2 matricies, returns matrix with element-wise max.  if 2 scalars, returns max of the two values.");
  cssElCFun_inst_nm(cssMisc::Functions, min, cssEl::VarArg, "min", CSS_FUN,
		    "(x,y) Returns the minimum value -- if one arg that is a matrix, returns minimum value in matrix.  if 1 matrix and a scalar, or 2 matricies, returns matrix with element-wise min.  if 2 scalars, returns min of the two values.");
  cssElCFun_inst_nm(cssMisc::Functions, max, cssEl::VarArg, "MAX", CSS_FUN,
		    "(x[,y]) Returns the maximum value -- if one arg that is a matrix, returns maximum value in matrix.  if 1 matrix and a scalar, or 2 matricies, returns matrix with element-wise max.  if 2 scalars, returns max of the two values.");
  cssElCFun_inst_nm(cssMisc::Functions, min, cssEl::VarArg, "MIN", CSS_FUN,
		    "(x,y) Returns the minimum value -- if one arg that is a matrix, returns minimum value in matrix.  if 1 matrix and a scalar, or 2 matricies, returns matrix with element-wise min.  if 2 scalars, returns min of the two values.");
  cssElCFun_inst_nm(cssMisc::Functions, abs, 1, "abs", CSS_FUN,
		    "(x) Returns the absolute value -- works for scalars and matrix objects (where it returns element-wise abs) -- for complex numbers, it computes the magnitude of the complex numbers (sqrt(r^2 + i^2)).");
  cssElCFun_inst_nm(cssMisc::Functions, abs, 1, "ABS", CSS_FUN,
		    "(x) Returns the absolute value -- works for scalars and matrix objects (where it returns element-wise abs) -- for complex numbers, it computes the magnitude of the complex numbers (sqrt(r^2 + i^2)).");
  cssElCFun_inst_nm(cssMisc::Functions, rc, cssEl::VarArg, "rc", CSS_FUN,
		    "(r,c,..|[r,c,..]) Returns indexes with row and column inputs switched -- c,r -- converts from mathematical matrix standard of r,c to natural Matrix access standard of c,r (x,y).");

  cssConstReal_inst_nm(cssMisc::Constants, 3.14159265358979323846,  "PI"   );
//   cssConstReal_inst(cssMisc::Constants, 2.71828182845904523536,  E     );
  cssConstReal_inst(cssMisc::Constants, 0.57721566490153286060,  GAMMA ); // Euler
  cssConstReal_inst(cssMisc::Constants, 57.29577951308232087680, DEG   ); // deg/radian
  cssConstReal_inst(cssMisc::Constants, 1.61803398874989484820,  PHI   ); // golden ratio
  cssConstReal_inst(cssMisc::Constants, 96484.56,                FARADAY); // coulombs/mole
//   cssConstReal_inst(cssMisc::Constants, 8.31441,                 R     );  // molar gas constant, joules/mole/deg-K

  cssConstReal_inst_nm(cssMisc::Constants, FLT_MAX,             "FLT_MAX");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_MAX_10_EXP,      "FLT_MAX_10_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_MAX_EXP,         "FLT_MAX_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_MAX,             "MAXFLOAT");	// compatibiltiy

  cssConstReal_inst_nm(cssMisc::Constants, FLT_MIN,             "FLT_MIN");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_MIN_10_EXP,      "FLT_MIN_10_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_MIN_EXP,         "FLT_MIN_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, FLT_EPSILON,         "FLT_EPSILON");

  cssConstReal_inst_nm(cssMisc::Constants, DBL_MAX,             "DBL_MAX");
  cssConstReal_inst_nm(cssMisc::Constants, DBL_MAX_10_EXP,      "DBL_MAX_10_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, DBL_MAX_EXP,         "DBL_MAX_EXP");

  cssConstReal_inst_nm(cssMisc::Constants, DBL_MIN,             "DBL_MIN");
  cssConstReal_inst_nm(cssMisc::Constants, DBL_MIN_10_EXP,      "DBL_MIN_10_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, DBL_MIN_EXP,         "DBL_MIN_EXP");
  cssConstReal_inst_nm(cssMisc::Constants, DBL_EPSILON,         "DBL_EPSILON");

  cssConstReal_inst_nm(cssMisc::Constants, INT_MAX,             "INT_MAX");
  cssConstReal_inst_nm(cssMisc::Constants, INT_MIN,             "INT_MIN");
  cssConstReal_inst_nm(cssMisc::Constants, LONG_MAX,            "LONG_MAX");
  cssConstReal_inst_nm(cssMisc::Constants, LONG_MIN,            "LONG_MIN");
}


//////////////////////////////////
// 	Misc Functions  	//
//////////////////////////////////

static cssEl* cssElCFun_CancelEditObj_stub(int, cssEl* arg[]) {
  cssEl* aobj = arg[1]->GetActualObj();
  if(aobj->GetType() != cssEl::T_Class)
    cssMisc::Error(arg[0]->prog, "CancelEditObj: arg not a class instance");
  else {
#ifdef TA_GUI
    cssClassInst* obj = (cssClassInst*)aobj;
    cssiSession::CancelObjEdits(obj);
#endif
  }
  return &cssMisc::Void;
}

static cssEl* cssElCFun_Extern_stub(int, cssEl* arg[]) {
  cssElPtr ptr = cssMisc::Externs.FindName(arg[1]->GetStr());
  return ptr.El();
}

static cssEl* cssElCFun_Token_stub(int, cssEl* arg[]) {
  return arg[1]->GetToken((int)*(arg[2]));
}

static cssEl* cssElCFun_Type_stub(int, cssEl* arg[]) {
  TypeDef* td = taMisc::FindTypeName(arg[1]->GetStr());
  if(td == NULL) {
    cssMisc::Error(arg[0]->prog, "Could not find type:", arg[1]->GetStr());
    return &cssMisc::Void;
  }
  return new cssTypeDef(td, 1, &TA_TypeDef);
}

static cssEl* cssElCFun_Dir_stub(int na, cssEl* arg[]) {
  String dir_nm = ".";
  if(na > 0)
    dir_nm = arg[1]->GetStr();
  String_Array& rval = Dir(dir_nm);
  static cssTA_Base* rv_ta = NULL;
  if(rv_ta == NULL) {
    rv_ta = new cssTA_Base(NULL, 1, &TA_String_Array);
    cssEl::Ref(rv_ta);
  }
  rv_ta->ptr = (void*)&rval;
  return new cssRef(rv_ta);
}

// this reads a line from a file and returns a string array with
// one entry per column from the file
static cssEl* cssElCFun_ReadLine_stub(int, cssEl* arg[]) {
  istream* fh = (istream*)*(arg[1]);
  if(fh == NULL)
    return &cssMisc::Void;
  String_Array& rval = ReadLine(*fh);
  static cssTA_Base* rv_ta = NULL;
  if(rv_ta == NULL) {
    rv_ta = new cssTA_Base(NULL, 1, &TA_String_Array);
    cssEl::Ref(rv_ta);
  }
  rv_ta->ptr = (void*)&rval;
  return new cssRef(rv_ta);
}

// old-fashioned C file io

static cssEl* cssElCFun_fopen_stub(int, cssEl* arg[]) {
  fstream* strm = new fstream;	// this guy never gets free'd..
  String optype = arg[2]->GetStr();

  if(optype == "r")
    strm->open(*arg[1], ios::in);
  else if(optype == "w")
    strm->open(*arg[1], ios::out);
  else if(optype == "a")
    strm->open(*arg[1], ios::out | ios::app);
  else {
    delete strm;
    cssMisc::Error(arg[0]->prog, "fopen: open type not recognized: ", (char*)optype);
    return &cssMisc::Void;
  }
  return new cssIOS((void*)strm, 1, &TA_fstream);
}
static cssEl* cssElCFun_fclose_stub(int, cssEl* arg[]) {
  fstream* fh = (fstream*)*(arg[1]);
  if((fh != NULL) && fh->rdbuf()->is_open()) {
    fh->close(); fh->clear();
  }
  return &cssMisc::Void;
}
static cssEl* cssElCFun_fprintf_stub(int na, cssEl* arg[]) {
  ostream* fh = (ostream*)*(arg[1]);
  if((fh == NULL) || !fh->good())
    return &cssMisc::Void;
  String str;
  for(int i=2; i <= na; i++) {
    str << (arg[i])->PrintFStr();
  }
  if(arg[1]->name == "cout" || arg[1]->name == "cerr") {
    taMisc::ConsoleOutputChars(str);
  }
  else {
	*fh << str;
	fh->flush();
  }
  return &cssMisc::Void;
}
static cssEl* cssElCFun_printf_stub(int na, cssEl* arg[]) {
//  cssProg* cp = arg[0]->prog;
  String str;
  for(int i=1; i <= na; i++) {
    str << (arg[i])->PrintFStr();
  }
  taMisc::ConsoleOutputChars(str);
  return &cssMisc::Void;
}

//////////////////////////////////
// 	POSIX Functions  	//
//////////////////////////////////
#if (!defined(TA_OS_WIN))
static cssEl* cssElCFun_access_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = access(arg[1]->GetStr(), (int)*arg[2]);
  return rval;
}
static cssEl* cssElCFun_alarm_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = alarm((Int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_chdir_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = chdir(arg[1]->GetStr());
  return rval;
}
static cssEl* cssElCFun_chown_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = chown(arg[1]->GetStr(), (int)*arg[2], (int)*arg[3]);
  return rval;
}
static cssEl* cssElCFun_ctermid_stub(int, cssEl**) {
  cssString* rval = new cssString();
  char buf[L_ctermid];
  rval->val = ctermid(buf);
  return rval;
}
#ifndef TA_OS_MAC
static cssEl* cssElCFun_cuserid_stub(int, cssEl**) {
  cssString* rval = new cssString();
  rval->val = cuserid(NULL);
  return rval;
}
#endif
static cssEl* cssElCFun_getcwd_stub(int, cssEl**) {
  cssString* rval = new cssString();
  char* cwd = getcwd(NULL, 1024);
  rval->val = cwd;
  free(cwd);
  return rval;
}
static cssEl* cssElCFun_getenv_stub(int, cssEl* arg[]) {
  cssString* rval = new cssString();
  char* env_val = getenv(arg[1]->GetStr());
  if(env_val != NULL)
    rval->val = env_val;
  return rval;
}
static cssEl* cssElCFun_getegid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)getegid();
  return rval;
}
static cssEl* cssElCFun_geteuid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)geteuid();
  return rval;
}
static cssEl* cssElCFun_getgid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)getegid();
  return rval;
}
static cssEl* cssElCFun_getuid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)getuid();
  return rval;
}
static cssEl* cssElCFun_getlogin_stub(int, cssEl**) {
  cssString* rval = new cssString();
  rval->val = getlogin();
  return rval;
}
static cssEl* cssElCFun_getpgrp_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
// #if defined(SUN4) && !defined(SOLARIS)
//   rval->val = (int)getpgrp(0);
// #else
  rval->val = (int)getpgrp();
// #endif
  return rval;
}
static cssEl* cssElCFun_getpid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)getpid();
  return rval;
}
static cssEl* cssElCFun_getppid_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)getppid();
  return rval;
}
static cssEl* cssElCFun_isatty_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)isatty((int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_link_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)link(arg[1]->GetStr(), arg[2]->GetStr());
  return rval;
}

#if defined(SUN4)|| defined(SUN4solaris)
extern "C" int symlink(const char*, const char*);
extern "C" int rename(const char*, const char*);
#endif

static cssEl* cssElCFun_symlink_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)symlink(arg[1]->GetStr(), arg[2]->GetStr());
  return rval;
}
static cssEl* cssElCFun_unlink_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)unlink(arg[1]->GetStr());
  return rval;
}
static cssEl* cssElCFun_pause_stub(int, cssEl**) {
  cssInt* rval = new cssInt();
  rval->val = (int)pause();
  return rval;
}
static cssEl* cssElCFun_rename_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)rename(arg[1]->GetStr(), arg[2]->GetStr());
  return rval;
}
static cssEl* cssElCFun_rmdir_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)rmdir(arg[1]->GetStr());
  return rval;
}
static cssEl* cssElCFun_setgid_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)setgid((int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_setpgid_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)setpgid((int)*arg[1], (int)*arg[2]);
  return rval;
}
static cssEl* cssElCFun_setuid_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)setuid((int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_sleep_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  taMisc::SleepS((int)*arg[1]);
  rval->val = 0;
  return rval;
}
static cssEl* cssElCFun_sleepms_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  taMisc::SleepMs((int)*arg[1]);
  rval->val = 0;
  return rval;
}
static cssEl* cssElCFun_tcgetpgrp_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)tcgetpgrp((int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_tcsetpgrp_stub(int, cssEl* arg[]) {
  cssInt* rval = new cssInt();
  rval->val = (int)tcsetpgrp((int)*arg[1], (int)*arg[2]);
  return rval;
}
static cssEl* cssElCFun_ttyname_stub(int, cssEl* arg[]) {
  cssString* rval = new cssString();
  rval->val = ttyname((int)*arg[1]);
  return rval;
}
static cssEl* cssElCFun_perror_stub(int, cssEl* arg[]) {
  perror(arg[1]->GetStr());
  return &cssMisc::Void;
}

static cssEl* cssElCFun_putenv_stub(int, cssEl* arg[]) {
  static String_Array env_vals;	// need to keep strings around since putenv uses it..
  String env_val = arg[1]->GetStr();
  String env_cue = env_val.through('=');	// must have an equals..

  int i;
  for(i=0; i<env_vals.size; i++) {
    if(env_vals.FastEl(i).at(0,env_cue.length()) == env_cue) { // redefining existing
      env_vals.FastEl(i) = env_val;
      const char* val = (const char*)env_vals.FastEl(i);
      if(setenv(val, val, 1))   // setenv preferred to putenv
	return cssBI::true_int; // no sense in creating more baggage..
      else
	return cssBI::false_int;
    }
  }
  env_vals.Add(env_val);
  const char* val = (const char*)env_vals.Peek();
  if(setenv(val, val, 1))       // setenv preferred to putenv
    return cssBI::true_int;     // no sense in creating more baggage..
  else
    return cssBI::false_int;
}

// possibly non-posix but nonetheless useful functions

// #if defined(SUN4) && !defined(SOLARIS)
// extern "C" {
//   int gettimeofday(struct timeval*, struct timezone*);
//   clock_t clock();
// }
// #define CLOCKS_PER_SEC 1000000
// #endif

static cssEl* cssElCFun_gettimesec_stub(int, cssEl**) {
  struct timezone tzp;
  struct timeval tp;
  gettimeofday(&tp,&tzp);
  return new cssInt((int)tp.tv_sec);
}

static cssEl* cssElCFun_gettimemsec_stub(int, cssEl**) {
  struct timezone tzp;
  struct timeval tp;
  gettimeofday(&tp,&tzp);
  return new cssInt((int)tp.tv_usec);
}

static cssEl* cssElCFun_clock_stub(int, cssEl**) {
  clock_t clk = clock();
  double sec = (double)clk / (double)CLOCKS_PER_SEC;
  return new cssReal(sec);
}
#endif // Posix-only functions

// apparently this is not available on all platforms..
//
// #include <sys/resource.h>
//
// extern "C" getrusage(int, struct rusage*);
//
// static cssEl* cssElCFun_getrusage_stub(int, cssEl**) {
//   static int_Array vals;
//   static cssTA* rv_ta = NULL;
//   struct rusage rus;
//   getrusage(RUSAGE_SELF,&rus);
//   vals.SetSize(18);
//   vals[0] = (int)rus.ru_utime.tv_sec;  // user time used, secs
//   vals[1] = (int)rus.ru_utime.tv_usec; // user time used, microsecs
//   vals[2] = (int)rus.ru_stime.tv_sec;  // system time used, secs
//   vals[3] = (int)rus.ru_stime.tv_usec; // system time used, microsecs
//   vals[4] = (int)rus.ru_maxrss;	  // max resident size
//   vals[5] = (int)rus.ru_ixrss;	  // integral shared text memory size
//   vals[6] = (int)rus.ru_idrss;	  // integral unshared data size
//   vals[7] = (int)rus.ru_isrss;	  // integral unshared stack size
//   vals[8] = (int)rus.ru_minflt;	  // page reclaims
//   vals[9] = (int)rus.ru_majflt;	  // page faults
//   vals[10]= (int)rus.ru_nswap;	  // swaps
//   vals[11]= (int)rus.ru_inblock;	  // block input operations
//   vals[12]= (int)rus.ru_oublock;	  // block output operations
//   vals[13]= (int)rus.ru_msgsnd;	  // messages sent
//   vals[14]= (int)rus.ru_msgrcv;	  // messages received
//   vals[15]= (int)rus.ru_nsignals;	  // signals received
//   vals[16]= (int)rus.ru_nvcsw;	  // voluntary context switches
//   vals[17]= (int)rus.ru_nivcsw;	  // involuntary context switches
//   if(rv_ta == NULL) {
//     rv_ta = new cssTA(NULL, 1, &TA_int_Array);
//     cssEl::Ref(rv_ta);
//   }
//   rv_ta->ptr = (void*)&vals;
//   return new cssRef(rv_ta);
// }
//

//////////////////////////////////
//	Install_MiscFun		//
//////////////////////////////////

static void Install_MiscFun() {

  // misc functions
  cssElCFun_inst_nm(cssMisc::Functions, edit, cssEl::VarArg, "EditObj", CSS_FUN,
"(<object>) This is the function version of the edit command.  If the TA_GUI\
 (graphical user interface) is active (i.e., by using -gui to\
 start up CSS), edit will bring up a graphical edit dialog for the given\
 object, which must be either a script-defined or hard-coded class\
 object.  The optional second argument, if TRUE, will cause the system to\
 wait for the user to close the edit dialog before continuing execution\
 of the script.");
  cssElCFun_inst(cssMisc::Functions, CancelEditObj, 1, CSS_FUN,
"(<object>) Cancels the edit dialog for the given object that would have been opened\
 by EditObj.");
  cssElCFun_inst(cssMisc::Functions, Extern, 1, CSS_FUN,
"(String& name) Returns the object with the given name on the 'extern' variable list.\
 This provides a mechanism for passing arbitrary (i.e., class objects)\
 data across different name spaces (i.e., across different instances of\
 the css program space), since you can pass the name of the extern class\
 object that contains data relevant to another script, and use this\
 function to get that object from its name.");
  cssElCFun_inst(cssMisc::Functions, Token, 2, CSS_FUN,
"(<obj_type>, Int tok_no) Returns the token of the given type of object at index tok_no in\
 the list of tokens.  Use the tokens command to obtain a listing\
 of the tokens of a given type of object.");
  cssElCFun_inst(cssMisc::Functions, Type, 1, new cssTypeDef(NULL, 1, &TA_TypeDef),
"(String& typ_nm | <obj_type>) Returns a type descriptor object (generated by TypeAccess), for the\
 given type name or type object (the type object can be used directly in\
 some situations, but not all).");
  cssElCFun_inst(cssMisc::Functions, Dir, cssEl::VarArg, new cssTA(NULL, 1, &TA_String_Array),
"([String& dir_nm]) Fills an array with the names of all the files in the given directory\
 (defaults to \".\" if no directory name is passed).  The user should\
 copy the array if they want to keep it around, since the one returned\
 is just a pointer to an internal array object.");
  cssElCFun_inst(cssMisc::Functions, ReadLine, 1, new cssTA(NULL, 1, &TA_String_Array),
"(istream& strm) Reads a line of data from the given stream, and returns a reference to\
 an internal array (which is reused upon a subsequent call to ReadLine)\
 of strings with elements containing the whitespace-delimited columns of\
 the line.  The size of the array gives the number of columns, etc.  This\
 allows one to easily implement much of the functionality of awk.  See\
 the file css_awk.css in css/include for an example.");

  // old file-io
  cssElCFun_inst(cssMisc::Functions, fprintf, cssEl::VarArg, CSS_FUN,
"(FILE strm, v1 [,v2...]) Prints the given arguments (which must be comma separated) to the\
 stream.  Values to be printed can be of any type, and are actually\
 printed with the << operator of the stream classes.  Unlike the\
 standard C function, there is no provision for specifying formatting\
 information.  Instead, the formatting must be specified by changing the\
 parameters of the stream object.  The FILE type is not actually a\
 standard C FILE, but actually a fstream type, so stream\
 operations can be performed on it.");
  cssElCFun_inst(cssMisc::Functions, fopen, 2, CSS_FUN,
"(String& file_nm, String& mode) Opens given file name in the given mode, where the modes are \"r\",\
 \"w\", and \"a\" for read, write and append. The FILE type is not\
 actually astandard C FILE, but actually a fstream type, so\
 stream operations can be performed on it.");
  cssElCFun_inst(cssMisc::Functions, fclose, 1, CSS_FUN,
"(FILE fh) Closes the file, which was opened by fopen.  The FILE type is not\
 actually a standard C FILE, but actually a fstream type, so\
 stream operations can be performed on it.");
  cssElCFun_inst(cssMisc::Functions, printf, cssEl::VarArg, CSS_FUN,
"(v1 [,v2...]) Prints the given arguments (which must be comma separated) to the\
 standard output stream.  Values to be printed can be of any type, and\
 are actually printed with the @code{<<} operator of the stream classes.\
 Unlike the standard C function, there is no provision for specifying\
 formatting information.  Instead, the formatting must be specified by\
 changing the parameters of the standard stream output object,\
 cout.  The FILE type is not actually a standard C FILE, but\
 actually a fstream type, so stream operations can be performed on\
 it.");

  // POSIX functions
#if (!defined(TA_OS_WIN))
  cssElCFun_inst(cssMisc::Functions, access, 2, CSS_FUN,
"(String fname, int ac_type) This POSIX command determines if the given file name is accessible\
 according to the ac_type argument, which should be some bitwise OR of\
 the enums R_OK W_OK X_OK F_OK.  Returns success and sets errno\
 flag on failure.");
  cssEnum_inst_nm(cssMisc::Enums, R_OK, "R_OK");
  cssEnum_inst_nm(cssMisc::Enums, W_OK, "W_OK");
  cssEnum_inst_nm(cssMisc::Enums, X_OK, "X_OK");
  cssEnum_inst_nm(cssMisc::Enums, F_OK, "F_OK");
  cssElCFun_inst(cssMisc::Functions, alarm, 1, CSS_FUN,
"(int seconds) Generate an alarm signal in the given number of seconds.  Returns\
 success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, chdir, 1, CSS_FUN,
"(String dir_name) Change the current directory to given argument.  Returns success and\
 sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, chown, 3, CSS_FUN,
"(String fname, int user, int group) Changes the ownership of the given file to the given user and group\
 numbers.  Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, ctermid, 0, CSS_FUN,
"Returns the character-id of the current terminal.");
#ifndef TA_OS_MAC
  cssElCFun_inst(cssMisc::Functions, cuserid, 0, CSS_FUN,
"Returns the character-id of the current user.");
#endif
  cssElCFun_inst(cssMisc::Functions, getcwd, 0, CSS_FUN,
"Returns the current working directory path.");
  cssElCFun_inst(cssMisc::Functions, getenv, 1, CSS_FUN,
"Returns the environment variable definition for varable var.");
  cssElCFun_inst(cssMisc::Functions, getegid, 0, CSS_FUN,
"Returns the current effective group id number for this process.");
  cssElCFun_inst(cssMisc::Functions, geteuid, 0, CSS_FUN,
"Returns the current effective user id number for this process.");
  cssElCFun_inst(cssMisc::Functions, getgid, 0, CSS_FUN,
"Returns the current group id number for this process.");
  cssElCFun_inst(cssMisc::Functions, getuid, 0, CSS_FUN,
"Returns the current user id number for this process.");
  cssElCFun_inst(cssMisc::Functions, getlogin, 0, CSS_FUN,
"Returns the name the current user logged in as.");
  cssElCFun_inst(cssMisc::Functions, getpgrp, 0, CSS_FUN,
"Returns the process group id for current process.");
  cssElCFun_inst(cssMisc::Functions, getpid, 0, CSS_FUN,
"Returns the process id for current process.");
  cssElCFun_inst(cssMisc::Functions, getppid, 0, CSS_FUN,
"Returns the parent process id for current process.");
  cssElCFun_inst(cssMisc::Functions, isatty, 1, CSS_FUN,
"Returns true if the current input terminal is a tty (as opposed to a\
 file or a pipe or something else).");
  cssElCFun_inst(cssMisc::Functions, link, 2, CSS_FUN,
"(String from, String to) Creates a hard link from given file to other file.  (see also symlink).\
 Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, symlink, 2, CSS_FUN,
"(String from, String to) Creates a symbolic link from given file to other file.  (see also link).\
 Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, unlink, 1, CSS_FUN,
"(String fname) Unlinks (removes) the given file name.");
  cssElCFun_inst(cssMisc::Functions, pause, 0, CSS_FUN,
"Pause (wait) until an alarm or other signal is received.  Returns\
 success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, rename, 2, CSS_FUN,
"(String from, String to) Renames given file.  Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, rmdir, 1, CSS_FUN,
"(String dir_name) Removes given directory.  Returns success and sets errno flag on\
 failure.");
  cssElCFun_inst(cssMisc::Functions, setgid, 1, CSS_FUN,
"(Int id) Sets group id for given process to that given.  Note that only the\
 super-user can in general do this.  Returns success and sets errno flag\
 on failure.");
  cssElCFun_inst(cssMisc::Functions, setpgid, 2, CSS_FUN,
"(Int id) Sets process group id for given process to that given.  Note that only\
 the super-user can in general do this.  Returns success and sets errno\
 flag on failure.");
  cssElCFun_inst(cssMisc::Functions, setuid, 1, CSS_FUN,
"(Int id) Sets user id for given process to that given.  Note that only the\
 super-user can in general do this.  Returns success and sets errno flag\
 on failure.");
  cssElCFun_inst(cssMisc::Functions, sleep, 1, CSS_FUN,
"(Int seconds) Causes the process to wait for given number of seconds.  Returns success=0\
 and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, sleepms, 1, CSS_FUN,
"(Int milliseconds) Causes the process to wait for given number of milliseconds.  Returns success=0\
 and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, tcgetpgrp, 1, CSS_FUN,
"(Int file_no) Gets the process group associated with the given file descriptor.\
 Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, tcsetpgrp, 2, CSS_FUN,
"(Int file_no) Sets the process group associated with the given file descriptor.\
 Returns success and sets errno flag on failure.");
  cssElCFun_inst(cssMisc::Functions, ttyname, 1, CSS_FUN,
"(Int file_no) Returns the terminal name associated with the given file descriptor.");
  cssElCFun_inst(cssMisc::Functions, perror, 1, CSS_FUN,
"(String prompt) Prints out the current error message to stderr (cerr).  The prompt\
 argument is printed before the error message.  Also, the global variable\
 errno can be checked.  Further, there is an include file in\
 css/include called errno.css that defines an enumerated type for the\
 defined values of errno.");
  cssCPtr_int_inst(cssMisc::HardVars, errno);
  cssElCFun_inst(cssMisc::Functions, putenv, 1, CSS_FUN,
"(String env_val) Put the environment value into the list of environment values (avail\
 through getenv).");
  cssElCFun_inst(cssMisc::Functions, system, 1, CSS_FUN,
"(String& cmd) Executes the given command in the Unix shell.");

  cssElCFun_inst(cssMisc::Functions, gettimesec, 0, CSS_FUN,
"Returns current time of day in seconds.");
  cssElCFun_inst(cssMisc::Functions, gettimemsec, 0, CSS_FUN,
"Returns current time of day in microseconds.");
  cssElCFun_inst(cssMisc::Functions, clock, 0, CSS_FUN,
"Returns processor time used by current process in seconds (with\
 fractions expressed in decimals).");
//  cssElCFun_inst(cssMisc::Functions, getrusage, 0, CSS_FUN, " ");
#endif // Posix
  // parsed internal ptrs
  cssMisc::HardVars.Push(new cssIOS((void*)&cout, 1, &TA_ostream, "cout"));
  cssMisc::HardVars.Push(new cssIOS((void*)&cin, 1, &TA_istream, "cin"));
  cssMisc::HardVars.Push(new cssIOS((void*)&cerr, 1, &TA_ostream, "cerr"));

  cssMisc::HardVars.Push(new cssIOS((void*)&cout, 1, &TA_ostream, "stdout"));
  cssMisc::HardVars.Push(new cssIOS((void*)&cin, 1, &TA_istream, "stdin"));
  cssMisc::HardVars.Push(new cssIOS((void*)&cerr, 1, &TA_ostream, "stderr"));

  // magic stream codes!
  cssMisc::Constants.Push(new cssConstBool(false, "ws"));
  cssMisc::Constants.Push(new cssConstBool(false, "flush"));
  cssMisc::Constants.Push(new cssConstBool(false, "endl"));
  cssMisc::Constants.Push(new cssConstBool(false, "ends"));
  cssMisc::Constants.Push(new cssConstBool(false, "dec"));
  cssMisc::Constants.Push(new cssConstBool(false, "hex"));
  cssMisc::Constants.Push(new cssConstBool(false, "oct"));
  cssMisc::Constants.Push(new cssConstBool(false, "lock"));
  cssMisc::Constants.Push(new cssConstBool(false, "unlock"));

  // constants
  cssMisc::Enums.Push(new cssConstBool(true, "true"));
  cssMisc::Enums.Push(new cssConstBool(false, "false"));
  cssBI::true_int = new cssConstInt(1, "true");
  cssBI::false_int = new cssConstInt(0, "false");
  cssMisc::Internal.Push(cssBI::true_int); // keep them on this list (to refn)
  cssMisc::Internal.Push(cssBI::false_int);
  cssConstInt_inst(cssMisc::Constants, 0, NULL);
  cssMisc::Constants.Push(new cssVariant(_nilVariant, "_nilVariant"));

  // matrix parsing magic foo
  cssBI::semicolon_mark = new cssConstBool(false, "semicolon_mark");
  cssBI::colon_mark = new cssConstBool(false, "colon_mark");
  cssBI::comma_mark = new cssConstBool(false, "comma_mark");
  cssBI::matrix_inst = new cssTA_Matrix;
  cssBI::matrix_inst->type_def = &TA_taMatrix; // base type..
  cssMisc::Internal.Push(cssBI::semicolon_mark);
  cssMisc::Internal.Push(cssBI::colon_mark);
  cssMisc::Internal.Push(cssBI::comma_mark);
  cssMisc::Internal.Push(cssBI::matrix_inst);

  // taBase enums that should be generally accessible to all objs..
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_STRING, "VT_STRING");
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_DOUBLE, "VT_DOUBLE");
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_FLOAT, "VT_FLOAT");
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_INT, "VT_INT");
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_BYTE, "VT_BYTE");
  cssEnum_inst_nm(cssMisc::Enums, taBase::VT_VARIANT, "VT_VARIANT");
}


//////////////////////////////////
//	Install_Types		//
//////////////////////////////////

static void GeneratePtrRefTypes() {
  for(int i=0; i<cssMisc::TypesSpace.size; i++) {
    cssEl* tp = cssMisc::TypesSpace.FastEl(i);
    if(tp->GetType() == cssEl::T_TA) {
      cssTA* tap = (cssTA*)tp->GetNonRefObj();
      tap->ptr_cnt--;		// decrement ptr cnt from install to prevent obj creation
      tap->flags = cssCPtr::NO_PTR_FLAGS; // turn off the OWN_OBJ flag if it was set!
    }
    cssMisc::TypesSpace_refs.Push(tp->MakeRefType());

    cssEl* ptr1 = tp->MakePtrType(1);
    cssMisc::TypesSpace_ptrs.Push(ptr1);
    cssMisc::TypesSpace_refs.Push(ptr1->MakeRefType());

    if((tp->GetType() == cssEl::T_C_Ptr) || (tp->GetType() == cssEl::T_TA)) {
      cssEl* ptr2 = tp->MakePtrType(2);
      cssMisc::TypesSpace_ptrs.Push(ptr2);
      cssMisc::TypesSpace_refs.Push(ptr2->MakeRefType());
    }
  }
}

static void Install_Types() {
  // basic CSS types
  cssMisc::TypesSpace.Push(&cssMisc::Void);
  cssMisc::TypesSpace.Push(new cssEl("void"));
  cssInt_inst_nm(cssMisc::TypesSpace, 0, "Int");
  cssInt_inst_nm(cssMisc::TypesSpace, 0, "int");
  cssInt64_inst_nm(cssMisc::TypesSpace, 0, "Int64");
  cssInt64_inst_nm(cssMisc::TypesSpace, 0, "int64_t");
  cssMisc::TypesSpace.Push(new cssBool(false, "bool"));
  cssMisc::TypesSpace.Push(new cssBool(false, "Bool"));
  cssReal_inst_nm(cssMisc::TypesSpace, 0.0, "Real");
  //  cssReal_inst_nm(cssMisc::TypesSpace, 0.0, "real");
  cssReal_inst_nm(cssMisc::TypesSpace, 0.0, "float");
  cssReal_inst_nm(cssMisc::TypesSpace, 0.0, "double");
  cssString_inst_nm(cssMisc::TypesSpace, "", "String");
  cssString_inst_nm(cssMisc::TypesSpace, "", "string");
  cssVariant_inst_nm(cssMisc::TypesSpace, "", "Variant");
  cssVariant_inst_nm(cssMisc::TypesSpace, "", "variant");
  cssMisc::TypesSpace.Push(new cssChar(' ', "char"));
  cssCPtr_inst_nm(cssMisc::TypesSpace, 0, 1, "c_ptr");
  cssMisc::TypesSpace.Push(new cssSubShell("SubShell"));

  // CPtr types
  cssMisc::TypesSpace.Push(new cssCPtr_int(NULL, 0, "c_int"));
  cssMisc::TypesSpace.Push(new cssCPtr_bool(NULL, 0, "c_bool"));
  cssMisc::TypesSpace.Push(new cssCPtr_short(NULL, 0, "c_short"));
  cssMisc::TypesSpace.Push(new cssCPtr_long(NULL, 0, "c_long"));
  cssMisc::TypesSpace.Push(new cssCPtr_long_long(NULL, 0, "c_long_long"));
  cssMisc::TypesSpace.Push(new cssCPtr_char(NULL, 0, "c_char"));
  cssMisc::TypesSpace.Push(new cssCPtr_enum(NULL, 0, "c_enum"));
  cssMisc::TypesSpace.Push(new cssCPtr_double(NULL, 0, "c_double"));
  cssMisc::TypesSpace.Push(new cssCPtr_float(NULL, 0, "c_float"));
  cssMisc::TypesSpace.Push(new cssCPtr_String(NULL, 0, "c_String"));
  cssMisc::TypesSpace.Push(new cssCPtr_Variant(NULL, 0, "c_Variant"));
  cssMisc::TypesSpace.Push(new cssCPtr_DynEnum(NULL, 0, "c_DynEnum"));

  // NOTE: all TA types are installed with ptr_cnt +1 -- these are then -1
  // in GeneratePtrRefTypes() -- this is to prevent automatic creation of actual tokens!

  // ta types
  cssTA_inst_nm (cssMisc::TypesSpace, NULL, 1, &TA_void, "TA");
  // cssTA_Base_inst_nm (cssMisc::TypesSpace, NULL, 1, &TA_taBase, "taBase");
  cssMisc::TypesSpace.Push(new cssFStream("fstream"));
  cssMisc::TypesSpace.Push(new cssSStream("sstream"));
  cssMisc::TypesSpace.Push(new cssIOS(NULL, 2, &TA_fstream, "FILE"));
  cssMisc::TypesSpace.Push(new cssLeafItr("taLeafItr"));

  cssMisc::TypesSpace.Push(new cssTypeDef(NULL, 1, &TA_TypeDef, "TypeDef"));
  cssMisc::TypesSpace.Push(new cssMemberDef(NULL, 1, &TA_MemberDef, "MemberDef"));
  cssMisc::TypesSpace.Push(new cssMethodDef(NULL, 1, &TA_MethodDef, "MethodDef"));

  // add all user-defined types..
  int i, j;
  for(i=0; i<taMisc::types.size; i++) {
    TypeDef* tmp = taMisc::types.FastEl(i);
    if(tmp->IsActualClass()) {
      // don't add any template instances that have a single further subclass
      // (use the subclass instead)
      if(tmp->IsTemplInst()) {
	if((tmp->children.size != 1) || (tmp->children.FastEl(0)->parents.size > 1)) {
	  if(tmp->InheritsFrom(TA_taMatrix)) {
	    taBase* inst = (taBase*)tmp->GetInstance();
	    if(inst) taBase::Ref(inst); // ref it!
	    cssMisc::TypesSpace.Push(new cssTA_Matrix(inst, 1, tmp, tmp->name));
	  }
	  else if(tmp->IsTaBase()) {
	    taBase* inst = (taBase*)tmp->GetInstance();
	    if(inst) taBase::Ref(inst); // ref it!
	    cssTA_Base_inst_nm(cssMisc::TypesSpace, inst, 1, tmp, tmp->name);
	  }
	  else {
	    cssTA_inst_nm(cssMisc::TypesSpace, tmp->GetInstance(), 1, tmp, tmp->name);
	  }
	}
      }
      else if(!((tmp->members.size==0) && (tmp->methods.size==0)) &&
	      (tmp->name != "fstream") && (tmp->name != "class") &&
	      (tmp->name != "taString") && (tmp->name != "taLeafItr") &&
	      (tmp->name != "sstream") && (tmp->name != "Variant"))
      {
	if(tmp->InheritsFrom(TA_taMatrix)) {
	  taBase* inst = (taBase*)tmp->GetInstance();
	  if(inst) taBase::Ref(inst); // ref it!
	  cssMisc::TypesSpace.Push(new cssTA_Matrix(inst, 1, tmp, tmp->name));
	}
	else if(tmp->IsTaBase()) {
	  taBase* inst = (taBase*)tmp->GetInstance();
	  if(inst) taBase::Ref(inst); // ref it!
	  cssTA_Base_inst_nm(cssMisc::TypesSpace, inst, 1, tmp, tmp->name);
	}
	else {
	  cssTA_inst_nm(cssMisc::TypesSpace, tmp->GetInstance(), 1, tmp, tmp->name);
	}
      }
    }
    else if(tmp->IsEnum()) {
      if((tmp->name != "SigLinkSignal") && (tmp->name != "CancelOp") &&
	 (tmp->name != "NodeBitmapFlags")) {
	cssEnum_inst_nm(cssMisc::Enums, 0, tmp->name);
	for(j=0; j < tmp->enum_vals.size; j++)
	  cssInt_inst_nm(cssMisc::Enums, tmp->enum_vals.FastEl(j)->enum_no,
			 tmp->enum_vals.FastEl(j)->name);
      }
    }
    else if(tmp->IsFunction() && tmp->IsActual()) { // RegFun
      if(tmp->methods.size == 1) {
        MethodDef* md = tmp->methods[0];
        if(md->stubp != NULL) {
          cssMisc::HardFuns.Push(new cssMbrCFun(md->fun_argc, NULL, md->stubp,
                                                md->name));
        }
      }
    }
  }
  cssMisc::TypesSpace.Sort();		// must sort before anything happens
  GeneratePtrRefTypes();
}

//////////////////////////////////
//   Completion in Readline	//
//////////////////////////////////

extern "C" {
  typedef int rl_function(void);
  typedef char* rl_generator_fun(const char*, int);
  extern char *rl_line_buffer;
  extern char** (*rl_attempted_completion_function)(const char*, int, int);
  extern char** completion_matches (const char* text, rl_generator_fun* gen);

  extern char* css_path_generator(const char* text, int state); // rl_generator_fun
  extern char* css_scoped_generator(const char* text, int state); // rl_generator_fun
  extern char* css_keyword_generator(const char* text, int state); // rl_generator_fun
}

char** css_attempted_completion(const char* text, int start, int) {
  char** matches = NULL;
  String txt_str = text;

  // if starting with a quote, assume its a file name
  if((start > 0) && (rl_line_buffer[start-1] == '\"')) // "
    return matches;
  // if it starts with a dot, then its a path
  if(text[0] == '.')
    matches = completion_matches(text, css_path_generator);
  // check for a scoped type
  else if(txt_str.contains("::"))
    matches = completion_matches(text, css_scoped_generator);
  // otherwise, it must be a keyword of some sort..
  else
    matches = completion_matches(text, css_keyword_generator);

  return matches;
}

static char* css_malloc_string(const String& str) {
  char* rval = (char*)malloc((str.length()+1) * sizeof(char*));
  strcpy(rval, (const char*)str);
  return rval;
}

static char* css_cmd_gen_get_nm(cssSpace* spc, int& idx, const char* text, int len) {
  while(idx < spc->size) {
    cssEl* el = spc->FastEl(idx++);
    if(el->name(0,len) == text)
      return css_malloc_string(el->name);
  }
  return NULL;
}

char* css_keyword_generator(const char* text, int state) {
  static int spc_idx;		// index in spaces searching through
  static int item_idx;		// index in items searching through
  static int len;

  if(state == 0) {
    spc_idx = 0;
    item_idx = 0;
    len = (int)strlen(text); //64: can't possibly be >4G
  }

  cssSpace* spc = NULL;
  int extra_spcs = 2;		// both type spaces
  if(cssMisc::cur_class != NULL)
    extra_spcs++;

  if(spc_idx == 0)
    spc = &(cssMisc::cur_top->types);
  else if(spc_idx == 1)
    spc = &cssMisc::TypesSpace;
  else if((cssMisc::cur_class != NULL) && (spc_idx == 2))
    spc = cssMisc::cur_class->types;
  else
    spc = cssMisc::cur_top->GetParseSpace(spc_idx-extra_spcs);

  char* rval = NULL;
  while((spc != NULL) && ((rval = css_cmd_gen_get_nm(spc, item_idx, text, len)) == NULL)) {
    item_idx = 0;
    spc_idx++;			// move to next index
    if(spc_idx == 1)
      spc = &cssMisc::TypesSpace;
    else if((cssMisc::cur_class != NULL) && (spc_idx == 2))
      spc = cssMisc::cur_class->types;
    else
      spc = cssMisc::cur_top->GetParseSpace(spc_idx - extra_spcs);
  }
  return rval;
}

static String* css_path_search(TypeDef* td, int lst_idx, int& idx, String& mb_name) {
  if(lst_idx == 0) {
    while(idx < td->members.size) {
      MemberDef* md = td->members.FastEl(idx++);
      if(md->name(0,mb_name.length()) == mb_name)
	return &(md->name);
    }
    return NULL;
  }
  else {
    while(idx < td->methods.size) {
      MethodDef* md = td->methods.FastEl(idx++);
      if(md->name(0,mb_name.length()) == mb_name)
	return &(md->name);
    }
    return NULL;
  }
}

char* css_path_generator(const char* text, int state) {
  static int lst_idx;		// list index
  static int item_idx;		// index for searching within list
  static String mb_name;
  static String par_path;
  static taBase* parent;
  static TypeDef* par_td;

  if(cssBI::root == NULL)
    return NULL;

  if(state == 0) {
    par_path = text;
    par_path = par_path.before('.',-1);
    if(par_path == "") {
      parent = (taBase*)cssBI::root->ptr;
      if(parent != NULL)
	par_td = parent->GetTypeDef();
    }
    else {
      MemberDef* md = NULL;
      taBase* root = (taBase*)cssBI::root->ptr;
      if(root == NULL)
	parent = NULL;
      else {
	parent = root->FindFromPath(par_path, md);
	if(parent != NULL) {
	  par_td = parent->GetTypeDef();
// no need to do this..
//	  par_path = parent->GetPath();
	}
      }
    }
    mb_name = text;
    mb_name = mb_name.after('.', -1);
    item_idx = 0;
    lst_idx = 0;
  }

  if(parent == NULL)
    return NULL;

  String* rval = NULL;
  while((lst_idx < 2) &&
	((rval = css_path_search(par_td, lst_idx, item_idx, mb_name)) == NULL))
  {
    item_idx = 0;
    lst_idx++;
  }

  if(rval == NULL)
    return NULL;

  String full_val = par_path;
  full_val += ".";
  full_val += *rval;
  return css_malloc_string(full_val);
}

static String* css_scope_search(TypeDef* td, int lst_idx, int& idx, int& sub_itm,
				String& mb_name)
{
  if(lst_idx == 0) {
    while(idx < td->members.size) {
      MemberDef* md = td->members.FastEl(idx++);
      if(md->name(0,mb_name.length()) == mb_name)
	return &(md->name);
    }
    return NULL;
  }
  else if(lst_idx == 1) {
    while(idx < td->methods.size) {
      MethodDef* md = td->methods.FastEl(idx++);
      if(md->name(0,mb_name.length()) == mb_name)
	return &(md->name);
    }
    return NULL;
  }
  else {
    while(idx < td->sub_types.size) {
      TypeDef* st = td->sub_types.FastEl(idx);
      if(st->IsEnum()) {
	while(sub_itm < st->enum_vals.size) {
	  EnumDef* en = st->enum_vals.FastEl(sub_itm++);
	  if(en->name(0,mb_name.length()) == mb_name)
	    return &(en->name);
	}
      }
      idx++;
      sub_itm = 0;
      if(st->name(0,mb_name.length()) == mb_name)
	return &(st->name);
    }
    return NULL;
  }
}

char* css_scoped_generator(const char* text, int state) {
  static int lst_idx;		// list index
  static int item_idx;		// index for searching within list
  static int sub_item_idx;	// index of sub-item within item (if applic)
  static String mb_name;
  static String par_path;
  static TypeDef* par_td;

  if(state == 0) {
    par_path = text;
    par_path = par_path.before("::",-1);
    par_td = taMisc::FindTypeName(par_path);
    mb_name = text;
    mb_name = mb_name.after("::", -1);
    item_idx = 0;
    lst_idx = 0;
    sub_item_idx = 0;
  }

  if(par_td == NULL)
    return NULL;

  String* rval = NULL;
  while((lst_idx < 3) &&
	((rval = css_scope_search(par_td, lst_idx, item_idx, sub_item_idx, mb_name))
	 == NULL)) {
    item_idx = 0;
    sub_item_idx = 0;
    lst_idx++;
  }

  if(rval == NULL)
    return NULL;

  String full_val = par_path;
  full_val += "::";
  full_val += *rval;
  return css_malloc_string(full_val);
}

//////////////////////////////////
//	Initialize		//
//////////////////////////////////

extern int yydebug;

extern "C" {
  typedef int rl_function(void);
  extern char** (*rl_attempted_completion_function)(const char*, int, int);
}

bool cssMisc::Initialize() {
  // use our completion function
  rl_attempted_completion_function = css_attempted_completion;

  cssMisc::proc_events_timer.start(); // always start initialized

  // functions
  cssEl::Ref(&cssMisc::Void);		// reference this to keep it around

  cssMisc::Top = new cssProgSpace("css");	// top level
  cssMisc::ConstExprTop = new cssProgSpace("Constant Expression Top Level");
  cssMisc::ConstExpr = cssMisc::ConstExprTop->Prog(); // this guy is child of top
  cssMisc::CDtorProg = new cssProg("Constructor/Destructor Prog");
  cssMisc::CDtorProg->state |= cssProg::State_IsTmpProg;
  cssMisc::CallFunProg = new cssProg("CallFun Prog");
  cssMisc::CallFunProg->state |= cssProg::State_IsTmpProg;
  cssProg::Ref(cssMisc::CDtorProg);
  cssProg::Ref(cssMisc::CallFunProg);
  cssMisc::cur_top = cssMisc::Top;

  cssMisc::TopShell = new cssCmdShell("css shell");
  //   cssMisc::TopShell->PushSrcProg(cssMisc::Top); // this happens when specific
  // shell is constructed, in css_machine cmdshell code

  Install_Internals();
  Install_Commands();
  Install_Types();
  Install_Math();
  Install_MiscFun();

  cssMisc::s_argv = new cssArray(1, cssMisc::VoidString.Clone(), "argv");
  cssMisc::s_argc = new cssInt(0, "argc");
  cssMisc::HardVars.Push(cssMisc::s_argv);
  cssMisc::HardVars.Push(cssMisc::s_argc);

  // setting variables
  cssMisc::Settings.Push(new cssTA(&taMisc::css_include_paths, 1, &TA_String_PArray,
				    "css_include_paths"));

  cssMisc::Settings.Push(new cssTA(TA_taMisc.GetInstance(), 1, &TA_taMisc,
				    "cssSettings"));

  cssMisc::HardVars.Push(cssBI::root = new cssTA_Base(tabMisc::root, 1, &TA_taRootBase, "root"));
  cssMisc::Top->name = cssMisc::prompt;	// changes prompt?

  cssMisc::Commands.Sort();
  cssMisc::Functions.Sort();		// must sort before anything happens
  cssMisc::Constants.Sort();		// must sort before anything happens

  //////////////////////////////////////////////////////////////
  // 	Process Args
  
  // note: these names are defined in ta_project.cpp: taRootBase::Startup_InitArgs
  cssMisc::startup_file = taMisc::FindArgByName("CssScript");
  cssMisc::startup_code = taMisc::FindArgByName("CssCode");
  if(!cssMisc::startup_code.empty())
    cssMisc::startup_code += "\n";		  // add a final cr for good measure

  cssMisc::init_debug = taMisc::FindArgByName("CssDebug");
  String ibp = taMisc::FindArgByName("CssBreakpoint");
  if(!ibp.empty()) cssMisc::init_bpoint = ibp;
  cssMisc::refcnt_trace = taMisc::CheckArgByName("RefCountTrace");

  if(cssMisc::init_debug > 2) {
    yydebug = 1;		// this prints parse trace
  }

  //////////////////////////////////////////////////////////////////////////////
  // support legacy v3 arg info: new projects should use taMisc::*Arg* code

  cssMisc::s_argv->Alloc(taMisc::args_raw.size); // allocate proper number of args
  *cssMisc::s_argc = 0;
  for(int i=0;i<taMisc::args_raw.size;i++) {
    String tmp = taMisc::args_raw[i];

    if(((tmp == "-f") || (tmp == "-file")) && (i+1 < taMisc::args_raw.size)) {
      i++;			// skip over next
    }
    else if(((tmp == "-e") || (tmp == "-exec")) && (i+1 < taMisc::args_raw.size)) {
      i++;			// skip over next
    }
    else if((tmp == "-i") || (tmp == "-interactive")) {
    }
    else if(tmp(0,2) == "-v") {
    }
    else if(((tmp == "-b") || (tmp == "-bp")) && (i+1 < taMisc::args_raw.size)) {
      i++;			// skip over next
    }
    else {	   // pass the arg to the script
      int ac = (int)(*cssMisc::s_argc);
      cssString* av = (cssString*)(*cssMisc::s_argv)[(Variant)ac];
      *av = tmp;
      *cssMisc::s_argc = (int)(*cssMisc::s_argc) + 1;
    }
  }

  return true;
}


//////////////////////////////////
//	Shutdown		//
//////////////////////////////////

bool cssMisc::Shutdown() {
  if(cur_top)
    cur_top->Stop();
  if(TopShell) {
    TopShell->Exit();
    delete TopShell;
    TopShell = NULL;
  }
  if(Top) {
    Top->Stop();
    delete Top;
    Top = NULL;
  }
  if(ConstExprTop) {
    ConstExprTop->Stop();
    delete ConstExprTop;
    ConstExprTop = NULL;
  }
  ConstExpr = NULL;
  if(CDtorProg) {
    delete CDtorProg;
    CDtorProg = NULL;
  }
  if(CallFunProg) {
    delete CallFunProg;
    CallFunProg = NULL;
  }
    
  cur_top = NULL;
  code_cur_top = NULL;
  cur_class = NULL;
  cur_scope = NULL;
  cur_method = NULL;
  cur_enum = NULL;
  default_args.Reset();
  s_argv = NULL;
  s_argc = NULL;
  
  Defines.Reset();
  Settings.Reset();
  Enums.Reset();
  Constants.Reset();
  Functions.Reset();
  Commands.Reset();
  HardVars.Reset();
  HardFuns.Reset();
  Externs.Reset();
  PreProcessor.Reset();
  Parse.Reset();
  Internal.Reset();
  TypesSpace_ptrs.Reset();
  TypesSpace_refs.Reset();
  TypesSpace.Reset();

  return true;
}

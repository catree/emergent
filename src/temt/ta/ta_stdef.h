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


#ifndef ta_stdef_h
#define ta_stdef_h 1

// standard definitions, etc

#include "ta_global.h"

#ifdef __MAKETA__
//NOTE: must use "" for msvc
#  include "maketa_iostream.h"
#  include "maketa_fstream.h"
#  include "maketa_sstream.h"
#else
# include <iostream>
# include <fstream>
// note: usually considered bad practice to put 'using' in a header file, but we have no choice
// because MAKETA will choke on std::item syntax
using namespace std;
#endif

// some files used pretty much everywhere
//#include "ta_string.h"
//#include "icolor.h"

#ifdef List
#undef List
#endif

#if (defined(TA_OS_SOLARIS) && !(defined(__MAKETA__)))
#include <sys/types.h>
#endif


// Posix Signal function definitions

#define SIGNAL_PROC_FUN_TYPE void (*)(int)
#define SIGNAL_PROC_FUN_ARG(x) void (*x)(int)

// Some misc. compatability defines -- made porting to Qt easier


#ifdef TA_USE_QT
// dummy defines for Qt -- its header files are too complex and parameterized for maketa
#  include "ta_maketa_defs.h"
#endif

// required for ODE -- we use single precision only
#define dSINGLE

// Some global forward declarations

class TypeDef;
class MemberDef;
class MethodDef;


#endif // ta_stdef_h

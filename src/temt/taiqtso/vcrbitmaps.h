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


#ifndef vcrbitmaps_h
#define vcrbitmaps_h

#include "taiqtso_def.h"

#define VCR_BUTTON_WIDTH 32

// full rewind

#define fr_width 12
#define fr_height 12
static char TAIQTSO_API fr_bits[] = {
   0x04, 0x0c, 0x0e, 0x0e, 0x0e, 0x0f, 0x8e, 0x0f, 0xce, 0x0f, 0xee, 0x0f,
   0xee, 0x0f, 0xce, 0x0f, 0x8e, 0x0f, 0x0e, 0x0f, 0x0e, 0x0e, 0x04, 0x0c};

// fast rewind

#define fsr_width 12
#define fsr_height 12
static char TAIQTSO_API fsr_bits[] = {
   0x20, 0x08, 0x30, 0x0c, 0x38, 0x0e, 0x3c, 0x0f, 0xbe, 0x0f, 0xff, 0x0f,
   0xff, 0x0f, 0xbe, 0x0f, 0x3c, 0x0f, 0x38, 0x0e, 0x30, 0x0c, 0x20, 0x08};


// rewind

#define r_width 12
#define r_height 12
static char TAIQTSO_API r_bits[] = {
   0x00, 0x0c, 0x00, 0x0e, 0x00, 0x0f, 0x80, 0x0f, 0xc0, 0x0f, 0xe0, 0x0f,
   0xe0, 0x0f, 0xc0, 0x0f, 0x80, 0x0f, 0x00, 0x0f, 0x00, 0x0e, 0x00, 0x0c};

// forward

#define f_width 12
#define f_height 12
static char TAIQTSO_API f_bits[] = {
   0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x1f, 0x00, 0x3f, 0x00, 0x7f, 0x00,
   0x7f, 0x00, 0x3f, 0x00, 0x1f, 0x00, 0x0f, 0x00, 0x07, 0x00, 0x03, 0x00};

// fast forward

#define fsf_width 12
#define fsf_height 12
static char TAIQTSO_API fsf_bits[] = {
   0x41, 0x00, 0xc3, 0x00, 0xc7, 0x01, 0xcf, 0x03, 0xdf, 0x07, 0xff, 0x0f,
   0xff, 0x0f, 0xdf, 0x07, 0xcf, 0x03, 0xc7, 0x01, 0xc3, 0x00, 0x41, 0x00};

// full forward

#define ff_width 12
#define ff_height 12
static char TAIQTSO_API ff_bits[] = {
   0x03, 0x02, 0x07, 0x07, 0x0f, 0x07, 0x1f, 0x07, 0x3f, 0x07, 0x7f, 0x07,
   0x7f, 0x07, 0x3f, 0x07, 0x1f, 0x07, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x02};



#endif



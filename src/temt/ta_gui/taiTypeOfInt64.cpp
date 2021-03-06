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

#include "taiTypeOfInt64.h"
#include <BuiltinTypeDefs>


int taiTypeOfInt64::BidForType(TypeDef* td){
  // we handle all 64-bit types
  if (td->DerivesFrom(&TA_int64_t) || td->DerivesFrom(&TA_uint64_t))
    return (taiType::BidForType(td) +1);
  return 0;
}

//TODO: we really are still just using the taiType defaults
// need to create a 64-bit spin, or at least a customized edit

// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#include "HippoEncoderConSpec.h"

void HippoEncoderConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.005f;		// works better with tiny bit of hebb apparently..
  lmix.err = 1.0f;
//   lmix.err_sb = false;

  // SetUnique("wt_limits", true);
  // wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 6.0f;  wt_sig.off = 1.0f; // 1.0f seems to work better actually

  SetUnique("savg_cor", true);
  savg_cor.cor = 1.0f;

//   SetUnique("xcalm", true);
//   xcalm.use_sb = false;
}

void HippoEncoderConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  // actually it does so turning this off!
  // lmix.hebb = 0.0f;
  // lmix.err = 1.0f;
}

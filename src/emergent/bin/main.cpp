// Copyright, 1995-2007, Regents of the University of Colorado,
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


#include <taMisc>
#include <EmergentMisc>

int main(int argc, const char* argv[]) {
  taMisc::app_name = "emergent";
  taMisc::app_prefs_key = "emergent"; // note: we share prefs, etc. w/ css
//taMisc::default_app_install_folder_name = "Emergent";
  taMisc::use_plugins = true;
  return EmergentMisc::Main(argc, argv);
}

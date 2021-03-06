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

#include "EmergentMisc.h"

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

eTypeDef_Of(EmergentRoot);

#include <css_machine.h>

#ifdef TA_GUI
# include <QApplication>
# include <QPixmap>
# include <QIcon>
#endif

#ifdef TA_GUI
#define emergent_bitmap_width 64
#define emergent_bitmap_height 64
static const unsigned char emergent_bitmap_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x83, 0xff,
  0x1f, 0xfc, 0xff, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x1f, 0x82, 0xf8,
  0x10, 0xc4, 0x87, 0x00, 0x10, 0x32, 0x82, 0x10, 0x11, 0x84, 0x8c, 0x00,
  0x10, 0x22, 0x82, 0x10, 0x11, 0x84, 0x88, 0x00, 0x10, 0x32, 0x82, 0x10,
  0x11, 0x84, 0x8c, 0x00, 0x10, 0x1e, 0x82, 0x10, 0x11, 0x84, 0x87, 0x00,
  0x10, 0x02, 0x82, 0x10, 0x11, 0x84, 0x80, 0x00, 0x10, 0x02, 0x82, 0x10,
  0x11, 0x84, 0x80, 0x00, 0x10, 0x0f, 0x82, 0xf8, 0x10, 0xc4, 0x83, 0x00,
  0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00,
  0x10, 0x04, 0x80, 0x00, 0x10, 0x00, 0x82, 0x00, 0x10, 0x04, 0x80, 0x00,
  0xf0, 0xff, 0x83, 0xff, 0x1f, 0xfc, 0xff, 0x00, 0x00, 0x78, 0x00, 0x50,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x90, 0x03, 0x88, 0x00, 0x4e, 0x00, 0x00,
  0x00, 0x20, 0x0c, 0x04, 0xc1, 0x21, 0x00, 0x00, 0x00, 0x40, 0x70, 0x02,
  0x3a, 0x10, 0x00, 0x00, 0x00, 0x40, 0x80, 0x03, 0x06, 0x08, 0x00, 0x00,
  0x00, 0x80, 0x80, 0xdc, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
  0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x22, 0x07, 0x13, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xf4, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07,
  0xff, 0x3f, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x08, 0x04, 0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x08, 0x04,
  0x41, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04,
  0x01, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x01, 0x20, 0x00, 0x00,
  0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x01,
  0x7c, 0x01, 0x00, 0x00, 0x00, 0x00, 0x42, 0x9e, 0x23, 0x06, 0x00, 0x00,
  0x00, 0x00, 0x81, 0xf9, 0x11, 0x08, 0x00, 0x00, 0x00, 0x80, 0x00, 0x07,
  0x1e, 0x10, 0x00, 0x00, 0x00, 0x40, 0xf0, 0x0c, 0xe2, 0x21, 0x00, 0x00,
  0x00, 0x20, 0x0e, 0x10, 0x01, 0xde, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xe0,
  0x00, 0xe0, 0x01, 0x00, 0xe0, 0xff, 0x07, 0xff, 0x3f, 0xf8, 0xff, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x3e, 0x04, 0xf1, 0x21, 0x88, 0x0f, 0x01,
  0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01, 0x20, 0x44, 0x04, 0x21,
  0x22, 0x08, 0x11, 0x01, 0x20, 0x64, 0x04, 0x21, 0x22, 0x08, 0x19, 0x01,
  0x20, 0x3c, 0x04, 0x21, 0x22, 0x08, 0x0f, 0x01, 0x20, 0x04, 0x04, 0x21,
  0x22, 0x08, 0x01, 0x01, 0x20, 0x04, 0x04, 0x21, 0x22, 0x08, 0x01, 0x01,
  0x20, 0x1e, 0x04, 0xf1, 0x21, 0x88, 0x07, 0x01, 0x20, 0x00, 0x04, 0x01,
  0x20, 0x08, 0x00, 0x01, 0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01,
  0x20, 0x00, 0x04, 0x01, 0x20, 0x08, 0x00, 0x01, 0xe0, 0xff, 0x07, 0xff,
  0x3f, 0xf8, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#endif


//////////////////////////
//	EmergentMisc	//
//////////////////////////

int EmergentMisc::Main(int& argc, const char *argv[]) {
  taMisc::web_home = "https://grey.colorado.edu/emergent/index.php/Main_Page";
  taMisc::web_help_wiki = "emergent";
  taMisc::web_help_general = "https://grey.colorado.edu/emergent/index.php/User_hub";

  taMisc::wiki1_url = NamedURL("emergent", "https://grey.colorado.edu/emergent");
  taMisc::wiki2_url = NamedURL("CCN", "https://grey.colorado.edu/CompCogNeuro");

  if(!taRootBase::Startup_Main(argc, argv, &TA_EmergentRoot)) return 1;
#ifdef TA_GUI
  if(taMisc::gui_active) {
    QPixmap* pm = new QPixmap((const char*)emergent_bitmap_bits);
    qApp->setWindowIcon(*pm);
    delete pm;
  }
#endif
  cssMisc::TopShell->cmd_prog->CompileRunClear(".emergentrc");
  if(taRootBase::Startup_Run())
    return 0;
  else
    return 2;
}


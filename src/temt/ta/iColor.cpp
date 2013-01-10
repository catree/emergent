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

#include "iColor.h"

#include "icolor.h"

#ifdef TA_GUI
# include <qcolor.h>
# include <qbrush.h>
#endif

#ifdef TA_USE_INVENTOR
# include <Inventor/fields/SoMFColor.h>
#endif

#include <string.h>

// ------------------------------------------------------------------
// translates a color name to the X11 string format of an rgb
// specification (ie #??????).  The colormap name is basically a
// section in the colormap.ini file.
// ------------------------------------------------------------------
// oreilly: just put the entire thing in here so installation is easy -- no
// dependency on app defaults or other install location files at all

static struct { const char* name; int value; } cc[] = {
  {"snow", 0xfffafa},  {"ghost white", 0xf8f8ff},  {"GhostWhite", 0xf8f8ff},
  {"white smoke", 0xf5f5f5},  {"WhiteSmoke", 0xf5f5f5},  {"gainsboro", 0xdcdcdc},
  {"floral white", 0xfffaf0},  {"FloralWhite", 0xfffaf0},  {"old lace", 0xfdf5e6},
  {"OldLace", 0xfdf5e6},  {"linen", 0xfaf0e6},  {"antique white", 0xfaebd7},
  {"AntiqueWhite", 0xfaebd7},  {"papaya whip", 0xffefd5},  {"PapayaWhip", 0xffefd5},
  {"blanched almond", 0xffebcd},  {"BlanchedAlmond", 0xffebcd},  {"bisque", 0xffe4c4},
  {"peach puff", 0xffdab9},  {"PeachPuff", 0xffdab9},  {"navajo white", 0xffdead},
  {"NavajoWhite", 0xffdead},  {"moccasin", 0xffe4b5},  {"cornsilk", 0xfff8dc},
  {"ivory", 0xfffff0},  {"lemon chiffon", 0xfffacd},  {"LemonChiffon", 0xfffacd},
  {"seashell", 0xfff5ee},  {"honeydew", 0xf0fff0},  {"mint cream", 0xf5fffa},
  {"MintCream", 0xf5fffa},  {"azure", 0xf0ffff},  {"alice blue", 0xf0f8ff},
  {"AliceBlue", 0xf0f8ff},  {"lavender", 0xe6e6fa},  {"lavender blush", 0xfff0f5},
  {"LavenderBlush", 0xfff0f5},  {"misty rose", 0xffe4e1},  {"MistyRose", 0xffe4e1},
  {"white", 0xffffff},  {"black", 0x000000},  {"dark slate gray", 0x2f4f4f},
  {"DarkSlateGray", 0x2f4f4f},  {"dark slate grey", 0x2f4f4f},  {"DarkSlateGrey", 0x2f4f4f},
  {"dim gray", 0x696969},  {"DimGray", 0x696969},  {"dim grey", 0x696969},
  {"DimGrey", 0x696969},  {"slate gray", 0x708090},  {"SlateGray", 0x708090},
  {"slate grey", 0x708090},  {"SlateGrey", 0x708090},  {"light slate gray", 0x778899},
  {"LightSlateGray", 0x778899},  {"light slate grey", 0x778899},  {"LightSlateGrey", 0x778899},
  {"gray", 0xbebebe},  {"grey", 0xbebebe},  {"light grey", 0xd3d3d3},
  {"LightGrey", 0xd3d3d3},  {"light gray", 0xd3d3d3},  {"LightGray", 0xd3d3d3},
  {"midnight blue", 0x191970},  {"MidnightBlue", 0x191970},  {"navy", 0x000080},
  {"navy blue", 0x000080},  {"NavyBlue", 0x000080},  {"cornflower blue", 0x6495ed},
  {"CornflowerBlue", 0x6495ed},  {"dark slate blue", 0x483d8b},  {"DarkSlateBlue", 0x483d8b},
  {"slate blue", 0x6a5acd},  {"SlateBlue", 0x6a5acd},  {"medium slate blue", 0x7b68ee},
  {"MediumSlateBlue", 0x7b68ee},  {"light slate blue", 0x8470ff},  {"LightSlateBlue", 0x8470ff},
  {"medium blue", 0x0000cd},  {"MediumBlue", 0x0000cd},  {"royal blue", 0x4169e1},
  {"RoyalBlue", 0x4169e1},  {"blue", 0x0000ff},  {"dodger blue", 0x1e90ff},
  {"DodgerBlue", 0x1e90ff},  {"deep sky blue", 0x00bfff},  {"DeepSkyBlue", 0x00bfff},
  {"sky blue", 0x87ceeb},  {"SkyBlue", 0x87ceeb},  {"light sky blue", 0x87cefa},
  {"LightSkyBlue", 0x87cefa},  {"steel blue", 0x4682b4},  {"SteelBlue", 0x4682b4},
  {"light steel blue", 0xb0c4de},  {"LightSteelBlue", 0xb0c4de},  {"light blue", 0xadd8e6},
  {"LightBlue", 0xadd8e6},  {"powder blue", 0xb0e0e6},  {"PowderBlue", 0xb0e0e6},
  {"pale turquoise", 0xafeeee},  {"PaleTurquoise", 0xafeeee},  {"dark turquoise", 0x00ced1},
  {"DarkTurquoise", 0x00ced1},  {"medium turquoise", 0x48d1cc},  {"MediumTurquoise", 0x48d1cc},
  {"turquoise", 0x40e0d0},  {"cyan", 0x00ffff},  {"light cyan", 0xe0ffff},
  {"LightCyan", 0xe0ffff},  {"cadet blue", 0x5f9ea0},  {"CadetBlue", 0x5f9ea0},
  {"medium aquamarine", 0x66cdaa},  {"MediumAquamarine", 0x66cdaa},  {"aquamarine", 0x7fffd4},
  {"dark green", 0x006400},  {"DarkGreen", 0x006400},  {"dark olive green", 0x556b2f},
  {"DarkOliveGreen", 0x556b2f},  {"dark sea green", 0x8fbc8f},  {"DarkSeaGreen", 0x8fbc8f},
  {"sea green", 0x2e8b57},  {"SeaGreen", 0x2e8b57},  {"medium sea green", 0x3cb371},
  {"MediumSeaGreen", 0x3cb371},  {"light sea green", 0x20b2aa},  {"LightSeaGreen", 0x20b2aa},
  {"pale green", 0x98fb98},  {"PaleGreen", 0x98fb98},  {"spring green", 0x00ff7f},
  {"SpringGreen", 0x00ff7f},  {"lawn green", 0x7cfc00},  {"LawnGreen", 0x7cfc00},
  {"green", 0x00ff00},  {"chartreuse", 0x7fff00},  {"medium spring green", 0x00fa9a},
  {"MediumSpringGreen", 0x00fa9a},  {"green yellow", 0xadff2f},  {"GreenYellow", 0xadff2f},
  {"lime green", 0x32cd32},  {"LimeGreen", 0x32cd32},  {"yellow green", 0x9acd32},
  {"YellowGreen", 0x9acd32},  {"forest green", 0x228b22},  {"ForestGreen", 0x228b22},
  {"olive drab", 0x6b8e23},  {"OliveDrab", 0x6b8e23},  {"dark khaki", 0xbdb76b},
  {"DarkKhaki", 0xbdb76b},  {"khaki", 0xf0e68c},  {"pale goldenrod", 0xeee8aa},
  {"PaleGoldenrod", 0xeee8aa},  {"light goldenrod yellow", 0xfafad2},  {"LightGoldenrodYellow", 0xfafad2},
  {"light yellow", 0xffffe0},  {"LightYellow", 0xffffe0},  {"yellow", 0xffff00},
  {"gold", 0xffd700},  {"light goldenrod", 0xeedd82},  {"LightGoldenrod", 0xeedd82},
  {"goldenrod", 0xdaa520},  {"dark goldenrod", 0xb8860b},  {"DarkGoldenrod", 0xb8860b},
  {"rosy brown", 0xbc8f8f},  {"RosyBrown", 0xbc8f8f},  {"indian red", 0xcd5c5c},
  {"IndianRed", 0xcd5c5c},  {"saddle brown", 0x8b4513},  {"SaddleBrown", 0x8b4513},
  {"sienna", 0xa0522d},  {"peru", 0xcd853f},  {"burlywood", 0xdeb887},
  {"beige", 0xf5f5dc},  {"wheat", 0xf5deb3},  {"sandy brown", 0xf4a460},
  {"SandyBrown", 0xf4a460},  {"tan", 0xd2b48c},  {"chocolate", 0xd2691e},
  {"firebrick", 0xb22222},  {"brown", 0xa52a2a},  {"dark salmon", 0xe9967a},
  {"DarkSalmon", 0xe9967a},  {"salmon", 0xfa8072},  {"light salmon", 0xffa07a},
  {"LightSalmon", 0xffa07a},  {"orange", 0xffa500},  {"dark orange", 0xff8c00},
  {"DarkOrange", 0xff8c00},  {"coral", 0xff7f50},  {"light coral", 0xf08080},
  {"LightCoral", 0xf08080},  {"tomato", 0xff6347},  {"orange red", 0xff4500},
  {"OrangeRed", 0xff4500},  {"red", 0xff0000},  {"hot pink", 0xff69b4},
  {"HotPink", 0xff69b4},  {"deep pink", 0xff1493},  {"DeepPink", 0xff1493},
  {"pink", 0xffc0cb},  {"light pink", 0xffb6c1},  {"LightPink", 0xffb6c1},
  {"pale violet red", 0xdb7093},  {"PaleVioletRed", 0xdb7093},  {"maroon", 0xb03060},
  {"medium violet red", 0xc71585},  {"MediumVioletRed", 0xc71585},  {"violet red", 0xd02090},
  {"VioletRed", 0xd02090},  {"magenta", 0xff00ff},  {"violet", 0xee82ee},
  {"plum", 0xdda0dd},  {"orchid", 0xda70d6},  {"medium orchid", 0xba55d3},
  {"MediumOrchid", 0xba55d3},  {"dark orchid", 0x9932cc},  {"DarkOrchid", 0x9932cc},
  {"dark violet", 0x9400d3},  {"DarkViolet", 0x9400d3},  {"blue violet", 0x8a2be2},
  {"BlueViolet", 0x8a2be2},  {"purple", 0xa020f0},  {"medium purple", 0x9370db},
  {"MediumPurple", 0x9370db},  {"thistle", 0xd8bfd8},  {"snow1", 0xfffafa},
  {"snow2", 0xeee9e9},  {"snow3", 0xcdc9c9},  {"snow4", 0x8b8989},
  {"seashell1", 0xfff5ee},  {"seashell2", 0xeee5de},  {"seashell3", 0xcdc5bf},
  {"seashell4", 0x8b8682},  {"AntiqueWhite1", 0xffefdb},  {"AntiqueWhite2", 0xeedfcc},
  {"AntiqueWhite3", 0xcdc0b0},  {"AntiqueWhite4", 0x8b8378},  {"bisque1", 0xffe4c4},
  {"bisque2", 0xeed5b7},  {"bisque3", 0xcdb79e},  {"bisque4", 0x8b7d6b},
  {"PeachPuff1", 0xffdab9},  {"PeachPuff2", 0xeecbad},  {"PeachPuff3", 0xcdaf95},
  {"PeachPuff4", 0x8b7765},  {"NavajoWhite1", 0xffdead},  {"NavajoWhite2", 0xeecfa1},
  {"NavajoWhite3", 0xcdb38b},  {"NavajoWhite4", 0x8b795e},  {"LemonChiffon1", 0xfffacd},
  {"LemonChiffon2", 0xeee9bf},  {"LemonChiffon3", 0xcdc9a5},  {"LemonChiffon4", 0x8b8970},
  {"cornsilk1", 0xfff8dc},  {"cornsilk2", 0xeee8cd},  {"cornsilk3", 0xcdc8b1},
  {"cornsilk4", 0x8b8878},  {"ivory1", 0xfffff0},  {"ivory2", 0xeeeee0},
  {"ivory3", 0xcdcdc1},  {"ivory4", 0x8b8b83},  {"honeydew1", 0xf0fff0},
  {"honeydew2", 0xe0eee0},  {"honeydew3", 0xc1cdc1},  {"honeydew4", 0x838b83},
  {"LavenderBlush1", 0xfff0f5},  {"LavenderBlush2", 0xeee0e5},  {"LavenderBlush3", 0xcdc1c5},
  {"LavenderBlush4", 0x8b8386},  {"MistyRose1", 0xffe4e1},  {"MistyRose2", 0xeed5d2},
  {"MistyRose3", 0xcdb7b5},  {"MistyRose4", 0x8b7d7b},  {"azure1", 0xf0ffff},
  {"azure2", 0xe0eeee},  {"azure3", 0xc1cdcd},  {"azure4", 0x838b8b},
  {"SlateBlue1", 0x836fff},  {"SlateBlue2", 0x7a67ee},  {"SlateBlue3", 0x6959cd},
  {"SlateBlue4", 0x473c8b},  {"RoyalBlue1", 0x4876ff},  {"RoyalBlue2", 0x436eee},
  {"RoyalBlue3", 0x3a5fcd},  {"RoyalBlue4", 0x27408b},  {"blue1", 0x0000ff},
  {"blue2", 0x0000ee},  {"blue3", 0x0000cd},  {"blue4", 0x00008b},
  {"DodgerBlue1", 0x1e90ff},  {"DodgerBlue2", 0x1c86ee},  {"DodgerBlue3", 0x1874cd},
  {"DodgerBlue4", 0x104e8b},  {"SteelBlue1", 0x63b8ff},  {"SteelBlue2", 0x5cacee},
  {"SteelBlue3", 0x4f94cd},  {"SteelBlue4", 0x36648b},  {"DeepSkyBlue1", 0x00bfff},
  {"DeepSkyBlue2", 0x00b2ee},  {"DeepSkyBlue3", 0x009acd},  {"DeepSkyBlue4", 0x00688b},
  {"SkyBlue1", 0x87ceff},  {"SkyBlue2", 0x7ec0ee},  {"SkyBlue3", 0x6ca6cd},
  {"SkyBlue4", 0x4a708b},  {"LightSkyBlue1", 0xb0e2ff},  {"LightSkyBlue2", 0xa4d3ee},
  {"LightSkyBlue3", 0x8db6cd},  {"LightSkyBlue4", 0x607b8b},  {"SlateGray1", 0xc6e2ff},
  {"SlateGray2", 0xb9d3ee},  {"SlateGray3", 0x9fb6cd},  {"SlateGray4", 0x6c7b8b},
  {"LightSteelBlue1", 0xcae1ff},  {"LightSteelBlue2", 0xbcd2ee},  {"LightSteelBlue3", 0xa2b5cd},
  {"LightSteelBlue4", 0x6e7b8b},  {"LightBlue1", 0xbfefff},  {"LightBlue2", 0xb2dfee},
  {"LightBlue3", 0x9ac0cd},  {"LightBlue4", 0x68838b},  {"LightCyan1", 0xe0ffff},
  {"LightCyan2", 0xd1eeee},  {"LightCyan3", 0xb4cdcd},  {"LightCyan4", 0x7a8b8b},
  {"PaleTurquoise1", 0xbbffff},  {"PaleTurquoise2", 0xaeeeee},  {"PaleTurquoise3", 0x96cdcd},
  {"PaleTurquoise4", 0x668b8b},  {"CadetBlue1", 0x98f5ff},  {"CadetBlue2", 0x8ee5ee},
  {"CadetBlue3", 0x7ac5cd},  {"CadetBlue4", 0x53868b},  {"turquoise1", 0x00f5ff},
  {"turquoise2", 0x00e5ee},  {"turquoise3", 0x00c5cd},  {"turquoise4", 0x00868b},
  {"cyan1", 0x00ffff},  {"cyan2", 0x00eeee},  {"cyan3", 0x00cdcd},
  {"cyan4", 0x008b8b},  {"DarkSlateGray1", 0x97ffff},  {"DarkSlateGray2", 0x8deeee},
  {"DarkSlateGray3", 0x79cdcd},  {"DarkSlateGray4", 0x528b8b},  {"aquamarine1", 0x7fffd4},
  {"aquamarine2", 0x76eec6},  {"aquamarine3", 0x66cdaa},  {"aquamarine4", 0x458b74},
  {"DarkSeaGreen1", 0xc1ffc1},  {"DarkSeaGreen2", 0xb4eeb4},  {"DarkSeaGreen3", 0x9bcd9b},
  {"DarkSeaGreen4", 0x698b69},  {"SeaGreen1", 0x54ff9f},  {"SeaGreen2", 0x4eee94},
  {"SeaGreen3", 0x43cd80},  {"SeaGreen4", 0x2e8b57},  {"PaleGreen1", 0x9aff9a},
  {"PaleGreen2", 0x90ee90},  {"PaleGreen3", 0x7ccd7c},  {"PaleGreen4", 0x548b54},
  {"SpringGreen1", 0x00ff7f},  {"SpringGreen2", 0x00ee76},  {"SpringGreen3", 0x00cd66},
  {"SpringGreen4", 0x008b45},  {"green1", 0x00ff00},  {"green2", 0x00ee00},
  {"green3", 0x00cd00},  {"green4", 0x008b00},  {"chartreuse1", 0x7fff00},
  {"chartreuse2", 0x76ee00},  {"chartreuse3", 0x66cd00},  {"chartreuse4", 0x458b00},
  {"OliveDrab1", 0xc0ff3e},  {"OliveDrab2", 0xb3ee3a},  {"OliveDrab3", 0x9acd32},
  {"OliveDrab4", 0x698b22},  {"DarkOliveGreen1", 0xcaff70},  {"DarkOliveGreen2", 0xbcee68},
  {"DarkOliveGreen3", 0xa2cd5a},  {"DarkOliveGreen4", 0x6e8b3d},  {"khaki1", 0xfff68f},
  {"khaki2", 0xeee685},  {"khaki3", 0xcdc673},  {"khaki4", 0x8b864e},
  {"LightGoldenrod1", 0xffec8b},  {"LightGoldenrod2", 0xeedc82},  {"LightGoldenrod3", 0xcdbe70},
  {"LightGoldenrod4", 0x8b814c},  {"LightYellow1", 0xffffe0},  {"LightYellow2", 0xeeeed1},
  {"LightYellow3", 0xcdcdb4},  {"LightYellow4", 0x8b8b7a},  {"yellow1", 0xffff00},
  {"yellow2", 0xeeee00},  {"yellow3", 0xcdcd00},  {"yellow4", 0x8b8b00},
  {"gold1", 0xffd700},  {"gold2", 0xeec900},  {"gold3", 0xcdad00},
  {"gold4", 0x8b7500},  {"goldenrod1", 0xffc125},  {"goldenrod2", 0xeeb422},
  {"goldenrod3", 0xcd9b1d},  {"goldenrod4", 0x8b6914},  {"DarkGoldenrod1", 0xffb90f},
  {"DarkGoldenrod2", 0xeead0e},  {"DarkGoldenrod3", 0xcd950c},  {"DarkGoldenrod4", 0x8b6508},
  {"RosyBrown1", 0xffc1c1},  {"RosyBrown2", 0xeeb4b4},  {"RosyBrown3", 0xcd9b9b},
  {"RosyBrown4", 0x8b6969},  {"IndianRed1", 0xff6a6a},  {"IndianRed2", 0xee6363},
  {"IndianRed3", 0xcd5555},  {"IndianRed4", 0x8b3a3a},  {"sienna1", 0xff8247},
  {"sienna2", 0xee7942},  {"sienna3", 0xcd6839},  {"sienna4", 0x8b4726},
  {"burlywood1", 0xffd39b},  {"burlywood2", 0xeec591},  {"burlywood3", 0xcdaa7d},
  {"burlywood4", 0x8b7355},  {"wheat1", 0xffe7ba},  {"wheat2", 0xeed8ae},
  {"wheat3", 0xcdba96},  {"wheat4", 0x8b7e66},  {"tan1", 0xffa54f},
  {"tan2", 0xee9a49},  {"tan3", 0xcd853f},  {"tan4", 0x8b5a2b},
  {"chocolate1", 0xff7f24},  {"chocolate2", 0xee7621},  {"chocolate3", 0xcd661d},
  {"chocolate4", 0x8b4513},  {"firebrick1", 0xff3030},  {"firebrick2", 0xee2c2c},
  {"firebrick3", 0xcd2626},  {"firebrick4", 0x8b1a1a},  {"brown1", 0xff4040},
  {"brown2", 0xee3b3b},  {"brown3", 0xcd3333},  {"brown4", 0x8b2323},
  {"salmon1", 0xff8c69},  {"salmon2", 0xee8262},  {"salmon3", 0xcd7054},
  {"salmon4", 0x8b4c39},  {"LightSalmon1", 0xffa07a},  {"LightSalmon2", 0xee9572},
  {"LightSalmon3", 0xcd8162},  {"LightSalmon4", 0x8b5742},  {"orange1", 0xffa500},
  {"orange2", 0xee9a00},  {"orange3", 0xcd8500},  {"orange4", 0x8b5a00},
  {"DarkOrange1", 0xff7f00},  {"DarkOrange2", 0xee7600},  {"DarkOrange3", 0xcd6600},
  {"DarkOrange4", 0x8b4500},  {"coral1", 0xff7256},  {"coral2", 0xee6a50},
  {"coral3", 0xcd5b45},  {"coral4", 0x8b3e2f},  {"tomato1", 0xff6347},
  {"tomato2", 0xee5c42},  {"tomato3", 0xcd4f39},  {"tomato4", 0x8b3626},
  {"OrangeRed1", 0xff4500},  {"OrangeRed2", 0xee4000},  {"OrangeRed3", 0xcd3700},
  {"OrangeRed4", 0x8b2500},  {"red1", 0xff0000},  {"red2", 0xee0000},
  {"red3", 0xcd0000},  {"red4", 0x8b0000},  {"DeepPink1", 0xff1493},
  {"DeepPink2", 0xee1289},  {"DeepPink3", 0xcd1076},  {"DeepPink4", 0x8b0a50},
  {"HotPink1", 0xff6eb4},  {"HotPink2", 0xee6aa7},  {"HotPink3", 0xcd6090},
  {"HotPink4", 0x8b3a62},  {"pink1", 0xffb5c5},  {"pink2", 0xeea9b8},
  {"pink3", 0xcd919e},  {"pink4", 0x8b636c},  {"LightPink1", 0xffaeb9},
  {"LightPink2", 0xeea2ad},  {"LightPink3", 0xcd8c95},  {"LightPink4", 0x8b5f65},
  {"PaleVioletRed1", 0xff82ab},  {"PaleVioletRed2", 0xee799f},  {"PaleVioletRed3", 0xcd6889},
  {"PaleVioletRed4", 0x8b475d},  {"maroon1", 0xff34b3},  {"maroon2", 0xee30a7},
  {"maroon3", 0xcd2990},  {"maroon4", 0x8b1c62},  {"VioletRed1", 0xff3e96},
  {"VioletRed2", 0xee3a8c},  {"VioletRed3", 0xcd3278},  {"VioletRed4", 0x8b2252},
  {"magenta1", 0xff00ff},  {"magenta2", 0xee00ee},  {"magenta3", 0xcd00cd},
  {"magenta4", 0x8b008b},  {"orchid1", 0xff83fa},  {"orchid2", 0xee7ae9},
  {"orchid3", 0xcd69c9},  {"orchid4", 0x8b4789},  {"plum1", 0xffbbff},
  {"plum2", 0xeeaeee},  {"plum3", 0xcd96cd},  {"plum4", 0x8b668b},
  {"MediumOrchid1", 0xe066ff},  {"MediumOrchid2", 0xd15fee},  {"MediumOrchid3", 0xb452cd},
  {"MediumOrchid4", 0x7a378b},  {"DarkOrchid1", 0xbf3eff},  {"DarkOrchid2", 0xb23aee},
  {"DarkOrchid3", 0x9a32cd},  {"DarkOrchid4", 0x68228b},  {"purple1", 0x9b30ff},
  {"purple2", 0x912cee},  {"purple3", 0x7d26cd},  {"purple4", 0x551a8b},
  {"MediumPurple1", 0xab82ff},  {"MediumPurple2", 0x9f79ee},  {"MediumPurple3", 0x8968cd},
  {"MediumPurple4", 0x5d478b},  {"thistle1", 0xffe1ff},  {"thistle2", 0xeed2ee},
  {"thistle3", 0xcdb5cd},  {"thistle4", 0x8b7b8b},  {"gray0", 0x000000},
  {"grey0", 0x000000},  {"gray1", 0x030303},  {"grey1", 0x030303},
  {"gray2", 0x050505},  {"grey2", 0x050505},  {"gray3", 0x080808},
  {"grey3", 0x080808},  {"gray4", 0x0a0a0a},  {"grey4", 0x0a0a0a},
  {"gray5", 0x0d0d0d},  {"grey5", 0x0d0d0d},  {"gray6", 0x0f0f0f},
  {"grey6", 0x0f0f0f},  {"gray7", 0x121212},  {"grey7", 0x121212},
  {"gray8", 0x141414},  {"grey8", 0x141414},  {"gray9", 0x171717},
  {"grey9", 0x171717},  {"gray10", 0x1a1a1a},  {"grey10", 0x1a1a1a},
  {"gray11", 0x1c1c1c},  {"grey11", 0x1c1c1c},  {"gray12", 0x1f1f1f},
  {"grey12", 0x1f1f1f},  {"gray13", 0x212121},  {"grey13", 0x212121},
  {"gray14", 0x242424},  {"grey14", 0x242424},  {"gray15", 0x262626},
  {"grey15", 0x262626},  {"gray16", 0x292929},  {"grey16", 0x292929},
  {"gray17", 0x2b2b2b},  {"grey17", 0x2b2b2b},  {"gray18", 0x2e2e2e},
  {"grey18", 0x2e2e2e},  {"gray19", 0x303030},  {"grey19", 0x303030},
  {"gray20", 0x333333},  {"grey20", 0x333333},  {"gray21", 0x363636},
  {"grey21", 0x363636},  {"gray22", 0x383838},  {"grey22", 0x383838},
  {"gray23", 0x3b3b3b},  {"grey23", 0x3b3b3b},  {"gray24", 0x3d3d3d},
  {"grey24", 0x3d3d3d},  {"gray25", 0x404040},  {"grey25", 0x404040},
  {"gray26", 0x424242},  {"grey26", 0x424242},  {"gray27", 0x454545},
  {"grey27", 0x454545},  {"gray28", 0x474747},  {"grey28", 0x474747},
  {"gray29", 0x4a4a4a},  {"grey29", 0x4a4a4a},  {"gray30", 0x4d4d4d},
  {"grey30", 0x4d4d4d},  {"gray31", 0x4f4f4f},  {"grey31", 0x4f4f4f},
  {"gray32", 0x525252},  {"grey32", 0x525252},  {"gray33", 0x545454},
  {"grey33", 0x545454},  {"gray34", 0x575757},  {"grey34", 0x575757},
  {"gray35", 0x595959},  {"grey35", 0x595959},  {"gray36", 0x5c5c5c},
  {"grey36", 0x5c5c5c},  {"gray37", 0x5e5e5e},  {"grey37", 0x5e5e5e},
  {"gray38", 0x616161},  {"grey38", 0x616161},  {"gray39", 0x636363},
  {"grey39", 0x636363},  {"gray40", 0x666666},  {"grey40", 0x666666},
  {"gray41", 0x696969},  {"grey41", 0x696969},  {"gray42", 0x6b6b6b},
  {"grey42", 0x6b6b6b},  {"gray43", 0x6e6e6e},  {"grey43", 0x6e6e6e},
  {"gray44", 0x707070},  {"grey44", 0x707070},  {"gray45", 0x737373},
  {"grey45", 0x737373},  {"gray46", 0x757575},  {"grey46", 0x757575},
  {"gray47", 0x787878},  {"grey47", 0x787878},  {"gray48", 0x7a7a7a},
  {"grey48", 0x7a7a7a},  {"gray49", 0x7d7d7d},  {"grey49", 0x7d7d7d},
  {"gray50", 0x7f7f7f},  {"grey50", 0x7f7f7f},  {"gray51", 0x828282},
  {"grey51", 0x828282},  {"gray52", 0x858585},  {"grey52", 0x858585},
  {"gray53", 0x878787},  {"grey53", 0x878787},  {"gray54", 0x8a8a8a},
  {"grey54", 0x8a8a8a},  {"gray55", 0x8c8c8c},  {"grey55", 0x8c8c8c},
  {"gray56", 0x8f8f8f},  {"grey56", 0x8f8f8f},  {"gray57", 0x919191},
  {"grey57", 0x919191},  {"gray58", 0x949494},  {"grey58", 0x949494},
  {"gray59", 0x969696},  {"grey59", 0x969696},  {"gray60", 0x999999},
  {"grey60", 0x999999},  {"gray61", 0x9c9c9c},  {"grey61", 0x9c9c9c},
  {"gray62", 0x9e9e9e},  {"grey62", 0x9e9e9e},  {"gray63", 0xa1a1a1},
  {"grey63", 0xa1a1a1},  {"gray64", 0xa3a3a3},  {"grey64", 0xa3a3a3},
  {"gray65", 0xa6a6a6},  {"grey65", 0xa6a6a6},  {"gray66", 0xa8a8a8},
  {"grey66", 0xa8a8a8},  {"gray67", 0xababab},  {"grey67", 0xababab},
  {"gray68", 0xadadad},  {"grey68", 0xadadad},  {"gray69", 0xb0b0b0},
  {"grey69", 0xb0b0b0},  {"gray70", 0xb3b3b3},  {"grey70", 0xb3b3b3},
  {"gray71", 0xb5b5b5},  {"grey71", 0xb5b5b5},  {"gray72", 0xb8b8b8},
  {"grey72", 0xb8b8b8},  {"gray73", 0xbababa},  {"grey73", 0xbababa},
  {"gray74", 0xbdbdbd},  {"grey74", 0xbdbdbd},  {"gray75", 0xbfbfbf},
  {"grey75", 0xbfbfbf},  {"gray76", 0xc2c2c2},  {"grey76", 0xc2c2c2},
  {"gray77", 0xc4c4c4},  {"grey77", 0xc4c4c4},  {"gray78", 0xc7c7c7},
  {"grey78", 0xc7c7c7},  {"gray79", 0xc9c9c9},  {"grey79", 0xc9c9c9},
  {"gray80", 0xcccccc},  {"grey80", 0xcccccc},  {"gray81", 0xcfcfcf},
  {"grey81", 0xcfcfcf},  {"gray82", 0xd1d1d1},  {"grey82", 0xd1d1d1},
  {"gray83", 0xd4d4d4},  {"grey83", 0xd4d4d4},  {"gray84", 0xd6d6d6},
  {"grey84", 0xd6d6d6},  {"gray85", 0xd9d9d9},  {"grey85", 0xd9d9d9},
  {"gray86", 0xdbdbdb},  {"grey86", 0xdbdbdb},  {"gray87", 0xdedede},
  {"grey87", 0xdedede},  {"gray88", 0xe0e0e0},  {"grey88", 0xe0e0e0},
  {"gray89", 0xe3e3e3},  {"grey89", 0xe3e3e3},  {"gray90", 0xe5e5e5},
  {"grey90", 0xe5e5e5},  {"gray91", 0xe8e8e8},  {"grey91", 0xe8e8e8},
  {"gray92", 0xebebeb},  {"grey92", 0xebebeb},  {"gray93", 0xededed},
  {"grey93", 0xededed},  {"gray94", 0xf0f0f0},  {"grey94", 0xf0f0f0},
  {"gray95", 0xf2f2f2},  {"grey95", 0xf2f2f2},  {"gray96", 0xf5f5f5},
  {"grey96", 0xf5f5f5},  {"gray97", 0xf7f7f7},  {"grey97", 0xf7f7f7},
  {"gray98", 0xfafafa},  {"grey98", 0xfafafa},  {"gray99", 0xfcfcfc},
  {"grey99", 0xfcfcfc},  {"gray100", 0xffffff},  {"grey100", 0xffffff},
  {NULL, 0} // sentinel
};

//TODO: sort the array on startup, and do a binary search instead

int cc_cmp(void* p1, void* p2) {
  return strcmp((char*)p1, (char*)p2);
}

bool name2rgb(const char* name, int& value) {
/*  static int cc_count = 0;
  if (cc_count == 0) {
    while (cc[cc_count].name != NULL) ++cc_count;
    qsort(cc, cc_count, sizeof(cc[0]), cc_cmp);
  } else {
    //TODO: write the binary search
  } */
  for (int i = 0; cc[i].name; ++i) {
    if (strcmp(cc[i].name, name) == 0) {
      value = cc[i].value;
      return true;
    }
  }
  return false;
}

  
//////////////////////////
//  rgb_t		//
//////////////////////////

void rgb_t::setInt(int i) {
  b = (uint8_t)(i & 0xFF);
  i = i >> 8;
  g = (uint8_t)(i & 0xFF);
  i = i >> 8;
  r = (uint8_t)(i & 0xFF);
}

void rgb_t::setString(const String& s) {
  setInt(s.HexToInt());
}

const String rgb_t::toString() const {
  return String(toInt(), "%06x");
}

//////////////////////////
//  iColor		//
//////////////////////////

const iColor	iColor::black_;

bool iColor::find (const char* name, float& r, float& g, float& b) {
  int rgb;
  bool rval = name2rgb(name, rgb);
  if (rval) {
    iColor c(rgb);
    c.intensities(r, g, b);
  }
  return rval;
}

void iColor::setRgb(int r_, int g_, int b_) {
  r = r_ & 0xFF;
  g = g_ & 0xFF;
  b = b_ & 0xFF;
  //don't change a 
}

void iColor::setRgb(int rgb_) {
  b = rgb_ & 0xFF;
  rgb_ = rgb_ >> 8;
  g = rgb_ & 0xFF;
  rgb_ = rgb_ >> 8;
  r = rgb_ & 0xFF;
}

void iColor::setRgba(int r_, int g_, int b_, int a_) {
  r = r_ & 0xFF;
  g = g_ & 0xFF;
  b = b_ & 0xFF;
  a = a_ & 0xFF;
}

/* iColor& iColor::operator=(const iColor& src) {
  c = src.c; // note: this=this handled ok -- faster to just copy over ourself, and not test
  return *this;
} */

/*
iColor::operator iBrush() const {
  return iBrush(this);
} */


void iColor::intensities(float& r_, float& g_, float& b_) {
  r_ = ic2fc(r);
  g_ = ic2fc(g);
  b_ = ic2fc(b);
}

void iColor::setString(const String& s) {
  rgb_t rgb;
  rgb.setString(s);
  setRgb(rgb.toInt());
}

const String iColor::toString() const {
  rgb_t rgb;
  rgb.setInt(c);
  return rgb.toString();
}

/*
//////////////////////////
// 	iBrush	        //
//////////////////////////

void iBrush::set(const iBrush& src) {
  m_c = src.m_c;
} */


#ifdef TA_GUI
iColor::iColor(const QColor& src) {
  setRgba(src.red(), src.green(), src.blue(), src.alpha());
}

void iColor::set(const QColor& src) {
  setRgba(src.red(), src.green(), src.blue(), src.alpha());
}

void iColor::set(const QColor* src) {
  if (src == NULL)
    clear();
  else
    setRgba(src->red(), src->green(), src->blue(), src->alpha());
}

iColor& iColor::operator=(const QColor&  src) {
  setRgba(src.red(), src.green(), src.blue(), src.alpha());
  return *this;
}

iColor::operator QColor() const {
  return QColor(r, g, b, a);
}

#endif

#ifdef TA_USE_INVENTOR
void iColor::copyTo(SoMFColor& col) const {
  col.setValue(redf(), greenf(), bluef());
}

#endif
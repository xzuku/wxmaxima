///
///  Copyright (C) 2004-2007 Andrej Vodopivec <andrejv@users.sourceforge.net>
///
///  This program is free software; you can redistribute it and/or modify
///  it under the terms of the GNU General Public License as published by
///  the Free Software Foundation; either version 2 of the License, or
///  (at your option) any later version.
///
///  This program is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  GNU General Public License for more details.
///
///
///  You should have received a copy of the GNU General Public License
///  along with this program; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///

#ifndef _TEXTSTYLE_H
#define _TEXTSTYLE_H

struct style
{
  style() : bold(false), italic(false), underlined(false) { };
  wxString color;
  bool bold;
  bool italic;
  bool underlined;
};

enum {
  TS_NORMAL_TEXT = 0,
  TS_HIDDEN_GROUP,
  TS_MAIN_PROMPT,
  TS_OTHER_PROMPT,
  TS_LABEL,
  TS_INPUT,
  TS_SPECIAL_CONSTANT,
  TS_NUMBER,
  TS_STRING,
  TS_GREEK_CONSTANT,
  TS_VARIABLE,
  TS_FUNCTION,
  TS_HIGHLIGHT,
  TS_TEXT_BACKGROUND
};

#endif

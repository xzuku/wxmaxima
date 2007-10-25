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

#include "MathParser.h"

#include "FracCell.h"
#include "ExptCell.h"
#include "TextCell.h"
#include "SubCell.h"
#include "SqrtCell.h"
#include "LimitCell.h"
#include "MatrCell.h"
#include "ParenCell.h"
#include "AbsCell.h"
#include "AtCell.h"
#include "DiffCell.h"
#include "SumCell.h"
#include "IntCell.h"
#include "FunCell.h"
#include "EditorCell.h"
#include "ImgCell.h"
#include "SubSupCell.h"

#include <wx/wx.h>
#include <wx/config.h>

#define MAXLENGTH 50000

MathParser::MathParser()
{}

MathParser::~MathParser()
{}

MathCell* MathParser::ParseFracTag(xmlNodePtr node)
{
  FracCell *frac = new FracCell;
  frac->SetFracStyle(m_FracStyle);
  frac->SetHighlight(m_highlight);
  xmlNodePtr child = node->children;
  if (child)
  {
    frac->SetNum(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      frac->SetDenom(ParseTag(child, false));
      if (node->properties != NULL)
      {
        frac->SetFracStyle(FC_CHOOSE);
      }
      frac->SetType(m_ParserStyle);
      frac->SetupBreakUps();
      return frac;
    }
  }
  delete frac;
  return NULL;
}

MathCell* MathParser::ParseDiffTag(xmlNodePtr node)
{
  DiffCell *diff = new DiffCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    int fc = m_FracStyle;
    m_FracStyle = FC_DIFF;
    diff->SetDiff(ParseTag(child, false));
    m_FracStyle = fc;
    child = child->next;
    if (child)
    {
      diff->SetBase(ParseTag(child, true));
      diff->SetType(m_ParserStyle);
      return diff;
    }
  }
  delete diff;
  return NULL;
}

MathCell* MathParser::ParseSupTag(xmlNodePtr node)
{
  ExptCell *expt = new ExptCell;
  if (node->properties != NULL)
    expt->IsMatrix(true);
  xmlNodePtr child = node->children;
  if (child)
  {
    expt->SetBase(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      MathCell* power = ParseTag(child, false);
      power->SetExponentFlag();
      expt->SetPower(power);
      expt->SetType(m_ParserStyle);
      return expt;
    }
  }
  delete expt;
  return NULL;
}

MathCell* MathParser::ParseSubSupTag(xmlNodePtr node)
{
  SubSupCell *subsup = new SubSupCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    subsup->SetBase(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      MathCell* index = ParseTag(child, false);
      index->SetExponentFlag();
      subsup->SetIndex(index);
      child = child->next;
      if (child)
      {
        MathCell* power = ParseTag(child, false);
        power->SetExponentFlag();
        subsup->SetExponent(power);
        subsup->SetType(m_ParserStyle);
        return subsup;
      }
    }
  }
  delete subsup;
  return NULL;
}

MathCell* MathParser::ParseSubTag(xmlNodePtr node)
{
  SubCell *sub = new SubCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    sub->SetBase(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      MathCell* index = ParseTag(child, false);
      index->SetExponentFlag();
      sub->SetIndex(index);
      sub->SetType(m_ParserStyle);
      return sub;
    }
  }
  delete sub;
  return NULL;
}

MathCell* MathParser::ParseAtTag(xmlNodePtr node)
{
  AtCell *at = new AtCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    at->SetBase(ParseTag(child, false));
    at->SetHighlight(m_highlight);
    child = child->next;
    if (child)
    {
      at->SetIndex(ParseTag(child, false));
      at->SetType(m_ParserStyle);
      return at;
    }
  }
  delete at;
  return NULL;
}

MathCell* MathParser::ParseFunTag(xmlNodePtr node)
{
  FunCell *fun = new FunCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    fun->SetName(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      fun->SetType(m_ParserStyle);
      fun->SetArg(ParseTag(child, false));
      return fun;
    }
  }
  delete fun;
  return NULL;
}

MathCell* MathParser::ParseText(xmlNodePtr node, int style)
{
  TextCell* cell = new TextCell;
  if (node != NULL && node->content != NULL)
  {
    wxString str((const char*)(node->content), wxConvUTF8);
    str = ToLocal(str);
    if (style == TS_NUMBER)
    {
      if (str.Length() > 100) // This could be made configurable.
        str = str.Left(30) + wxString::Format(wxT("[%d digits]"), str.Length() - 60) + str.Right(30);
    }
    cell->SetValue(str);
    cell->SetType(m_ParserStyle);
    cell->SetStyle(style);
    cell->SetHighlight(m_highlight);
  }
  return cell;
}

MathCell* MathParser::ParseSqrtTag(xmlNodePtr node)
{
  xmlNodePtr child = node->children;
  SqrtCell* cell = new SqrtCell;
  cell->SetInner(ParseTag(child, true));
  cell->SetType(m_ParserStyle);
  cell->SetHighlight(m_highlight);
  return cell;
}

MathCell* MathParser::ParseAbsTag(xmlNodePtr node)
{
  xmlNodePtr child = node->children;
  AbsCell* cell = new AbsCell;
  cell->SetInner(ParseTag(child, true));
  cell->SetType(m_ParserStyle);
  cell->SetHighlight(m_highlight);
  return cell;
}

MathCell* MathParser::ParseParenTag(xmlNodePtr node)
{
  xmlNodePtr child = node->children;
  ParenCell* cell = new ParenCell;
  cell->SetInner(ParseTag(child, true), m_ParserStyle);
  cell->SetHighlight(m_highlight);
  if (node->properties != NULL)
    cell->SetPrint(false);
  return cell;
}

MathCell* MathParser::ParseLimitTag(xmlNodePtr node)
{
  LimitCell *limit = new LimitCell;
  xmlNodePtr child = node->children;
  if (child)
  {
    limit->SetName(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      limit->SetUnder(ParseTag(child, false));
      child = child->next;
      if (child)
      {
        limit->SetBase(ParseTag(child, false));
        limit->SetType(m_ParserStyle);
        return limit;
      }
    }
  }
  delete limit;
  return NULL;
}

MathCell* MathParser::ParseSumTag(xmlNodePtr node)
{
  SumCell *sum = new SumCell;
  xmlNodePtr child = node->children;
  if (node->properties != NULL)
    sum->SetSumStyle(SM_PROD);
  sum->SetHighlight(m_highlight);
  if (child)
  {
    sum->SetUnder(ParseTag(child, false));
    child = child->next;
    if (child)
    {
      sum->SetOver(ParseTag(child, false));
      child = child->next;
      if (child)
      {
        sum->SetBase(ParseTag(child, false));
        sum->SetType(m_ParserStyle);
        return sum;
      }
    }
  }
  delete sum;
  return NULL;
}

MathCell* MathParser::ParseIntTag(xmlNodePtr node)
{
  IntCell *in = new IntCell;
  xmlNodePtr child = node->children;
  in->SetHighlight(m_highlight);
  if (node->properties == NULL)
  {
    in->SetIntStyle(INT_DEF);
    if (child)
    {
      in->SetUnder(ParseTag(child, false));
      child = child->next;
      if (child)
      {
        in->SetOver(ParseTag(child, false));
        child = child->next;
        if (child)
        {
          in->SetBase(ParseTag(child, false));
          child = child->next;
          if (child)
          {
            in->SetVar(ParseTag(child, true));
            in->SetType(m_ParserStyle);
            return in;
          }
        }
      }
    }
  }
  else
  {
    if (child)
    {
      in->SetBase(ParseTag(child, false));
      child = child->next;
      if (child)
      {
        in->SetVar(ParseTag(child, true));
        in->SetType(m_ParserStyle);
        return in;
      }
    }
  }
  delete in;
  return NULL;
}

MathCell* MathParser::ParseTableTag(xmlNodePtr node)
{
  MatrCell *matrix = new MatrCell;
  matrix->SetHighlight(m_highlight);

  if (node->properties != NULL)
    matrix->SetSpecialFlag(true);

  xmlNodePtr rows = node->children;
  while (rows)
  {
    matrix->NewRow();
    xmlNodePtr cells = rows->children;
    while (cells)
    {
      matrix->NewColumn();
      matrix->AddNewCell(ParseTag(cells, false));
      cells = cells->next;
    }
    rows = rows->next;
  }
  matrix->SetType(m_ParserStyle);
  matrix->SetDimension();
  return matrix;
}

MathCell* MathParser::ParseTag(xmlNodePtr node, bool all)
{
  //  wxYield();
  MathCell* tmp = NULL;
  MathCell* cell = NULL;
  while (node)
  {
    // Parse tags
    if (node->type == XML_ELEMENT_NODE)
    {
      wxString tagName((const char*)(node->name), wxConvUTF8);
      tagName = ToLocal(tagName);

      if (tagName == wxT("v"))
      {               // Variables (atoms)
        if (cell == NULL)
          cell = ParseText(node->children, TS_VARIABLE);
        else
          cell->AppendCell(ParseText(node->children, TS_VARIABLE));
      }
      else if (tagName == wxT("t"))
      {          // Other text
        if (cell == NULL)
          cell = ParseText(node->children, TS_NORMAL_TEXT);
        else
          cell->AppendCell(ParseText(node->children, TS_NORMAL_TEXT));
      }
      else if (tagName == wxT("n"))
      {          // Numbers
        if (cell == NULL)
          cell = ParseText(node->children, TS_NUMBER);
        else
          cell->AppendCell(ParseText(node->children, TS_NUMBER));
      }
      else if (tagName == wxT("h"))
      {          // Hidden cells (*)
        MathCell* tmp = ParseText(node->children);
        tmp->m_isHidden = true;
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("p"))
      {          // Parenthesis
        if (cell == NULL)
          cell = ParseParenTag(node);
        else
          cell->AppendCell(ParseParenTag(node));
      }
      else if (tagName == wxT("f"))
      {               // Fractions
        if (cell == NULL)
          cell = ParseFracTag(node);
        else
          cell->AppendCell(ParseFracTag(node));
      }
      else if (tagName == wxT("e"))
      {          // Exponentials
        if (cell == NULL)
          cell = ParseSupTag(node);
        else
          cell->AppendCell(ParseSupTag(node));
      }
      else if (tagName == wxT("i"))
      {          // Subscripts
        if (cell == NULL)
          cell = ParseSubTag(node);
        else
          cell->AppendCell(ParseSubTag(node));
      }
      else if (tagName == wxT("fn"))
      {         // Functions
        if (cell == NULL)
          cell = ParseFunTag(node);
        else
          cell->AppendCell(ParseFunTag(node));
      }
      else if (tagName == wxT("g"))
      {          // Greek constants
        MathCell* tmp = ParseText(node->children, TS_GREEK_CONSTANT);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("s"))
      {          // Special constants %e,...
        MathCell* tmp = ParseText(node->children, TS_SPECIAL_CONSTANT);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("fnm"))
      {         // Function names
        MathCell* tmp = ParseText(node->children, TS_FUNCTION);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("q"))
      {          // Square roots
        if (cell == NULL)
          cell = ParseSqrtTag(node);
        else
          cell->AppendCell(ParseSqrtTag(node));
      }
      else if (tagName == wxT("d"))
      {          // Differentials
        if (cell == NULL)
          cell = ParseDiffTag(node);
        else
          cell->AppendCell(ParseDiffTag(node));
      }
      else if (tagName == wxT("sm"))
      {         // Sums
        if (cell == NULL)
          cell = ParseSumTag(node);
        else
          cell->AppendCell(ParseSumTag(node));
      }
      else if (tagName == wxT("in"))
      {         // integrals
        if (cell == NULL)
          cell = ParseIntTag(node);
        else
          cell->AppendCell(ParseIntTag(node));
      }
      else if (tagName == wxT("mspace"))
      {
        if (cell == NULL)
          cell = new TextCell(wxT(" "));
        else
          cell->AppendCell(new TextCell(wxT(" ")));
      }
      else if (tagName == wxT("at"))
      {
        if (cell == NULL)
          cell = ParseAtTag(node);
        else
          cell->AppendCell(ParseAtTag(node));
      }
      else if (tagName == wxT("a"))
      {
        if (cell == NULL)
          cell = ParseAbsTag(node);
        else
          cell->AppendCell(ParseAbsTag(node));
      }
      else if (tagName == wxT("ie"))
      {
        if (cell == NULL)
          cell = ParseSubSupTag(node);
        else
          cell->AppendCell(ParseSubSupTag(node));
      }
      else if (tagName == wxT("lm"))
      {
        if (cell == NULL)
          cell = ParseLimitTag(node);
        else
          cell->AppendCell(ParseLimitTag(node));
      }
      else if (tagName == wxT("r"))
      {
        if (cell == NULL)
          cell = ParseTag(node->children);
        else
          cell->AppendCell(ParseTag(node->children));
      }
      else if (tagName == wxT("tb"))
      {
        if (cell == NULL)
          cell = ParseTableTag(node);
        else
          cell->AppendCell(ParseTableTag(node));
      }
      else if (tagName == wxT("mth"))
      {
        MathCell *tmp = ParseTag(node->children);
        if (tmp != NULL)
          tmp->ForceBreakLine(true);
        else
          tmp = new TextCell(wxT(" "));
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("lbl"))
      {
        int oldPS = m_ParserStyle;
        m_ParserStyle = MC_TYPE_LABEL;
        MathCell* tmp = ParseTag(node->children);
        m_ParserStyle = oldPS;
        tmp->ForceBreakLine(true);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("st"))
      {
        MathCell* tmp = ParseText(node->children, TS_STRING);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("hl"))
      {
        bool highlight = m_highlight;
        m_highlight = true;
        MathCell* tmp = ParseTag(node->children);
        m_highlight = highlight;
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (tagName == wxT("img"))
      {
        wxString filename((const char*)(node->children->content), wxConvUTF8);
        filename = ToLocal(filename);

        ImgCell *tmp = new ImgCell;
        tmp->LoadImage(filename);
        wxRemoveFile(filename);
        if (cell == NULL)
          cell = tmp;
        else
          cell->AppendCell(tmp);
      }
      else if (node->children)
      {
        if (cell == NULL)
          cell = ParseTag(node->children);
        else
          cell->AppendCell(ParseTag(node->children));
      }
    }
    // Parse text
    else
    {
      if (cell == NULL)
        cell = ParseText(node);
      else
        cell->AppendCell(ParseText(node));
    }
    if (!all)
      break;

    if (tmp == NULL)
      tmp = cell;
    else
      cell = cell->m_next;

    node = node->next;
  }

  if (tmp != NULL)
    return tmp;
  return cell;
}

/***
 * Parse the string s, which is (correct) xml fragment.
 * Put the result in line.
 */
MathCell* MathParser::ParseLine(wxString s, int style)
{
  m_ParserStyle = style;
  m_FracStyle = FC_NORMAL;
  m_highlight = false;
  MathCell* cell = NULL;

  wxConfigBase* config = wxConfig::Get();
  bool showLong = false;
  config->Read(wxT("showLong"), &showLong);

  if (s.Length() < MAXLENGTH || showLong)
  {
    s = ToUnicode(s);

#if wxUSE_UNICODE
    char *buf;
    wxWX2MBbuf tmp = wxConvertWX2MB(s.wx_str());
    buf = strdup(tmp);
    xmlDocPtr doc = xmlParseMemory(buf, strlen(buf));
    free(buf);
#else
    xmlDocPtr doc = xmlParseMemory(s.c_str(), s.Length());
#endif

    if (doc != NULL)
    {
      cell = ParseTag(xmlDocGetRootElement(doc));
      xmlFreeDoc(doc);
    }
  }
  else
  {
    cell = new TextCell(_(" << Expression too long to display! >>"));
    cell->ForceBreakLine(true);
  }
  return cell;
}

wxString MathParser::ToUnicode(wxString s)
{
#if wxUSE_UNICODE
  return s;
#else
  return wxString(s.wc_str(wxConvLocal), wxConvUTF8);
#endif
}

wxString MathParser::ToLocal(wxString s)
{
#if wxUSE_UNICODE
  return s;
#else
  return wxString(s.wc_str(wxConvUTF8), wxConvLocal);
#endif
}

//-----------------------------------------------------------------------------
// Desc: VM Opcodes
//
// Copyright © 2019 J McIntosh
//
//   This file is part of redB.
//
//   redB is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with redB.  If not, see <https://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------

#ifndef OPCODE_H
#define OPCODE_H 1

namespace red {

  /****************************************************************************
   * VM Opcodes
   */
  uint8_t const OP_NOP          = 0;
  uint8_t const OP_ADD          = 1;
  uint8_t const OP_ADDCLMN      = 2;
  uint8_t const OP_ADDFGNKEY    = 3;
  uint8_t const OP_ADDNDX       = 4;
  uint8_t const OP_ALTDB        = 5;
  uint8_t const OP_ALTUSR       = 6;
  uint8_t const OP_AND          = 7;
  uint8_t const OP_CALL         = 8;
  uint8_t const OP_CMPEQ        = 9;
  uint8_t const OP_CMPGE        = 10;
  uint8_t const OP_CMPGT        = 11;
  uint8_t const OP_CMPLE        = 12;
  uint8_t const OP_CMPLT        = 13;
  uint8_t const OP_CMPNE        = 14;
  uint8_t const OP_CRJOIN       = 15;
  uint8_t const OP_CRTDB        = 16;
  uint8_t const OP_CRTTBL       = 17;
  uint8_t const OP_CRTUSR       = 18;
  uint8_t const OP_DELETE       = 19;
  uint8_t const OP_DESCRIBE     = 20;
  uint8_t const OP_DIV          = 21;
  uint8_t const OP_DROPCLMN     = 22;
  uint8_t const OP_DROPDB       = 23;
  uint8_t const OP_DROPFGNKEY   = 24;
  uint8_t const OP_DROPNDX      = 25;
  uint8_t const OP_DROPTBL      = 26;
  uint8_t const OP_DROPUSR      = 27;
  uint8_t const OP_GRANT        = 28;
  uint8_t const OP_INJOIN       = 29;
  uint8_t const OP_INSERT       = 30;
  uint8_t const OP_LOADDATA     = 31;
  uint8_t const OP_LTJOIN       = 32;
  uint8_t const OP_MKASSIGN     = 33;
  uint8_t const OP_MKASTEXPR    = 34;
  uint8_t const OP_MKCLMNDEF    = 35;
  uint8_t const OP_MKCLMNEXPR   = 36;
  uint8_t const OP_MKEXPR       = 37;
  uint8_t const OP_MKEXPRHDR    = 38;
  uint8_t const OP_MKEXPRLST    = 39;
  uint8_t const OP_MKFGNKEY     = 40;
  uint8_t const OP_MKPRIKEY     = 41;
  uint8_t const OP_MKUNIKEY     = 42;
  uint8_t const OP_MKUSR        = 43;
  uint8_t const OP_MKWRKSCH     = 44;
  uint8_t const OP_MUL          = 45;
  uint8_t const OP_NEGATE       = 46;
  uint8_t const OP_NOT          = 47;
  uint8_t const OP_ON           = 48;
  uint8_t const OP_OPNTBL       = 49;
  uint8_t const OP_OR           = 50;
  uint8_t const OP_ORDERBY      = 51;
  uint8_t const OP_PUSH         = 52;
  uint8_t const OP_PUSHO        = 53;
  uint8_t const OP_RET          = 54;
  uint8_t const OP_REVOKE       = 55;
  uint8_t const OP_RTJOIN       = 56;
  uint8_t const OP_SELECT       = 57;
  uint8_t const OP_SETOUTF      = 58;
  uint8_t const OP_SHOWDBS      = 59;
  uint8_t const OP_SHOWTBLS     = 60;
  uint8_t const OP_SLCKTBL      = 61;
  uint8_t const OP_SUB          = 62;
  uint8_t const OP_ULCKTBL      = 63;
  uint8_t const OP_UPDATE       = 64;
  uint8_t const OP_USE          = 65;
  uint8_t const OP_USING        = 66;
  uint8_t const OP_WHERE        = 67;

} // namespace

#endif  // OPCODE_H


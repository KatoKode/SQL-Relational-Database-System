//-----------------------------------------------------------------------------
// Desc: SQL lexer/parser library (compiler)
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

#include "comp.h"

namespace red::comp {

  /****************************************************************************
   * Token Types
   */
  int32_t const ADD         = 0;
  int32_t const ADDCLMN     = 1;
  int32_t const ADDFGNKEY   = 2;
  int32_t const ADDNDX      = 3;
  int32_t const ALTDB       = 4;
  int32_t const ALTUSR      = 5;
  int32_t const AND         = 6;
  int32_t const ASTERISK    = 7;
  int32_t const BACKSLASH   = 8;
  int32_t const CALL        = 9;
  int32_t const CHAR        = 10;
  int32_t const COMMA       = 11;
  int32_t const CMPEQ       = 12;
  int32_t const CMPGE       = 13;
  int32_t const CMPGT       = 14;
  int32_t const CMPLE       = 15;
  int32_t const CMPLT       = 16;
  int32_t const CMPNE       = 17;
  int32_t const CRJOIN      = 18;
  int32_t const CRTDB       = 19;
  int32_t const CRTTBL      = 20;
  int32_t const CRTUSR      = 21;
  int32_t const DATA        = 22;
  int32_t const DELETE      = 23;
  int32_t const DESCRIBE    = 24;
  int32_t const DIV         = 25;
  int32_t const DOLLAR      = 26;
  int32_t const DOT         = 27;
  int32_t const DQUOTMARK   = 28;
  int32_t const DROPCLMN    = 29;
  int32_t const DROPDB      = 30;
  int32_t const DROPFGNKEY  = 31;
  int32_t const DROPNDX     = 32;
  int32_t const DROPTBL     = 33;
  int32_t const DROPUSR     = 34;
  int32_t const FLOAT       = 35;
  int32_t const GRANT       = 36;
  int32_t const HASHTAG     = 37;
  int32_t const INJOIN      = 38;
  int32_t const INSERT      = 39;
  int32_t const INT         = 40;
  int32_t const LOADDATA    = 41;
  int32_t const LTJOIN      = 42;
  int32_t const L_PAREN     = 43;
  int32_t const MINUS       = 44;
  int32_t const MKASSIGN    = 45;
  int32_t const MKASTEXPR   = 46;
  int32_t const MKCLMNDEF   = 47;
  int32_t const MKCLMNEXPR  = 48;
  int32_t const MKEXPR      = 49;
  int32_t const MKEXPRHDR   = 50;
  int32_t const MKEXPRLST   = 51;
  int32_t const MKFGNKEY    = 52;
  int32_t const MKPRIKEY    = 53;
  int32_t const MKUNIKEY    = 54;
  int32_t const MKUSR       = 55;
  int32_t const MKWRKSCH    = 56;
  int32_t const MUL         = 57;
  int32_t const NEGATE      = 58;
  int32_t const NOP         = 59;
  int32_t const NOT         = 60;
  int32_t const ON          = 61;
  int32_t const OPNTBL      = 62;
  int32_t const OR          = 63;
  int32_t const ORDERBY     = 64;
  int32_t const PERCENT     = 65;
  int32_t const PLUS        = 66;
  int32_t const PRIMARY     = 67;
  int32_t const PUSH        = 68;
  int32_t const PUSHO       = 69;
  int32_t const QUOTMARK    = 70;
  int32_t const R_PAREN     = 71;
  int32_t const RET         = 72;
  int32_t const REVOKE      = 73;
  int32_t const RTJOIN      = 74;
  int32_t const SELECT      = 75;
  int32_t const SETOUTF     = 76;
  int32_t const SHOWDBS     = 77;
  int32_t const SHOWTBLS    = 78;
  int32_t const SLCKTBL     = 79;
  int32_t const SUB         = 80;
  int32_t const TEXT        = 81;
  int32_t const ULCKTBL     = 82;
  int32_t const UPDATE      = 83;
  int32_t const USE         = 84;
  int32_t const USING       = 85;
  int32_t const VARCHAR     = 86;
  int32_t const WHERE       = 87;

  /****************************************************************************
   * Symbols
   */
  std::vector<std::pair<std::string, int32_t> > symbol {
    {"ADD", ADD},
    {"ADDCLMN", ADDCLMN},
    {"ADDFGNKEY", ADDFGNKEY},
    {"ADDNDX", ADDNDX},
    {"ALTUSR", ALTUSR},
    {"AND", AND},
    {"CALL", CALL},
    {"CHAR", CHAR},
    {"CMPEQ", CMPEQ},
    {"CMPGE", CMPGE},
    {"CMPGT", CMPGT},
    {"CMPLE", CMPLE},
    {"CMPLT", CMPLT},
    {"CMPNE", CMPNE},
    {"CRJOIN", CRJOIN},
    {"CRTDB", CRTDB},
    {"CRTTBL", CRTTBL},
    {"CRTUSR", CRTUSR},
    {"DATA", DATA},
    {"DELETE", DELETE},
    {"DESCRIBE", DESCRIBE},
    {"DIV", DIV},
    {"DROPCLMN", DROPCLMN},
    {"DROPDB", DROPDB},
    {"DROPFGNKEY", DROPFGNKEY},
    {"DROPNDX", DROPNDX},
    {"DROPTBL", DROPTBL},
    {"DROPUSR", DROPUSR},
    {"FLOAT", FLOAT},
    {"GRANT", GRANT},
    {"INJOIN", INJOIN},
    {"INSERT", INSERT},
    {"INT", INT},
    {"LOADDATA", LOADDATA},
    {"LTJOIN", LTJOIN},
    {"MKASSIGN", MKASSIGN},
    {"MKASTEXPR", MKASTEXPR},
    {"MKCLMNDEF", MKCLMNDEF},
    {"MKCLMNEXPR", MKCLMNEXPR},
    {"MKEXPR", MKEXPR},
    {"MKEXPRHDR", MKEXPRHDR},
    {"MKEXPRLST", MKEXPRLST},
    {"MKFGNKEY", MKFGNKEY},
    {"MKPRIKEY", MKPRIKEY},
    {"MKUNIKEY", MKUNIKEY},
    {"MKUSR", MKUSR},
    {"MKWRKSCH", MKWRKSCH},
    {"MUL", MUL},
    {"NEGATE", NEGATE},
    {"NOP", NOP},
    {"NOT", NOT},
    {"ON", ON},
    {"OPNTBL", OPNTBL},
    {"OR", OR},
    {"ORDERBY", ORDERBY},
    {"PRIMARY", PRIMARY},
    {"PUSH", PUSH},
    {"PUSHO", PUSHO},
    {"RET", RET},
    {"REVOKE", REVOKE},
    {"RTJOIN", RTJOIN},
    {"SELECT", SELECT},
    {"SETOUTF", SETOUTF},
    {"SHOWDBS", SHOWDBS},
    {"SHOWTBLS", SHOWTBLS},
    {"SLCKTBL", SLCKTBL},
    {"SUB", SUB},
    {"TEXT", TEXT},
    {"ULCKTBL", ULCKTBL},
    {"UPDATE", UPDATE},
    {"USE", USE},
    {"USING", USING},
    {"VARCHAR", VARCHAR},
    {"WHERE", WHERE},
  };

  /****************************************************************************
   * Compiler Types
   */
  int32_t const TYPE_NAME       = 16; /* translator and compiler type */

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS COMP_LEXER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS COMP_LEXER: Move Assignment
   */
  comp_lexer& comp_lexer::operator=(comp_lexer&& o) {
    if (this == &o) return *this;
    m_input = o.m_input;
    m_mark  = o.m_mark;
    m_index = o.m_index;
    m_ch    = o.m_ch;
    return *this;
  }

  /****************************************************************************
   *
   */
  std::string comp_lexer::remaining () const {
    return m_input.substr(m_mark, m_input.size() - m_mark);
  }

  /****************************************************************************
   *
   */
  token comp_lexer::next_token () {
    m_mark = m_index;
    while (m_ch != comp_lexer::EOI) {
      switch (m_ch) {
        case ' ': case '\t': case '\n': ws(); break;
        case '\"': return varchar();
        case '#': consume(); return token {HASHTAG, "#"};
        case '$': consume(); return token {DOLLAR, "$"};
        case '%': consume(); return token {PERCENT, "%"};
        case '\'': consume(); return token {QUOTMARK, "\'"};
        case '(': consume(); return token {L_PAREN, "("};
        case ')': consume(); return token {R_PAREN, ")"};
        case '*': consume(); return token {ASTERISK, "*"};
        case '+': consume(); return token {PLUS, "+"};
        case ',': consume(); return token {COMMA, ","};
        case '.':
          {
            // either DOT or FLOAT
            return compound();
          }
        case '\\':
          {
            return varchar();
          }
        default:
          if (std::isalpha(m_ch) || m_ch == '_') return name();
          else if (std::isdigit(m_ch) || m_ch == '-') return number();
          else throw std::runtime_error{
            red::eno::get_msg(red::eno::ERRNO_TOKEN_ERROR, remaining())};
      }
    }
    return token{comp_lexer::EOI, "<EOI>"};
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS COMP_LEXER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void comp_lexer::consume () {
    if (m_index + 1 < m_input.size()) { m_ch = m_input [ ++m_index ]; }
    else m_ch = comp_lexer::EOI;
  }

  /****************************************************************************
   *
   */
  token comp_lexer::compound () {
    switch (m_ch) {
      case '.':
        {
          if (std::isdigit(peek())) return number();
          consume();
          return token {DOT, "."};
        }
      default: throw std::runtime_error{red::eno::get_msg(
                   red::eno::ERRNO_TOKEN_ERROR, remaining())};
    }
  }

  /****************************************************************************
   *
   */
  int32_t comp_lexer::get_symbol_type (std::string const& text) {
    // convert string text to uppercase
    std::string uc_text;
    for (char ch : text) uc_text += std::toupper(ch);
    // search symbol table for string uc_text 
    int32_t lo = 0;
    int32_t hi = symbol.size() - 1;
    while (lo <= hi) {
      int32_t mid = (lo + hi) / 2;
      int32_t cond = uc_text.compare(symbol [ mid ].first);
      if (cond < 0) hi = mid - 1;
      else if (cond > 0) lo = mid + 1;
      else return symbol [ mid ].second;
    }
    return -1;
  }

  /****************************************************************************
   *
   */
  void comp_lexer::init () {
    if (not m_input.empty()) m_ch = m_input [ m_index ];
    else m_ch = comp_lexer::EOI;
  }

  /****************************************************************************
   *
   */
  token comp_lexer::name () {
    std::string text;
    while (std::isalnum(m_ch) || m_ch == '_') {
      text += m_ch;
      consume();
    }
    int32_t type = get_symbol_type(text);
    return token {(type < 0 ? VARCHAR : type), text};
  }

  /****************************************************************************
   *
   */
  token comp_lexer::number () {
    std::string value;
    int32_t type = INT; // assume INT
    while (std::isdigit(m_ch)) { value += m_ch; consume(); }
    if (m_ch == '.') {  // FLOAT token
      type = FLOAT;
      value += m_ch;
      consume();
      while (std::isdigit(m_ch)) { value += m_ch; consume(); }
    }
    return token {type, value};
  }

  /****************************************************************************
   *
   */
  char comp_lexer::peek () {
    if (m_index >= m_input.size()) return red::comp::comp_lexer::EOI;
    else return m_input [ m_index + 1 ];
  }

  /****************************************************************************
   *
   */
  token comp_lexer::varchar () {
    std::string text;
    consume();  // consume first '\"'
    while (m_ch != comp_lexer::EOI) {
      if (m_ch == '\\') {
        text += m_ch;
        consume();
      } else if (m_ch == '\"') break;
      text += m_ch;
      consume();
    }
    consume();  // consume last '\"'
    return token {VARCHAR, text};
  }

  /****************************************************************************
   *
   */
  void comp_lexer::ws () {
    while (std::isspace(m_ch)) consume();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS PARSER2: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void comp_parser::parse () {
    compile();
  }

  /****************************************************************************
   *
   */
  std::pair<red::buffer, red::buffer> comp_parser::dump () {
    m_data.trip();
    m_code.trip();
    return std::make_pair(m_data, m_code);
  }
#ifdef COMP_DEBUG
  /****************************************************************************
   *
   */
  void comp_parser::dump2 () {
    m_data.trip();
    std::cout << ".DATA\n";
    std::cout << std::hex << "\t00  ";
    for (int32_t i = 0, c = 0; i < m_data.bound(); ++i) {
      std::cout << std::setfill('0') << std::setw(2)
        << static_cast<int32_t>(m_data.get(i)) << ' ';
      ++c;
      if (c == 16) {
        std::cout << "\n\t" << std::setfill('0') << std::setw(2)
          << (i + 1) << "  ";
        c = 0;
      }
    }
    std::cout << "\n.TEXT\n";
    m_code.trip();
    std::cout << std::hex << "\t00  ";
    for (int32_t i = 0, c = 0; i < m_code.bound(); ++i) {
      std::cout << std::setfill('0') << std::setw(2)
        << static_cast<int32_t>(m_code.get(i)) << ' ';
      ++c;
      if (c == 16) {
        std::cout << "\n\t" << std::setfill('0') << std::setw(2)
          << (i + 1) << "  ";
        c = 0;
      }
    }
    std::cout << std::dec << '\n';
  }
#endif
  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS PARSER2: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void comp_parser::init () {
    load_lookahead(1);
  }

  /****************************************************************************
   *
   */
  void comp_parser::consume () {
    ++m_index;
    if (m_index >= m_lookahead.size()) {
      m_index = 0;
      m_lookahead.clear();
    }
    load_lookahead(1);
  }

  /****************************************************************************
   *
   */
  token comp_parser::get_lookahead_token(size_t i) {
    load_lookahead(i);
    return m_lookahead [m_index + i];
  }

  /****************************************************************************
   *
   */
  void comp_parser::load_lookahead (size_t i) {
    while (m_lookahead.size() < m_index + i) {
      m_lookahead.push_back(m_lexer.next_token());
    }
  }

  /****************************************************************************
   *
   */
  void comp_parser::match (int32_t type) {
    if (get_lookahead_type(0) != type)
      throw std::runtime_error{
        red::eno::get_msg(red::eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};

    consume();
  }

  /****************************************************************************
   *
   */
  void comp_parser::call () {
    m_code.put(red::OP_CALL);

    switch (get_lookahead_type(0)) {
      case ADD: consume(); m_code.put(red::OP_ADD); break;
      case ADDCLMN: consume(); m_code.put(red::OP_ADDCLMN); break;
      case ADDFGNKEY: consume(); m_code.put(red::OP_ADDFGNKEY); break;
      case ADDNDX: consume(); m_code.put(red::OP_ADDNDX); break;
      case ALTDB: consume(); m_code.put(red::OP_ALTDB); break;
      case ALTUSR: consume(); m_code.put(red::OP_ALTUSR); break;
      case AND: consume(); m_code.put(red::OP_AND); break;
      case CMPEQ: consume(); m_code.put(red::OP_CMPEQ); break;
      case CMPGE: consume(); m_code.put(red::OP_CMPGE); break;
      case CMPGT: consume(); m_code.put(red::OP_CMPGT); break;
      case CMPLE: consume(); m_code.put(red::OP_CMPLE); break;
      case CMPLT: consume(); m_code.put(red::OP_CMPLT); break;
      case CMPNE: consume(); m_code.put(red::OP_CMPNE); break;
      case CRJOIN: consume(); m_code.put(red::OP_CRJOIN); break;
      case CRTDB: consume(); m_code.put(red::OP_CRTDB); break;
      case CRTTBL: consume(); m_code.put(red::OP_CRTTBL); break;
      case CRTUSR: consume(); m_code.put(red::OP_CRTUSR); break;
      case DELETE: consume(); m_code.put(red::OP_DELETE); break;
      case DESCRIBE: consume(); m_code.put(red::OP_DESCRIBE); break;
      case DIV: consume(); m_code.put(red::OP_DIV); break;
      case DROPCLMN: consume(); m_code.put(red::OP_DROPCLMN); break;
      case DROPDB: consume(); m_code.put(red::OP_DROPDB); break;
      case DROPFGNKEY: consume(); m_code.put(red::OP_DROPFGNKEY); break;
      case DROPNDX: consume(); m_code.put(red::OP_DROPNDX); break;
      case DROPTBL: consume(); m_code.put(red::OP_DROPTBL); break;
      case DROPUSR: consume(); m_code.put(red::OP_DROPUSR); break;
      case GRANT: consume(); m_code.put(red::OP_GRANT); break;
      case INJOIN: consume(); m_code.put(red::OP_INJOIN); break;
      case INSERT: consume(); m_code.put(red::OP_INSERT); break;
      case LOADDATA: consume(); m_code.put(red::OP_LOADDATA); break;
      case LTJOIN: consume(); m_code.put(red::OP_LTJOIN); break;
      case MKASSIGN: consume(); m_code.put(red::OP_MKASSIGN); break;
      case MKASTEXPR: consume(); m_code.put(red::OP_MKASTEXPR); break;
      case MKCLMNDEF: consume(); m_code.put(red::OP_MKCLMNDEF); break;
      case MKCLMNEXPR: consume(); m_code.put(red::OP_MKCLMNEXPR); break;
      case MKEXPR: consume(); m_code.put(red::OP_MKEXPR); break;
      case MKEXPRHDR: consume(); m_code.put(red::OP_MKEXPRHDR); break;
      case MKEXPRLST: consume(); m_code.put(red::OP_MKEXPRLST); break;
      case MKFGNKEY: consume(); m_code.put(red::OP_MKFGNKEY); break;
      case MKPRIKEY: consume(); m_code.put(red::OP_MKPRIKEY); break;
      case MKUNIKEY: consume(); m_code.put(red::OP_MKUNIKEY); break;
      case MKUSR: consume(); m_code.put(red::OP_MKUSR); break;
      case MKWRKSCH: consume(); m_code.put(red::OP_MKWRKSCH); break;
      case MUL: consume(); m_code.put(red::OP_MUL); break;
      case NEGATE: consume(); m_code.put(red::OP_NEGATE); break;
      case NOT: consume(); m_code.put(red::OP_NOT); break;
      case ON: consume(); m_code.put(red::OP_ON); break;
      case OPNTBL: consume(); m_code.put(red::OP_OPNTBL); break;
      case OR: consume(); m_code.put(red::OP_OR); break;
      case ORDERBY: consume(); m_code.put(red::OP_ORDERBY); break;
      case REVOKE: consume(); m_code.put(red::OP_REVOKE); break;
      case RTJOIN: consume(); m_code.put(red::OP_RTJOIN); break;
      case SELECT: consume(); m_code.put(red::OP_SELECT); break;
      case SETOUTF: consume(); m_code.put(red::OP_SETOUTF); break;
      case SHOWDBS: consume(); m_code.put(red::OP_SHOWDBS); break;
      case SHOWTBLS: consume(); m_code.put(red::OP_SHOWTBLS); break;
      case SLCKTBL: consume(); m_code.put(red::OP_SLCKTBL); break;
      case SUB: consume(); m_code.put(red::OP_SUB); break;
      case ULCKTBL: consume(); m_code.put(red::OP_ULCKTBL); break;
      case UPDATE: consume(); m_code.put(red::OP_UPDATE); break;
      case USE: consume(); m_code.put(red::OP_USE); break;
      case USING: consume(); m_code.put(red::OP_USING); break;
      case WHERE: consume(); m_code.put(red::OP_WHERE); break;
      default: throw std::runtime_error{red::eno::get_msg(
                   red::eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  void comp_parser::compile () {
    std::vector<int32_t> offset;
    data_segment(offset);
    text_segment(offset);
  }

  void comp_parser::data (std::vector<int32_t>& offset) {
    if (get_lookahead_type(0) == PERCENT) return;  // end of DATA segment

    offset.push_back(m_data.index());
    m_data.put(static_cast<int8_t>(std::stoi(get_lookahead_token(0).text())));
    consume();
    m_data.put_int16(static_cast<int16_t>(get_lookahead_token(0).text().size()));
    m_data.put_varchar(get_lookahead_token(0).text());
    consume();

    data(offset);
  }

  void comp_parser::data_segment (std::vector<int32_t>& offset) {
    match(DOT);
    match(DATA);
    data(offset);
    match(PERCENT);
    match(PERCENT);
  }

  void comp_parser::push () {
    m_code.put(red::OP_PUSH);
    int32_t value = std::stoi(get_lookahead_token(0).text());
    match(INT);
    m_code.put_int32(value);
  }

  void comp_parser::pusho (std::vector<int32_t>& offset) {
    m_code.put(red::OP_PUSHO);
    int32_t index = std::stoi(get_lookahead_token(0).text());
    match(INT);
    m_code.put_int32(offset [ index ]);
  }

  void comp_parser::text (std::vector<int32_t>& offset) {
    switch (get_lookahead_type(0)) {
      case CALL: consume(); call(); break;
      case NOP: consume(); break;
      case PUSH: consume(); push(); break;
      case PUSHO: consume(); pusho(offset); break;
      case RET: consume(); m_code.put(red::OP_RET); return;
      default: throw std::runtime_error{red::eno::get_msg(
                   red::eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }

    text(offset);
  }

  void comp_parser::text_segment (std::vector<int32_t>& offset) {
    match(DOT);
    match(TEXT);
    text (offset);
    match(PERCENT);
    match(PERCENT);
  }

} // namespace red::comp


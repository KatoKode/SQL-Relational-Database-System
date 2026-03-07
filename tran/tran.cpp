//-----------------------------------------------------------------------------
// Desc: SQL lexer/parser library (translator)
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

#include "tran.h"

namespace red::tran {

  /****************************************************************************
   * Token Types
   */
  int32_t const ADD         = 0;
  int32_t const AFTER       = 1;
  int32_t const ALL         = 2;
  int32_t const ALTER       = 3;
  int32_t const AND         = 4;
  int32_t const AS          = 5;
  int32_t const ASTERISK    = 6;
  int32_t const ATSIGN      = 7;
  int32_t const AUTO_INC    = 8;
  int32_t const BACKSLASH   = 9;
  int32_t const BY          = 10;
  int32_t const CHAR        = 11;
  int32_t const COLUMN      = 12;
  int32_t const COMMA       = 13;
  int32_t const COMMENT     = 14;
  int32_t const CREATE      = 15;
  int32_t const CROSS       = 16;
  int32_t const DATA        = 17;
  int32_t const DATABASE    = 18;
  int32_t const DATABASES   = 19;
  int32_t const DATE        = 20;
  int32_t const DATETIME    = 21;
  int32_t const DELETE      = 22;
  int32_t const DESCRIBE    = 23;
  int32_t const DOT         = 24;
  int32_t const DROP        = 25;
  int32_t const ENCLOSED    = 26;
  int32_t const ENUM        = 27;
  int32_t const EQ          = 28;
  int32_t const FILE_       = 29;
  int32_t const FIRST       = 30;
  int32_t const FLOAT       = 31;
  int32_t const FOR         = 32;
  int32_t const FOREIGN     = 33;
  int32_t const FROM        = 34;
  int32_t const GRANT       = 35;
  int32_t const GE          = 36;
  int32_t const GT          = 37;
  int32_t const HASHTAG     = 38;
  int32_t const IDENTIFIED  = 39;
  int32_t const IGNORE      = 40;
  int32_t const INDEX       = 41;
  int32_t const INFILE      = 42;
  int32_t const INNER       = 43;
  int32_t const INSERT      = 44;
  int32_t const INT         = 45;
  int32_t const INTO        = 46;
  int32_t const JOIN        = 47;
  int32_t const KEY         = 48;
  int32_t const LEFT        = 49;
  int32_t const L_PAREN     = 50;
  int32_t const LE          = 51;
  int32_t const LIMIT       = 52;
  int32_t const LT          = 53;
  int32_t const LOAD        = 54;
  int32_t const MINUS       = 55;
  int32_t const MOD         = 56;
  int32_t const NAME        = 57;
  int32_t const NE          = 58;
  int32_t const NOT         = 59;
  int32_t const NULL_SYM    = 60;
  int32_t const ON          = 61;
  int32_t const OR          = 62;
  int32_t const ORDER       = 63;
  int32_t const OUTER       = 64;
  int32_t const OUTFILE     = 65;
  int32_t const PASSWORD    = 66;
  int32_t const PLUS        = 67;
  int32_t const PRIMARY     = 68;
  int32_t const REFERENCES  = 69;
  int32_t const RELOAD      = 70;
  int32_t const RENAME      = 71;
  int32_t const REPLACE     = 72;
  int32_t const REVOKE      = 73;
  int32_t const RIGHT       = 74;
  int32_t const R_PAREN     = 75;
  int32_t const SELECT      = 76;
  int32_t const SEMICOLON   = 77;
  int32_t const SET         = 78;
  int32_t const SHOW        = 79;
  int32_t const SHUTDOWN    = 80;
  int32_t const TABLE       = 81;
  int32_t const TABLES      = 82;
  int32_t const TIMESTAMP   = 83;
  int32_t const TO          = 84;
  int32_t const UNIQUE      = 85;
  int32_t const UNSIGNED    = 86;
  int32_t const UPDATE      = 87;
  int32_t const USE         = 88;
  int32_t const USER        = 89;
  int32_t const USING       = 90;
  int32_t const VALUES      = 91;
  int32_t const VARCHAR     = 92;
  int32_t const WHERE       = 93;
  int32_t const WHITESPACE  = 94;
  int32_t const ZEROFILL    = 95;

  /****************************************************************************
   * Translator Types
   */
  int32_t const TYPE_NAME   = 16; /* translator and compiler type */

  /****************************************************************************
   * Symbols
   */
  std::vector<std::pair<std::string, int32_t> > symbol {
    {"ADD", ADD},
    {"AFTER", AFTER},
    {"ALL", ALL},
    {"ALTER", ALTER},
    {"AND", AND},
    {"AS", AS},
    {"AUTO_INCREMENT", AUTO_INC},
    {"BY", BY},
    {"CHAR", CHAR},
    {"COLUMN", COLUMN},
    {"CREATE", CREATE},
    {"CROSS", CROSS},
    {"DATA", DATA},
    {"DATABASE", DATABASE},
    {"DATABASES", DATABASES},
    {"DATE", DATE},
    {"DATETIME", DATETIME},
    {"DELETE", DELETE},
    {"DESCRIBE", DESCRIBE},
    {"DROP", DROP},
    {"ENUM", ENUM},
    {"FILE", FILE_},
    {"FIRST", FIRST},
    {"FLOAT", FLOAT},
    {"FOR", FOR},
    {"FOREIGN", FOREIGN},
    {"FROM", FROM},
    {"GRANT", GRANT},
    {"IDENTIFIED", IDENTIFIED},
    {"INDEX", INDEX},
    {"INFILE", INFILE},
    {"INNER", INNER},
    {"INSERT", INSERT},
    {"INT", INT},
    {"INTO", INTO},
    {"JOIN", JOIN},
    {"KEY", KEY},
    {"LEFT", LEFT},
    {"LIMIT", LIMIT},
    {"LOAD", LOAD},
    {"MOD", MOD},
    {"NOT", NOT},
    {"NULL", NULL_SYM},
    {"ON", ON},
    {"OR", OR},
    {"ORDER", ORDER},
    {"OUTER", OUTER},
    {"OUTFILE", OUTFILE},
    {"PASSWORD", PASSWORD},
    {"PRIMARY", PRIMARY},
    {"REFERENCES", REFERENCES},
    {"RELOAD", RELOAD},
    {"RENAME", RENAME},
    {"REPLACE", REPLACE},
    {"REVOKE", REVOKE},
    {"RIGHT", RIGHT},
    {"SELECT", SELECT},
    {"SET", SET},
    {"SHOW", SHOW},
    {"SHUTDOWN", SHUTDOWN},
    {"TABLE", TABLE},
    {"TABLES", TABLES},
    {"TIMESTAMP", TIMESTAMP},
    {"TO", TO},
    {"UNIQUE", UNIQUE},
    {"UNSIGNED", UNSIGNED},
    {"UPDATE", UPDATE},
    {"USE", USE},
    {"USER", USER},
    {"USING", USING},
    {"VALUES", VALUES},
    {"VARCHAR", VARCHAR},
    {"WHERE", WHERE}
  };

  /****************************************************************************
   * IPv4 Regular Expression
   */
  std::regex const ip4_address_regex{
    "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
    "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
    "(\\/([0-9]|[1-2][0-9]|3[0-2]))?$"};

  std::string const select_tok {"select"};
  std::string const from_tok {"from"};

  /****************************************************************************
   * Speculation Lists
   */
  std::vector<int32_t> math_list {ASTERISK, BACKSLASH, MINUS, PLUS};
  std::vector<int32_t> logic_list {NOT, AND, OR};
  std::vector<int32_t> comp_list {EQ, GE, GT, LE, LT, NE};
  std::vector<int32_t> join_list {CROSS, INNER, JOIN, LEFT, RIGHT};
  std::vector<int32_t> name_dot_name_list {NAME, DOT, NAME};
  std::vector<int32_t> name_dot_name_dot_name_list {NAME, DOT, NAME,
    DOT, NAME};
  std::vector<int32_t> name_dot_asterisk_list {NAME, DOT, ASTERISK};
  std::vector<int32_t> name_dot_name_dot_asterisk_list {NAME, DOT, NAME,
    DOT, ASTERISK};
  std::vector<int32_t> primary_list {FLOAT, INT, NAME, VARCHAR};

  /****************************************************************************
   * Default Data
   */
  std::string empty {"\"\""};

  /****************************************************************************
   * Constants
   */
  char const SPC  = ' ';
  char const TAB  = '\t';

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LEXER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS LEXER: Move Assignment
   */
  lexer& lexer::operator=(lexer&& o) {
    if (this == &o) return *this;
    m_input   = o.m_input;
    m_mark    = o.m_mark;
    m_index   = o.m_index;
    m_ch      = o.m_ch;
    m_token_list  = o.m_token_list;
    return *this;
  }

  /****************************************************************************
   *
   */
  std::string lexer::remaining () const {
    return m_input.substr(m_mark, m_input.size() - m_mark);
  }

  /****************************************************************************
   *
   */
  token lexer::next_token () {
    m_mark = m_index;
    while (m_ch != lexer::EOI) {
      switch (m_ch) {
        case ' ': case '\t': case '\n': ws(); break;
        case '\"': case '\'':
          {
            m_token_list.push_back(varchar());
            return m_token_list.back();
          }
        case '#': comment(); break;
        case '&':
          {
            // BWAND or AND
            m_token_list.push_back(compound());
            return m_token_list.back();
          }
        case '(':
          {
            consume();
            m_token_list.emplace_back(L_PAREN, "(");
            return m_token_list.back();
          }
        case ')':
          {
            consume();
            m_token_list.emplace_back(R_PAREN, ")");
            return m_token_list.back();
          }
        case '*':
          {
            consume();
            m_token_list.emplace_back(ASTERISK, "*");
            return m_token_list.back();
          }
        case '+':
          {
            consume();
            m_token_list.emplace_back(PLUS, "+");
            return m_token_list.back();
          }
        case ',':
          {
            consume();
            m_token_list.emplace_back(COMMA, ",");
            return m_token_list.back();
          }
        case '-':
          {
            consume();
            m_token_list.emplace_back(MINUS, "-");
            return m_token_list.back();
          }
        case '.':
          {
            // either DOT or FLOAT
            m_token_list.push_back(compound());
            return m_token_list.back();
          }
        case '/':
          {
            consume();
            m_token_list.emplace_back(BACKSLASH, "/");
            return m_token_list.back();
          }
        case ';':
          {
            consume();
            m_token_list.emplace_back(SEMICOLON, ";");
            return m_token_list.back();
          }
        case '=':
          {
            consume();
            m_token_list.emplace_back(EQ, "=");
            return m_token_list.back();
          }
        case '!': case '<': case '>':
          {
            // NE, LE, LT, GE or GT
            m_token_list.push_back(compound());
            return m_token_list.back();
          }
        case '@':
          {
            consume();
            m_token_list.emplace_back(ATSIGN, "@");
            return m_token_list.back();
          }
        case '|':
          {
            // BWOR or OR
            m_token_list.push_back(compound());
            return m_token_list.back();
          }
        default:
          if (std::isalpha(m_ch)) {
            m_token_list.push_back(name());
            return m_token_list.back();
          } else if (std::isdigit(m_ch)) {
            m_token_list.push_back(number());
            return m_token_list.back();
          } else throw std::runtime_error{eno::get_msg(
              eno::ERRNO_TOKEN_ERROR, remaining())};
      }
    }
    return token {lexer::EOI, "<EOI>"};
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LEXER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void lexer::comment () {
    std::string text {m_ch};  // token list
    consume();
    while (m_ch != '\n' && m_ch != lexer::EOI) {
      text += m_ch; // token list
      consume();
    }
    consume();
    m_token_list.emplace_back(COMMENT, text); // token list
  }

  /****************************************************************************
   *
   */
  void lexer::consume () {
    if (m_index + 1 < m_input.size()) {
      m_ch = m_input [ ++m_index ];
    } else m_ch = lexer::EOI;
  }

  /****************************************************************************
   *
   */
  token lexer::compound () {
    switch (m_ch) {
      case '&':
        consume();
        if (m_ch == '&') {
          consume();
          return token {AND, "&&"};
        } else {
//          return token {BW_AND, "&"};
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_TOKEN_ERROR, remaining())};
        }
      case '.':
        {
          if (std::isdigit(peek())) return number();
          consume();
          return token {DOT, "."};
        }
      case '<':
        consume();
        if (m_ch == '=') {
          consume();
          return token {LE, "<="};
        } else return token {LT, "<"};
      case '>':
        consume();
        if (m_ch == '=') {
          consume();
          return token {GE, ">="};
        } else return token {GT, ">"};
      case '!':
        consume();
        if (m_ch == '=') {
          consume();
          return token {NE, "!="};
        }
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_TOKEN_ERROR, remaining())};
      case '|':
        consume();
        if (m_ch == '|') {
          consume();
          return token {OR, "||"};
        } else {
//          return token {BW_OR, "|"};
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_TOKEN_ERROR, remaining())};
        }
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, remaining())};
    }
  }

  /****************************************************************************
   *
   */
  int32_t lexer::get_symbol_type (std::string const& text) {
    // convert text to uppercase
    std::string uc_text;
    for (char ch : text) uc_text += std::toupper(ch);
    // search symbol table
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
  void lexer::init () {
    if (not m_input.empty()) m_ch = m_input [ m_index ];
    else m_ch = lexer::EOI;
  }

  /****************************************************************************
   *
   */
  token lexer::name () {
    std::string text;
    while (std::isalnum(m_ch) || m_ch == '_') {
      text += m_ch;
      consume();
    }
    int32_t type = get_symbol_type(text);
    return token {(type < 0 ? NAME : type), text};
  }

  /****************************************************************************
   *
   */
  token lexer::number () {
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
  char lexer::peek () {
    if (m_index >= m_input.size()) return lexer::EOI;
    else return m_input [ m_index + 1 ];
  }

  /****************************************************************************
   *
   */
  token lexer::varchar () {
    std::string text {'\"'};
    consume();  // consume first '\"' || '\''
    while (m_ch != '\"' && m_ch != '\'' && m_ch != lexer::EOI) {
      text += m_ch;
      consume();
    }
    text += '\"';
    consume();  // consume last '\"' || '\''
    return token {VARCHAR, text};
  }

  /****************************************************************************
   *
   */
  void lexer::ws () {
    std::string text;
    while (std::isspace(m_ch)) {
      text += m_ch;
      consume();
    }
    m_token_list.emplace_back(WHITESPACE, text);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS PARSER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  int32_t to_schema_type (int32_t type) {
    switch (type) {
      case FLOAT: return schema::TYPE_FLOAT;
      case INT: return schema::TYPE_INT;
      case NAME: return TYPE_NAME;
      case VARCHAR: return schema::TYPE_VARCHAR;
    }
    return type;
  }

  /****************************************************************************
   *
   */
  std::string parser::dump () const {
    std::string text = ".DATA\n";
    for (token tok : m_data)
      text += "  " + std::to_string(to_schema_type(tok.type()))
        + ' ' + tok.text() + '\n';
    text += "%%\n.TEXT\n";
    if (!m_code.empty())
      for (auto iter = m_code.end() - 1; ; --iter) {
        text += "  " + *iter + '\n';
        if (iter == m_code.begin()) break;
      }
    text += "%%\n";
#ifdef TRAN_DEBUG
    std::cout << text << '\n';
#endif
    return text;
  }

  /****************************************************************************
   *
   */
  void parser::parse () {
    stmt();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS PARSER: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void parser::init () {
    m_data.push_back(token {schema::TYPE_VARCHAR, "\"\""});
    load_lookahead(1);
  }

  /****************************************************************************
   *
   */
  void parser::consume () {
    try {
      ++m_index;
      if (not is_speculating()) {
        if (m_lookahead.size() <= m_index) {
          m_index = 0;
          m_lookahead.clear();
        }
      }
      load_lookahead(1);
    } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  std::string parser::data (int32_t type) {
    std::string index = find_data(get_lookahead_token(0).text());
    if (index == "") {
      index = std::to_string(m_data.size());
      m_data.push_back(get_lookahead_token(0));
    }
    match(type);
    return index;
  }

  /****************************************************************************
   *
   */
  std::string parser::data (std::string const& text) {
    std::string index = find_data(text);
    if (index == "") {
      index = std::to_string(m_data.size());
      m_data.emplace_back(VARCHAR, text);
    }
    return index;
  }

  /****************************************************************************
   *
   */
  std::string parser::find_data (std::string const& name) {
    for (size_t i = 0; i < m_data.size(); ++i)
      if (name == m_data [ i ].text()) return std::to_string(i);
    return "";
  }

  /****************************************************************************
   *
   */
  token parser::get_lookahead_token(size_t i) {
    load_lookahead(i);
    return m_lookahead[m_index + i];
  }

  /****************************************************************************
   *
   */
  void parser::load_lookahead (size_t i) {
    while (m_lookahead.size() < m_index + i) {
      m_lookahead.push_back(m_lexer.next_token());
    }
  }

  /****************************************************************************
   *
   */
  void parser::match (int32_t type) {
    if (get_lookahead_type(0) != type)
      throw std::runtime_error{
        eno::get_msg(eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};

    consume();
  }

  /****************************************************************************
   *
   */
  void parser::match (std::vector<int32_t>& type_list) {
    for (int32_t type : type_list) {
      try {
        match(type);
        return;
      } catch (std::runtime_error) { }
    }

  throw std::runtime_error{eno::get_msg(eno::ERRNO_TOKEN_ERROR,
      m_lexer.remaining())};
}

  /****************************************************************************
   *
   */
  std::string parser::next_header () {
    auto iter = m_lexer.m_token_list.begin();
    if ((*iter).type() == SELECT) {
      ++iter; // skip SELECT
      ++iter; // skip WHITESPACE
    }
    // get expression header
    std::string header;
    for (; iter != m_lexer.m_token_list.end(); ++iter) {
      if ((*iter).type() == COMMA || (*iter).type() == FROM) break;
      header += (*iter).text();
    }
    // erase iterated tokens from list
    if (iter != m_lexer.m_token_list.end()) ++iter; // skip COMMA
    m_lexer.m_token_list.erase(m_lexer.m_token_list.begin(), iter);
    // trim whitespace off front of header
    while (std::isspace(header.front())) header.erase(0, 1);
    // trim whitespace off back of header
    while (std::isspace(header.back())) header.pop_back();
    // replace TAB with SPACE
    for (auto& ch : header) if (ch == TAB) ch = SPC;
    // put header in double-quotes
    std::string temp;
    for (auto ch : header) {
      if (ch == '\"') temp += '\\';
      temp += ch;
    }
    header = temp;
//    if (header.front() != '\"')
    header.insert(header.begin(), '\"');
//    if (header.back() != '\"')
    header.push_back('\"');
    return header;
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::stmt () {
    m_code.push_back("RET");

    while (get_lookahead_type(0) != lexer::EOI) {
      switch (get_lookahead_type(0)) {
        case ALTER: consume(); alter_stmt(); break;
        case CREATE: consume(); create_stmt(); break;
        case DELETE: consume(); delete_stmt(); break;
        case DESCRIBE: consume(); describe_stmt(); break;
        case DROP: consume(); drop_stmt(); break;
        case GRANT: consume(); grant_stmt(); break;
        case INSERT: consume(); insert_stmt(); break;
        case LOAD: consume(); load_stmt(); break;
        case REVOKE: consume(); revoke_stmt(); break;
        case SELECT: consume(); select_stmt(); break;
        case SHOW: consume(); show_stmt(); break;
        case SEMICOLON: consume(); break;
        case UPDATE: consume(); update_stmt(); break;
        case USE: consume(); use_stmt(); break;
        default: throw std::runtime_error{eno::get_msg(
                     eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
      }
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * ALTER STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::alter_stmt () {
    switch (get_lookahead_type(0)) {
      case DATABASE: consume(); alter_database(); break;
      case TABLE: consume(); alter_table(); break;
      case USER: consume(); alter_user(); break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::alter_database () {
    m_code.push_back("CALL ALTDB");
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****************************************************************************
   *
   */
  void parser::alter_table () {
    auto mrk = m_code.size(); // mark position of ALTER call (no pun intended)
    m_code.push_back("CALL ALT"); // allocate space for ALTER call

    load_lookahead(2);

    std::vector<std::string> tbl_code;

    tbl_code.push_back("CALL ULCKTBL");

    table_reference(tbl_code);

    switch (get_lookahead_type(0)) {
      case ADD: consume(); alter_add(mrk, tbl_code); break;
      case DROP: consume(); alter_drop(mrk); break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }

    for (auto c : tbl_code) m_code.push_back(c);

    m_code.push_back("CALL MKWRKSCH");
  }

  /****************************************************************************
   *
   */
  void parser::alter_user () {
    m_code.push_back("CALL ALTUSR");
    if (get_lookahead_type(0) == USER) {
      consume();
      match(L_PAREN);
      match(R_PAREN);
      m_code.push_back("CALL MKUSR");
    } else {
      m_code.push_back("PUSHO " + data(VARCHAR)); // username
      match(ATSIGN);
      m_code.push_back("PUSHO " + data(VARCHAR)); // hostname
    }
    match(IDENTIFIED);
    match(BY);
    m_code.push_back("PUSHO " + data(VARCHAR)); // new plain text password
    if (get_lookahead_type(0) == REPLACE) {
      consume();
      m_code.push_back("PUSHO " + data(VARCHAR)); // old plain text password
    } else m_code.push_back("PUSHO " + find_data(empty)); // no password
  }

  /****************************************************************************
   *
   */
  void parser::alter_add (size_t const mrk, std::vector<std::string>& tbl_code)
  {
    switch(get_lookahead_type(0)) {
      case COLUMN:
        consume();
        add_column(mrk);
        break;
      case FOREIGN:
        consume();
        add_foreign_key(mrk, tbl_code);
        break;
      case INDEX:
        consume();
        add_index(mrk);
        break;
      case UNIQUE:
        consume();
        add_unique_index(mrk);
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::add_column (size_t const mrk) {
    m_code[ mrk ] = "CALL ADDCLMN";
    m_code.push_back("PUSHO " + data(NAME));
    data_type(m_code);
    constraint(m_code);
    switch (get_lookahead_type(0)) {
      case AFTER:
        consume();
        m_code.push_back("PUSH 2");
        m_code.push_back("PUSHO " + data(NAME));
        break;
      case FIRST:
        consume();
        m_code.push_back("PUSH 1");
        m_code.push_back("PUSHO " + data(NAME));
        break;
      default: m_code.push_back("PUSH 0");
    }
  }

  /****************************************************************************
   *
   */
  void parser::add_foreign_key (size_t const mrk,
      std::vector<std::string>& tbl_code)
  {
    match(KEY);
    m_code[ mrk ] = "CALL ADDFGNKEY";
    index_key();
    match(REFERENCES);
    tbl_code.push_back("CALL ULCKTBL");
    table_reference(tbl_code);
    index_key();
  }

  /****************************************************************************
   *
   */
  void parser::add_index (size_t const mrk) {
    m_code[ mrk ] = "CALL ADDNDX";
    index_key();
  }

  /****************************************************************************
   *
   */
  void parser::add_unique_index (size_t const mrk) {
    match(INDEX);
    m_code[ mrk ] = "CALL ADDUNINDX";
    index_key();
  }

  /****************************************************************************
   *
   */
  void parser::index_key () {
    match(L_PAREN);
    auto mrk = m_code.size();  // mark position of column-count
    m_code.push_back(""); // allocate space for column-count
    int32_t count = 0;
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));
      ++count;
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
    m_code [ mrk ] = "PUSH " + std::to_string(count); // update column-count
    match(R_PAREN);
  }

  /****************************************************************************
   *
   */
  void parser::alter_drop (size_t const mrk) {
    switch(get_lookahead_type(0)) {
      case COLUMN:
        consume();
        drop_column(mrk);
        break;
      case FOREIGN:
        consume();
        drop_foreign_key(mrk);
        break;
      case INDEX:
        consume();
        drop_index(mrk);
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::drop_column (size_t const mrk) {
    m_code[ mrk ] = "CALL DROPCLMN";
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****************************************************************************
   *
   */
  void parser::drop_foreign_key (size_t const mrk) {
    match(KEY);
    m_code[ mrk ] = "CALL DROPFGNKEY";
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****************************************************************************
   *
   */
  void parser::drop_index (size_t const mrk) {
    m_code[ mrk ] = "CALL DROPNDX";
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * CREATE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::create_stmt () {
    switch (get_lookahead_type(0)) {
      case DATABASE:
        consume();
        create_database();
        break;
      case TABLE:
        consume();
        create_table();
        break;
      case USER:
        consume();
        create_user();
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * CREATE DATABASE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::create_database () {
    m_code.push_back("CALL CRTDB");
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * CREATE TABLE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::create_table () {
    m_code.push_back("CALL CRTTBL");

    load_lookahead(2);

    if (get_lookahead_type(0) == NAME && get_lookahead_type(1) == DOT) {
      m_code.push_back("PUSHO " + data(NAME));
      match(DOT);
      m_code.push_back("PUSHO " + data(NAME));
    } else {
      m_code.push_back("PUSHO " + find_data(empty));
      m_code.push_back("PUSHO " + data(NAME));
    }

    match(L_PAREN);

    std::vector<std::string> code;

    column_definition_list (code);

    for (auto c : code) m_code.push_back(c);

    match(R_PAREN);

    m_code.push_back("CALL MKWRKSCH");
  }

  /****************************************************************************
   *
   */
  void parser::column_definition_list (std::vector<std::string>& code) {
    std::vector<std::vector<std::string> > code_list;

    while (true) {
      std::vector<std::string> cd_code;

      column_definition(cd_code);

      code_list.push_back(cd_code);

      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }

    std::reverse(code_list.begin(), code_list.end());

    for (auto cl : code_list)
      for (auto c : cl) code.push_back(c);
  }

  /****************************************************************************
   *
   */
  void parser::column_definition (std::vector<std::string>& code)
  {
    switch (get_lookahead_type(0)) {
      case NAME:
        code.push_back("NOP");  // tentative position for CALL FINFGN
        code.push_back("CALL MKCLMNDEF");
        code.push_back("PUSHO " + data(NAME));
        data_type(code);
        constraint(code);
        break;
      case FOREIGN:
      case PRIMARY:
        key_definition();
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::data_type (std::vector<std::string>& code) {
    switch (get_lookahead_type(0)) {
      case CHAR:
        match(CHAR);
        code.push_back("PUSH " + std::to_string(schema::TYPE_CHAR));
        match(L_PAREN);
        code.push_back("PUSH " + get_lookahead_token(0).text());
        match(INT);
        match(R_PAREN);
        break;
      case DATE:
        consume();
        code.push_back("PUSH " + std::to_string(schema::TYPE_DATE));
        code.push_back("PUSH 10");
        break;
      case DATETIME:
        consume();
        code.push_back("PUSH " + std::to_string(schema::TYPE_DATETIME));
        code.push_back("PUSH 19");
        break;
      case ENUM:
        {
          match(ENUM);
          code.push_back("PUSH " + std::to_string(schema::TYPE_ENUM));
          // mark position of push-enum-count code and push an empty string
          size_t mrk = code.size();
          code.push_back("");
          // continue parsing enum
          match(L_PAREN);
          int count = 0;
          while (true) {
            code.push_back("PUSHO " + data(VARCHAR));
            ++count;
            if (get_lookahead_type(0) == R_PAREN) break;
            match(COMMA);
          }
          match(R_PAREN);
          // update push-enum-count code
          code [ mrk ] = "PUSH " + std::to_string(count);
          break;
        }
      case FLOAT:
        match(FLOAT);
        code.push_back("PUSH " + std::to_string(schema::TYPE_FLOAT));
        match(L_PAREN);
        code.push_back("PUSH " + get_lookahead_token(0).text());  // width
        match(INT);
        match(COMMA);
        code.push_back("PUSH " + get_lookahead_token(0).text());  // decimal places
        match(INT);
        match(R_PAREN);
        break;
      case INT:
        match(INT);
        code.push_back("PUSH " + std::to_string(schema::TYPE_INT));
        code.push_back("PUSH 10");
        break;
      case TIMESTAMP:
        consume();
        code.push_back("PUSH " + std::to_string(schema::TYPE_TIMESTAMP));
        code.push_back("PUSH 19");
        break;
      case VARCHAR:
        match(VARCHAR);
        code.push_back("PUSH " + std::to_string(schema::TYPE_VARCHAR));
        match(L_PAREN);
        code.push_back("PUSH " + get_lookahead_token(0).text());
        match(INT);
        match(R_PAREN);
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::constraint (std::vector<std::string>& code) {
    // pop back flag
    int32_t flag = 0;
    auto mrk1 = code.size(); // mark position to push column flags
    code.push_back("PUSH 0");  // tentative flags
    while (true) {
      switch (get_lookahead_type(0)) {
        case AUTO_INC: 
          consume();
          flag |= schema::FLAG_AUTO_INC;
          break;
        case UNSIGNED:
          consume();
          flag |= schema::FLAG_UNSIGNED;
          break;
        case ZEROFILL:
          consume();
          flag |= schema::FLAG_ZEROFILL;
          break;
        default: // update column flags and return
          code [ mrk1 ] = "PUSH " + std::to_string(flag);
          return;
      }
    }
  }

  /****************************************************************************
   *
   */
  void parser::key_definition () {
    switch (get_lookahead_type(0)) {
      case FOREIGN: consume(); match(KEY); foreign_key(); break;
      case PRIMARY: consume(); match(KEY); primary_key(); break;
      case UNIQUE: consume(); unique_key(); break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void parser::foreign_key () {
    m_code.push_back("CALL MKFGNKEY");
    // parse and translate local column name(s)
    match(L_PAREN);
    auto mrk = m_code.size();  // mark position of column-count
    m_code.push_back(""); // allocate space for column-count
    int32_t count = 0;
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));
      ++count;
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
    m_code [ mrk ] = "PUSH " + std::to_string(count); // update column-count
    match(R_PAREN);
    match(REFERENCES);
    // parse and translate foreign table reference
    load_lookahead(2);
    if (get_lookahead_type(0) == NAME && get_lookahead_type(1) == DOT) {
      m_code.push_back("PUSHO " + data(NAME));
      match(DOT);
      m_code.push_back("PUSHO " + data(NAME));
    } else {
      m_code.push_back("PUSHO " + find_data(empty));
      m_code.push_back("PUSHO " + data(NAME));
    }
    // parse and translate foreign column name(s)
    match(L_PAREN);
    mrk = m_code.size();  // mark position of column-count
    m_code.push_back(""); // allocate space for column-count
    count = 0;
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));
      ++count;
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
    m_code [ mrk ] = "PUSH " + std::to_string(count); // update column-count
    match(R_PAREN);
  }

  /****************************************************************************
   *
   */
  void parser::primary_key () {
    m_code.push_back("CALL MKPRIKEY");
    match(L_PAREN);
    auto mrk = m_code.size();  // mark position of column-count
    m_code.push_back(""); // allocate space for column-count
    int32_t count = 0;
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));
      ++count;
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
    m_code [ mrk ] = "PUSH " + std::to_string(count); // update column-count
    match(R_PAREN);
  }

  /****************************************************************************
   *
   */
  void parser::unique_key () {
    m_code.push_back("CALL MKUNIKEY");
    match(L_PAREN);
    auto mrk = m_code.size();  // mark position of column-count
    m_code.push_back(""); // allocate space for column-count
    int32_t count = 0;
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));
      ++count;
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
    m_code [ mrk ] = "PUSH " + std::to_string(count); // update column-count
    match(R_PAREN);
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * CREATE USER STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::create_user () {
    m_code.push_back("CALL CRTUSR");
    m_code.push_back("PUSHO " + data(VARCHAR)); // username
    match(ATSIGN);
    m_code.push_back("PUSHO " + data(VARCHAR)); // hostname
    match(IDENTIFIED);
    match(BY);
    m_code.push_back("PUSHO " + data(VARCHAR)); // plain text password
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DELETE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::delete_stmt () {
    m_code.push_back("CALL DELETE");

    match(FROM);

    std::vector<std::string> tbl_code;

    tbl_code.push_back("CALL ULCKTBL");

    table_reference(tbl_code);

    opt_where();

    for (auto c : tbl_code) m_code.push_back(c);
  }


  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DESCRIBE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::describe_stmt () {
    m_code.push_back("CALL DESCRIBE");
    m_code.push_back("CALL SLCKTBL");
    table_reference(m_code);
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DROP STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::drop_stmt () {
    switch (get_lookahead_type(0)) {
      case DATABASE:
        consume();
        drop_database();
        break;
      case TABLE:
        consume();
        drop_table();
        break;
      case USER:
        consume();
        drop_user();
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DROP DATABASE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::drop_database () {
    m_code.push_back("CALL DROPDB");
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DROP TABLE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::drop_table () {
    m_code.push_back("CALL DROPTBL");
    load_lookahead(2);
    if (get_lookahead_type(0) == NAME && get_lookahead_type(1) == DOT) {
      m_code.push_back("PUSHO " + data(NAME));
      match(DOT);
      m_code.push_back("PUSHO " + data(NAME));
    } else {
      m_code.push_back("PUSHO " + find_data(empty));
      m_code.push_back("PUSHO " + data(NAME));
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * DROP USER STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::drop_user () {
    m_code.push_back("CALL DROPUSR");
    // 'username'@'hostname'
    m_code.push_back("PUSHO " + data(VARCHAR));
    match(ATSIGN);
    m_code.push_back("PUSHO " + data(VARCHAR));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * GRANT STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::grant_stmt () {
    m_code.push_back("CALL GRANT");

    auto mrk = m_code.size(); // mark position of count
    m_code.push_back(""); // allocate count

    bool loop {true};
    size_t count {0L};
    while (loop) {
      switch (get_lookahead_type(0)) {
        case COMMA: consume(); break;
        case ON: loop = false; break;
        case ALL:
        case ALTER:
        case DELETE:
        case DROP:
        case FILE_:
        case GRANT:
        case INDEX:
        case INSERT:
        case RELOAD:
        case SELECT:
        case SHUTDOWN:
        case UPDATE:
          {
            std::string name = get_lookahead_token(0).text() + "_priv";
            for (auto& ch : name) ch = std::tolower(ch);
            m_code.push_back("PUSHO " + data(name));
            consume();
            ++count;
          }
          break;
        case CREATE:
          {
            consume();
            load_lookahead(2);
            if (get_lookahead_type(1) == USER) {
              consume();
              std::string name = "create_user_priv";
              m_code.push_back("PUSHO " + data(name));
              ++count;
            } else {
              std::string name = "create_priv";
              m_code.push_back("PUSHO " + data(name));
              ++count;
            }
          }
          break;
        case SHOW:
          {
            consume();
            match(DATABASE);
            std::string name = "show_db_priv";
            m_code.push_back("PUSHO " + data(name));
            ++count;
          }
          break;
        default: throw std::runtime_error{eno::get_msg(
                     eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
      }
    }
    // update count
    m_code[ mrk ] = "PUSH " + std::to_string(count);

    match(ON);

    switch (get_lookahead_type(0)) {
      case ASTERISK:
        load_lookahead(2);
        if (get_lookahead_type(1) == DOT) {
          m_code.push_back("PUSHO " + data(ASTERISK));
          match(DOT);
          m_code.push_back("PUSHO " + data(ASTERISK));
        } else {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(ASTERISK));
        }
        break;
      case NAME:
        load_lookahead(3);
        if (get_lookahead_type(1) == DOT) {
          if (get_lookahead_type(2) == ASTERISK) {
            m_code.push_back("PUSHO " + data(NAME));
            match(DOT);
            m_code.push_back("PUSHO " + data(ASTERISK));
          } else if (get_lookahead_type(2) == NAME) {
            m_code.push_back("PUSHO " + data(NAME));
            match(DOT);
            m_code.push_back("PUSHO " + data(NAME));
          }
        } else {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(NAME));
        }
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }

    match(TO);

    m_code.push_back("PUSHO " + data(VARCHAR));
    match(ATSIGN);
    m_code.push_back("PUSHO " + data(VARCHAR));

    if (get_lookahead_type(0) == IDENTIFIED) {
      consume();
      match(BY);
      m_code.push_back("PUSH 1");
      m_code.push_back("PUSHO " + data(VARCHAR));
    } else m_code.push_back("PUSH 0");
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * INSERT STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::insert_stmt () {
    m_code.push_back("CALL INSERT");

    match(INTO);

    std::vector<std::string> tbl_code;

    tbl_code.push_back("CALL ULCKTBL");

    table_reference(tbl_code);

    opt_column_list();

    match(VALUES);

    value_set_list();

    for (auto c : tbl_code) m_code.push_back(c);
  }

  /****************************************************************************
   *
   */
  void parser::opt_column_list() {
    auto mrk = m_code.size(); // mark index of column-count

    m_code.push_back(""); // construct column-count element

    int32_t count {0};

    if (get_lookahead_type(0) == L_PAREN) {
      consume();

      column_list(count);

      match(R_PAREN);
    }

    m_code[ mrk ] = "PUSH " + std::to_string(count);  // update column-count
  }

  /****************************************************************************
   *
   */
  void parser::column_list (int32_t& count) {
    while (true) {
      m_code.push_back("PUSHO " + data(NAME));

      ++count;

      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
  }

  /****************************************************************************
   *
   */
  void parser::value_set_list () {
    auto mrk = m_code.size(); // mark index of set-count

    m_code.push_back(""); // construct set-count element

    int32_t count {0};
    while (true) {
      match(L_PAREN);
      value_set();
      ++count;
      match(R_PAREN);
      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }

    m_code[ mrk ] = "PUSH " + std::to_string(count);  // update set-count
  }

  /****************************************************************************
   *
   */
  void parser::value_set () {
    size_t mrk = m_code.size(); // mark index of value-count

    m_code.push_back(""); // construct value-count element

    int count = 0;
    while (true) {
      value_expr(); // translate an expression

      ++count;  // increment value count

      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }

    m_code [ mrk ] = "PUSH " + std::to_string(count); // update value-count
  }

  /****************************************************************************
   *
   */
  void parser::value_expr () {
    std::vector<std::string> code;
    expr(code);
    for (auto c : code) m_code.push_back(c);
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * LOAD STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::load_stmt () {
    m_code.push_back("CALL LOADDATA");
    match(DATA);
    match(INFILE);
    m_code.push_back("PUSHO " + data(VARCHAR));
    switch (get_lookahead_type(0)) {
      case IGNORE:
        m_code.push_back("PUSH 1");
        break;
      case REPLACE:
        m_code.push_back("PUSH 2");
        break;
      default: m_code.push_back("PUSH 0");
    }
    match(INTO);
    match(TABLE);
    m_code.push_back("CALL ULCKTBL");
    table_reference(m_code);
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * REVOKE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::revoke_stmt () {
    m_code.push_back("CALL REVOKE");

    auto mrk = m_code.size(); // mark position of count
    m_code.push_back(""); // allocate count

    bool loop {true};
    size_t count {0L};
    while (loop) {
      switch (get_lookahead_type(0)) {
        case COMMA: consume(); break;
        case ON: loop = false; break;
        case ALL:
        case ALTER:
        case DELETE:
        case DROP:
        case FILE_:
        case GRANT:
        case INDEX:
        case INSERT:
        case RELOAD:
        case SELECT:
        case SHUTDOWN:
        case UPDATE:
          {
            std::string name = get_lookahead_token(0).text() + "_priv";
            for (auto& ch : name) ch = std::tolower(ch);
            m_code.push_back("PUSHO " + data(name));
            consume();
            ++count;
          }
          break;
        case CREATE:
          {
            consume();
            load_lookahead(2);
            if (get_lookahead_type(1) == USER) {
              consume();
              std::string name = "create_user_priv";
              m_code.push_back("PUSHO " + data(name));
              ++count;
            } else {
              std::string name = "create_priv";
              m_code.push_back("PUSHO " + data(name));
              ++count;
            }
          }
          break;
        case SHOW:
          {
            consume();
            match(DATABASE);
            std::string name = "show_db_priv";
            m_code.push_back("PUSHO " + data(name));
            ++count;
          }
          break;
        default: throw std::runtime_error{eno::get_msg(
                     eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
      }
    }
    // update count
    m_code[ mrk ] = "PUSH " + std::to_string(count);

    match(ON);

    switch (get_lookahead_type(0)) {
      case ASTERISK:
        load_lookahead(2);
        if (get_lookahead_type(1) == DOT) {
          m_code.push_back("PUSHO " + data(ASTERISK));
          match(DOT);
          m_code.push_back("PUSHO " + data(ASTERISK));
        } else {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(ASTERISK));
        }
        break;
      case NAME:
        load_lookahead(3);
        if (get_lookahead_type(1) == DOT) {
          if (get_lookahead_type(2) == ASTERISK) {
            m_code.push_back("PUSHO " + data(NAME));
            match(DOT);
            m_code.push_back("PUSHO " + data(ASTERISK));
          } else if (get_lookahead_type(2) == NAME) {
            m_code.push_back("PUSHO " + data(NAME));
            match(DOT);
            m_code.push_back("PUSHO " + data(NAME));
          }
        } else {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(NAME));
        }
        break;
      default: throw std::runtime_error{eno::get_msg(
                   eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }

    match(FROM);

    m_code.push_back("PUSHO " + data(VARCHAR));
    match(ATSIGN);
    m_code.push_back("PUSHO " + data(VARCHAR));

    if (get_lookahead_type(0) == IDENTIFIED) {
      consume();
      match(BY);
      m_code.push_back("PUSH 1");
      m_code.push_back("PUSHO " + data(VARCHAR));
    } else m_code.push_back("PUSH 0");
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * SELECT STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::select_stmt () {
    m_code.push_back("CALL SELECT");

    std::vector<std::string> code;

    select_expr_list(code);

    match(FROM);

    table_reference_list(code);

    opt_where();

    opt_order_by();

    for (std::string c : code) m_code.push_back(c);

    opt_into_outfile();
  }

  /****************************************************************************
   *
   */
  void parser::select_expr_list (std::vector<std::string>& code) {
    code.push_back("CALL MKEXPRLST");

    while (true) {
      select_expr(code);

      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
  }

  /****************************************************************************
   *
   */
  void parser::select_expr (std::vector<std::string>& code) {
    expr(code);

    code.push_back("CALL MKEXPRHDR");
    code.push_back("PUSHO " + data(next_header()));
  }

  /****************************************************************************
   *
   */
  void parser::table_reference_list (std::vector<std::string>& code) {
    std::vector<std::string> tbl_code;
    std::vector<std::vector<std::string> > join_code_list;

    tbl_code.push_back("CALL SLCKTBL");

    table_reference(tbl_code);

    while (speculate_list_to(join_list)) {
      std::vector<std::string> join_code;

      join(join_code);

      tbl_code.push_back("CALL SLCKTBL");

      table_reference(tbl_code);

      on_using (join_code);

      join_code_list.push_back(join_code);
    }
    // append join and on/using code
    while (!join_code_list.empty()) {
      auto jc = join_code_list.back(); join_code_list.pop_back();
      for (auto c : jc) code.push_back(c);
    }
    // append table reference code
    for (auto c : tbl_code) code.push_back(c);
  }

  /****************************************************************************
   *
   */
  void parser::table_reference (std::vector<std::string>& code) {
    code.push_back("CALL OPNTBL");

    load_lookahead(2);

    if (get_lookahead_type(0) == NAME && get_lookahead_type(1) == DOT) {
      code.push_back("PUSHO " + data(NAME));
      match(DOT);
      code.push_back("PUSHO " + data(NAME));
    } else {
      code.push_back("PUSHO " + find_data(empty));
      code.push_back("PUSHO " + data(NAME));
    }

    opt_as_alias(code);
  }

  /****************************************************************************
   *
   */
  void parser::opt_as_alias (std::vector<std::string>& code) {
    if (get_lookahead_type(0) == AS) {
      match(AS);
      code.push_back("PUSHO " + data(NAME));
    } else if (get_lookahead_type(0) == NAME) {
      code.push_back("PUSHO " + data(NAME));
    } else code.push_back("PUSHO " + find_data(empty));
  }

  /****************************************************************************
   *
   */
  void parser::join (std::vector<std::string>& code) {
    switch (get_lookahead_type(0)) {
      case CROSS:
        match(CROSS);
        // this statement will fall through
      case JOIN:
        match(JOIN);
        code.push_back("CALL CRJOIN");
        break;
      case INNER:
        match(INNER);
        match(JOIN);
        code.push_back("CALL INJOIN");
        break;
      case LEFT:
        match(LEFT);
        if (get_lookahead_type(0) == OUTER) match(OUTER);
        match(JOIN);
        code.push_back("CALL LTJOIN");
        break;
      case RIGHT:
        match(RIGHT);
        if (get_lookahead_type(0) == OUTER) match(OUTER);
        match(JOIN);
        code.push_back("CALL RTJOIN");
        break;
      default:;
    }
  }

  /****************************************************************************
   *
   */
  void parser::on_using (std::vector<std::string>& code) {
    if (get_lookahead_type(0) == ON) {
      match(ON);

      code.push_back("CALL ON");

      expr(code); // translate an expression
    } else if (get_lookahead_type(0) == USING) {
      match(USING);

      auto mrk1 = code.size();

      code.push_back("PUSH 0"); // placeholder

      auto mrk2 = code.size();

      match(L_PAREN);

      size_t count {0L};
      while (true) {
        auto mrk3 = code.size();
        expr(code); // translate an expression
        ++count;
        if (count > 1) code [ mrk3 ] = "NOP";
        if (get_lookahead_type(0) != COMMA) break;
        match(COMMA);
      }

      match(R_PAREN);

      code [ mrk2 ] = "PUSH " + std::to_string(count);

      code [ mrk1 ] = "CALL USING";
    }
  }

  /****************************************************************************
   *
   */
  void parser::opt_where () {
    if (get_lookahead_type(0) == WHERE) {
      m_code.push_back("CALL WHERE");
      match(WHERE);
      std::vector<std::string> code;
      expr(code); // translate an expression
      for (auto c : code) m_code.push_back(c);
    }
  }

  /****************************************************************************
   *
   */
  void parser::opt_order_by () {
    load_lookahead(6);

    if (get_lookahead_type(0) == ORDER && get_lookahead_type(1) == BY) {
      m_code.push_back("CALL ORDERBY");
      match(ORDER);
      match(BY);

      auto mrk = m_code.size();
      m_code.push_back("PUSH ");

      bool have_parentheses {true};
      if (get_lookahead_type(0) == L_PAREN) consume();
      else have_parentheses = false;

      size_t count {0L};
      while (true) {
        m_code.push_back("CALL MKCLMNEXPR");
        if (speculate_list(name_dot_name_dot_name_list)) {
          m_code.push_back("PUSHO " + find_data(empty));
          match(DOT);
          m_code.push_back("PUSHO " + data(NAME));
          match(DOT);
          m_code.push_back("PUSHO " + data(NAME));
        } else if (speculate_list(name_dot_name_list)) {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(NAME));
          match(DOT);
          m_code.push_back("PUSHO " + data(NAME));
        } else {
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + find_data(empty));
          m_code.push_back("PUSHO " + data(NAME));
        }

        ++count;

        if (get_lookahead_type(0) == COMMA) consume();
        else break;
      }

      m_code[ mrk ] += std::to_string(count);

      if (have_parentheses) {
        if (get_lookahead_type(0) == R_PAREN) consume();
        else throw std::runtime_error{eno::get_msg(
            eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
      }
    }
  }

  /****************************************************************************
   *
   */
  void parser::opt_into_outfile () {
    if (get_lookahead_type(0) != INTO) return;

    consume();
    match(OUTFILE);

    m_code.push_back("CALL SETOUTF");
    m_code.push_back("PUSHO " + data(VARCHAR));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * SHOW STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::show_stmt () {
    switch (get_lookahead_type(0)) {
      case DATABASES: consume(); m_code.push_back("CALL SHOWDBS"); break;
      case TABLES: consume(); m_code.push_back("CALL SHOWTBLS"); break;
      default:
        throw std::runtime_error{eno::get_msg(
            eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * UPDATE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::update_stmt () {
    m_code.push_back("CALL UPDATE");

    std::vector<std::string> tbl_code;

    tbl_code.push_back("CALL ULCKTBL");

    table_reference(tbl_code);

    match(SET);

    assignment_list();

    opt_where();

    for (auto c : tbl_code) m_code.push_back(c);
  }

  /****************************************************************************
   *
   */
  void parser::assignment_list () {
    while (true) {
      assignment();

      if (get_lookahead_type(0) == COMMA) consume();
      else break;
    }
  }

  /****************************************************************************
   *
   */
  void parser::assignment () {
    auto mrk = m_code.size(); // mark position of assignment call
    std::vector<std::string> code;
    expr(code); // translate an expression
    for (auto c : code) m_code.push_back(c);
    m_code [ mrk ] = "CALL MKASSIGN"; // assign assignment call
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * USE STATEMENT
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::use_stmt () {
    m_code.push_back("CALL USE");
    m_code.push_back("PUSHO " + data(NAME));
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * EXPRESSION STATEMENT (NAME)
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  void parser::name (std::vector<std::string>& code) {
    if (speculate_list(name_dot_name_dot_asterisk_list)) {
      code.push_back("CALL MKASTEXPR");
      code.push_back("PUSHO " + data(NAME));
      match(DOT);
      code.push_back("PUSHO " + data(NAME));
      match(DOT);
      match(ASTERISK);
    } else if (speculate_list(name_dot_asterisk_list)) {
      code.push_back("CALL MKASTEXPR");
      code.push_back("PUSHO " + find_data(empty));
      code.push_back("PUSHO " + data(NAME));
      match(DOT);
      match(ASTERISK);
    } else {
      code.push_back("CALL MKCLMNEXPR");
      if (speculate_list(name_dot_name_dot_name_list)) {
        code.push_back("PUSHO " + find_data(empty));
        match(DOT);
        code.push_back("PUSHO " + data(NAME));
        match(DOT);
        code.push_back("PUSHO " + data(NAME));
      } else if (speculate_list(name_dot_name_list)) {
        code.push_back("PUSHO " + find_data(empty));
        code.push_back("PUSHO " + data(NAME));
        match(DOT);
        code.push_back("PUSHO " + data(NAME));
      } else {
        code.push_back("PUSHO " + find_data(empty));
        code.push_back("PUSHO " + find_data(empty));
        code.push_back("PUSHO " + data(NAME));
      }
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * EXPRESSION STATEMENT (LOGIC)
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/


  /****************************************************************************
   *
   */
  void parser::logic_expr (std::vector<std::string>& code) {
    std::vector<std::string> term_code;

    logic_term(term_code, true);

    while (true) {
      switch (get_lookahead_type(0)) {
        case AND:
          consume();
          
          code.emplace(code.begin(), "CALL AND");

          for (std::string c : term_code) code.push_back(c);

          logic_term(term_code, false);

          for (std::string c : term_code) code.push_back(c);

          term_code.clear();

          break;
        default:
          for (std::string c : term_code) code.push_back(c);
          return;
      }
    }
  }


  /****************************************************************************
   *
   */
  void parser::logic_primary (std::vector<std::string>& code,
      bool use_asterisk)
  {
    code.clear();
    comparison_expr(code, use_asterisk);
  }


  /****************************************************************************
   *
   */
  void parser::logic_term (std::vector<std::string>& code, bool use_asterisk) {
    code.clear();

    std::vector<std::string> primary_code;

    logic_primary(primary_code, use_asterisk);

    while (true) {
      switch (get_lookahead_type(0)) {
        case OR:
          consume();

          code.emplace(code.begin(), "CALL OR");

          for (std::string c : primary_code) code.push_back(c);

          logic_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        default:
          for (std::string c : primary_code) code.push_back(c);
          return;
      }
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * EXPRESSION STATEMENT (COMPARE)
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::comparison_expr (std::vector<std::string>& code,
      bool use_asterisk)
  {
    std::vector<std::string> term_code;

    comparison_term(term_code, use_asterisk);

    while (true) {
      switch (get_lookahead_type(0)) {
        case EQ:
          consume();

          code.emplace(code.begin(), "CALL CMPEQ");

          for (std::string c : term_code) code.push_back(c);

          comparison_term(term_code, false);

          for (std::string c : term_code) code.push_back(c);

          term_code.clear();

          break;
        case NE:
          consume();

          code.emplace(code.begin(), "CALL CMPNE");

          for (std::string c : term_code) code.push_back(c);

          comparison_term(term_code, false);

          for (std::string c : term_code) code.push_back(c);

          term_code.clear();

          break;
        default:
          for (std::string c : term_code) code.push_back(c);
          return;
      }
    }
  }

  /****************************************************************************
   *
   */
  void parser::comparison_primary (std::vector<std::string>& code,
      bool use_asterisk)
  {
    code.clear();
    math_expr(code, use_asterisk);
  }

  /****************************************************************************
   *
   */
  void parser::comparison_term (std::vector<std::string>& code,
      bool use_asterisk)
  {
    code.clear();

    std::vector<std::string> primary_code;

    comparison_primary(primary_code, use_asterisk);

    while (true) {
      switch (get_lookahead_type(0)) {
        case GE:
          consume();

          code.emplace(code.begin(), "CALL CMPGE");

          for (std::string c : primary_code) code.push_back(c);

          comparison_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        case GT:
          consume();

          code.emplace(code.begin(), "CALL CMPGT");

          for (std::string c : primary_code) code.push_back(c);

          comparison_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        case LE:
          consume();

          code.emplace(code.begin(), "CALL CMPLE");

          for (std::string c : primary_code) code.push_back(c);

          comparison_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        case LT:
          consume();

          code.emplace(code.begin(), "CALL CMPLT");

          for (std::string c : primary_code) code.push_back(c);

          comparison_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        default:
          for (std::string c : primary_code) code.push_back(c);
          return;
      }
    }
  }
  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * EXPRESSION STATEMENT (MATH)
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  void parser::math_expr (std::vector<std::string>& code, bool use_asterisk) {
    std::vector<std::string> term_code;

    math_term(term_code, use_asterisk);

    while (true) {
      switch (get_lookahead_type(0)) {
        case MINUS:
          consume();

          code.emplace(code.begin(), "CALL SUB");

          for (std::string c : term_code) code.push_back(c);

          math_term(term_code, false);

          for (std::string c : term_code) code.push_back(c);

          term_code.clear();

          break;
        case PLUS:
          consume();

          code.emplace(code.begin(), "CALL ADD");

          for (std::string c : term_code) code.push_back(c);

          math_term(term_code, false);

          for (std::string c : term_code) code.push_back(c);

          term_code.clear();

          break;
        default:
          for (std::string c : term_code) code.push_back(c);
          return;
      }
    }
  }

  /****************************************************************************
   *
   */
  void parser::math_primary (std::vector<std::string>& code, bool use_asterisk)
  {
    code.clear();

    load_lookahead(6);

    switch (get_lookahead_type(0)) {
      case ASTERISK:
        if (not use_asterisk)
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};

        consume();

        if (get_lookahead_type(0) == ASTERISK)
          throw std::runtime_error{eno::get_msg(
              eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};

        code.push_back("CALL MKASTEXPR");
        code.push_back("PUSHO " + find_data(empty));
        code.push_back("PUSHO " + find_data(empty));
        break;
      case FLOAT:
      case INT:
        code.push_back("CALL MKEXPR");
        code.push_back("PUSHO " + data(get_lookahead_type(0)));
        break;
      case L_PAREN:
        consume();
        logic_expr(code);
        match(R_PAREN);
        break;
      case MINUS:
        {
          consume();
          math_primary(code, false);
          code.insert(code.begin(), "CALL NEGATE");
        }
        break;
      case NAME:
        name(code);
        break;
      case NULL_SYM:
        consume();
        code.push_back("CALL MKEXPR");
        code.push_back("PUSHO " + data("null"));
        break;
      case PLUS:
        consume();
        math_primary(code, false);
        break;
      case VARCHAR:
        code.push_back("CALL MKEXPR");
        code.push_back("PUSHO " + data(VARCHAR));
        break;
      default: throw std::runtime_error{
                 eno::get_msg(eno::ERRNO_TOKEN_ERROR, m_lexer.remaining())};
    }
  }

  /****************************************************************************
   *
   */
  void parser::math_term (std::vector<std::string>& code, bool use_asterisk) {
    code.clear();

    std::vector<std::string> primary_code;

    math_primary(primary_code, use_asterisk);

    while (true) {
      switch (get_lookahead_type(0)) {
        case ASTERISK:
          consume();

          code.emplace(code.begin(), "CALL MUL");

          for (std::string c : primary_code) code.push_back(c);

          math_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        case BACKSLASH:
          consume();

          code.emplace(code.begin(), "CALL DIV");

          for (std::string c : primary_code) code.push_back(c);

          math_primary(primary_code, false);

          for (std::string c : primary_code) code.push_back(c);

          primary_code.clear();

          break;
        default:
          for (std::string c : primary_code) code.push_back(c);
          return;
      }
    }
  }

  /****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****
   * SPECULATE
   ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****/

  /****************************************************************************
   *
   */
  bool parser::speculate_list (std::vector<int32_t>& list) {
    bool success {true};

    mark();

    try {
      for (int32_t type : list) match(type);
    } catch (std::runtime_error&) { success = false; }

    unmark();

    return success;
  }

  /****************************************************************************
   *
   */
  bool parser::speculate_list_to (std::vector<int32_t>& list) {
    bool success = false;

    for (int32_t type : list)
      if (get_lookahead_type(0) == type) { success = true; break; }

    return success;
  }

} // namespace


//-----------------------------------------------------------------------------
// Desc: Token library
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

#ifndef TOKEN_H
#define TOKEN_H 1

#include <cstddef>
#include <cstdint>
#include <string>

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS TOKEN
   */
  class token {
    public:
      token () : m_type {-1} { }

      token (int32_t type, std::string const& text)
        : m_type {type}, m_text {text}
      { }

      ~token () { }

      token (token const& tok)
        : m_type {tok.m_type}, m_text {tok.m_text}
      { }

      token (token&& tok)
        : m_type {tok.m_type}, m_text {tok.m_text}
      { }


      token& operator=(token const&);
      token& operator=(token&&);

      int32_t type () const { return m_type; }
      std::string text () const { return m_text; }
    private:
      int32_t     m_type;
      std::string m_text;
  };

} // namespace

#endif  // TOKEN_H


//-----------------------------------------------------------------------------
// Desc: Btree calculator library
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

#ifndef BCALC_H
#define BCALC_H 1

#include <cstdint>
#include "../btree/const.h"

namespace red::tree {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BTREE CALCULATOR
   */
  class bcalc {
    public:
      bcalc (int32_t key_size) : m_key_size {key_size} { }

      bcalc (bcalc const& o) : m_key_size {o.m_key_size} { }
      bcalc (bcalc&& o) : m_key_size {o.m_key_size} { }

      bcalc& operator=(bcalc const&);
      bcalc& operator=(bcalc&&);

      int32_t order ();
      int32_t min_degree ();
    private:
      int32_t m_key_size;
  };

} // namespace

#endif  // BCALC_H


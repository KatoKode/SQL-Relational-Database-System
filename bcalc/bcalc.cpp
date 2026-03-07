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

#include "bcalc.h"

namespace red::tree {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BCALC: PUBLIC
   */

  /****************************************************************************
   * CLASS BCALC: Copy Assignment
   */
  bcalc& bcalc::operator=(bcalc const& o) {
    if (this == &o) return *this;
    m_key_size = o.m_key_size;
    return *this;
  }

  /****************************************************************************
   * CLASS BCALC: Move Assignment
   */
  bcalc& bcalc::operator=(bcalc&& o) {
    if (this == &o) return *this;
    m_key_size = o.m_key_size;
    return *this;
  }

  /****************************************************************************
   *
   */
  int32_t bcalc::order () {
    // calculate vector element size for a non-leaf node
    int32_t const VECTOR_ELEMENT_SZ = CHILD_OFFSET_SZ + m_key_size;
    // calculate b-tree order based on a non-leaf node buffer capacity
    return (BUFFER_SZ - (NODE_HDR_SZ + CHILD_OFFSET_SZ)) / VECTOR_ELEMENT_SZ;
  }

  /****************************************************************************
   *
   */
  int32_t bcalc::min_degree () {
    // calculate vector element size for a non-leaf node
    int32_t const VECTOR_ELEMENT_SZ = CHILD_OFFSET_SZ + m_key_size;
    // calculate b-tree order based on a non-leaf node buffer capacity
    int32_t order = (BUFFER_SZ - (NODE_HDR_SZ + CHILD_OFFSET_SZ))
      / VECTOR_ELEMENT_SZ;
    return order / 2;
  }

} // namespace


//-----------------------------------------------------------------------------
// Desc: Utility Library
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

#include "util.h"

namespace red {

  /****************************************************************************
   *
   */
  int32_t rand_int32(int32_t const min, int32_t const max) {
    std::uniform_int_distribution<int32_t> distribution(min, max);
    std::random_device rd;
    std::default_random_engine generator( rd() );
    return distribution(generator);
  }

} // namespace


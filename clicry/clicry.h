//-----------------------------------------------------------------------------
// Desc: Private/Public Key Encryption/Decryption Library
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

// https://www.gnupg.org/software/libgcrypt/index.html
// https://www.gnupg.org/documentation/manuals/gcrypt.pdf

#ifndef CLICRY_H
#define CLICRY_H 1

#pragma once

#include <unistd.h>
#include <gcrypt.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include "eno.h"
#include "logger.h"
#include "util.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS CLICRY
   */
  class clicry {
    public:
      clicry (size_t buflen, uint8_t* buffer)
        : m_is_initialized {true}, m_pubkey {nullptr}
      {
        init();
        buffer_to_pubkey(buflen, buffer);
      }

      ~clicry () { gcry_sexp_release(m_pubkey); }

      clicry (clicry const&) = delete;
      clicry (clicry&&) = delete;

      clicry& operator=(clicry const&) = delete;
      clicry& operator=(clicry&&) = delete;

      bool encrypt (size_t, uint8_t*, size_t*, uint8_t*);
    private:
      bool m_is_initialized;
      gcry_sexp_t m_pubkey;

      void init ();
      bool is_initialized () { return m_is_initialized; }
      void buffer_to_pubkey (size_t, uint8_t*);
  };

} // namespace

#endif  // CLICRY_H

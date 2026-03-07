//-----------------------------------------------------------------------------
// Desc: Server Public Key Encryption/Decryption Library
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

// https://www.gnupg.org/software/libsrvcrypt/index.html
// https://www.gnupg.org/documentation/manuals/srvcrypt.pdf

#ifndef SRVCRY_H
#define SRVCRY_H 1

#pragma once

#include <unistd.h>
#include <gcrypt.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <sstream>
#include <random>
#include "eno.h"
#include "logger.h"
#include "util.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS GCRY
   */
  class srvcry {
    public:
      srvcry (std::shared_ptr<logger> logger, std::string const& base_path,
          std::string const& pkey_name, std::string const& keyp_name,
          std::string const& hash_name)
        : m_is_initialized {true},
        m_logger {logger},
        m_base_path {base_path},
        m_pkey_name {pkey_name},
        m_keyp_name {keyp_name},
        m_hash_name {hash_name},
        m_prikey {nullptr},
        m_pubkey {nullptr}
      {
        init();
      }

      ~srvcry () {
        gcry_sexp_release(m_pubkey);
        gcry_sexp_release(m_prikey);
      }

      srvcry (srvcry const&) = delete;
      srvcry (srvcry&&) = delete;

      srvcry& operator=(srvcry const&) = delete;
      srvcry& operator=(srvcry&&) = delete;

      uint8_t* decrypt (size_t, uint8_t*);
      bool hash_text (std::string&, std::string const&);
      bool hashgen ();
      bool keygen ();
      bool load_hash ();
      bool load_keypair ();
      bool pubkey_to_buffer(size_t*, uint8_t*);
    private:
      bool  m_is_initialized;
      std::shared_ptr<logger> m_logger;
      std::string   m_base_path;
      std::string   m_pkey_name;
      std::string   m_keyp_name;
      std::string   m_hash_name;
      gcry_sexp_t   m_prikey;
      gcry_sexp_t   m_pubkey;
      std::string   m_hashkey;

      std::string get_password ();
      void init();
      bool is_initialized () { return m_is_initialized; }
      bool init_aes_hd(gcry_cipher_hd_t* aes_hd);
  };

} // namespace

#endif  // SRVCRY_H

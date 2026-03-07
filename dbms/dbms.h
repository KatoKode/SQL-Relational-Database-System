//-----------------------------------------------------------------------------
// Desc: DBMS library
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

#ifndef DBMS_H
#define DBMS_H 1

#include <unistd.h>
#include <stdlib.h>
#include <cerrno>
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <queue>
#include <utility>
#include <random>
#include "eno.h"
#include "logger.h"
#include "dio.h"
#include "schema.h"
#include "btree.h"
#include "lock.h"
#include "cache.h"

namespace red::dbms {

  typedef std::string (*decrypt_credential_t) (size_t, uint8_t*);

  std::string const BASE_DIR {"/var/lib/redb"};
  std::string const SRC_DIR {"/tmp/redb"};
  std::string const ETC_DIR {"etc"};
  std::string const LOG_DIR {"log"};
  std::string const TMP_DIR {"tmp"};
  std::string const CRED_TBL {"cred"};
  std::string const PRIV_TBL {"priv"};
  std::string const HOST_FLD {"hostname"};
  std::string const USER_FLD {"username"};
  std::string const PWRD_FLD {"password"};
  std::string const HASH_FIL {".redbhash"};
  std::string const PKEY_FIL {".redbpkey"};
  std::string const KEYP_FIL {".redbkeyp"};
  std::string const ALL_PRIV {"all_priv"};
  std::string const ALTER_PRIV {"alter_priv"};
  std::string const CREATE_PRIV {"create_priv"};
  std::string const CREATE_USER_PRIV {"create_user_priv"};
  std::string const DELETE_PRIV {"delete_priv"};
  std::string const DROP_PRIV {"drop_priv"};
  std::string const FILE_PRIV {"file_priv"};
  std::string const GRANT_PRIV {"grant_priv"};
  std::string const INDEX_PRIV {"index_priv"};
  std::string const INSERT_PRIV {"insert_priv"};
  std::string const RELOAD_PRIV {"reload_priv"};
  std::string const SELECT_PRIV {"select_priv"};
  std::string const SHOW_DB_PRIV {"show_db_priv"};
  std::string const SHUTDOWN_PRIV {"shutdown_priv"};
  std::string const UPDATE_PRIV {"update_priv"};
  std::string const BAD_BASE {"DBMS\n\tBad base path: "};
  std::string const NO_ENV {"DBMS\n\tMissing base path environment variable"};
  std::string const ASTERISK {"*"};
  std::string const YES {"Y"};
  std::string const NO {"N"};
  std::pair<std::string const, std::string const> const PRIV_KEY {
    ETC_DIR, PRIV_TBL
  };
  std::vector<std::string> const GLOBAL_PRIVILEGE_LIST {
    ALTER_PRIV,
    CREATE_PRIV,
    CREATE_USER_PRIV,
    DELETE_PRIV,
    DROP_PRIV,
    FILE_PRIV,
    GRANT_PRIV,
    INDEX_PRIV,
    INSERT_PRIV,
    RELOAD_PRIV,
    SELECT_PRIV,
    SHOW_DB_PRIV,
    SHUTDOWN_PRIV,
    UPDATE_PRIV,
  };
  std::vector<std::string> const LOCAL_PRIVILEGE_LIST {
    ALTER_PRIV,
    CREATE_PRIV,
    DELETE_PRIV,
    DROP_PRIV,
    FILE_PRIV,
    GRANT_PRIV,
    INDEX_PRIV,
    INSERT_PRIV,
    SELECT_PRIV,
    UPDATE_PRIV,
  };

  size_t const TBL_NAME_MAX = (64L < static_cast<size_t>(FILENAME_MAX) ? 64L
      : static_cast<size_t>(FILENAME_MAX));

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS DBMS
   */
  class dbms {
    public:
      dbms (std::shared_ptr<schema::manager>, std::shared_ptr<logger>,
          decrypt_credential_t);

      ~dbms () { }

      dbms (dbms const&) = delete;
      dbms (dbms&&) = delete;

      dbms& operator=(dbms const&) = delete;
      dbms& operator=(dbms&&) = delete;

      void drop_schema (std::string const& name) {
        m_schema_mgr->drop_schema(name);
      }

      std::string decrypt_cred (size_t size, uint8_t* buffer) {
        return m_decrypt_cred(size, buffer);
      }

      void drop_block_cache (std::string const& name) {
        block_cache_drop{m_file_cache, name}.drop();
      }

      void flush (std::shared_ptr<tree::tree>);

      std::shared_ptr<block_cache>
      get_block_cache (std::string const&);

      std::shared_ptr<schema::manager>
      get_schema_mgr () const { return m_schema_mgr; }

      std::shared_ptr<block_cache>
      load_block_cache (std::string const&);

      void kill ();

      std::shared_ptr<block_cache>
      make_block_cache (dio&, std::shared_ptr<schema::schema>);

      std::string path () const { return m_path; }
    private:
      std::string   m_path; // full path to dbms directory
      std::queue<std::string>     m_queue;
      std::shared_ptr<file_cache> m_file_cache;
      std::shared_ptr<schema::manager>  m_schema_mgr;
      std::shared_ptr<logger>     m_logger;

      decrypt_credential_t    m_decrypt_cred;
  };

} // namespace

#endif  // DBMS_H


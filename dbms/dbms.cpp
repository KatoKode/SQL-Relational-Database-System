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

#include "dbms.h"

namespace red::dbms {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DBMS: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS DBMS: Constructor
   */
  dbms::dbms (std::shared_ptr<schema::manager> schema_mgr,
      std::shared_ptr<logger> lggr, decrypt_credential_t decrypt_cred)
    : m_path {BASE_DIR},
    m_file_cache{new file_cache{std::bind(&dbms::path, this), schema_mgr}},
    m_schema_mgr {schema_mgr}, m_logger {lggr}, m_decrypt_cred {decrypt_cred}
  {
    // change directory to dbms base path
    errno = 0;
    if (chdir(m_path.c_str()) < 0) {
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BASE_ERROR, errno)};
    }
  }

  /******************************************************************************
   *
   */
  void dbms::flush (std::shared_ptr<tree::tree> a_tree) {
    m_file_cache->add_job(file_cache::TYPE_FLUSH, a_tree->name());
  }

  /****************************************************************************
   *
   */
  std::shared_ptr<block_cache>
  dbms::get_block_cache (std::string const& name)
  {
    return m_file_cache->get_block_cache(name);
  }

  /****************************************************************************
   *
   */
  void dbms::kill () {
//    m_cache.kill();
//    m_cache.join();
  }

  /****************************************************************************
   *
   */
  std::shared_ptr<block_cache>
  dbms::load_block_cache (std::string const& name) {
    return m_file_cache->load_block_cache(name);
  }

  /****************************************************************************
   *
   */
  std::shared_ptr<block_cache>
  dbms::make_block_cache (dio& io, std::shared_ptr<schema::schema> sch)
  {
    return m_file_cache->make_block_cache(io, sch);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DBMS: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

} // namespace


//-----------------------------------------------------------------------------
// Desc: File-block cache library
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

#ifndef CACHE_H
#define CACHE_H 1

#include <cstring>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <deque>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "eno.h"
#include "logger.h"
#include "dio.h"
#include "schema.h"
#include "lock.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BLOCK-CACHE
   */
  class block_cache {
    public:
      block_cache (red::dio& io, std::shared_ptr<red::schema::schema> sch)
        : m_dio {std::move(io)}, m_lock {new red::lock}, m_schema {sch}
      { }

      block_cache (block_cache const&) = delete;
      block_cache (block_cache&) = delete;

      block_cache& operator=(block_cache const&) = delete;
      block_cache& operator=(block_cache&) = delete;

      void drop_block (off_t);
      void flush_block (off_t);
      void flush_cache ();
      std::shared_ptr<uint8_t> get_block (off_t);
      std::shared_ptr<red::schema::schema> get_schema () { return m_schema; }
      std::shared_ptr<uint8_t> load_block (off_t);
      std::shared_ptr<uint8_t> new_block (off_t);
      red::lock* get_lock () { return m_lock; }
    private:
      red::dio   m_dio;
      red::lock* m_lock;
      std::shared_ptr<red::schema::schema> m_schema;
      std::unordered_map<off_t, std::shared_ptr<uint8_t> >  m_cache;
      std::vector<off_t>  m_flush_list;

      std::shared_ptr<uint8_t> alloc_block ();
      bool read_block(std::shared_ptr<uint8_t>, off_t);
      bool write_block(std::shared_ptr<uint8_t>, off_t);
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS FILE_CACHE
   */
  using dbms_path_t = std::function<std::string()>;

  class file_cache {
    public:
      static int32_t const TYPE_KILL  = 0;
      static int32_t const TYPE_FLUSH = 7;
      static int32_t const TYPE_PURGE = 15;

      file_cache (dbms_path_t dbms_path,
          std::shared_ptr<red::schema::manager> schema_mgr)
        : m_flag {true}, m_dbms_path {dbms_path}, m_schema_mgr {schema_mgr}
      {
        m_thrd = std::thread ([this]{run();});
      }

      file_cache (file_cache const&) = delete;
      file_cache (file_cache&&) = delete;

      file_cache& operator=(file_cache const&) = delete;
      file_cache& operator=(file_cache&&) = delete;

      void add_job (int32_t const, std::string const&);
      void join () { m_thrd.join(); }
      void kill ();
      std::shared_ptr<block_cache> get_block_cache (std::string const&);
      std::shared_ptr<block_cache> load_block_cache (std::string const&);
      std::shared_ptr<block_cache> make_block_cache (red::dio&,
          std::shared_ptr<red::schema::schema>);
    private:
      bool          m_flag; // signal file-cache manager to exit
      dbms_path_t   m_dbms_path;
      std::thread   m_thrd;
      std::mutex    m_mutex;
      std::condition_variable m_cond;
      std::deque<std::pair<int32_t, std::string> > m_job_q;
      std::shared_ptr<red::schema::manager>  m_schema_mgr;
      std::unordered_map<std::string, std::shared_ptr<block_cache> > m_cache;

      void do_work ();
      void drop_block_cache (std::string const&);
      std::shared_ptr<block_cache> get_block_cache_no_lock (std::string const&);
      void run ();

      friend class block_cache_drop;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BLOCK_CACHE_DROP
   */
  class block_cache_drop {
    public:
      block_cache_drop (std::shared_ptr<file_cache> cache, std::string const& name)
        : m_cache {cache}, m_name {name}
      { }

      void drop () { m_cache->drop_block_cache(m_name); }
    private:
      std::shared_ptr<file_cache> m_cache;
      std::string   m_name;
  };

} // namespace

#endif  // CACHE_H


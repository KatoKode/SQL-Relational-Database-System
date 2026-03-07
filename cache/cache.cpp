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

#include "cache.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS BLOCK-CACHE: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Find block in cache; if block exists in cache, remove the block from the
   * cache.
   */
  void block_cache::drop_block (off_t offset) {
    auto iter = m_cache.find(offset);
    if (iter == m_cache.end()) return;
    m_cache.erase(iter);
  }

  /****************************************************************************
   * Find block-offset in flush-list; if block-offset not in flust-list, push
   * block-offset onto list.
   */
  void block_cache::flush_block (off_t offset) {
    auto iter = std::find(m_flush_list.begin(), m_flush_list.end(), offset);
    if (iter == m_flush_list.end()) m_flush_list.push_back(offset);
  }

  /****************************************************************************
   * Iterator through flush-list; get block-offset; find block in cache; if
   * block in cache, write block to disk; and clear flush-list.
   */
  void block_cache::flush_cache () {
    for (auto offset : m_flush_list) {
      auto iter = m_cache.find(offset);
      if (iter != m_cache.end()) write_block((*iter).second, (*iter).first);
    }
    m_flush_list.clear();
  }

  /****************************************************************************
   * Get the block at offset; if block-offset is invalid, return nullptr;
   * otherwise, find block in cache; if block not in cache; and load the block
   * from disk.
   */
  std::shared_ptr<uint8_t> block_cache::get_block (off_t offset)
  {
    if (offset < 0L || (offset % red::tree::BUFFER_SZ) != 0) return nullptr;

    auto iter = m_cache.find(offset);

    if (iter != m_cache.end()) return (*iter).second;

    return load_block(offset);
  }

  /****************************************************************************
   * Load a block from disk. If block-offset is equal-to or greater-than the
   * next-offset, return nullptr; otherwise, construct a block; read in block
   * from disk; insert block into cache; and return block.
   */
  std::shared_ptr<uint8_t> block_cache::load_block (off_t offset) {
    if (offset >= m_schema->get_header().next_offset()) return nullptr;

    auto block = alloc_block();

    if (block == nullptr) return nullptr;

    if (!read_block(block, offset)) return nullptr;

    m_cache [ offset ] = block;

    return block;
  }

  /****************************************************************************
   * Construct block; write block out to disk; insert block into cache; and
   * return block.
   */
  std::shared_ptr<uint8_t> block_cache::new_block (off_t offset) {
    auto block = alloc_block();

    if (block == nullptr) return nullptr;

    if (not write_block(block, offset)) return nullptr;

    m_cache [ offset ] = block;

    return block;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS BLOCK-CACHE: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Allocate block.
   */
  std::shared_ptr<uint8_t> block_cache::alloc_block () {
    uint8_t* uint8_ptr {nullptr};
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
    if (posix_memalign(reinterpret_cast<void**>(&uint8_ptr),
          m_dio.rec_xfer_align(),
          static_cast<size_t> (m_schema->get_header().block_size())) != 0)
      return nullptr;

    (void) memset(uint8_ptr, 0,
        static_cast<size_t>(m_schema->get_header().block_size()));
#elif (_ISOC11_SOURCE)
    uint8_ptr = static_cast<uint8_t*>(aligned_alloc (m_dio.rec_xfer_align(),
          static_cast<size_t> (m_schema->get_header().block_size())));

    if (uint8_ptr != nullptr) return nullptr;

    (void) memset(uint8_ptr, 0,
        static_cast<size_t>(m_schema->get_header().block_size()));
#else
#error missing both routines: posix_memalign() and aligned_alloc()
#endif
    std::shared_ptr<uint8_t> block {uint8_ptr};

    return block;
  }

  /****************************************************************************
   * Read in block from disk.
   */
  bool block_cache::read_block(std::shared_ptr<uint8_t> block, off_t offset) {
    ssize_t const block_size
      = static_cast<ssize_t>(m_schema->get_header().block_size());
    ssize_t nread = 0;
    while (nread < block_size) {
      ssize_t n;
      if ((n = m_dio.io_pread(static_cast<void*>(&block.get()[ nread ]),
              m_dio.page_size(), offset + nread)) < 0) {
        return false;
      }
      nread += n;
    }
    return true;
  }

  /****************************************************************************
   * Write out block to disk.
   */
  bool block_cache::write_block(std::shared_ptr<uint8_t> block, off_t offset) {
    if (not m_dio.write_enabled()) return true;
    ssize_t const block_size
      = static_cast<ssize_t>(m_schema->get_header().block_size());
    ssize_t nwritten = 0;
    while (nwritten < block_size) {
      ssize_t n;
      if ((n = m_dio.io_pwrite(static_cast<void*>(&block.get()[ nwritten ]),
              m_dio.page_size(), offset + nwritten)) < 0)
        return false;
      nwritten += n;
    }
    return true;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS TREE-CACHE: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Add job to job queue.
   */
  void file_cache::add_job (int32_t const type, std::string const& name) {
    {
      std::unique_lock<std::mutex> lck {m_mutex};
      auto p = std::make_pair(type, name);
      m_job_q.push_back(p);
    }
    m_cond.notify_one();
  }

  /****************************************************************************
   * Get block-cache; unique-lock file-cache; and get block-cache.
   */
  std::shared_ptr<block_cache>
  file_cache::get_block_cache (std::string const& name)
  {
    std::unique_lock<std::mutex> lck {m_mutex};
    return get_block_cache_no_lock(name);
  }

  /****************************************************************************
   * Signal the file-cache manager to terminate; unique-lock file-cache; make
   * kill-job; push kill-job onto queue; set flag; and notify manager.
   */
  void file_cache::kill () {
    {
      std::unique_lock<std::mutex> lck {m_mutex};
      auto p = std::make_pair(red::file_cache::TYPE_KILL, "kill");
      m_job_q.push_back(p);
      m_flag = false;
    }
    m_cond.notify_one();
  }

  /****************************************************************************
   * NOTE: Always unique-lock file-cache before calling this function.
   * Load schema and open file; construct block-cache; insert block-cache into
   * cache; and return block-cache object;
   */
  std::shared_ptr<block_cache>
  file_cache::load_block_cache (std::string const& name)
  {
    try {
      auto sch = m_schema_mgr->get_schema(name);
      if (sch == nullptr) return nullptr;
      std::ostringstream path;
      path << m_dbms_path() << '/' << sch->get_header().db_name() << '/'
        << sch->get_header().tbl_name() << ".d";
      red::dio io {false, path.str()};
      auto new_block_cache
        = std::shared_ptr<block_cache>{new block_cache{io, sch}};
      m_cache [ name ] = new_block_cache;
      return new_block_cache;
    } catch (std::runtime_error& e) {
      if (std::strlen(e.what()) != 0) throw;
    } catch (...) { }
    return nullptr;
  }

  /****************************************************************************
   * Make a block-cache; save schema; construct block-cache; insert block-cache
   * int file-cache; and return block-cache.
   */
  std::shared_ptr<block_cache>
  file_cache::make_block_cache (red::dio& io,
      std::shared_ptr<red::schema::schema> sch)
  {
    m_schema_mgr->save_schema(sch);
    std::ostringstream path;
    auto bc = std::shared_ptr<block_cache>{new red::block_cache{io, sch}};
    m_cache [ sch->name() ] = bc;
    return bc;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS TREE-CACHE: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Work the job-queue
   */
  void file_cache::do_work () {
    std::unique_lock<std::mutex> lck {m_mutex};
    while (not m_job_q.empty()) {
      auto p = m_job_q.front(); m_job_q.pop_front();
      if (p.first == file_cache::TYPE_KILL || p.second == "kill") return;
      auto blk_cache = get_block_cache_no_lock(p.second);
      if (blk_cache == nullptr) return;
      switch(p.first) {
        case file_cache::TYPE_PURGE:
          {
            // TODO: Purge a block cache
          }
        case file_cache::TYPE_FLUSH:
          {
            if (blk_cache != nullptr) {
              blk_cache->flush_cache();
              m_schema_mgr->save_schema(blk_cache->get_schema());
            }
          }
        default:;
      }
    }
  }

  /****************************************************************************
   * Find block-cache; if the block-cache in cache, remove block-cache from
   * cache.
   */
  void file_cache::drop_block_cache (std::string const& name) {
    std::unique_lock<std::mutex> lck {m_mutex};
    auto iter = m_cache.find(name);
    if (iter != m_cache.end()) {
      while ((*iter).second.use_count() > 1)
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
      m_cache.erase(iter);
    }
  }

  /****************************************************************************
   * NOTE: Public functions always unique-lock file-cache before calling this
   * function.
   * Get block-cache without unique-lock; find block-cache; if block-cache in
   * file-cache, return block-cache; otherwise, attempt to load block cache.
   */
  std::shared_ptr<block_cache>
  file_cache::get_block_cache_no_lock (std::string const& name)
  {
    auto iter = m_cache.find(name);
    if (iter != m_cache.end()) return (*iter).second;
    return load_block_cache(name);
  }

  /****************************************************************************
   * File-cache manager thread.
   */
  void file_cache::run () {
    // pause to allow cache constructor to finish
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // continue with management of cache
    while (m_flag) {
      {
        std::unique_lock<std::mutex> lck {m_mutex};
        m_cond.wait(lck, [this]{return (not m_flag || not m_job_q.empty());});
      }
      do_work();
    }
    do_work();
  }

} // namespace


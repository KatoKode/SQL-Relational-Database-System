//-----------------------------------------------------------------------------
// Desc: Log file library
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

#include "logger.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LOGGER - PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS LOGGER: Move Assignment
   */
  logger& logger::operator=(logger&& o) {
    if (this == &o) return *this;
    std::unique_lock<std::mutex> lck {o.m_mutex};
    m_size  = std::move(o.m_size);
    m_path  = std::move(o.m_path);
    m_file  = std::move(o.m_file);
    return *this;
  }

  /****************************************************************************
   * 
   */
  void logger::write (size_t const level, char const* msg) {
    std::unique_lock<std::mutex> lck {m_mutex};
    resize();
    namespace sc = std::chrono;
    time_t tt = sc::system_clock::to_time_t(sc::system_clock::now());
    struct tm tm = *localtime(&tt);
    char ts [ 48 ];
    (void) strftime(ts, sizeof(ts), "%F %T %p", &tm);
    m_file << "[ " << ts << " ] " << LOG_LEVEL[level] << msg << '\n';
    m_file.sync();
    m_file.clear();
    ++m_size;
  }

  /****************************************************************************
   * 
   */
  void logger::write (size_t const level, std::string const& msg) {
    write(level, msg.c_str());
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LOGGER - PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * 
   */
  void logger::init () {
    std::unique_lock<std::mutex> lck {m_mutex};
    std::ostringstream path;
    path << m_path << '/' << LOGNAME;
    m_file.open(path.str(), (m_file.app | m_file.in | m_file.out));
    if (!m_file.is_open())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_LOGFILE_ERROR)};
    m_size = 0L;
    std::string line;
    m_file.clear();
    m_file.seekg(0);
    while (m_file.good()) {
      std::getline(m_file, line, '\n');
      if (m_file.good()) ++m_size;
    }
    m_file.clear();
  }

  /****************************************************************************
   * 
   */
  void logger::resize () {
    if (m_size < MAXLOGSIZE) return;
    // read all entries from log file
    std::deque<std::string> log_entry_q;
    m_file.clear();
    m_file.seekg(0);
    while (not m_file.eof()) {
      m_file.clear();
      std::string line;
      std::getline(m_file, line, '\n');
      if (not m_file.eof()) log_entry_q.push_back(line);
    }
    m_file.clear();
    // close log file
    m_file.close();
    // truncate log file
    std::ostringstream path;
    path << m_path << '/' << LOGNAME;
    m_file.open(path.str(), (m_file.trunc | m_file.out));
    m_file.close();
    // open file
    m_file.open(path.str(), (m_file.app | m_file.in | m_file.out));
    if (!m_file.is_open()) {
      throw std::runtime_error{eno::get_msg(eno::ERRNO_LOGFILE_ERROR)};
    }
    // purge leading log entries
    while (log_entry_q.size() > MINLOGSIZE) log_entry_q.pop_front();
    // write remaining log entries to log file
    m_file.clear();
    m_size = 0L;
    for (auto entry : log_entry_q) {
      m_file << entry << '\n';;
      ++m_size;
    }
    m_file.sync();
    m_file.clear();
  }

} // namespace


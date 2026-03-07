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

#ifndef LOGGER_H
#define LOGGER_H 1

#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <cerrno>
#include <chrono>
#include <mutex>
#include <utility>
#include <deque>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include "eno.h"

namespace red {

  size_t const INFO     = 0;
  size_t const NOTIFY   = 1;
  size_t const WARNING  = 2;
  size_t const ERROR    = 3;
  size_t const CRITICAL = 4;

  size_t const MINLOGSIZE = 960;
  size_t const MAXLOGSIZE = 1024; // maximum number of lines in log

  std::string const LOGNAME {"redd.log"};

  std::vector<std::string> const LOG_LEVEL {
    "INFO: ", "NOTIFY: ", "WARNING: ", "ERROR: ", "CRITICAL: "
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS LOGGER
   */
  class logger {
    public:
      logger (std::string const& path) : m_size {0L}, m_path {path} { init(); }

      ~logger () { m_file.close(); }

      logger (logger const&) = delete;
      logger (logger&& o)
        : m_size {o.m_size}, m_path {std::move(o.m_path)},
        m_file {std::move(o.m_file)}
      { }

      logger& operator=(logger const&) = delete;
      logger& operator=(logger&&);

      void write (size_t const, char const*);
      void write (size_t const, std::string const&);
    private:
      size_t        m_size;
      std::mutex    m_mutex;
      std::string   m_path;
      std::fstream  m_file;

      void init ();
      void resize ();
  };

} // namespace

#endif  // LOGGER_H


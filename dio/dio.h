//-----------------------------------------------------------------------------
// Desc: Direct I/O library
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

#ifndef DIO_H
#define DIO_H 1

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdint>
#include <cassert>
#include <cerrno>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "eno.h"
#include "logger.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS DIO (DIRECT I/O)
   */
  class dio {
    public:
      dio () : m_fd {-1}, m_flag {false} { }

      dio (bool, char const*);

      dio (bool, std::string const&);

      ~dio () { io_close (); }

      dio (dio const&) = delete;

      dio (dio&& o) : m_fd {o.m_fd}, m_flag {o.m_flag}
      {
        o.m_fd  = -1;
        o.m_flag  = false;
      }

      dio& operator=(dio const&) = delete;
      dio& operator=(dio&&);

      int io_data_sync ();
      int io_sync ();

      ssize_t io_pread (void*, size_t, off_t);
      ssize_t io_pwrite (void const*, size_t, off_t); 
      ssize_t io_read (void*, size_t);
      ssize_t io_write (void const*, size_t);

      long max_xfer_size ();
      long page_size ();
      long rec_xfer_align ();

      void write_enabled (bool const value) { m_flag = value; }
      bool write_enabled () const { return m_flag; }
    private:
      int   m_fd;
      bool  m_flag;

      void init (bool, char const*);
      void init (bool, std::string const&);
      int io_close ();
      int io_creat (char const*);
      int io_open (char const*);
  };

} // namespace

#endif  // DIO_H


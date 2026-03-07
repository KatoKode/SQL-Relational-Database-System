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

#include "dio.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DIO: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS DIO: Constructor
   */
  dio::dio (bool creat, char const* path) : m_flag {true} {
    init(creat, path);
  }

  /****************************************************************************
   * CLASS DIO: Constructor
   */
  dio::dio (bool creat, std::string const& path) : m_flag {true} {
    init(creat, path);
  }

  /****************************************************************************
   * CLASS DIO: Move Assignment
   */
  dio& dio::operator=(dio&& o) {
    if (this == &o) return *this;
    m_fd    = o.m_fd;
    m_flag  = o.m_flag;
    o.m_fd  = -1;
    o.m_flag  = false;
    return *this;
  }

  /****************************************************************************
   * Sync buffers to disk.
   */
  int dio::io_data_sync () {
#if (_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE >= 500)
    return fdatasync (m_fd);
#endif
    return io_sync ();
  }

  /****************************************************************************
   * Sync buffers to disk.
   */
  int dio::io_sync () {
#if (_BSD_SOURCE || _XOPEN_SOURCE)
    return fsync (m_fd);
#endif
    return 0;
  }

  /****************************************************************************
   * Read from file. Read (nbyte) bytes from the file associated with file
   * descriptor (m_fd) starting at file offset (offset) into buffer (buf).
   */
  ssize_t dio::io_pread (void* buf, size_t nbyte, off_t offset) {
    ssize_t nread = 0;
    ssize_t n;
    errno = 0;
    do {
      if ((n = pread (m_fd,
              &((char*)buf) [ nread ],
              nbyte - nread,
              offset + nread)) < 0)
      {
        if (errno != EINTR) return -1;

        errno = 0;
      }

      if (n == 0) return nread; /* EOF */

      nread += n;
    } while (static_cast<size_t>(nread) < nbyte);

    return nread;
  }

  /****************************************************************************
   * Write to file. Write (nbyte) bytes from buffer (buf) to the file associ-
   * ated with file descriptor (m_fd) starting at file offset (offset).
   */
  ssize_t dio::io_pwrite (void const* buf, size_t nbyte, off_t offset) {
    ssize_t nwritten = 0;
    ssize_t n;
    errno = 0;
    do {
      if ((n = pwrite (m_fd,
              &((char*)buf) [ nwritten ],
              nbyte - nwritten,
              offset + nwritten)) < 0)
      {
        if (errno != EINTR) return -1;

        errno = 0;
      }

      nwritten += n;

    } while (static_cast<size_t>(nwritten) < nbyte);

    return nwritten;
  }

  /****************************************************************************
   * Read from file. Read (nbyte) bytes from the file associated with file
   * descriptor (m_fd) into buffer (buf).
   */
  ssize_t dio::io_read (void* buf, size_t nbyte) {
    ssize_t nread = 0;
    ssize_t n;
    errno = 0;
    do {
      if ((n = read (m_fd, &((char*)buf) [ nread ], nbyte - nread)) < 0)
      {
        if (errno != EINTR) return -1;

        errno = 0;
      }

      if (n == 0) return nread; /* EOF */

      nread += n;

    } while (static_cast<size_t>(nread) < nbyte);

    return nread;
  }

  /****************************************************************************
   * Write to file. Write (nbyte) bytes from buffer (buf) to the file associ-
   * ated with file descriptor (m_fd).
   */
  ssize_t dio::io_write (void const* buf, size_t nbyte) {
    ssize_t nwritten = 0;
    ssize_t n;
    errno = 0;
    do {
      if ((n = write (m_fd, &((char*)buf) [ nwritten ],
              nbyte - nwritten)) < 0)
      {
        if (errno != EINTR) return -1;

        errno = 0;
      }

      nwritten += n;

    } while (static_cast<size_t>(nwritten) < nbyte);

    return nwritten;
  }


  /****************************************************************************
   *
   */
  long dio::max_xfer_size () {
    return fpathconf (m_fd, _PC_REC_MAX_XFER_SIZE);
  }


  /****************************************************************************
   *
   */
  long dio::page_size () {
    return sysconf (_SC_PAGESIZE);
  }


  /****************************************************************************
   *
   */
  long dio::rec_xfer_align () {
    return fpathconf (m_fd, _PC_REC_XFER_ALIGN);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DIO: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void dio::init (bool creat, char const* path) {
    errno = 0;
    if (creat) {
      if (io_creat (path) < 0) 
        throw std::runtime_error{
          red::eno::get_msg(red::eno::ERRNO_CREATE_ERROR, errno)};
    } else {
      if (io_open (path) < 0) {
        if (errno == ENOENT) throw std::runtime_error{""};
        else throw std::runtime_error{
          red::eno::get_msg(red::eno::ERRNO_OPEN_ERROR, errno)};
      }
    }
  }

  /****************************************************************************
   *
   */
  void dio::init (bool creat, std::string const& path) {
    errno = 0;
    init (creat, path.c_str());
  }

  /****************************************************************************
   * Close the file associated with file descriptor (m_fd).
   */
  int dio::io_close () {
    while (close (m_fd) < 0) {
      if (errno != EINTR) return -1;
      errno = 0;
    }
    return 0;
  }

  /****************************************************************************
  * Create the file corresponding to path (path).
  */
  int dio::io_creat (char const* path) {
    int flags = (O_CREAT | O_DIRECT | O_RDWR | O_SYNC | O_TRUNC);
    mode_t mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    while ((m_fd = open (path, flags, mode)) < 0) {
      if (errno != EINTR) return -1;
      errno = 0;
    }
    return 0;
  }

  /****************************************************************************
  * Open the file corresponding to path (path).
  */
  int dio::io_open (char const* path) {
    int flags = (O_DIRECT | O_RDWR | O_SYNC);
    while ((m_fd = open (path, flags)) < 0) {
      if (errno != EINTR) return -1;
      errno = 0;
    }
    return 0;
  }

} // namespace


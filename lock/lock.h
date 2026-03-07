//-----------------------------------------------------------------------------
// Desc: Shared/Unique lock library
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

#ifndef LOCK_H
#define LOCK_H 1

#include <stdlib.h>
#include <pthread.h>
#include <iostream>

namespace red {

  class shared_lock;
  class unique_lock;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS LOCK
   */
  class lock {
    public:
      lock ();

      ~lock ();

      lock (lock const& lk) : m_rwlock {lk.m_rwlock} { }
      lock (lock&& lk) : m_rwlock {lk.m_rwlock} { lk.m_rwlock = nullptr; }

      lock& operator=(lock const&);
      lock& operator=(lock&&);
    private:
      pthread_rwlock_t*   m_rwlock;

      friend shared_lock;
      friend unique_lock;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS SHARED_LOCK
   */
  class shared_lock {
    public:
      explicit shared_lock (red::lock* lock) : m_lock {lock} {
        pthread_rwlock_rdlock (m_lock->m_rwlock);
      }

      ~shared_lock ();

      shared_lock (shared_lock const&) = delete;
      shared_lock (shared_lock&&);

      shared_lock& operator=(shared_lock const&) = delete;
      shared_lock& operator=(shared_lock&&);
    private:
      red::lock*  m_lock;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS UNIQUE_LOCK
   */
  class unique_lock {
    public:
      explicit unique_lock (red::lock* lock) : m_lock {lock} {
        pthread_rwlock_wrlock (m_lock->m_rwlock);
      }

      ~unique_lock ();

      unique_lock (unique_lock const&) = delete;
      unique_lock (unique_lock&&);

      unique_lock& operator=(unique_lock const&) = delete;
      unique_lock& operator=(unique_lock&&);
    private:
      red::lock*  m_lock;
  };

} // namespace

#endif  // LOCK_H


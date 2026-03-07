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

#include "lock.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LOCK: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS LOCK: Default Constructor
   */
  lock::lock () {
    void* ptr = calloc (1, sizeof (pthread_rwlock_t));
    m_rwlock = static_cast<pthread_rwlock_t*> (ptr);
    pthread_rwlock_init (m_rwlock, nullptr);
  }

  /***************************************************************************
   * CLASS LOCK: Destructor
   */
  lock::~lock () {
    pthread_rwlock_destroy (m_rwlock);
    free (m_rwlock);
  }

  /***************************************************************************
   * CLASS LOCK: Copy Assignment
   */
  lock& lock::operator=(lock const& o) {
    if (this == &o) return *this;
    m_rwlock = o.m_rwlock;
    return *this;
  }

  /***************************************************************************
   * CLASS LOCK: Move Assignment
   */
  lock& lock::operator=(lock&& o) {
    if (this == &o) return *this;
    m_rwlock    = o.m_rwlock;
    o.m_rwlock = nullptr;
    return *this;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS LOCK: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS SHARED_LOCK: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS SHARED_LOCK: Destructor
   */
  shared_lock::~shared_lock () {
    if (m_lock) {
      pthread_rwlock_unlock (m_lock->m_rwlock);
      m_lock = nullptr;
    }
  }

  /***************************************************************************
   * CLASS SHARED_LOCK: Move Constructor
   */
  shared_lock::shared_lock (shared_lock&& o) : m_lock {o.m_lock}
  {
    o.m_lock = nullptr;
  }

  /***************************************************************************
   * CLASS SHARED_LOCK: Move Assignment
   */
  shared_lock& shared_lock::operator=(shared_lock&& o) {
    if (this == &o) return *this;
    m_lock = o.m_lock;
    o.m_lock = nullptr;
    return *this;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS SHARED_LOCK: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS UNIQUE_LOCK: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS UNIQUE_LOCK: Destructor
   */
  unique_lock::~unique_lock () {
    if (m_lock) {
      pthread_rwlock_unlock (m_lock->m_rwlock);
      m_lock = nullptr;
    }
  }

  /***************************************************************************
   * CLASS UNIQUE_LOCK: Move Constructor
   */
  unique_lock::unique_lock (unique_lock&& o) : m_lock {o.m_lock}
  {
    o.m_lock = nullptr;
  }

  /***************************************************************************
   * CLASS UNIQUE_LOCK: Move Assignment
   */
  unique_lock& unique_lock::operator=(unique_lock&& o) {
    if (this == &o) return *this;
    m_lock = o.m_lock;
    o.m_lock = nullptr;
    return *this;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS UNIQUE_LOCK: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

} // namespace


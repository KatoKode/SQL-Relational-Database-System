//-----------------------------------------------------------------------------
// Desc: RedB database installation program
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

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <gcrypt.h>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include "eno.h"
#include "logger.h"
#include "dio.h"
#include "buffer.h"
#include "bcalc.h"
#include "schema.h"
#include "const.h"
#include "btree.h"
#include "dbms.h"
#include "srvcry.h"

/******************************************************************************
 * Global declarations and definitions
 */

int32_t const RANGE_BASE = 31;
int32_t const MAX_RANGE = 95;
int32_t const MIN_RAND_VALUE = 0;
int32_t const MAX_RAND_VALUE = std::numeric_limits<int32_t>::max();
size_t const MAXHASH = 63;

std::string const USER_OPT {"--user"};
std::string hashcode;

std::shared_ptr<red::logger>  logger;
std::shared_ptr<red::schema::manager>  schema_mgr;
std::shared_ptr<red::dbms::dbms>  dbms;
std::shared_ptr<red::srvcry>  crypto;

typedef	void sigfunc(int);

void exit_handler ();
void signal_handler (int);
sigfunc* sa_signal (int, sigfunc*);
void create_cred_tbl ();
void create_hash_file ();
void create_keyp_file ();
void create_priv_tbl ();
void create_root_user ();
red::schema::schema* make_cred_schema ();
red::schema::schema* make_priv_schema ();
bool set_effective_user_id (char const*);


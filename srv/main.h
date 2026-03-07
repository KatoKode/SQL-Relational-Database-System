//-----------------------------------------------------------------------------
// Desc: RedB server program
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

#define GCRYPT_NO_MPI_MACROS 1
#define GCRYPT_NO_DEPRECATED 1

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
#include <chrono>
#include <functional>
#include <thread>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include "eno.h"
#include "logger.h"
#include "comm.h"
#include "btree.h"
#include "sql.h"
#include "dbms.h"
#include "vm.h"
#include "srvcry.h"

/******************************************************************************
 * SERVER : Global declarations and definitions
 */

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

constexpr int32_t ACCEPT_BACKLOG {8};
constexpr int32_t MIN_RAND_VALUE {0};
constexpr int32_t MAX_RAND_VALUE {std::numeric_limits<int32_t>::max()};

std::regex const SOURCE_CMD {
  "^(source)\\s*('|\\\")((.*/)?(?:$|(.+?)(?:(\\.[^.]*|))))('|\\\")\\s*(;)$",
  std::regex::ECMAScript | std::regex::icase};

std::string const LOCALHOST {"localhost"};
std::string const SRVRHOST {"redbhost"};
std::string const SRVRPORT {"redb"};
std::string const USER_OPT {"--user"};

std::list<
  std::pair<
    std::shared_ptr<std::thread>, std::shared_ptr<red::comm::fsm>
  >
> cli_list;

bool comm_up {true};
bool all_systems_go {true};
std::mutex  cli_mutex;
std::mutex  srv_mutex;
std::shared_ptr<red::logger>  logger;
std::shared_ptr<red::schema::manager>  schema_mgr;
std::shared_ptr<red::dbms::dbms> dbms;
std::shared_ptr<red::srvcry> crypto;

typedef	void sigfunc(int);

void exit_handler ();
void init_signal (sigset_t*, sigset_t*, sigfunc *);
void signal_handler (int);
sigfunc* sa_signal (int, sigfunc*);

bool authenticate (std::shared_ptr<red::comm::fsm>, std::string const&,
    std::string const&);
std::string decrypt_stmt (size_t, uint8_t*);
bool do_unique_lock (std::string const&);
bool exec_source (std::shared_ptr<red::comm::fsm>, std::string const&);
std::string get_encrypted_stmt(size_t, uint8_t*);
bool get_hashcode ();
bool get_keypair ();
void rcvd (std::shared_ptr<red::comm::fsm>);
void report_error (std::shared_ptr<red::comm::fsm>, std::runtime_error&,
    int32_t const);
bool set_effective_user_id (char const*);
void comm_agent (std::shared_ptr<red::comm::fsm>);
void comm_server ();


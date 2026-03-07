//-----------------------------------------------------------------------------
// Desc: RedB client program
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <gcrypt.h>
#include <ncurses.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <limits>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <thread>
#include <regex>
#include "eno.h"
#include "util.h"
#include "comm.h"
#include "clicry.h"

int32_t const RANGE_BASE  = 31;
int32_t const MAX_RANGE   = 95;

int32_t const MIN_RAND = 0;
int32_t const MAX_RAND = std::numeric_limits<int32_t>::max();

size_t const SEG_SIZE = 256;
size_t const MAX_CRED_SIZE  = 256;
size_t const MAX_STMT_SIZE  = 1280;

size_t HIST_NDX {0};

std::vector<std::string> ERROR_MSG {
  {"Cipher error"},
  {"Select error"}
};

std::string hashcode;

std::regex ALTER_USER_REGEX{"^alter\\s+user",
  std::regex::ECMAScript | std::regex::icase};
std::regex CREATE_USER_REGEX{"^create\\s+user",
  std::regex::ECMAScript | std::regex::icase};
std::regex GRANT_PRIV_REGEX{"^grant\\s",
  std::regex::ECMAScript | std::regex::icase};
std::vector<std::regex> regex_list {
  ALTER_USER_REGEX, CREATE_USER_REGEX, GRANT_PRIV_REGEX
};

std::mutex mtx;
std::condition_variable cond;

std::string const SRVRHOST {"redbhost"};
std::string const SRVRPORT {"redb"};
std::string const HOME_ENV {"HOME"};
std::string const HIST_FILE {".redcli_history"};
std::string const EXIT_STMT {"exit;"};
std::string const QUIT_STMT {"quit;"};
std::string const SEMICOLON {";"};

std::string const LICENSE {"redcli Copyright © 2019 J McIntosh\n"
  "This program comes with ABSOLUTELY NO WARRANTY; for details see\n"
  "GNU GPL v3. This is free software, and you are welcome to redistribute\n"
  "it under certain conditions; see GNU GPL v3 for details."};

bool auth_ok {false};
bool all_systems_go {true};
bool conn_ok {false};
bool done {true};

std::string const SRVR_TERM_MSG {"Server has terminated connection"};
std::string const UNKNOWN_ERROR_MSG {"Unknown error occurred"};
std::string const pw_prompt {"Enter password: "};
std::string prompt {"redb"};
std::string norm_brckt {">"};
std::string cont_brckt {"-->"};
std::string brckt {norm_brckt};

std::shared_ptr<red::clicry> crypto;

typedef	void sigfunc(int);

void exit_handler ();
void init_signal (sigset_t*, sigset_t*, sigfunc *);
void signal_handler (int);
sigfunc* sa_signal (int, sigfunc*);

void cli_agent (std::shared_ptr<red::comm::fsm>);
bool do_encrypt (std::string const&);
bool do_exit();
int get_ch ();
std::string getpass ();
bool encrypt_cred (std::string&, std::string&, size_t*, uint8_t*);
bool encrypt_stmt (std::string const&, size_t*, uint8_t*);
std::string hash_password (std::string const&);
bool have_history ();
void load_history ();
void parse_line (std::vector<std::string>&, std::string const&);
void push_history (std::string const&);
void rcvd (std::shared_ptr<red::comm::fsm>);
void save_history ();
void set_done (bool);
void set_exit ();
void wait_done ();


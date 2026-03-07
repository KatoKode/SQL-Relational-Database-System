//-----------------------------------------------------------------------------
// Desc: Error message library
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

#ifndef ENO
#define ENO 1

#include <cstdint>
#include <vector>
#include <map>
#include <sstream>

namespace red::eno {

  // LOGGER:  1000
  int32_t const ERRNO_LOGFILE_ERROR           = 1000;
  // 1100: TCP
  int32_t const ERRNO_ACCEPT_ERROR            = 1100;
  int32_t const ERRNO_ADDRINFO_ERROR          = 1101;
  int32_t const ERRNO_SOCKET_ERROR            = 1102;
  int32_t const ERRNO_CONNECT_ERROR           = 1103;
  int32_t const ERRNO_RECV_ERROR              = 1104;
  int32_t const ERRNO_BIND_ERROR              = 1105;
  int32_t const ERRNO_LISTEN_ERROR            = 1106;
  int32_t const ERRNO_SEND_ERROR              = 1107;
  int32_t const ERRNO_GETIFADDRS_ERROR        = 1108;
  int32_t const ERRNO_INET_NTOP_ERROR         = 1109;
  // 1200: SNDRCV
  int32_t const ERRNO_CIDR_ERROR              = 1200;
  int32_t const ERRNO_SOCKOPT_ERROR           = 1201;
  int32_t const ERRNO_SERVICE_ERROR           = 1202;
  // 1300: DIO
  int32_t const ERRNO_CREATE_ERROR            = 1300;
  int32_t const ERRNO_OPEN_ERROR              = 1301;
  // 1400: BUFFER
  int32_t const ERRNO_BUF_BOUND_ERROR         = 1400;
  int32_t const ERRNO_BUF_INDEX_ERROR         = 1401;
  int32_t const ERRNO_BUF_SIZE_ERROR          = 1402;
  int32_t const ERRNO_BUF_REMAINING_ERROR     = 1403;
  // 1600: SCHEMA
  int32_t const ERRNO_LOAD_SCH_ERROR          = 1600;
  int32_t const ERRNO_SAVE_SCH_ERROR          = 1601;
  int32_t const ERRNO_COLUMN_ERROR            = 1602;
  int32_t const ERRNO_INDEX_ERROR             = 1603;
  int32_t const ERRNO_FOREIGN_ERROR           = 1604;
  // 1700: ROW
  int32_t const ERRNO_VALUE_ERROR             = 1700;
  // 1800: BTREE
  int32_t const ERRNO_SYSTEM_ERROR            = 1800;
  // 1900: TRAN, COMP
  int32_t const ERRNO_TOKEN_ERROR             = 1900;
  // 2000: DBMS   
  int32_t const ERRNO_BASE_ERROR              = 2000;
  // 2100: VM
  int32_t const ERRNO_OPCODE_ERROR            = 2100;
  int32_t const ERRNO_DBMS_ERROR              = 2101;
  int32_t const ERRNO_MALFORMED_COLUMN_EXPR   = 2102;
  int32_t const ERRNO_MALFORMED_COLUMN_LIST   = 2103;
  int32_t const ERRNO_MALFORMED_COMPARISON    = 2104;
  int32_t const ERRNO_MALFORMED_DATABASE_REF  = 2105;
  int32_t const ERRNO_MALFORMED_FOREIGN_KEY   = 2106;
  int32_t const ERRNO_MALFORMED_JOIN          = 2107;
  int32_t const ERRNO_MALFORMED_PRIMARY_KEY   = 2108;
  int32_t const ERRNO_MALFORMED_TABLE_REF     = 2109;
  int32_t const ERRNO_MALFORMED_UNIQUE_KEY    = 2110;
  int32_t const ERRNO_MALFORMED_VALUE_LIST    = 2111;
  int32_t const ERRNO_MALFORMED_EXPR_LIST     = 2112;
  int32_t const ERRNO_MALFORMED_EXPR          = 2113;
  int32_t const ERRNO_MULTIPLE_PRIMARY_KEY    = 2114;
  int32_t const ERRNO_CREAT_DATABASE_ERROR    = 2115;
  int32_t const ERRNO_CREAT_TABLE_ERROR       = 2116;
  int32_t const ERRNO_CREAT_USER_ERROR        = 2117;
  int32_t const ERRNO_NO_DATABASE_ERROR       = 2118;
  int32_t const ERRNO_UPDATE_ERROR            = 2119;
  int32_t const ERRNO_ALTER_ERROR             = 2120;
  int32_t const ERRNO_ALTER_ADD_COLUMN        = 2121;
  int32_t const ERRNO_MALFORMED_NAME          = 2122;
  int32_t const ERRNO_ACCESS_DENIED           = 2123;
  int32_t const ERRNO_BAD_PASSWORD            = 2124;
  int32_t const ERRNO_DIVIDE_BY_ZERO          = 2125;
  int32_t const ERRNO_MALFORMED_ADDITION      = 2126;
  int32_t const ERRNO_MALFORMED_DIVISION      = 2127;
  int32_t const ERRNO_MALFORMED_MULTIPLY      = 2128;
  int32_t const ERRNO_MALFORMED_SUBTRACT      = 2129;
  int32_t const ERRNO_MALFORMED_REFERENCE     = 2130;
  int32_t const ERRNO_DROP_PRIMARY_KEY        = 2131;
  int32_t const ERRNO_INFILE_ERROR            = 2132;
  int32_t const ERRNO_DUPLICATE_KEY           = 2133;
  int32_t const ERRNO_OUTFILE_ERROR           = 2134;
  // 2200: GCRY
  int32_t const ERRNO_CRYPTO_ERROR            = 2200;
  // 2300: REDCLI, REDD, REDB_INSTALL_DB
  int32_t const ERRNO_CIPHER_ERROR            = 2300;
  int32_t const ERRNO_SOURCE_ERROR            = 2301;
  int32_t const ERRNO_HASH_FILE_ERROR         = 2302;
  int32_t const ERRNO_HOME_VAR                = 2303;
  int32_t const ERRNO_READ_HISTORY            = 2304;
  int32_t const ERRNO_WRITE_HISTORY           = 2305;
  int32_t const ERRNO_SCHEMA_ERROR            = 2306;

  std::string get_msg (int32_t);
  std::string get_msg (int32_t, int32_t);
  std::string get_msg (int32_t, std::string const&);
  void init ();

} // namespace

#endif  // ENO


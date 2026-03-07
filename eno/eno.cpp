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

#include "eno.h"

namespace red::eno {

  std::vector<std::pair<int32_t, std::string> > const ERROR_LIST {
    {ERRNO_LOGFILE_ERROR, "Log file error"},
    {ERRNO_ACCEPT_ERROR, "Accept connection error"},
    {ERRNO_ADDRINFO_ERROR, "Address info error"},
    {ERRNO_SOCKET_ERROR, "Socket error"},
    {ERRNO_CONNECT_ERROR, "Connect error"},
    {ERRNO_RECV_ERROR, "Receive error"},
    {ERRNO_BIND_ERROR, "Bind error"},
    {ERRNO_LISTEN_ERROR, "Listen error"},
    {ERRNO_SEND_ERROR, "Send error"},
    {ERRNO_GETIFADDRS_ERROR, "Interface error"},
    {ERRNO_INET_NTOP_ERROR, "Address error"},
    {ERRNO_CIDR_ERROR, "CIDR error"},
    {ERRNO_SOCKOPT_ERROR, "Socket Option error"},
    {ERRNO_SERVICE_ERROR, "Get service error"},
    {ERRNO_CREATE_ERROR, "Create file failed"},
    {ERRNO_OPEN_ERROR, "Open file failed"},
    {ERRNO_BUF_BOUND_ERROR, "Bad buffer-bound"},
    {ERRNO_BUF_INDEX_ERROR, "Bad buffer-index"},
    {ERRNO_BUF_SIZE_ERROR, "buffer-size mismatch"},
    {ERRNO_BUF_REMAINING_ERROR, "buffer-remaining underflow"},
    {ERRNO_LOAD_SCH_ERROR, "Load schema failed"},
    {ERRNO_SAVE_SCH_ERROR, "Save schema failed"},
    {ERRNO_COLUMN_ERROR, "Bad column name"},
    {ERRNO_INDEX_ERROR, "Bad index name"},
    {ERRNO_FOREIGN_ERROR, "Bad foreign name"},
    {ERRNO_VALUE_ERROR, "Bad row value"},
    {ERRNO_TOKEN_ERROR, "Unexpected token"},
    {ERRNO_BASE_ERROR, "Bad base path"},
    {ERRNO_OPCODE_ERROR, "Unexpected opcode"},
    {ERRNO_DBMS_ERROR, "DBMS error"},
    {ERRNO_SCHEMA_ERROR, "Schema error"},
    {ERRNO_SYSTEM_ERROR, "System error"},
    {ERRNO_MALFORMED_COLUMN_EXPR, "Malformed column expression"},
    {ERRNO_MALFORMED_COLUMN_LIST, "Malformed column list"},
    {ERRNO_MALFORMED_COMPARISON, "Malformed comparison"},
    {ERRNO_MALFORMED_DATABASE_REF, "Malformed database reference"},
    {ERRNO_MALFORMED_FOREIGN_KEY, "Malformed foreign key"},
    {ERRNO_MALFORMED_JOIN, "Malformed join"},
    {ERRNO_MALFORMED_PRIMARY_KEY, "Malformed primary key"},
    {ERRNO_MALFORMED_TABLE_REF, "Malformed table reference"},
    {ERRNO_MALFORMED_UNIQUE_KEY, "Malformed unique key"},
    {ERRNO_MALFORMED_VALUE_LIST, "Malformed value list"},
    {ERRNO_MALFORMED_EXPR_LIST, "Malformed expression list"},
    {ERRNO_MALFORMED_EXPR, "Malformed expression"},
    {ERRNO_MULTIPLE_PRIMARY_KEY, "Multiple primary key"},
    {ERRNO_CREAT_DATABASE_ERROR,
      "Create database failed; database already exists"},
    {ERRNO_CREAT_TABLE_ERROR, "Create table failed; table already exists"},
    {ERRNO_CREAT_USER_ERROR, "Create user failed; user already exists"},
    {ERRNO_NO_DATABASE_ERROR, "No database selected"},
    {ERRNO_UPDATE_ERROR,
      "Update of auto-increment or primary-key field not allowed"},
    {ERRNO_ALTER_ERROR, "Currently only empty tables can be altered"},
    {ERRNO_ALTER_ADD_COLUMN, "Attempting to add column into primary-key"},
    {ERRNO_MALFORMED_NAME, "Malformed username or hostname"},
    {ERRNO_ACCESS_DENIED, "Access denied"},
    {ERRNO_BAD_PASSWORD, "Bad password"},
    {ERRNO_DIVIDE_BY_ZERO, "Divide by zero"},
    {ERRNO_MALFORMED_ADDITION, "Malformed addition"},
    {ERRNO_MALFORMED_DIVISION, "Malformed division"},
    {ERRNO_MALFORMED_MULTIPLY, "Malformed multiplication"},
    {ERRNO_MALFORMED_SUBTRACT, "Malformed subtraction"},
    {ERRNO_MALFORMED_REFERENCE, "Malformed reference"},
    {ERRNO_DROP_PRIMARY_KEY, "Attempt to drop primary key"},
    {ERRNO_INFILE_ERROR, "Failed to open infile (see README for INFILE"\
      " usage)"},
    {ERRNO_DUPLICATE_KEY, "Duplicate key error"},
    {ERRNO_OUTFILE_ERROR, "Failed to open outfile (see README for OUTFILE"\
      " usage)"},
    {ERRNO_CRYPTO_ERROR, "Crypto error"},
    {ERRNO_CIPHER_ERROR, "Cipher error"},
    {ERRNO_SOURCE_ERROR, "Source error"},
    {ERRNO_HASH_FILE_ERROR, "Hash file error"},
    {ERRNO_HOME_VAR, "HOME environment variable missing"},
    {ERRNO_READ_HISTORY, "History file read error"},
    {ERRNO_READ_HISTORY, "History file write error"}
  };

  std::map<int32_t, std::string> ERROR_MSG;

  std::string get_msg (int32_t ERRNO) {
    auto iter = ERROR_MSG.find(ERRNO);
    if (iter == ERROR_MSG.end()) return "";
    std::ostringstream msg;
    msg << "ERROR<" << ERRNO  << "> " << (*iter).second;
    return msg.str();
  }

  std::string get_msg (int32_t ERRNO, int32_t code) {
    auto iter = ERROR_MSG.find(ERRNO);
    if (iter == ERROR_MSG.end()) return "";
    std::ostringstream msg;
    msg << "ERROR<" << ERRNO << "> " << (*iter).second << " (" << code << ')';
    return msg.str();
  }

  std::string get_msg (int32_t ERRNO, std::string const& rem) {
    auto iter = ERROR_MSG.find(ERRNO);
    if (iter == ERROR_MSG.end()) return "";
    std::ostringstream msg;
    msg << "ERROR<" << ERRNO << "> " << (*iter).second << ": " << rem;
    return msg.str();
  }

  void init () {
    for (auto p : ERROR_LIST) {
      ERROR_MSG[ p.first ] = p.second;
    }
  }

} // namespace


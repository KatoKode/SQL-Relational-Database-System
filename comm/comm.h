//-----------------------------------------------------------------------------
// Desc: Communication library
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

#ifndef COMM_H
#define COMM_H 1

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>
#include "eno.h"
#include "logger.h"

#undef COMM_DEBUG

namespace red::comm {

  int32_t const TYPE_CLIENT = 0x0F;
  int32_t const TYPE_SERVER = 0x7F;

  size_t const ADDRESS_SZ = 32L;
  size_t const ETHERNET_MSS = 1460L;
  size_t const BUFFER_SZ = ETHERNET_MSS;  // Assuming TCP over Ethernet
  size_t const PAD = 8L;

  std::regex const IP4_ADDR_REGEX {
    "^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
    "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
    "(\\/([0-9]|[1-2][0-9]|3[0-2]))?$"};

  std::regex const HOSTNAME_REGEX {
    "^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*"
    "([A-Za-z]|[A-Za-z][A-Za-z0-9\\-]*[A-Za-z0-9])$"};

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS CONN
   */
  class conn {
    public:
      conn ()
        : m_type {0},
        m_sockfd {-1},
        m_size {0L}
      { }

      ~conn () {
        if (m_sockfd > STDERR_FILENO) close(m_sockfd);
      }

      conn (conn const&) = delete;

      conn (conn&& o)
        : m_type {o.m_type},
        m_sockfd {o.m_sockfd},
        m_size {o.m_size},
        m_cli_addr {o.m_cli_addr},
        m_srv_addr {o.m_srv_addr}
      {
        for (ssize_t i = 0L; i < o.m_size; ++i)
          m_buffer[i] = o.m_buffer[i];

        o.m_sockfd = -1;
      }

      conn& operator=(conn const&) = delete;
      conn& operator=(conn&&);

      std::shared_ptr<conn> accept_conn ();

      uint8_t* buffer () { return m_buffer; }

      std::string cli_addr_name ();

      void cli_init (char const*, char const*);

      bool in_subnet (std::string const&) const;

      bool is_server () const { return (m_type == comm::TYPE_SERVER); }

      size_t read ();

      size_t remaining () const { return (comm::BUFFER_SZ - m_size); }

      size_t size () const { return static_cast<size_t>(m_size); }

      void size (size_t v) { m_size = static_cast<ssize_t>(v); }

      int sockfd () const { return m_sockfd; }

      std::string srv_addr () const;

      std::string srv_addr_name ();

      void srv_init (char const*, char const*, int);

      size_t write ();
    private:
      int32_t m_type;
      int     m_sockfd;
      ssize_t m_size;
      struct sockaddr_in  m_cli_addr;
      struct sockaddr_in  m_srv_addr;
      uint8_t m_buffer[comm::BUFFER_SZ + comm::PAD];
  };

  int32_t const OP_NULL     = 0;
  int32_t const OP_TERM_SYN = 1;
  int32_t const OP_TERM_ACK = 2;
  int32_t const OP_RSLT_ACK = 3;
  int32_t const OP_CONN_SYN = 4;
  int32_t const OP_AUTH_SYN = 5;
  int32_t const OP_STMT_SYN = 6;

//int32_t const OP_TERM_SYN = 1;
//int32_t const OP_TERM_ACK = 2;
  int32_t const OP_RSLT_SYN = 3;
  int32_t const OP_RSLT_FIN = 4;
  int32_t const OP_CONN_ACK = 5;
  int32_t const OP_CONN_ERR = 6;
  int32_t const OP_AUTH_ACK = 7;
  int32_t const OP_AUTH_ERR = 8;
  int32_t const OP_STMT_ERR = 9;

  int32_t const TYPE_TXT = 0;
  int32_t const TYPE_ENC = 1;

  size_t const OPCODE_SZ  = sizeof(uint16_t);
  size_t const OPCODE_POS = 0L;
  size_t const TYPE_POS = 1L;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS SNDRCV
   */
  class sndrcv {
    public:
      sndrcv (std::shared_ptr<conn> conn)
        : m_opsent {comm::OP_NULL}, m_conn {conn}
      { }

      uint8_t* begin () { return &m_conn->buffer()[comm::OPCODE_SZ]; }

      uint8_t* buffer () { return m_conn->buffer(); }

      uint8_t byte_at (size_t i) {
        return m_conn->buffer()[comm::OPCODE_SZ + i];
      }

      uint8_t* end () { return &m_conn->buffer()[m_conn->size()]; }

      int32_t get_opcode () const {
        return static_cast<int32_t>(m_conn->buffer()[comm::OPCODE_POS]);
      }

      int32_t get_opsent () const { return m_opsent; }

      int32_t get_type () const {
        return static_cast<int32_t>(m_conn->buffer()[comm::TYPE_POS]);
      }

      bool in_subnet (std::string const& name) const {
        return m_conn->in_subnet(name);
      }

      bool is_server () const { return m_conn->is_server(); }

      size_t recv ();

      size_t remaining () const { return m_conn->remaining(); }

      void reset () { m_conn->size(comm::OPCODE_SZ); }

      void reset (int32_t opcode) {
        reset();
        set_opcode(opcode);
      }

      void reset (int32_t opcode, int32_t type) {
        reset(opcode);
        set_type(type);
      }

      size_t send ();

      void set_opcode (int32_t opcode) {
        m_conn->buffer()[comm::OPCODE_POS] = static_cast<uint8_t>(opcode);
      }

      void set_type (int32_t type) {
        m_conn->buffer()[comm::TYPE_POS] = static_cast<uint8_t>(type);
      }

      size_t size () const { return m_conn->size(); }

      void size (size_t s) { return m_conn->size(s); }

      size_t size_of_data () { return (m_conn->size() - comm::OPCODE_SZ); }

      int sockfd () const { return m_conn->sockfd(); }
    private:
      int32_t m_opsent;
      std::shared_ptr<conn>  m_conn;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS INFO
   */
  class info {
    public:
      info () { }

      info (std::string const& hostname, std::string const& username)
      : m_hostname {hostname}, m_username {username}
      { }

      void db_name (std::string const& value) { m_db_name = value; }

      std::string hostname () const { return m_hostname; }
      std::string username () const { return m_username; }
      std::string db_name () const { return m_db_name; }
    private:
      std::string m_hostname;
      std::string m_username;
      std::string m_db_name;
  };

  class fsm;

  using rcvd_cb = std::function<void(std::shared_ptr<comm::fsm>)>;
  using flush_cb = std::function<void()>;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS FSM
   */
  class fsm : public sndrcv {
    public:
      fsm (std::shared_ptr<conn> conn, rcvd_cb cb_rcvd)
        : comm::sndrcv {conn},
        m_loop {true},
        m_oprcvd_max {0L},
        m_rcvd_cb {cb_rcvd}
      {
        if (is_server()) make_srv_fsm();
        else make_cli_fsm();
      }

      comm::info& get_info () { return m_info; }

      void cont () { m_loop = true; }

      void flush ();

      void loop (std::shared_ptr<fsm>);

      void nop (std::shared_ptr<fsm>);

      void put (size_t, uint8_t const*);

      void put (size_t size, char const* buf) {
        put(size, reinterpret_cast<uint8_t const*>(buf));
      }

      void put (std::string const& buf) {
        put(buf.size(), buf.c_str());
      }

      void rcvd (std::shared_ptr<fsm>);

      void set_flush_cb (flush_cb cb) { m_flush_cb = cb; }

      void set_info (comm::info& o) { m_info = o; }

      void term () { m_loop = false; }
    private:
      bool  m_loop;
      int32_t m_oprcvd_max;
      rcvd_cb m_rcvd_cb;
      flush_cb  m_flush_cb;
      comm::info  m_info;
      std::vector<
        std::vector<std::function<void(std::shared_ptr<comm::fsm>)> > > m_fsm;

      void make_cli_fsm ();
      void make_srv_fsm ();
  };

} // namespace

#endif  // COMM_H


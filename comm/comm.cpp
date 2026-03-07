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

#include "comm.h"

namespace red::comm {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS CONN: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   */

  /****************************************************************************
   * CLASS CONN: Move Assignment
   */
  conn& conn::operator=(conn&& o) {
    if (this == &o) return *this;
    m_type      = o.m_type;
    m_sockfd    = o.m_sockfd;
    m_size      = o.m_size;
    m_cli_addr  = o.m_cli_addr;
    m_srv_addr  = o.m_srv_addr;

    for (ssize_t i = 0L; i < o.m_size; ++i)
      m_buffer[i] = o.m_buffer[i];

    o.m_sockfd  = -1;
    return *this;
  }

  /****************************************************************************
   *
   */
  std::shared_ptr<conn> conn::accept_conn () {
    errno = 0;
    struct sockaddr_in cli_addr;
    socklen_t addr_len = sizeof(cli_addr);

    auto new_conn = std::shared_ptr<conn> {new conn};

    if ((new_conn->m_sockfd = accept(m_sockfd,
            reinterpret_cast<struct sockaddr*>(&cli_addr), &addr_len)) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_ACCEPT_ERROR, errno)};
#ifdef COMM_DEBUG
    char cli_addr_text[INET_ADDRSTRLEN+1];
    (void)inet_ntop(PF_INET,reinterpret_cast<const void*>
        (&cli_addr.sin_addr), cli_addr_text, INET_ADDRSTRLEN+1);
    std::cout << cli_addr_text << '\n';
#endif
    (void) memcpy(static_cast<void*>(&new_conn->m_cli_addr),
        static_cast<void*>(&cli_addr), addr_len);

    (void) memcpy(static_cast<void*>(&new_conn->m_srv_addr),
        static_cast<void*>(&m_srv_addr), sizeof(new_conn->m_srv_addr));

    new_conn->m_type = TYPE_SERVER;

    return new_conn;
  }

  /****************************************************************************
   *
   */
  std::string conn::cli_addr_name () {
    struct sockaddr_in cli_addr;
    (void) memcpy(static_cast<void*>(&cli_addr),
        static_cast<void*>(&m_cli_addr), sizeof(cli_addr));
    uint32_t haddr = ntohl(cli_addr.sin_addr.s_addr);
    uint16_t hport = ntohs(cli_addr.sin_port);
    char buf [comm::ADDRESS_SZ];
    (void) sprintf (buf, "%hhu.%hhu.%hhu.%hhu:%hu\n",
        ((haddr >> 24) & 0xFF), ((haddr >> 16) & 0xFF),
        ((haddr >> 8) & 0xFF), (haddr & 0xFF),  hport);
    return std::string {buf};
  }

  /****************************************************************************
   *
   */
  void conn::cli_init (char const* host, char const* service) {
    errno = 0;
    struct timeval timer;

    struct addrinfo* ai = nullptr;
    struct addrinfo hints
      = {0, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, nullptr,
        nullptr, nullptr};

    int code;
    if ((code = getaddrinfo(host, service, &hints, &ai)) != 0) {
      if (ai != nullptr) freeaddrinfo(ai);
      throw std::runtime_error{eno::get_msg(eno::ERRNO_ADDRINFO_ERROR, code)};
    }

    socklen_t addr_len = sizeof (m_srv_addr);

    (void) memcpy(static_cast<void*>(&m_srv_addr),
        static_cast<void*>(ai->ai_addr),
        (addr_len > ai->ai_addrlen ? ai->ai_addrlen : addr_len));

    freeaddrinfo (ai);

    if ((m_sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKET_ERROR, errno)};

    socklen_t len;
#ifdef SO_LINGER
    struct linger ling;
    ling.l_onoff  = 1;
    ling.l_linger = 120;
    len = sizeof (ling);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER,
          static_cast<const void *>(&ling), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
#ifdef SO_RCVTIMEO
    timer.tv_sec  = 9000L;
    timer.tv_usec = 0L;
    len = sizeof(timer);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO,
          static_cast<const void*>(&timer), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
#ifdef SO_SNDTIMEO
    timer.tv_sec  = 120L;
    timer.tv_usec = 0L;
    len = sizeof(timer);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO,
          static_cast<const void*>(&timer), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
    if (connect (m_sockfd,
          reinterpret_cast<struct sockaddr*>(&m_srv_addr), addr_len) < 0)
    {
      throw std::runtime_error{eno::get_msg(eno::ERRNO_CONNECT_ERROR, errno)};
    }

    m_type = TYPE_CLIENT;
  }

  /****************************************************************************
   *
   */
  bool conn::in_subnet (std::string const& name) const {
    struct in_addr host;
    struct in_addr mask {0};

    std::regex addr_regex {comm::IP4_ADDR_REGEX};
    std::regex name_regex {comm::HOSTNAME_REGEX};
    std::smatch addr_match;
    std::smatch name_match;
    if (std::regex_match(name, addr_match, addr_regex)) {
      if (addr_match.size() == 3) {
        std::string host_text = addr_match.str(0);
        host_text.erase(host_text.find(addr_match.str(1)));

        (void)inet_pton(PF_INET, host_text.c_str(),
            reinterpret_cast<void*>(&host));

        int32_t cidr = std::stoi(addr_match.str(2));

        if (cidr < 0 || cidr > 32)
          throw std::runtime_error{eno::get_msg(eno::ERRNO_CIDR_ERROR)};

        mask.s_addr = 0;
        for (int32_t i = 31; cidr > 0; --i, --cidr) {
          mask.s_addr |= (0xFFFFFFFF & (1 << i));
        }
        mask.s_addr = htonl(mask.s_addr);
      }

      return ((host.s_addr & mask.s_addr)
          == (m_cli_addr.sin_addr.s_addr & mask.s_addr));
    }
    else if (std::regex_match(name, name_match, name_regex))
    {
      struct sockaddr_in cli_addr;
      socklen_t addr_len = sizeof(cli_addr);

      (void) memcpy (&cli_addr, &m_cli_addr, addr_len);

      socklen_t BUFLEN {64};
      char buf[BUFLEN];
      if (getnameinfo(reinterpret_cast<struct sockaddr*>(&cli_addr), addr_len,
            buf, BUFLEN, nullptr, 0, (NI_NOFQDN | NI_NAMEREQD)) != 0)
        return false;

      std::string hostname {buf};

      if (name == hostname) return true;
    }

    return false;
  }

  /****************************************************************************
   *
   */
  size_t conn::read () {
    errno = 0;
    if ((m_size = recv (m_sockfd, m_buffer, comm::BUFFER_SZ, 0)) < 0)
    {
      if (errno == EAGAIN) m_size = 0;
      else
        throw std::runtime_error{eno::get_msg(eno::ERRNO_RECV_ERROR, errno)};
    }
    return static_cast<size_t>(m_size);
  }

  /****************************************************************************
   *
   */
  void conn::srv_init (char const* host, char const* service, int backlog)
  {
    errno = 0;
    socklen_t addr_len = sizeof (m_srv_addr);
    struct timeval timer;

    if (host != nullptr) {
      struct addrinfo* ai = nullptr;
      struct addrinfo hints
        = {AI_PASSIVE, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, nullptr,
          nullptr, nullptr};

      int code;
      if ((code = getaddrinfo (host, service, &hints, &ai)) != 0) {
        if (ai != nullptr) freeaddrinfo(ai);
        throw std::runtime_error{eno::get_msg(eno::ERRNO_ADDRINFO_ERROR, code)};
      }

      (void) memcpy (&m_srv_addr, ai->ai_addr,
          (addr_len <= ai->ai_addrlen) ? addr_len : ai->ai_addrlen);

      freeaddrinfo (ai);
    } else {
      m_srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // wildcard

      try {
        m_srv_addr.sin_port = std::stoi(service);
      } catch (...) {
        struct servent* sp;
        char const protocol[] = {"tcp"};
        if ((sp = getservbyname(service, protocol)) == nullptr)
          throw std::runtime_error{
            eno::get_msg(eno::ERRNO_SERVICE_ERROR, errno)};
        m_srv_addr.sin_port = sp->s_port;
      }
    }

    if ((m_sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKET_ERROR, errno)};

    socklen_t len;
#ifdef SO_LINGER
    struct linger ling;
    ling.l_onoff  = 1;
    ling.l_linger = 120;
    len = sizeof(ling);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER,
          static_cast<const void *>(&ling), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
#ifdef SO_REUSEADDR
    int val {1};
    len = sizeof(val);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR,
          static_cast<const void*>(&val), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
#ifdef SO_RCVTIMEO
    timer.tv_sec  = 0L;
    timer.tv_usec = 0L;
    len = sizeof(timer);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO,
          static_cast<const void*>(&timer), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
#ifdef SO_SNDTIMEO
    timer.tv_sec  = 8100L;
    timer.tv_usec = 0L;
    len = sizeof(timer);
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO,
          static_cast<const void*>(&timer), len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SOCKOPT_ERROR, errno)};
#endif
    if (bind (m_sockfd,
          reinterpret_cast<struct sockaddr*>(&m_srv_addr), addr_len) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BIND_ERROR, errno)};

    if (listen (m_sockfd, backlog) < 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_LISTEN_ERROR, errno)};

    m_type = TYPE_SERVER;
  }

  /******************************************************************************
   *
   */
  size_t conn::write () {
    errno = 0;
    ssize_t size {0};
    if ((size = send (m_sockfd, m_buffer, m_size, 0)) < 0)
    {
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SEND_ERROR, errno)};
    }
    return static_cast<size_t>(size);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS SNDRCV: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  size_t sndrcv::recv () {
    try { return m_conn->read(); } catch (...) { throw; }
  }

  /****************************************************************************
   *
   */
  size_t sndrcv::send () {
    m_opsent = get_opcode();
    try { return m_conn->write(); } catch (...) { throw; }
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS FSM: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void fsm::flush () {
    send();
    reset();
    if (is_server() && get_opsent() == comm::OP_RSLT_SYN) {
      m_flush_cb();
      // NUDGE: if this is the penultimate send then everything will be fine,
      //        otherwise, we need a nudge this train back on the track by
      //        restoring opcode OP_RSLT_SYN to the buffer
      //
      // set_opcode(get_opsent());
      //
      // KLUDGE: or OP_RSLT_SYN and OP_RSLT_ACK can be aligned so one is equal
      //         to the other which this is the design here
    }
  }

  /****************************************************************************
   *
   */
  void fsm::loop (std::shared_ptr<fsm> sp)
  {
    int32_t oprcvd {0};

    while (m_loop) {
      try {
        recv();
        if (size() == 0L) break;  // probably due to EAGAIN timeout
        oprcvd = get_opcode();
        if (oprcvd < comm::OP_NULL || oprcvd > m_oprcvd_max) {
          nop(sp);
          continue;
        }
        m_fsm[ get_opsent() ][ oprcvd ](sp);
      } catch (...) { throw; }
    }
  }

  /****************************************************************************
   *
   */
  void fsm::nop (std::shared_ptr<fsm>) {
    term();
  }

  /****************************************************************************
   *
   */
  void fsm::put (size_t len, uint8_t const* buf) {
    for (size_t i = 0; i < len; ++i) {
      if (size() == comm::BUFFER_SZ) flush();
      buffer()[size()] = buf[i];
      size(size() + 1L);
    }
  }

  /****************************************************************************
   *
   */
  void fsm::rcvd (std::shared_ptr<fsm> sp) {
    m_rcvd_cb(sp);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS FSM: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   *
   */
  void fsm::make_cli_fsm () {
    auto a_nop = std::bind(&fsm::nop, this, std::placeholders::_1);
    auto a_rcvd = std::bind(&fsm::rcvd, this, std::placeholders::_1);

    m_fsm.emplace_back();
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_NULL ]
    m_fsm[0].push_back(a_rcvd); // [ OP_NULL ] [ OP_TERM_SYN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_TERM_ACK ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_RSLT_SYN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_RSLT_FIN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_CONN_ACK ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_CONN_ERR ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_AUTH_ACK ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_AUTH_ERR ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_NULL ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_TERM_SYN ]
    m_fsm[1].push_back(a_rcvd); // [ OP_TERM_SYN ] [ OP_TERM_ACK ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_RSLT_SYN ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_RSLT_FIN ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_CONN_ACK ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_CONN_ERR ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_AUTH_ACK ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_AUTH_ERR ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_NULL ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_TERM_SYN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_TERM_ACK ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_RSLT_SYN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_RSLT_FIN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_CONN_ACK ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_CONN_ERR ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_AUTH_ACK ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_AUTH_ERR ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_NULL ]
    m_fsm[3].push_back(a_rcvd); // [ OP_RSLT_ACK ] [ OP_TERM_SYN ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_TERM_ACK ]
    m_fsm[3].push_back(a_rcvd); // [ OP_RSLT_ACK ] [ OP_RSLT_SYN ]
    m_fsm[3].push_back(a_rcvd); // [ OP_RSLT_ACK ] [ OP_RSLT_FIN ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_CONN_ACK ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_CONN_ERR ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_AUTH_ACK ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_AUTH_ERR ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_ACK ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_NULL ]
    m_fsm[4].push_back(a_rcvd); // [ OP_CONN_SYN ] [ OP_TERM_SYN ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_TERM_ACK ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_RSLT_SYN ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_RSLT_FIN ]
    m_fsm[4].push_back(a_rcvd); // [ OP_CONN_SYN ] [ OP_CONN_ACK ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_CONN_ERR ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_AUTH_ACK ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_AUTH_ERR ]
    m_fsm[4].push_back(a_nop);  // [ OP_CONN_SYN ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_NULL ]
    m_fsm[5].push_back(a_rcvd); // [ OP_AUTH_SYN ] [ OP_TERM_SYN ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_TERM_ACK ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_RSLT_SYN ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_RSLT_FIN ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_CONN_ACK ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_CONN_ERR ]
    m_fsm[5].push_back(a_rcvd); // [ OP_AUTH_SYN ] [ OP_AUTH_ACK ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_AUTH_ERR ]
    m_fsm[5].push_back(a_nop);  // [ OP_AUTH_SYN ] [ OP_STMT_ERR ]

    m_fsm.emplace_back();
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_NULL ]
    m_fsm[6].push_back(a_rcvd); // [ OP_STMT_SYN ] [ OP_TERM_SYN ]
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_TERM_ACK ]
    m_fsm[6].push_back(a_rcvd); // [ OP_STMT_SYN ] [ OP_RSLT_SYN ]
    m_fsm[6].push_back(a_rcvd); // [ OP_STMT_SYN ] [ OP_RSLT_FIN ]
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_CONN_ACK ]
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_CONN_ERR ]
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_AUTH_ACK ]
    m_fsm[6].push_back(a_nop);  // [ OP_STMT_SYN ] [ OP_AUTH_ERR ]
    m_fsm[6].push_back(a_rcvd); // [ OP_STMT_SYN ] [ OP_STMT_ERR ]

    m_oprcvd_max = static_cast<int32_t>(m_fsm[0].size()) - 1;
  }

  /****************************************************************************
   *
   */
  void fsm::make_srv_fsm () {
    auto a_nop = std::bind(&fsm::nop, this, std::placeholders::_1);
    auto a_rcvd = std::bind(&fsm::rcvd, this, std::placeholders::_1);

    m_fsm.emplace_back();
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_NULL ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_TERM_SYN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_TERM_ACK ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_RSLT_ACK ]
    m_fsm[0].push_back(a_rcvd); // [ OP_NULL ] [ OP_CONN_SYN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_AUTH_SYN ]
    m_fsm[0].push_back(a_nop);  // [ OP_NULL ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_NULL ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_TERM_SYN ]
    m_fsm[1].push_back(a_rcvd); // [ OP_TERM_SYN ] [ OP_TERM_ACK ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_RSLT_ACK ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_CONN_SYN ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_AUTH_SYN ]
    m_fsm[1].push_back(a_nop);  // [ OP_TERM_SYN ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_NULL ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_TERM_SYN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_TERM_ACK ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_RSLT_ACK ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_CONN_SYN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_AUTH_SYN ]
    m_fsm[2].push_back(a_nop);  // [ OP_TERM_ACK ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_SYN ] [ OP_NULL ]
    m_fsm[3].push_back(a_rcvd); // [ OP_RSLT_SYN ] [ OP_TERM_SYN ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_SYN ] [ OP_TERM_ACK ]
    m_fsm[3].push_back(a_rcvd); // [ OP_RSLT_SYN ] [ OP_RSLT_ACK ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_SYN ] [ OP_CONN_SYN ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_SYN ] [ OP_AUTH_SYN ]
    m_fsm[3].push_back(a_nop);  // [ OP_RSLT_SYN ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[4].push_back(a_nop);  // [ OP_RSLT_FIN ] [ OP_NULL ]
    m_fsm[4].push_back(a_rcvd); // [ OP_RSLT_FIN ] [ OP_TERM_SYN ]
    m_fsm[4].push_back(a_nop);  // [ OP_RSLT_FIN ] [ OP_TERM_ACK ]
    m_fsm[4].push_back(a_nop);  // [ OP_RSLT_FIN ] [ OP_RSLT_ACK ]
    m_fsm[4].push_back(a_nop);  // [ OP_RSLT_FIN ] [ OP_CONN_SYN ]
    m_fsm[4].push_back(a_nop);  // [ OP_RSLT_FIN ] [ OP_AUTH_SYN ]
    m_fsm[4].push_back(a_rcvd); // [ OP_RSLT_FIN ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[5].push_back(a_nop);  // [ OP_CONN_ACK ] [ OP_NULL ]
    m_fsm[5].push_back(a_rcvd); // [ OP_CONN_ACK ] [ OP_TERM_SYN ]
    m_fsm[5].push_back(a_nop);  // [ OP_CONN_ACK ] [ OP_TERM_ACK ]
    m_fsm[5].push_back(a_nop);  // [ OP_CONN_ACK ] [ OP_RSLT_ACK ]
    m_fsm[5].push_back(a_nop);  // [ OP_CONN_ACK ] [ OP_CONN_SYN ]
    m_fsm[5].push_back(a_rcvd); // [ OP_CONN_ACK ] [ OP_AUTH_SYN ]
    m_fsm[5].push_back(a_nop);  // [ OP_CONN_ACK ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_NULL ]
    m_fsm[6].push_back(a_rcvd);  // [ OP_CONN_ERR ] [ OP_TERM_SYN ]
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_TERM_ACK ]
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_RSLT_ACK ]
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_CONN_SYN ]
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_AUTH_SYN ]
    m_fsm[6].push_back(a_nop);   // [ OP_CONN_ERR ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[7].push_back(a_nop);  // [ OP_AUTH_ACK ] [ OP_NULL ]
    m_fsm[7].push_back(a_rcvd); // [ OP_AUTH_ACK ] [ OP_TERM_SYN ]
    m_fsm[7].push_back(a_nop);  // [ OP_AUTH_ACK ] [ OP_TERM_ACK ]
    m_fsm[7].push_back(a_nop);  // [ OP_AUTH_ACK ] [ OP_RSLT_ACK ]
    m_fsm[7].push_back(a_nop);  // [ OP_AUTH_ACK ] [ OP_CONN_SYN ]
    m_fsm[7].push_back(a_nop);  // [ OP_AUTH_ACK ] [ OP_AUTH_SYN ]
    m_fsm[7].push_back(a_rcvd); // [ OP_AUTH_ACK ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_NULL ]
    m_fsm[8].push_back(a_rcvd); // [ OP_AUTH_ERR ] [ OP_TERM_SYN ]
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_TERM_ACK ]
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_RSLT_ACK ]
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_CONN_SYN ]
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_AUTH_SYN ]
    m_fsm[8].push_back(a_nop);  // [ OP_AUTH_ERR ] [ OP_STMT_SYN ]

    m_fsm.emplace_back();
    m_fsm[9].push_back(a_nop);  // [ OP_STMT_ERR ] [ OP_NULL ]
    m_fsm[9].push_back(a_rcvd); // [ OP_STMT_ERR ] [ OP_TERM_SYN ]
    m_fsm[9].push_back(a_nop);  // [ OP_STMT_ERR ] [ OP_TERM_ACK ]
    m_fsm[9].push_back(a_nop);  // [ OP_STMT_ERR ] [ OP_RSLT_ACK ]
    m_fsm[9].push_back(a_nop);  // [ OP_STMT_ERR ] [ OP_CONN_SYN ]
    m_fsm[9].push_back(a_nop);  // [ OP_STMT_ERR ] [ OP_AUTH_SYN ]
    m_fsm[9].push_back(a_rcvd); // [ OP_STMT_ERR ] [ OP_STMT_SYN ]

    m_oprcvd_max = static_cast<int32_t>(m_fsm[0].size()) - 1;
  }

} // namespace


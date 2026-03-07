//-----------------------------------------------------------------------------
// Desc: Buffer library
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

#ifndef BUFFER_H
#define BUFFER_H 1

#include <endian.h>
#include <cstdint>
#include <climits>
#include <bitset>
#include <memory>
#include <iostream>
#include "eno.h"
#include "logger.h"


namespace red {

#if !defined (__BYTE_ORDER) && !defined (BYTE_ORDER)
#error neither __BYTE_ORDER nor BYTE_ORDER are definded!
#endif

  using byte = unsigned char;

	constexpr int16_t MASK_16_BYTE_0  = 0x00FF;
	constexpr int16_t MASK_16_BYTE_1  = 0xFF00;

	constexpr int32_t MASK_32_BYTE_0  = 0x000000FF;
	constexpr int32_t MASK_32_BYTE_1  = 0x0000FF00;
	constexpr int32_t MASK_32_BYTE_2  = 0x00FF0000;
	constexpr int32_t MASK_32_BYTE_3  = 0xFF000000;

	constexpr int64_t MASK_64_BYTE_0  = 0x00000000000000FF;
	constexpr int64_t MASK_64_BYTE_1  = 0x000000000000FF00;
	constexpr int64_t MASK_64_BYTE_2  = 0x0000000000FF0000;
	constexpr int64_t MASK_64_BYTE_3  = 0x00000000FF000000;
	constexpr int64_t MASK_64_BYTE_4  = 0x000000FF00000000;
	constexpr int64_t MASK_64_BYTE_5  = 0x0000FF0000000000;
	constexpr int64_t MASK_64_BYTE_6  = 0x00FF000000000000;
	constexpr int64_t MASK_64_BYTE_7  = 0xFF00000000000000;

	constexpr int32_t SHIFT_0   = 0;
	constexpr int32_t SHIFT_8   = 8;
	constexpr int32_t SHIFT_16  = 16;
	constexpr int32_t SHIFT_24  = 24;
	constexpr int32_t SHIFT_32  = 32;
	constexpr int32_t SHIFT_40  = 40;
	constexpr int32_t SHIFT_48  = 48;
	constexpr int32_t SHIFT_56  = 56;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BITS
   */
  class bits {
    public:
      enum class byte_order { NONE, BIG_END, LITTLE_END };

      bits () { }
      ~bits () { }

      template<typename R, typename T>
        R mutate (T v) {
          R* r = reinterpret_cast<R*>(&v);
          return *r;
        }

      template<typename T, int32_t SHIFT>
        byte shift_right(T value, T mask) {
          return byte((value & mask) >> SHIFT);
        }

      template<typename T, int32_t SHIFT>
        T shift_left(byte value) {
          T x = value;
          x <<= SHIFT;
          return x;
        }

      byte_order native_byte_order ();
      double to_double (byte, byte, byte, byte, byte, byte, byte, byte);
      float to_float (byte, byte, byte, byte);
      int16_t to_int16 (byte, byte);
      int32_t to_int32 (byte, byte, byte, byte);
      int64_t to_int64 (byte, byte, byte, byte, byte, byte, byte, byte);
      uint16_t to_uint16 (byte, byte);
      uint32_t to_uint32 (byte, byte, byte, byte);
      uint64_t to_uint64 (byte, byte, byte, byte, byte, byte, byte, byte);
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BBUFFER
   */
  class bbuffer {
    protected:
      bbuffer (int32_t size) :
        m_size {size},
        m_bound {size},
        m_index {0}
      { }

      bbuffer (const bbuffer& o) :
        m_size {o.m_size},
        m_bound {o.m_bound},
        m_index {o.m_index}
      { }

      bbuffer (bbuffer&& o) :
        m_size {o.m_size},
        m_bound {o.m_bound},
        m_index {o.m_index}
      { }

      bbuffer& operator=(const bbuffer&);
      bbuffer& operator=(bbuffer&&);
    public:
      int32_t bound () const { return m_bound; }
      int32_t remaining () const { return m_bound - m_index; }
      int32_t index () const { return m_index; }
      bool has_more () const { return m_bound > m_index; }
      int32_t size () const { return m_size; }

      void bound (int32_t);
      void clear ();
      void index (int32_t);
      void reset ();
      void rewind ();
      void trip ();
    private:
      int32_t m_size;
      int32_t m_bound;
      int32_t m_index;
  };

  using cb_commit_t = void (*) ();

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS BUFFER
   */
  class buffer : public bbuffer {
    public:
      buffer (int32_t size)
        : bbuffer {size},
        m_cb_commit{nullptr},
        m_buffer {std::shared_ptr<uint8_t> {new uint8_t[size]}}
      { }

      buffer (int32_t size, std::shared_ptr<uint8_t> buffer)
        : bbuffer {size},
        m_cb_commit {nullptr},
        m_buffer {buffer}
      { }

      buffer (buffer const& o)
        : bbuffer {o},
        m_cb_commit {o.m_cb_commit},
        m_buffer {o.m_buffer}
      { }

      buffer (buffer&& o)
        : bbuffer {o},
        m_cb_commit {o.m_cb_commit},
        m_buffer {o.m_buffer}
      { }

      buffer& operator=(buffer const&);
      buffer& operator=(buffer&&);

      uint8_t get ();
      uint8_t get (int32_t);
      void get (buffer&);
      void get (buffer&, int32_t);
      void get (buffer&, int32_t, int32_t);
      char get_char ();
      char get_char (int32_t);
      double get_double ();
      double get_double (int32_t);
      float get_float ();
      float get_float (int32_t);
      int16_t get_int16 ();
      int16_t get_int16 (int32_t);
      int32_t get_int32 ();
      int32_t get_int32 (int32_t);
      int64_t get_int64 ();
      int64_t get_int64 (int32_t);
      std::string get_varchar (int32_t);
      std::string get_varchar (int32_t, int32_t);
      bits::byte_order order () { return bits::byte_order::BIG_END; }
      void put (uint8_t);
      void put (int32_t, uint8_t);
      void put (buffer&);
      void put (int32_t, buffer&);
      void put (int32_t, int32_t, buffer&);
      void put_char (char);
      void put_char (int32_t, char);
      void put_double (double);
      void put_double (int32_t, double);
      void put_float (float);
      void put_float (int32_t, float);
      void put_int16 (int16_t);
      void put_int16 (int32_t, int16_t);
      void put_int32 (int32_t);
      void put_int32 (int32_t, int32_t);
      void put_int64 (int64_t);
      void put_int64 (int32_t, int64_t);
      void put_varchar (std::string const&);
      void put_varchar (int32_t, std::string const&);

      uint8_t* backing () { return m_buffer.get(); }
      void set_commit_cb (cb_commit_t cb_commit) { m_cb_commit = cb_commit; }
    private:
      bits      m_bits;
      cb_commit_t m_cb_commit;
      std::shared_ptr<uint8_t>  m_buffer;
  };

} // namespace

#endif  // BUFFER_H

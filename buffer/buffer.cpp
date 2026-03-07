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

#include "buffer.h"

namespace red {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS BITS: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /**************************************************************************
   * Return native byte-order as BIG_END (for BIG ENDIAN) or LITTLE_END (for
   * LITTE ENDIAN) or NONE (hmmmm).
   */
  bits::byte_order bits::native_byte_order () {
    int32_t x = 0x01000004;
    if (((char *)&x)[0] == 0x01) return bits::byte_order::BIG_END;
    else if (((char *)&x)[0] == 0x04) return bits::byte_order::LITTLE_END;
    return byte_order::NONE;
  }

  /**************************************************************************
   * Return double value resulting from the composition of byte parameters.
   */
  double bits::to_double(byte b0, byte b1, byte b2, byte b3,
      byte b4, byte b5, byte b6, byte b7)
  {
    uint64_t x = to_uint64(b0, b1, b2, b3, b4, b5, b6, b7);
    return mutate<double, uint64_t> (x);
  }

  /**************************************************************************
   * Return float value resulting from the composition of byte parameters.
   */
  float bits::to_float(byte b0, byte b1, byte b2, byte b3) {
    uint32_t x = to_uint32(b0, b1, b2, b3);
    return mutate<float, uint32_t> (x);
  }

  /**************************************************************************
   * Return int16_t value resulting from the composition of byte parameters.
   */
  int16_t bits::to_int16(byte b0, byte b1) {
    return to_uint16(b0, b1);
  }

  /**************************************************************************
   * Return int32_t value resulting from the composition of byte parameters.
   */
  int32_t bits::to_int32(byte b0, byte b1, byte b2, byte b3) {
    return to_uint32(b0, b1, b2, b3);
  }

  /**************************************************************************
   * Return int64_t value resulting from the composition of byte parameters.
   */
  int64_t bits::to_int64(byte b0, byte b1, byte b2, byte b3,
      byte b4, byte b5, byte b6, byte b7)
  {
    return to_uint64(b0, b1, b2, b3, b4, b5, b6, b7);
  }

  /**************************************************************************
   * Return uint16_t value resulting from the composition of byte parameters.
   */
  uint16_t bits::to_uint16(byte b0, byte b1) {
    uint16_t x = shift_left<uint16_t, SHIFT_8>(b1)
      | shift_left<uint16_t, SHIFT_0>(b0); 
    return x;
  }

    /**************************************************************************
     * Return uint32_t value resulting from the composition of byte parameters.
     */
    uint32_t bits::to_uint32(byte b0, byte b1, byte b2, byte b3) {
      uint32_t x = shift_left<uint32_t, SHIFT_24>(b3)
        | shift_left<uint32_t, SHIFT_16>(b2)
        | shift_left<uint32_t, SHIFT_8>(b1)
        | shift_left<uint32_t, SHIFT_0>(b0);
      return x;
    }

    /**************************************************************************
     * Return uint64_t value resulting from the composition of byte parameters.
     */
    uint64_t bits::to_uint64(byte b0, byte b1, byte b2, byte b3,
        byte b4, byte b5, byte b6, byte b7)
    {
      uint64_t x = shift_left<uint64_t, SHIFT_56>(b7)
        | shift_left<uint64_t, SHIFT_48>(b6)
        | shift_left<uint64_t, SHIFT_40>(b5)
        | shift_left<uint64_t, SHIFT_32>(b4)
        | shift_left<uint64_t, SHIFT_24>(b3)
        | shift_left<uint64_t, SHIFT_16>(b2)
        | shift_left<uint64_t, SHIFT_8>(b1)
        | shift_left<uint64_t, SHIFT_0>(b0); 
      return x;
    }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS BBUFFER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS BBUFFER: Copy Assignment
   */
  bbuffer& bbuffer::operator=(const bbuffer& o) {
    if (this == &o) return *this;
    m_size  = o.m_size;
    m_bound = o.m_bound;
    m_index = o.m_index;
    return *this;
  }

  /***************************************************************************
   * CLASS BBUFFER: Move Assignment
   */
  bbuffer& bbuffer::operator=(bbuffer&& o) {
    if (this == &o) return *this;
    m_size  = o.m_size;
    m_bound = o.m_bound;
    m_index = o.m_index;
    return *this;
  }

  /***************************************************************************
   * Set the bound of the buffer.
   */
  void bbuffer::bound (int32_t bound) {
    if (bound < 0 || bound > m_size)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_BOUND_ERROR)};

    m_bound = bound;

    if (m_index > bound) m_index = bound;
  }

  /***************************************************************************
   * Clear buffer.
   */
  void bbuffer::clear () {
    m_bound = m_size;
    m_index = 0;
  }

  /***************************************************************************
   * Set buffer index.
   */
  void bbuffer::index (int32_t value) {
    if (value < 0 || value > m_bound)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_INDEX_ERROR)};

    m_index = value;
  }

  /***************************************************************************
   * Reset buffer.
   */
   void bbuffer::reset() {
     m_bound = m_size;
     m_index = 0;
   }

  /***************************************************************************
   * Reset buffer.
   */
   void bbuffer::rewind() {
     m_index = 0;
   }

  /***************************************************************************
   * Trip buffer.
   */
  void bbuffer::trip () {
    m_bound = m_index;
    m_index = 0;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS BUFFER: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS BUFFER: Copy Assignment
   */
  buffer& buffer::operator=(buffer const& o) {
    if (this == &o) return *this;
    if (size() != o.size())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_SIZE_ERROR)};
    m_cb_commit = o.m_cb_commit;
    m_buffer = o.m_buffer;
    return *this;
  }

  /****************************************************************************
   * CLASS BUFFER: Move Assignment
   */
  buffer& buffer::operator=(buffer&& o) {
    if (this == &o) return *this;
    if (size() != o.size())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_SIZE_ERROR)};
    m_cb_commit = o.m_cb_commit;
    m_buffer = o.m_buffer;
    return *this;
  }

  /****************************************************************************
   * Return byte value at index ().
   */
  uint8_t buffer::get () {
    uint8_t b = m_buffer.get() [index()];
    index(index() + 1);
    return b;
  }

  /****************************************************************************
   * Return byte value at index(n).
   */
  uint8_t buffer::get (int32_t n) {
    index(n);
    return get();
  }

  /****************************************************************************
   * Copy the contents of this buffer to buffer (b). The number of bytes copied
   * depends on the number of remaining bytes in each buffer.
   */
  void buffer::get (buffer& b) {
    if (remaining() > b.remaining())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_REMAINING_ERROR)};
    while (remaining() > 0) b.put(get());
  }

  /****************************************************************************
   * Copy the contents of this buffer to buffer (b) starting at index (n). The
   * number of bytes copied depends on the number of remaining bytes in each
   * buffer.
   */
  void buffer::get (buffer& b, int32_t n) {
    index(n);
    get(b);
  }

  /****************************************************************************
   * Copy (size) bytes of the contents of this buffer to buffer (b) starting at
   * index (n).
   */
  void buffer::get (buffer& b, int32_t n, int32_t size) {
    index(n);
    if (size > remaining() && size > b.remaining())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_REMAINING_ERROR)};
    while (size-- > 0) b.put(get());
  }

  /****************************************************************************
   * Return char value at index ().
   */
  char buffer::get_char () {
    return static_cast<char>(get());
  }

  /****************************************************************************
   * Return a char value at index (n).
   */
  char buffer::get_char (int32_t n) {
    return static_cast<char>(get(n));
  }

  /****************************************************************************
  * Return double value at index ().
  */
  double buffer::get_double () {
    double value;
#if (BYTE_ORDER == BIG_ENDIAN)
    value = m_bits.to_double(get(), get(), get(), get(),
        get(), get(), get(), get());
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t const i = index();
    value = m_bits.to_double(get(i + 7), get (i + 6), get(i + 5), get(i + 4),
        get(i + 3), get(i + 2), get(i + 1), get(i));
    index(i + 8);
#endif
    return value;
  }

  /****************************************************************************
   * Return double value at index (n).
   */
  double buffer::get_double (int32_t n) {
    index(n);
    return get_double();
  }

  /****************************************************************************
   * Return float value at index ().
   */
  float buffer::get_float () {
    float value;
#if (BYTE_ORDER == BIG_ENDIAN)
    value = m_bits.to_float(get(), get(), get(), get());
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t const i = index();
    value = m_bits.to_float(get(i + 3), get(i + 2), get(i + 1), get(i));
    index(i + 4);
#endif
    return value;
  }

  /****************************************************************************
   * Return float value at index (n).
   */
  float buffer::get_float (int n) {
    index(n);
    return get_float();
  }

  /****************************************************************************
   * Return int16_t value at index ().
   */
  int16_t buffer::get_int16 () {
    int16_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
    value = m_bits.to_int16(get(), get());
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t const i = index();
    value = m_bits.to_int16(get(i + 1), get(i));
    index(i + 2);
#endif
    return value;
  }

  /****************************************************************************
   * Return int16_t value at index (n).
   */
  int16_t buffer::get_int16 (int32_t n) {
    index(n);
    return get_int16();
  }

  /****************************************************************************
   * Return int32_t value at index ().
   */
  int buffer::get_int32 () {
    int32_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
    value = m_bits.to_int32(get(), get(), get(), get());
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t const i = index();
    value = m_bits.to_int32(get(i + 3), get(i + 2), get(i + 1), get(i));
    index(i + 4);
#endif
    return value;
  }

  /****************************************************************************
   * Return int32_t value at index (n).
   */
  int32_t buffer::get_int32 (int32_t n) {
    index(n);
    return get_int32();
  }

  /****************************************************************************
   * Return int64_t value at index ().
   */
  int64_t buffer::get_int64 () {
    int64_t value;
#if (BYTE_ORDER == BIG_ENDIAN)
    value = m_bits.to_int64(get(), get(), get(), get(),
        get(), get(), get(), get());
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    int32_t const i = index();
    value = m_bits.to_int64(get(i + 7), get(i + 6), get(i + 5), get(i + 4),
        get(i + 3), get(i + 2), get(i + 1), get(i));
    index(i + 8);
#endif
    return value;
  }

  /****************************************************************************
   * Return int64_t value at index (n).
   */
  int64_t buffer::get_int64 (int32_t n) {
    index(n);
    return get_int64();
  }

  /****************************************************************************
   * Return varchar value (size) at index ().
   */
  std::string buffer::get_varchar (int32_t size) {
    std::string s;
    while (size-- > 0) s += get_char();
    return s;
  }

  /****************************************************************************
   * Return varchar value (size) at index (n).
   */
  std::string buffer::get_varchar (int32_t n, int32_t size) {
    index(n);
    return get_varchar(size);
  }

  /****************************************************************************
   * Populate buffer with uint8_t value.
   */
  void buffer::put (uint8_t value) {
    if (remaining() == 0)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_REMAINING_ERROR)};
    m_buffer.get()[index()] = value;
    index(index() + 1);
  }

  /****************************************************************************
   * Put uint8_t value in buffer at index (n).
   */
  void buffer::put (int32_t n, uint8_t value) {
    index(n);
    put(value);
  }

  /****************************************************************************
   * Copy the remaining bytes of buffer (b) into this buffer. The number of
   * bytes copied depends on the number of remaining bytes in each buffer.
   */
  void buffer::put (buffer& b) {
    if (b.remaining() > remaining())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_REMAINING_ERROR)};
    while (b.remaining() > 0) put(b.get());
  }

  /****************************************************************************
   * Copy the contents of buffer (b) to this buffer starting a index (n). The
   * number of bytes copied depends on the number of remaining bytes in each
   * buffer.
   */
  void buffer::put (int32_t n, buffer& b) {
    index(n);
    put(b);
  }

  /****************************************************************************
   * Copy (size) bytes of buffer (b) to this buffer starting at index (n) in
   * this buffer.
   */
  void buffer::put (int32_t n, int32_t size, buffer& b) {
    index (n);
    if (size > remaining() && size > b.remaining())
      throw std::runtime_error{eno::get_msg(eno::ERRNO_BUF_REMAINING_ERROR)};
    while (size-- > 0) put(b.get());
  }

  /****************************************************************************
   * Put char value in buffer at index ().
   */
  void buffer::put_char (char value) {
    put(value);
  }

  /****************************************************************************
   * Put char value in buffer at index (n).
   */
  void buffer::put_char (int32_t n, char value) {
    put(n, value);
  }

  /****************************************************************************
   * Put double value in buffer at index ().
   */
  void buffer::put_double (double value) {
    uint64_t ui64 = m_bits.mutate<uint64_t, double>(value);
#if (BYTE_ORDER == BIG_ENDIAN)
    put(m_bits.shift_right<uint64_t, SHIFT_0>(ui64, MASK_64_BYTE_0));
    put(m_bits.shift_right<uint64_t, SHIFT_8>(ui64, MASK_64_BYTE_1));
    put(m_bits.shift_right<uint64_t, SHIFT_16>(ui64, MASK_64_BYTE_2));
    put(m_bits.shift_right<uint64_t, SHIFT_24>(ui64, MASK_64_BYTE_3));
    put(m_bits.shift_right<uint64_t, SHIFT_32>(ui64, MASK_64_BYTE_4));
    put(m_bits.shift_right<uint64_t, SHIFT_40>(ui64, MASK_64_BYTE_5));
    put(m_bits.shift_right<uint64_t, SHIFT_48>(ui64, MASK_64_BYTE_6));
    put(m_bits.shift_right<uint64_t, SHIFT_56>(ui64, MASK_64_BYTE_7));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    put(m_bits.shift_right<uint64_t, SHIFT_56>(ui64, MASK_64_BYTE_7));
    put(m_bits.shift_right<uint64_t, SHIFT_48>(ui64, MASK_64_BYTE_6));
    put(m_bits.shift_right<uint64_t, SHIFT_40>(ui64, MASK_64_BYTE_5));
    put(m_bits.shift_right<uint64_t, SHIFT_32>(ui64, MASK_64_BYTE_4));
    put(m_bits.shift_right<uint64_t, SHIFT_24>(ui64, MASK_64_BYTE_3));
    put(m_bits.shift_right<uint64_t, SHIFT_16>(ui64, MASK_64_BYTE_2));
    put(m_bits.shift_right<uint64_t, SHIFT_8>(ui64, MASK_64_BYTE_1));
    put(m_bits.shift_right<uint64_t, SHIFT_0>(ui64, MASK_64_BYTE_0));
#endif
  }

  /****************************************************************************
   * Put double value in buffer at index (n).
   */
  void buffer::put_double (int32_t n, double value) {
    index(n);
    put_double(value);
  }

  /****************************************************************************
   * Put float value in buffer at index ().
   */
  void buffer::put_float (float value) {
    uint32_t ui32 = m_bits.mutate<uint32_t, float>(value);
#if (BYTE_ORDER == BIG_ENDIAN)
    put(m_bits.shift_right<uint32_t, SHIFT_0>(ui32, MASK_32_BYTE_0));
    put(m_bits.shift_right<uint32_t, SHIFT_8>(ui32, MASK_32_BYTE_1));
    put(m_bits.shift_right<uint32_t, SHIFT_16>(ui32, MASK_32_BYTE_2));
    put(m_bits.shift_right<uint32_t, SHIFT_24>(ui32, MASK_32_BYTE_3));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    put(m_bits.shift_right<uint32_t, SHIFT_24>(ui32, MASK_32_BYTE_3));
    put(m_bits.shift_right<uint32_t, SHIFT_16>(ui32, MASK_32_BYTE_2));
    put(m_bits.shift_right<uint32_t, SHIFT_8>(ui32, MASK_32_BYTE_1));
    put(m_bits.shift_right<uint32_t, SHIFT_0>(ui32, MASK_32_BYTE_0));
#endif
  }

  /****************************************************************************
   * Put float value in buffer at index (n).
   */
  void buffer::put_float (int32_t n, float value) {
    index(n);
    put_float(value);
  }

  /****************************************************************************
   * Put int16_t value in buffer at index ().
   */
  void buffer::put_int16 (int16_t value) {
    uint16_t ui16 = m_bits.mutate<uint16_t, int16_t>(value);
#if (BYTE_ORDER == BIG_ENDIAN)
    put(m_bits.shift_right<uint16_t, SHIFT_0>(ui16, MASK_16_BYTE_0));
    put(m_bits.shift_right<uint16_t, SHIFT_8>(ui16, MASK_16_BYTE_1));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    put(m_bits.shift_right<uint16_t, SHIFT_8>(ui16, MASK_16_BYTE_1));
    put(m_bits.shift_right<uint16_t, SHIFT_0>(ui16, MASK_16_BYTE_0));
#endif
  }

  /****************************************************************************
   * Put int16_t value in buffer at index (n).
   */
  void buffer::put_int16 (int32_t n, int16_t value) {
    index(n);
    put_int16(value);
  }

  /****************************************************************************
   * Put int32_t value in buffer at index ().
   */
  void buffer::put_int32 (int32_t value) {
    uint32_t ui32 = m_bits.mutate<uint32_t, int32_t>(value);
#if (BYTE_ORDER == BIG_ENDIAN)
    put(m_bits.shift_right<uint32_t, SHIFT_0>(ui32, MASK_32_BYTE_0));
    put(m_bits.shift_right<uint32_t, SHIFT_8>(ui32, MASK_32_BYTE_1));
    put(m_bits.shift_right<uint32_t, SHIFT_16>(ui32, MASK_32_BYTE_2));
    put(m_bits.shift_right<uint32_t, SHIFT_24>(ui32, MASK_32_BYTE_3));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    put(m_bits.shift_right<uint32_t, SHIFT_24>(ui32, MASK_32_BYTE_3));
    put(m_bits.shift_right<uint32_t, SHIFT_16>(ui32, MASK_32_BYTE_2));
    put(m_bits.shift_right<uint32_t, SHIFT_8>(ui32, MASK_32_BYTE_1));
    put(m_bits.shift_right<uint32_t, SHIFT_0>(ui32, MASK_32_BYTE_0));
#endif
  }

  /****************************************************************************
   * Put int32_t value in buffer at index (n).
   */
  void buffer::put_int32 (int32_t n, int32_t value) {
    index(n);
    put_int32(value);
  }

  /****************************************************************************
   * Put int64_t value in buffer at index ().
   */
  void buffer::put_int64 (int64_t value) {
    uint64_t ui64 = m_bits.mutate<uint64_t, int64_t>(value);
#if (BYTE_ORDER == BIG_ENDIAN)
    put(m_bits.shift_right<uint64_t, SHIFT_0>(ui64, MASK_64_BYTE_0));
    put(m_bits.shift_right<uint64_t, SHIFT_8>(ui64, MASK_64_BYTE_1));
    put(m_bits.shift_right<uint64_t, SHIFT_16>(ui64, MASK_64_BYTE_2));
    put(m_bits.shift_right<uint64_t, SHIFT_24>(ui64, MASK_64_BYTE_3));
    put(m_bits.shift_right<uint64_t, SHIFT_32>(ui64, MASK_64_BYTE_4));
    put(m_bits.shift_right<uint64_t, SHIFT_40>(ui64, MASK_64_BYTE_5));
    put(m_bits.shift_right<uint64_t, SHIFT_48>(ui64, MASK_64_BYTE_6));
    put(m_bits.shift_right<uint64_t, SHIFT_56>(ui64, MASK_64_BYTE_7));
#elif (BYTE_ORDER == LITTLE_ENDIAN)
    put(m_bits.shift_right<uint64_t, SHIFT_56>(ui64, MASK_64_BYTE_7));
    put(m_bits.shift_right<uint64_t, SHIFT_48>(ui64, MASK_64_BYTE_6));
    put(m_bits.shift_right<uint64_t, SHIFT_40>(ui64, MASK_64_BYTE_5));
    put(m_bits.shift_right<uint64_t, SHIFT_32>(ui64, MASK_64_BYTE_4));
    put(m_bits.shift_right<uint64_t, SHIFT_24>(ui64, MASK_64_BYTE_3));
    put(m_bits.shift_right<uint64_t, SHIFT_16>(ui64, MASK_64_BYTE_2));
    put(m_bits.shift_right<uint64_t, SHIFT_8>(ui64, MASK_64_BYTE_1));
    put(m_bits.shift_right<uint64_t, SHIFT_0>(ui64, MASK_64_BYTE_0));
#endif
  }

  /****************************************************************************
   * Put int64_t value in buffer at index (n).
   */
  void buffer::put_int64 (int32_t n, int64_t value) {
    index(n);
    put_int64 (value);
  }

  /****************************************************************************
   * Put varchar value in buffer at index ().
   */
  void buffer::put_varchar (std::string const& value) {
    for (char c : value) put_char(c);
  }

  /****************************************************************************
   * Put varchar value in buffer at index (n).
   */
  void buffer::put_varchar (int32_t n, std::string const& value) {
    index(n);
    put_varchar(value);
  }

} // namespace


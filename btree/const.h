#ifndef CONST_H
#define CONST_H  1

#include <limits>

namespace red::tree {

  // buffer size

  constexpr int32_t BUFFER_SZ             = 32768;

  // node header size

  constexpr int32_t NODE_HDR_SZ           = 24;

  // node header field sizes

  constexpr int32_t IS_LEAF_SZ            = sizeof (int8_t);
  constexpr int32_t NUM_KEY_SZ            = sizeof (int16_t);
  constexpr int32_t PREV_SIBLING_SZ       = sizeof (int64_t);
  constexpr int32_t NEXT_SIBLING_SZ       = sizeof (int64_t);

  // node header field positions

  constexpr int32_t IS_LEAF_POS           = 0;
  constexpr int32_t NUM_KEY_POS           = 2;
  constexpr int32_t PREV_SIBLING_POS      = 4;
  constexpr int32_t NEXT_SIBLING_POS      = 12;

  // node vector position

  constexpr int32_t ELEMENT_VECTOR_POS    = NODE_HDR_SZ;

  // vector element sizes

  constexpr int32_t CHILD_OFFSET_SZ       = sizeof (int64_t);
  constexpr int32_t BLOCK_OFFSET_SZ       = sizeof (int64_t);

  // vector element positions

  constexpr int32_t CHILD_OFFSET_POS      = 0;
  constexpr int32_t BLOCK_OFFSET_POS      = 0;
  constexpr int32_t KEY_POS               = 8;

  // data header size

  constexpr int32_t DATA_HDR_SZ           = 24;

  // data header field sizes

  constexpr int32_t REM_SZ                = sizeof (int32_t);
//constexpr int32_t PREV_SIBLING_SZ       = sizeof (int64_t);
//constexpr int32_t NEXT_SIBLING_SZ       = sizeof (int64_t);
  constexpr int32_t ROW_HEAD_SZ           = sizeof (int16_t);
  constexpr int32_t ROW_TAIL_SZ           = sizeof (int16_t);

  // data header field positions

  constexpr int32_t REM_POS               = 0;
//constexpr int32_t PREV_SIBLING_POS      = 4;
//constexpr int32_t NEXT_SIBLING_POS      = 12;
  constexpr int32_t ROW_HEAD_POS          = 20;
  constexpr int32_t ROW_TAIL_POS          = 22;

  // data row field sizes

  constexpr int32_t ROW_SIZE_SZ           = sizeof (int16_t);
  constexpr int32_t ROW_PREV_SZ           = sizeof (int16_t);
  constexpr int32_t ROW_NEXT_SZ           = sizeof (int16_t);
  constexpr int32_t COLUMN_SIZE_SZ        = sizeof (int8_t);

  // data row field positions

  constexpr int32_t ROW_SIZE_POS          = 0;
  constexpr int32_t ROW_PREV_POS          = 2;
  constexpr int32_t ROW_NEXT_POS          = 4;
  constexpr int32_t COLUMN_SIZE_POS       = 6;

  // invalid sibling reference (offset)

  constexpr int64_t INVALID_SIBLING_REF   = std::numeric_limits<int64_t>::min();

} // namespace

#endif  // CONST_H


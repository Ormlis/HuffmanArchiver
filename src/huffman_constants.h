#pragma once

#include <cstdint>
#include <limits.h>

namespace huffman {

using DEFAULT_CHAR_TYPE = uint16_t;  // NOLINT
inline const DEFAULT_CHAR_TYPE FILENAME_END = 256;
inline const DEFAULT_CHAR_TYPE ONE_MORE_FILE = 257;
inline const DEFAULT_CHAR_TYPE ARCHIVE_END = 258;
inline const size_t DEFAULT_IN_CHAR_SIZE = CHAR_BIT;
inline const size_t DEFAULT_OUT_CHAR_SIZE = 9;

};  // namespace huffman
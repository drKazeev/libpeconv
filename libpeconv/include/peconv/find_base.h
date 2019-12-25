#pragma once

#include <Windows.h>

namespace peconv {
    ULONGLONG find_base(BYTE *buf, size_t buf_size);
};

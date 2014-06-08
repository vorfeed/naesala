#pragma once

namespace naesala {

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

} // namespace naesala

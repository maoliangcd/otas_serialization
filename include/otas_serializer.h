#include "otas_binary.h"

namespace otas_serializer {

#define serialize(t, s) \
[](auto &&t, auto &&s) -> auto { \
    std::size_t offset{}; \
    serialize_helper<remove_cvref_t<decltype(t)>>::serialize_template(t, s, offset); \
}(t, s) \

#define deserialize(s, t) \
[](auto &&s, auto &&t) -> auto {\
    std::size_t offset{}; \
    deserialize_helper<remove_cvref_t<decltype(t)>>::deserialize_template(s, t, offset); \
}(s, t) \

}
#pragma once
#include <string>

namespace Helpers {
template <typename T>
class Element {
   public:
    virtual ~Element() = default;
    virtual void accept(T& visitor) = 0;
};

template <typename T>
class Visitor {
   public:
    virtual ~Visitor() = default;
    virtual void visit(T* element) = 0;
};
}  // namespace Helpers

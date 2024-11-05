#pragma once
#include <string>
#include "id_interface.hpp"

namespace Helpers {

class VisitorRoot {
   public:
    virtual ~VisitorRoot() = default;
};

class IElement : public IId {
   public:
    virtual ~IElement() = default;
    virtual void accept(VisitorRoot* visitor) = 0;
};

class VisitorBase : public VisitorRoot, public IId {
   public:
    virtual ~VisitorBase() = default;
    virtual void visit(IElement* element) = 0;
};

}  // namespace Helpers

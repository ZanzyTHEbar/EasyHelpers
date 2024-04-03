#include <Arduino.h>
#include <EasyHelpers.h>
#include <helpers/id_interface.hpp>

//! Very basic example of the Visitor pattern.

// Forward declaration of concrete elements
template <typename T>
class ConcreteVisitor : public Helpers::VisitorBase {
   public:
    void visit(Helpers::IElement* element) override {
        // Downcast to specific type if necessary. Safety checks should be
        // added.
        T* specificElement = dynamic_cast<T*>(element);
        if (specificElement) {
            // Now you can work with the specific type
        }
    }
};

// Example concrete Element class
class ConcreteElement : public Helpers::IElement {
   public:
    void accept(Helpers::VisitorRoot* visitor) override {
        Helpers::VisitorBase* visitorBase =
            dynamic_cast<Helpers::VisitorBase*>(visitor);
        if (visitorBase) {
            visitorBase->visit(this);
        }
        // Alternatively, use a static_cast if you're sure about the types
        // involved, but dynamic_cast provides type safety at runtime.
    }
};

void ClientCode(std::array<Helpers::IElement*, 2> components,
                Helpers::VisitorRoot* visitor) {
    // ...
    for (Helpers::IElement* comp : components) {
        comp->accept(visitor);
    }
    // ...
}

void setup() {
    Serial.begin(115200);
    pinMode(4, OUTPUT);

    digitalWrite(4, HIGH);

    delay(1000);

    std::array<Helpers::IElement*, 2> components = {new ConcreteElement};
    std::cout
        << "The client code works with all visitors via the base VisitorBase "
           "interface:\n";
    ConcreteVisitor<int>* visitor = new ConcreteVisitor<int>();
    ClientCode(components, visitor);
    std::cout << "\n";
    std::cout << "It allows the same client code to work with different types "
                 "of visitors:\n";

    for (const Helpers::IElement* comp : components) {
        delete comp;
    }
    delete visitor;
}

void loop() {}
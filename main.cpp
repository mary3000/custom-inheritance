#include <iostream>
#include <unordered_map>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::pair<bool, std::function<void(void*)>> vfun;
typedef std::unordered_map<std::string, vfun> vtable;

#define VIRTUAL_CLASS(base) \
vtable v##base; \
struct base { \
    std::string name_ = #base; \
    vtable* vptr = &v##base; \
    char base_;

#define END(base) \
};

#define VIRTUAL_CLASS_DERIVED(derived, base) \
vtable v##derived; \
struct derived { \
    std::string name_ = #derived; \
    vtable* vptr = &v##derived; \
    base base_;

#define END_DERIVE(derived, base) };

#define METHODS(base) \
static void init() { \

#define METHODS_END(base) \
}

#define METHODS_DERIVED(derived, base) \
static void init() { \
    for (auto& e : v##base) { \
        v##derived[e.first] = {false, e.second.second}; \
    }

#define METHODS_DERIVED_END(derive, base) \
}

#define DECLARE_METHOD(clazz, method) \
v##clazz[#method] = {true, [](void* obj) { \
    auto this_ = reinterpret_cast<clazz*>(obj);

#define DECLARE_METHOD_END(clazz, method) \
}};

#define VIRTUAL_CALL(obj, method) \
if ((obj)->vptr->find(#method) == (obj)->vptr->end()) { \
    throw std::invalid_argument("error: class " + (obj)->name_ + " doesn't have method " + #method); \
} \
std::cout << (obj)->name_ << "::" << #method << std::endl; \
(obj)->vptr->at(#method).first ? (obj)->vptr->at(#method).second(obj) : (obj)->vptr->at(#method).second(&(obj)->base_);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VIRTUAL_CLASS(Base)
    int base_var;
    METHODS(Base)

        DECLARE_METHOD(Base, Both)
                    std::cout << "both: " << this_->base_var << std::endl;
                    this_->base_var++;
                    std::cout << std::endl;
        DECLARE_METHOD_END(Base, Both)

        DECLARE_METHOD(Base, BaseOnly)
                    std::cout << "base only: " << this_->base_var << std::endl;
                    this_->base_var++;
                    std::cout << std::endl;
        DECLARE_METHOD_END(Base, BaseOnly)

    METHODS_END(Base)
END(Base)

VIRTUAL_CLASS_DERIVED(Derived, Base)
    int derived_var;
    METHODS_DERIVED(Derived, Base)

        DECLARE_METHOD(Derived, Both)
                    std::cout << "both: " << this_->derived_var << std::endl;
                    this_->derived_var++;
                    std::cout << std::endl;
        DECLARE_METHOD_END(Derived, Both)

        DECLARE_METHOD(Derived, DerivedOnly)
                    std::cout<< "derived only: " << this_->derived_var << std::endl;
                    this_->derived_var++;
                    std::cout << std::endl;
        DECLARE_METHOD_END(Derived, DerivedOnly)

    METHODS_DERIVED_END(Derived, Base)
END_DERIVE(Derived, Base)

int main() {
    Base::init();
    Derived::init();

    Base b;
    b.base_var = 10;
    Derived d;
    d.derived_var = 100;
    d.base_.base_var = 1000;
    auto d_ptr = reinterpret_cast<Base*>(&d);

    VIRTUAL_CALL(&b, Both)
    VIRTUAL_CALL(&b, BaseOnly)

    VIRTUAL_CALL(&d, Both)
    VIRTUAL_CALL(&d, BaseOnly)
    VIRTUAL_CALL(&d, DerivedOnly)

    VIRTUAL_CALL(d_ptr, Both)
    VIRTUAL_CALL(d_ptr, BaseOnly)
    VIRTUAL_CALL(d_ptr, DerivedOnly)

    try {
        VIRTUAL_CALL(&b, DerivedOnly)
    } catch (std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
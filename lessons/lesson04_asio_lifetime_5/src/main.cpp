#include <iostream>
#include <memory>

struct S {
    S() {
        std::cout << "S constructed\n";
    }

    ~S() {
        std::cout << "S destroyed\n";
    }
};

int main() {
    std::cout << "Create sp1 with make_shared\n";
    auto sp1 = std::make_shared<S>();

    std::cout << "sp1.get() = " << sp1.get() << '\n';

    std::cout << "Create sp2 from raw pointer sp1.get()\n";
    std::shared_ptr<S> sp2{sp1.get()};

    std::cout << "sp2.get() = " << sp2.get() << '\n';

    std::cout << "Both smart pointers point to the same object address,\n";
    std::cout << "but they do NOT share the same control block\n";

    std::cout << "Leaving main...\n";
}

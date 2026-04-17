#include <iostream>

int main() {
    std::cout << "Demo 2: overview of async HTTP server architecture" << std::endl;
    std::cout << "Current stage: theory only" << std::endl;
    std::cout << "Components:" << std::endl;
    std::cout << " - http_server::Listener" << std::endl;
    std::cout << " - http_server::SessionBase" << std::endl;
    std::cout << " - http_server::Session<RequestHandler>" << std::endl;
    std::cout << " - RequestHandler" << std::endl;
    std::cout << "Flow:" << std::endl;
    std::cout << " accept connection -> create session -> read request -> "
                 "handle request -> send response"
              << std::endl;
}

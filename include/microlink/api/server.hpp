#pragma once

#include <cstdint>

#include "microlink/application/application.hpp"

namespace microlink::api {

class Server {
public:
    explicit Server(
        application::Application& application
    );

    void run(std::uint16_t port);

private:
    application::Application& application_;
};

} // namespace microlink::api
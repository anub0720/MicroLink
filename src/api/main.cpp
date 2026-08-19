#include "microlink/api/server.hpp"

int main() {
    microlink::application::Application application;

    microlink::api::Server server(application);

    server.run(18080);

    return 0;
}
#include "microlink/api/server.hpp"

#include <crow.h>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace microlink::api
{

    namespace
    {

        struct CorsMiddleware
        {
            struct context
            {
            };

            void before_handle(
                crow::request &,
                crow::response &,
                context &)
            {
            }

            void after_handle(
                crow::request &,
                crow::response &response,
                context &)
            {
                response.set_header(
                    "Access-Control-Allow-Origin",
                    "http://localhost:5173");

                response.set_header(
                    "Access-Control-Allow-Methods",
                    "GET, POST, OPTIONS");

                response.set_header(
                    "Access-Control-Allow-Headers",
                    "Content-Type");
            }
        };

        void add_device_state(
            crow::json::wvalue &json,
            const application::Application &application)
        {
            json["gpio"] =
                static_cast<int>(application.gpio());

            json["gpio_direction"] =
                static_cast<int>(
                    application.gpio_direction());

            json["uart"] =
                application.uart_output();

            json["timer"] =
                static_cast<int>(
                    application.timer_counter());

            json["timer_compare"] =
                static_cast<int>(
                    application.timer_compare());

            json["timer_enabled"] =
                application.timer_enabled();

            json["timer_interrupt_pending"] =
                application.timer_interrupt_pending();
        }

    } // namespace

    Server::Server(
        application::Application &application)
        : application_(application)
    {
    }

    void Server::run(std::uint16_t port)
    {
        crow::App<CorsMiddleware> app;

        CROW_ROUTE(app, "/api/state")
        ([this]
         {
        const auto& state =
            application_.cpu_state();

        crow::json::wvalue json;

        json["pc"] =
            static_cast<int>(
                state.program_counter()
            );

        json["accumulator"] =
            static_cast<int>(
                state.accumulator()
            );

        json["stack_pointer"] =
            static_cast<int>(
                state.stack_pointer()
            );
        json["instruction_register"] =
            static_cast<int>(
                state.instruction_register()
        );
        json["halted"] =
            state.halted();

        add_device_state(
            json,
            application_
        );

        return json; });

        CROW_ROUTE(app, "/api/trace")
        ([this]
         {
        crow::json::wvalue result;

        const auto& trace =
            application_.trace();

        result["count"] =
            static_cast<int>(trace.size());

        auto& entries =
            result["entries"];

        for (std::size_t i = 0;
             i < trace.size();
             ++i) {

            const auto& event = trace[i];

            entries[i]["cycle"] =
                static_cast<int>(
                    event.cycle
                );

            entries[i]["pc_before"] =
                static_cast<int>(
                    event.program_counter_before
                );

            entries[i]["pc_after"] =
                static_cast<int>(
                    event.program_counter_after
                );

            entries[i]["accumulator_before"] =
                static_cast<int>(
                    event.accumulator_before
                );

            entries[i]["accumulator_after"] =
                static_cast<int>(
                    event.accumulator_after
                );

            entries[i]["opcode"] =
                static_cast<int>(
                    cpu::to_byte(
                        event.instruction.opcode()
                    )
                );
        }

        return result; });

        CROW_ROUTE(app, "/api/step")
            .methods(crow::HTTPMethod::POST)([this]
                                             {
        try {
            application_.step();

            crow::json::wvalue json;

            json["pc"] =
                static_cast<int>(
                    application_
                        .cpu_state()
                        .program_counter()
                );

            json["cycles"] =
                static_cast<int>(
                    application_
                        .trace()
                        .size()
                );

            add_device_state(
                json,
                application_
            );

            return crow::response(json);
        }
        catch (const std::exception& exception) {
            crow::json::wvalue json;

            json["error"] = exception.what();

            return crow::response(400, json);
        } });

        CROW_ROUTE(app, "/api/run")
            .methods(crow::HTTPMethod::POST)([this](const crow::request &request)
                                             {
        std::size_t cycle_limit = 1000;

        const auto body =
            crow::json::load(request.body);

        if (body && body.has("cycle_limit")) {
            const int requested_limit =
                body["cycle_limit"].i();

            if (requested_limit <= 0) {
                return crow::response(
                    400,
                    "cycle_limit must be positive"
                );
            }

            cycle_limit =
                static_cast<std::size_t>(
                    requested_limit
                );
        }

        try {
            const std::size_t executed =
                application_.run(
                    cycle_limit
                );

            crow::json::wvalue json;

            json["status"] =
                application_
                    .cpu_state()
                    .halted()
                    ? "halted"
                    : "cycle_limit";

            json["cycles"] =
                static_cast<int>(executed);

            json["pc"] =
                static_cast<int>(
                    application_
                        .cpu_state()
                        .program_counter()
                );

            json["accumulator"] =
                static_cast<int>(
                    application_
                        .cpu_state()
                        .accumulator()
                );

            add_device_state(
                json,
                application_
            );

            return crow::response(json);
        }
        catch (const std::exception& exception) {
            crow::json::wvalue json;

            json["error"] = exception.what();

            return crow::response(400, json);
        } });
        CROW_ROUTE(app, "/api/memory")
.methods(crow::HTTPMethod::GET)
([this] {
    crow::json::wvalue json;

    const auto& cpu =
        application_.cpu_state();

    const auto& memory =
        application_.memory();

    const cpu::Word start =
        cpu.program_counter();

    constexpr std::size_t bytes_to_show = 64;

    const std::size_t start_address =
        static_cast<std::size_t>(start);

    const std::size_t available =
        cpu::Memory::Size - start_address;

    const std::size_t count =
        std::min(
            bytes_to_show,
            available
        );

    json["start_address"] =
        static_cast<int>(start);

    auto& bytes = json["bytes"];

    for (std::size_t i = 0; i < count; ++i) {
        bytes[i] =
            static_cast<int>(
                memory.read(
                    static_cast<cpu::Word>(
                        start_address + i
                    )
                )
            );
    }

    return json;
});

        CROW_ROUTE(app, "/api/reset")
            .methods(crow::HTTPMethod::POST)([this]
                                             {
        application_.reset();

        crow::json::wvalue json;

        json["status"] = "reset";

        return crow::response(json); });

        CROW_ROUTE(app, "/api/load")
            .methods(crow::HTTPMethod::POST)([this](const crow::request &request)
                                             {
        const auto body =
            crow::json::load(request.body);

        if (!body) {
            return crow::response(
                400,
                "Invalid JSON"
            );
        }

        if (!body.has("start_address") ||
            !body.has("program")) {
            return crow::response(
                400,
                "Missing start_address or program"
            );
        }

        std::vector<cpu::Byte> program;

        for (const auto& value :
             body["program"]) {

            const int byte =
                value.i();

            if (byte < 0 || byte > 255) {
                return crow::response(
                    400,
                    "Program byte must be 0..255"
                );
            }

            program.push_back(
                static_cast<cpu::Byte>(
                    byte
                )
            );
        }

        const int requested_start =
            body["start_address"].i();

        if (
            requested_start < 0 ||
            requested_start > 65535
        ) {
            return crow::response(
                400,
                "start_address must be 0..65535"
            );
        }

        const cpu::Word start_address =
            static_cast<cpu::Word>(
                requested_start
            );

        try {
            application_.load_program(
                program,
                start_address
            );

            crow::json::wvalue json;

            json["status"] = "loaded";

            json["start_address"] =
                static_cast<int>(
                    start_address
                );

            json["program_size"] =
                static_cast<int>(
                    program.size()
                );

            add_device_state(
                json,
                application_
            );

            return crow::response(json);
        }
        catch (const std::exception& exception) {
            crow::json::wvalue json;

            json["error"] = exception.what();

            return crow::response(
                400,
                json
            );
        } });

        app.port(port)
            .multithreaded()
            .run();
    }

} // namespace microlink::api
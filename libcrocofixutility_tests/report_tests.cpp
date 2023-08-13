#include <catch.hpp>
#include <libcrocofixutility/report.hpp>
#include <sstream>

using namespace crocofix;

TEST_CASE("report", "[report]") {

    SECTION("basic formatting") {

        struct order
        {
            std::string sender_comp_id;
            std::string side;
            std::string symbol;
            long        quantity;
            double      price;
        };

        const std::vector<order> orders =
        {
            { "INITIATOR", "Buy", "PANW", 20000, 255.51 },
            { "INITIATOR", "Sell", "ORCL", 5555, 19.09 },
            { "INITIATOR", "Sell", "ORCL", 2000 },
            { "INITIATOR", "Sell", "", 1000, 19.09 }
        };

        report order_report {
            { "Sender\nCompID" },
            { "Side" },
            { "Symbol" },
            { "Quantity", report::justification::right },
            { "Price", report::justification::right }    
        };

        for (const auto& order : orders) {
            order_report.rows().emplace_back(
                crocofix::report::row {
                    order.sender_comp_id,
                    order.side,
                    order.symbol,
                    std::to_string(order.quantity),
                    std::to_string(order.price)
                }
            );
        }

        // Add a short row
        order_report.rows().emplace_back(
            crocofix::report::row {
                "INITIATOR",
                "Buy",
                "PANW",
                "1000"
            }
        );

        std::ostringstream actual;

        actual << order_report;

        const std::string expected = 
            "---------------------------------------------\n"
            "Sender                                       \n"
            "CompID     Side  Symbol  Quantity       Price\n"
            "---------------------------------------------\n"
            "INITIATOR  Buy   PANW       20000  255.510000\n"
            "INITIATOR  Sell  ORCL        5555   19.090000\n"
            "INITIATOR  Sell  ORCL        2000    0.000000\n"
            "INITIATOR  Sell              1000   19.090000\n"
            "INITIATOR  Buy   PANW        1000            \n"
            "---------------------------------------------\n";

        REQUIRE(actual.str() == expected);
    }

}


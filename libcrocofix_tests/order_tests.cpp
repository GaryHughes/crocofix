#include <catch.hpp>
#include <libcrocofix/order.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using namespace crocofix;

TEST_CASE("Order", "[order]") {

    SECTION("New OrderSingle") 
    {
        const std::string order_single = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(order_single);
        REQUIRE(complete);
        REQUIRE(consumed == order_single.length());

        const crocofix::order order{message};

        REQUIRE(order.BeginString() == "FIX.4.4");
        REQUIRE(order.SenderCompID() == "INITIATOR");
        REQUIRE(order.TargetCompID() == "ACCEPTOR");
        REQUIRE(order.ClOrdID() == "61");
        REQUIRE(order.Side() == crocofix::FIX_5_0SP2::field::Side::Buy);
        REQUIRE(order.OrdStatus() == crocofix::FIX_5_0SP2::field::OrdStatus::PendingNew);
        REQUIRE(order.OrdType() == crocofix::FIX_5_0SP2::field::OrdType::Limit);
        REQUIRE(order.TimeInForce() == crocofix::FIX_5_0SP2::field::TimeInForce::GoodTillCancel);
        REQUIRE(order.OrderQty().value() == "10000");
        REQUIRE(order.Price().value() == "20");
        REQUIRE(order.CumQty().value() == "0");
        REQUIRE(order.AvgPx().value() == "0");

        REQUIRE(order.key() == "INITIATOR-ACCEPTOR-61");
    }
}
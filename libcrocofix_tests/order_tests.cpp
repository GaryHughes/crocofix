#include <catch.hpp>
#include <libcrocofix/order.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using namespace crocofix;

TEST_CASE("Order", "[order]") {

    SECTION("NewOrderSingle") 
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

    SECTION("key_for_message NewOrderSingle")
    {
        const std::string order_single = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(order_single);
        REQUIRE(complete);
        REQUIRE(consumed == order_single.length());

        REQUIRE(crocofix::order::key_for_message(message) == "INITIATOR-ACCEPTOR-61");
        REQUIRE(crocofix::order::key_for_message(message, false) == "INITIATOR-ACCEPTOR-61");
        REQUIRE(crocofix::order::key_for_message(message, true) == "ACCEPTOR-INITIATOR-61");
    }

    SECTION("key_for_message ExecutionReport")
    {
        const std::string report_pending = "8=FIX.4.4\u00019=173\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=71852=20200114-08:13:20.072\u000139=0\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=1\u0001150=0\u0001151=10000\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=021\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(report_pending);
        REQUIRE(complete);
        REQUIRE(consumed == report_pending.length());

        REQUIRE(crocofix::order::key_for_message(message) == "ACCEPTOR-INITIATOR-61");
        REQUIRE(crocofix::order::key_for_message(message, false) == "ACCEPTOR-INITIATOR-61");
        REQUIRE(crocofix::order::key_for_message(message, true) == "INITIATOR-ACCEPTOR-61");
    }

    SECTION("key_for_message OrderCancelRequest")
    {
        const std::string cancel_request = "8=FIX.4.4\u00019=149\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2792\u000152=20200114-09:48:33.082\u000141=62\u000137=INITIATOR-ACCEPTOR-62\u000111=63\u000155=BHP.AX\u000154=1\u000160=20200114-09:48:28.540\u000138=10000\u000110=216\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(cancel_request);
        REQUIRE(complete);
        REQUIRE(consumed == cancel_request.length());

        REQUIRE(crocofix::order::key_for_message(message) == "INITIATOR-ACCEPTOR-62");
        REQUIRE(crocofix::order::key_for_message(message, false) == "INITIATOR-ACCEPTOR-62");
        REQUIRE(crocofix::order::key_for_message(message, true) == "ACCEPTOR-INITIATOR-62");
    }

    SECTION("key_for_message OrderCancelReplaceRequest")
    {
        const std::string cancel_replace_request = "8=FIX.4.4\u00019=180\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2799\u000152=20200114-09:50:03.498\u000137=INITIATOR-ACCEPTOR-66\u000141=66\u000111=67\u000170=64\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-09:49:57.988\u000138=20000\u000140=2\u000144=20\u000159=1\u000110=204\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(cancel_replace_request);
        REQUIRE(complete);
        REQUIRE(consumed == cancel_replace_request.length());

        REQUIRE(crocofix::order::key_for_message(message) == "INITIATOR-ACCEPTOR-66");
        REQUIRE(crocofix::order::key_for_message(message, false) == "INITIATOR-ACCEPTOR-66");
        REQUIRE(crocofix::order::key_for_message(message, true) == "ACCEPTOR-INITIATOR-66");
    }
}
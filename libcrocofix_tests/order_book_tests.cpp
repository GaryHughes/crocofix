#include <catch.hpp>
#include <libcrocofix/order_book.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

#include <iostream>

using namespace crocofix;

namespace
{

crocofix::message decode_message(const std::string& text)
{
    crocofix::message message;
    auto [consumed, complete] = message.decode(text);
    REQUIRE(complete);
    REQUIRE(consumed == text.length());
    return message;
}

}

TEST_CASE("OrderBook", "[order_book]") { // NOLINT(readability-function-cognitive-complexity)

    SECTION("MessageWithNoMsgTypeIgnored") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        const auto message = decode_message(text);
        order_book book;
        auto [processed, reason] = book.process(message);
        REQUIRE(reason == "message does not have a MsgType field");
        REQUIRE_FALSE(processed);
    }

    SECTION("MessageWithUnsupportedMsgTypeIgnored")
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=S\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        const auto message = decode_message(text);
        order_book book;
        auto [processed, reason] = book.process(message);
        REQUIRE(reason == "unsupported MsgType = S");
        REQUIRE_FALSE(processed);
    }

    SECTION("NewOrderSingleInserted") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        const auto message = decode_message(text);
        order_book book;
        auto [processed, reason] = book.process(message);
        REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
        REQUIRE(processed);
        REQUIRE(book.orders().size() == 1);
        const crocofix::order& order = book.orders().begin()->second;
        REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::PendingNew);
        REQUIRE(order.OrderQty().value() == "10000");
    }


    SECTION("NewOrderSingleFilled")
    {
        const std::array<std::string, 4> messages = {
            "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001",
            "8=FIX.4.4\u00019=173\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=71852=20200114-08:13:20.072\u000139=0\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=1\u0001150=0\u0001151=10000\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=021\u0001",
            "8=FIX.4.4\u00019=187\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=719\u000152=20200114-08:13:20.072\u000139=1\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=2\u0001150=1\u0001151=893\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=9107\u000131=20\u000114=9107\u00016=20\u000130=AUTO\u000140=2\u000110=081\u0001",
            "8=FIX.4.4\u00019=185\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=720\u000152=20200114-08:13:20.072\u000139=2\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=3\u0001150=2\u0001151=0\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=893\u000131=20\u000114=10000\u00016=20\u000130=AUTO\u000140=2\u000110=201\u0001"
        };
        
        order_book book;
        size_t index = 0;

        for (const auto& text : messages) {

            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            REQUIRE(book.orders().size() == 1);
            const crocofix::order& order = book.orders().begin()->second;

            switch (index)
            {
                case 0:
                    REQUIRE(order.SenderCompID() == "INITIATOR");
                    REQUIRE(order.TargetCompID() == "ACCEPTOR");
                    REQUIRE(order.ClOrdID() == "61");
                    REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::PendingNew);
                    REQUIRE(order.Side() == FIX_5_0SP2::field::Side::Buy);
                    REQUIRE(order.OrderQty().value() == "10000");
                    REQUIRE(order.Price().value() == "20");
                    break;

                case 1:
                    REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::New);
                    REQUIRE(order.CumQty().value() == "0");
                    REQUIRE(order.AvgPx().value() == "0");
                    break;

                case 2:
                    REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::PartiallyFilled);
                    REQUIRE(order.CumQty().value() == "9107");
                    REQUIRE(order.AvgPx().value() == "20");
                    break;

                case 3:
                    REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::Filled);
                    REQUIRE(order.CumQty().value() == "10000");
                    REQUIRE(order.AvgPx().value() == "20");
                    break;
            }

            ++index;
        }
    }

    SECTION("OrderCancelRequestForKnownOrder")
    {
        auto messages = std::array<std::string, 2> {
            "8=FIXT.1.1\u00019=138\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=8\u000152=20200114-21:43:31.260\u000111=2\u000170=1\u000155=WTF.AX54=1\u000160=20200114-21:41:07.549\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=000\u0001",
            "8=FIXT.1.1\u00019=124\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=9\u000152=20200114-21:43:47.154\u000141=2\u000137=2\u000111=3\u000155=WTF.AX\u000154=1\u000160=20200114-21:43:45.733\u000138=20000\u000110=142\u0001"
        };

        order_book book;

        for (const auto& text : messages) {
            std::cout << text << std::endl;
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
        }

        REQUIRE(book.orders().size() == 1);
        const crocofix::order& order = book.orders().begin()->second;
        REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::PendingCancel);
    }

    SECTION("OrderCancelRequestForUnknownOrder")
    {
        const std::string order_single_text = "8=FIXT.1.1\u00019=138\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=8\u000152=20200114-21:43:31.260\u000111=2\u000170=1\u000155=WTF.AX54=1\u000160=20200114-21:41:07.549\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=000\u0001";
        const std::string order_cancel_request_text = "8=FIXT.1.1\u00019=124\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=9\u000152=20200114-21:43:47.154\u000141=666\u000137=2\u000111=3\u000155=WTF.AX\u000154=1\u000160=20200114-21:43:45.733\u000138=20000\u000110=142\u0001";

        order_book book;

        const auto order_single = decode_message(order_single_text);
        auto [order_single_processed, order_single_reason] = book.process(order_single);
        REQUIRE(order_single_reason == ""); // NOLINT(readability-container-size-empty)
        REQUIRE(order_single_processed);

        const auto order_cancel_request = decode_message(order_cancel_request_text);
        auto [order_cancel_request_processed, order_cancel_request_reason] = book.process(order_cancel_request);
        REQUIRE(order_cancel_request_reason == "order book does not contain an order with key = INITIATOR-ACCEPTOR-666");
        REQUIRE_FALSE(order_cancel_request_processed);
    }

    SECTION("OrderCancelReplaceRequestForKnownOrder")
    {
        auto messages = std::array<std::string, 3> {
            "8=FIXT.1.1\u00019=140\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=580\u000152=20200115-02:29:00.467\u000111=4\u000170=3\u000155=WTF.AX\u000154=1\u000160=20200115-02:28:56.576\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=116\u0001",
            "8=FIXT.1.1\u00019=166\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=579\u000152=20200115-02:29:05.092\u000137=3\u000111=4\u000117=3\u0001150=0\u000139=0\u000155=WTF.AX\u000154=1\u000138=20000\u000132=0\u000131=0\u0001151=20000\u000114=0\u00016=0\u000160=20200115-02:29:03.202\u000110=173\u0001",
            "8=FIXT.1.1\u00019=150\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=582\u000152=20200115-02:29:23.468\u000137=3\u000141=4\u000111=5\u000170=3\u000155=WTF.AX\u000154=1\u000160=20200115-02:29:14.339\u000138=30000\u000140=2\u000144=11.56\u000159=1\u000110=044\u0001"
        };

        order_book book;

        for (const auto& text : messages) {
            std::cout << text << std::endl;
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
        }

        REQUIRE(book.orders().size() == 1);
        const crocofix::order& order = book.orders().begin()->second;
        REQUIRE(order.OrdStatus() == FIX_5_0SP2::field::OrdStatus::PendingReplace);
        REQUIRE(order.Price().value() == "11.56");
        REQUIRE(order.OrderQty().value() == "30000");
    }

    SECTION("OrderCancelReplaceRequestForUnknownOrder")
    {
        const std::string order_single_text = "8=FIXT.1.1\u00019=140\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=580\u000152=20200115-02:29:00.467\u000111=4\u000170=3\u000155=WTF.AX\u000154=1\u000160=20200115-02:28:56.576\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=116\u0001";
        const std::string order_cancel_replace_request_text = "8=FIXT.1.1\u00019=150\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=582\u000152=20200115-02:29:23.468\u000137=3\u000141=666\u000111=5\u000170=3\u000155=WTF.AX\u000154=1\u000160=20200115-02:29:14.339\u000138=30000\u000140=2\u000144=11.56\u000159=1\u000110=044\u0001";

        order_book book;

        const auto order_single = decode_message(order_single_text);
        auto [order_single_processed, order_single_reason] = book.process(order_single);
        REQUIRE(order_single_reason == ""); // NOLINT(readability-container-size-empty)
        REQUIRE(order_single_processed);

        const auto order_cancel_replace_request = decode_message(order_cancel_replace_request_text);
        auto [order_cancel_request_replace_processed, order_cancel_replace_request_reason] = book.process(order_cancel_replace_request);
        REQUIRE(order_cancel_replace_request_reason == "order book does not contain an order with key = INITIATOR-ACCEPTOR-666");
        REQUIRE_FALSE(order_cancel_request_replace_processed);
    }
}
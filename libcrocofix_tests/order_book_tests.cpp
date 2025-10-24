#include <catch2/catch_all.hpp>
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

    SECTION("DefaultBook") {
        auto book = order_book();
        REQUIRE(book.orders().empty());
    }

    SECTION("UnknownExecutionReport") {
        auto book = order_book();
        auto message = decode_message("8=FIX.4.4\u00019=164\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=232\u000152=20190929-04:51:06.981\u000139=0\u000111=51\u000137=INITIATOR-ACCEPTOR-51\u000117=2\u0001150=0\u0001151=10000\u000155=WTF\u000154=1\u000138=10000\u000132=0\u000131=0\u000114=0\u00016=0\u000140=1\u000110=115\u0001");
        auto [processed, reason] = book.process(message);
        REQUIRE_FALSE(processed);
    }

    SECTION("OrderCancelReplaceRequestForUnknownOrder") {
        auto book = order_book();
        auto message = decode_message("8=FIX.4.4\u00019=178\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2536\u000152=20191117-01:01:47.010\u000137=INITIATOR-ACCEPTOR-56\u000141=56\u000111=57\u000170=55\u0001100=AUTO\u000155=WTF\u000154=1\u000160=20191117-01:01:38.158\u000138=100000\u000140=2\u000144=11\u000159=0\u000110=035\u0001");
        auto [processed, reason] = book.process(message);
        REQUIRE_FALSE(processed);
    }

    SECTION("OrderCancelRequestForUnknownOrder") {
        auto book = order_book();
        auto message = decode_message("8=FIX.4.4\u00019=147\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2544\u000152=20191117-01:09:11.302\u000141=58\u000137=INITIATOR-ACCEPTOR-58\u000111=59\u000155=WTF\u000154=1\u000160=20191117-01:09:09.139\u000138=100000\u000110=092\u0001");
        auto [processed, reason] = book.process(message);
        REQUIRE_FALSE(processed);
    }

    SECTION("OrderCancelRejectForUnknownOrder") {
        auto book = order_book();
        auto message = decode_message("8=FIX.4.4\u00019=127\u000135=9\u000149=ACCEPTOR\u000156=INITIATOR\u000134=511\u000152=20191117-01:11:06.578\u000137=INITIATOR-ACCEPTOR-58\u000139=8\u000141=58\u0001434=1\u000111=59\u000158=Unknown order\u000110=208\u0001");
        auto [processed, reason] = book.process(message);
        REQUIRE_FALSE(processed);
    }

    SECTION("NewOrderSingleAcknowledged") {
        const std::array<std::string, 2> messages = {
            "8=FIX.4.4\u00019=140\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2283\u000152=20190929-04:51:06.973\u000111=51\u000170=50\u0001100=AUTO\u000155=WTF\u000154=1\u000160=20190929-04:35:33.562\u000138=10000\u000140=1\u000159=1\u000110=127\u0001",
            "8=FIX.4.4\u00019=164\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=232\u000152=20190929-04:51:06.981\u000139=0\u000111=51\u000137=INITIATOR-ACCEPTOR-51\u000117=2\u0001150=0\u0001151=10000\u000155=WTF\u000154=1\u000138=10000\u000132=0\u000131=0\u000114=0\u00016=0\u000140=1\u000110=115\u0001"
        };
        auto book = order_book();
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
        }
        REQUIRE(book.orders().size() == 1);
    }

    SECTION("Clear") {
        const std::array<std::string, 2> messages = {
            "8=FIX.4.4\u00019=140\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2283\u000152=20190929-04:51:06.973\u000111=51\u000170=50\u0001100=AUTO\u000155=WTF\u000154=1\u000160=20190929-04:35:33.562\u000138=10000\u000140=1\u000159=1\u000110=127\u0001",
            "8=FIX.4.4\u00019=164\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=232\u000152=20190929-04:51:06.981\u000139=0\u000111=51\u000137=INITIATOR-ACCEPTOR-51\u000117=2\u0001150=0\u0001151=10000\u000155=WTF\u000154=1\u000138=10000\u000132=0\u000131=0\u000114=0\u00016=0\u000140=1\u000110=115\u0001"
        };
        auto book = order_book();
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
        }
        REQUIRE(book.orders().size() == 1);
        book.clear();
        REQUIRE(book.orders().empty());
    }

    SECTION("OrderCancelRequestForKnownOrderAccepted") {
        const std::array<std::string, 5> messages = {
            "8=FIXT.1.1\u00019=148\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=24\u000152=20200119-04:43:20.679\u000111=8\u000170=6\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-04:43:18.221\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=081\u0001",
            "8=FIXT.1.1\u00019=173\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=31\u000152=20200119-04:43:35.419\u000139=0\u000111=8\u000137=INITIATOR-ACCEPTOR-8\u000117=1\u0001150=0\u0001151=20000\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=138\u0001",
            "8=FIXT.1.1\u00019=153\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=26\u000152=20200119-04:43:43.562\u000141=8\u000137=INITIATOR-ACCEPTOR-8\u000111=9\u000155=WTF.AX\u000154=1\u000160=20200119-04:43:42.213\u000138=20000\u0001100=AUTO\u000110=056\u0001",
            "8=FIXT.1.1\u00019=178\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=32\u000152=20200119-04:43:43.570\u000139=6\u000111=9\u000137=INITIATOR-ACCEPTOR-8\u000117=2\u0001150=6\u0001151=20000\u000141=8\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=118\u0001",
            "8=FIXT.1.1\u00019=174\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=33\u000152=20200119-04:43:51.214\u000139=4\u000111=9\u000137=INITIATOR-ACCEPTOR-8\u000117=3\u0001150=4\u0001151=0\u000141=8\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=168\u0001"
        };
        auto book = order_book();
        size_t index = 0;
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            switch (index) {
                case 0:
                    REQUIRE(book.orders().size() == 1);
                    break;
                case 1:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    }
                    break;
                case 2:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingCancel);
                    }
                    break;
                case 3:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingCancel);
                    }
                    break;
                case 4:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::Canceled);
                    }
                    break;
                default:
                    FAIL("unexpected number of lines in input: " + std::to_string(index));
                    break;
            }
            ++index;
        }
    }

    SECTION("OrderCancelRequestForKnownOrderRejected") {
        const std::array<std::string, 5> messages = {
            "8=FIXT.1.1\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=31\u000152=20200119-04:45:43.004\u000111=10\u000170=7\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-04:45:40.842\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=117\u0001",
            "8=FIXT.1.1\u00019=175\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=38\u000152=20200119-04:45:46.392\u000139=0\u000111=10\u000137=INITIATOR-ACCEPTOR-10\u000117=4\u0001150=0\u0001151=20000\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=236\u0001",
            "8=FIXT.1.1\u00019=156\u000135=F\u000149=INITIATOR\u000156=ACCEPTOR\u000134=32\u000152=20200119-04:45:53.320\u000141=10\u000137=INITIATOR-ACCEPTOR-10\u000111=11\u000155=WTF.AX\u000154=1\u000160=20200119-04:45:51.569\u000138=20000\u0001100=AUTO\u000110=190\u0001",
            "8=FIXT.1.1\u00019=181\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=39\u000152=20200119-04:45:53.331\u000139=6\u000111=11\u000137=INITIATOR-ACCEPTOR-10\u000117=5\u0001150=6\u0001151=20000\u000141=10\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=243\u0001",
            "8=FIXT.1.1\u00019=128\u000135=9\u000149=ACCEPTOR\u000156=INITIATOR\u000134=41\u000152=20200119-04:46:09.609\u000137=INITIATOR-ACCEPTOR-10\u000139=8\u000141=10\u0001434=1\u000111=11\u000158=Not telling you\u000110=092\u0001"
        };
        auto book = order_book();
        size_t index = 0;
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            switch (index) {
                case 0:
                    REQUIRE(book.orders().size() == 1);
                    break;
                case 1:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    }
                    break;
                case 2:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingCancel);
                    }
                    break;
                case 3:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingCancel);
                    }
                    break;
                case 4:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    }
                    break;
                default:
                    FAIL("unexpected number of lines in input: " + std::to_string(index));
                    break;
                }
                ++index;
            }
    }
   
    SECTION("OrderCancelReplaceRequestForKnownOrderAccepted") {
        const std::array<std::string, 5> messages = {
            "8=FIXT.1.1\u00019=148\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=10\u000152=20200119-02:35:09.990\u000111=1\u000170=1\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-02:30:33.801\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=061\u0001",
            "8=FIXT.1.1\u00019=173\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=10\u000152=20200119-02:35:12.810\u000139=0\u000111=1\u000137=INITIATOR-ACCEPTOR-1\u000117=1\u0001150=0\u0001151=20000\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=110\u0001",
            "8=FIXT.1.1\u00019=178\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=11\u000152=20200119-02:35:32.416\u000137=INITIATOR-ACCEPTOR-1\u000141=1\u000111=2\u000170=1\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-02:35:17.910\u000138=40000\u000140=2\u000144=11.565\u000159=1\u000110=132\u0001",
            "8=FIXT.1.1\u00019=178\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=11\u000152=20200119-02:35:32.434\u000139=E\u000111=2\u000137=INITIATOR-ACCEPTOR-1\u000117=2\u0001150=E\u0001151=20000\u000141=1\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=120\u0001",
            "8=FIXT.1.1\u00019=175\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=12\u000152=20200119-02:35:34.878\u000139=5\u000111=1\u000137=INITIATOR-ACCEPTOR-2\u000117=3\u0001150=5\u0001151=0\u000141=1\u000155=WTF.AX\u000154=1\u000138=40000\u000144=11.565\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=218\u0001"
        };
        auto book = order_book();
        size_t index = 0;
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            switch (index) {
            case 0: // OrderSingle IN
                REQUIRE(book.orders().size() == 1);
                break;
            case 1: // ER - New OUT
                {
                REQUIRE(book.orders().size() == 1);
                const crocofix::order& order = book.orders().begin()->second;
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                REQUIRE(order.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value() == "1");
                REQUIRE_THROWS(order.fields().get(FIX_5_0SP2::field::OrigClOrdID::Tag)); // TODO: try_get kills clang_tidy
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "20000");
                }
                break;
            case 2: // CR Request IN
                {
                REQUIRE(book.orders().size() == 1);
                const crocofix::order& order = book.orders().begin()->second;
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingReplace);
                REQUIRE(order.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value() == "1");
                REQUIRE_THROWS(order.fields().get(FIX_5_0SP2::field::OrigClOrdID::Tag)); // TODO: try_get kills clang_tidy
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "20000");
                REQUIRE(order.pending_fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "40000");
                }
                break;
            case 3: // ER - Pending - OUT
                {
                REQUIRE(book.orders().size() == 1);
                const crocofix::order& order = book.orders().begin()->second;
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingReplace);
                REQUIRE(order.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value() == "1");
                REQUIRE_THROWS(order.fields().get(FIX_5_0SP2::field::OrigClOrdID::Tag)); // TODO: try_get kills clang_tidy
                REQUIRE(order.fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "20000");
                REQUIRE(order.pending_fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "40000");
                }
                break;
            case 4: // ER - Replaced - OUT
                {
                REQUIRE(book.orders().size() == 2);
                const crocofix::order& original = book.orders().begin()->second;
                const crocofix::order& replacement = (++book.orders().begin())->second;
                REQUIRE(original.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::Replaced);
                REQUIRE(original.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value() == "1");
                REQUIRE(original.fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "20000");
                REQUIRE(replacement.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                REQUIRE(replacement.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value() == "2");
                REQUIRE(replacement.fields().get(FIX_5_0SP2::field::OrigClOrdID::Tag).value() == "1");
                REQUIRE(replacement.fields().get(FIX_5_0SP2::field::OrderQty::Tag).value() == "40000");
                }
                break;
            default:
                    FAIL("unexpected number of lines in input: " + std::to_string(index));
                    break;
            }
            ++index;
        }
    }
   
    SECTION("OrderCancelReplaceRequestForKnownOrderRejected") {
        const std::array<std::string, 5> messages = {
            "8=FIXT.1.1\u00019=150\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=48\u000152=20200119-04:52:10.221\u000111=14\u000170=10\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-04:52:08.245\u000138=20000\u000140=2\u000144=11.56\u000159=1\u000110=155\u0001",
            "8=FIXT.1.1\u00019=175\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=57\u000152=20200119-04:52:13.304\u000139=0\u000111=14\u000137=INITIATOR-ACCEPTOR-14\u000117=9\u0001150=0\u0001151=20000\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=235\u0001",
            "8=FIXT.1.1\u00019=189\u000135=G\u000149=INITIATOR\u000156=ACCEPTOR\u000134=50\u000152=20200119-04:52:29.864\u000137=INITIATOR-ACCEPTOR-14\u000141=14\u000111=15\u000170=10\u0001100=AUTO\u000155=WTF.AX\u000154=1\u000160=20200119-04:52:19.840\u000138=35000\u000140=2\u000144=12.10\u000159=1\u000158=Blah\u000110=080\u0001",
            "8=FIXT.1.1\u00019=182\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=59\u000152=20200119-04:52:29.874\u000139=E\u000111=15\u000137=INITIATOR-ACCEPTOR-14\u000117=10\u0001150=E\u0001151=20000\u000141=14\u000155=WTF.AX\u000154=1\u000138=20000\u000144=11.56\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=089\u0001",
            "8=FIXT.1.1\u00019=124\u000135=9\u000149=ACCEPTOR\u000156=INITIATOR\u000134=60\u000152=20200119-04:52:40.220\u000137=INITIATOR-ACCEPTOR-14\u000139=8\u000141=14\u0001434=2\u000111=15\u000158=Not telling\u000110=214\u0001"  
        };
        auto book = order_book();
        size_t index = 0;
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            switch (index) {
                case 0:
                    REQUIRE(book.orders().size() == 1);
                    break;
                case 1:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    }
                    break;
                case 2:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingReplace);
                    }
                    break;
                case 3:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PendingReplace);
                    }
                    break;
                case 4:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    }
                    break;
                default:
                    FAIL("unexpected number of lines in input: " + std::to_string(index));
                    break;
            }
            ++index;
        }
    }
   
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

    SECTION("NewOrderSingleFilled")
    {
        const std::array<std::string, 4> messages = {
            "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001",
            "8=FIX.4.4\u00019=173\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=718\u000152=20200114-08:13:20.072\u000139=0\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=1\u0001150=0\u0001151=10000\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=0\u000131=0\u000114=0\u00016=0\u000140=2\u000110=021\u0001",
            "8=FIX.4.4\u00019=187\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=719\u000152=20200114-08:13:20.072\u000139=1\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=2\u0001150=1\u0001151=893\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=9107\u000131=20\u000114=9107\u00016=20\u000130=AUTO\u000140=2\u000110=081\u0001",
            "8=FIX.4.4\u00019=185\u000135=8\u000149=ACCEPTOR\u000156=INITIATOR\u000134=720\u000152=20200114-08:13:20.072\u000139=2\u000111=61\u000137=INITIATOR-ACCEPTOR-61\u000117=3\u0001150=2\u0001151=0\u000155=BHP.AX\u000154=1\u000138=10000\u000144=20\u000132=893\u000131=20\u000114=10000\u00016=20\u000130=AUTO\u000140=2\u000110=201\u0001"
        };
        auto book = order_book();
        size_t index = 0;
        for (const auto& text : messages) {
            const auto message = decode_message(text);
            auto [processed, reason] = book.process(message);
            REQUIRE(reason == ""); // NOLINT(readability-container-size-empty)
            REQUIRE(processed);
            switch (index) {
                case 0:
                    REQUIRE(book.orders().size() == 1);
                    break;
                case 1:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::New);
                    // TODO - numeric comparisons?
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::CumQty::Tag).value() == "0");
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::AvgPx::Tag).value() == "0");
                    }
                    break;
                case 2:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::PartiallyFilled);
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::CumQty::Tag).value() == "9107");
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::AvgPx::Tag).value() == "20");
                    }
                    break;
                case 3:
                    {
                    REQUIRE(book.orders().size() == 1);
                    const crocofix::order& order = book.orders().begin()->second;
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::OrdStatus::Tag) == FIX_5_0SP2::field::OrdStatus::Filled);
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::CumQty::Tag).value() == "10000");
                    REQUIRE(order.fields().get(FIX_5_0SP2::field::AvgPx::Tag).value() == "20");
                    }
                    break;
                default:
                    FAIL("unexpected number of lines in input: " + std::to_string(index));
                    break;
            }
            ++index;
        }
    }
}
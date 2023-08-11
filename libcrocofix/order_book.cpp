#include "order_book.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixutility/report.hpp>

namespace crocofix
{

void order_book::clear()
{
    m_orders.clear();
}

order_book::process_result order_book::process(const message& message)
{
    auto MsgType = message.fields().try_get(FIX_5_0SP2::field::MsgType::Tag);

    if (!MsgType.has_value()) {
        return { false, "message does not have a MsgType field" };
    }

    if (MsgType.value() == FIX_5_0SP2::field::MsgType::NewOrderSingle) {
        return process_order_single(message);
    }

    if (MsgType.value() == FIX_5_0SP2::field::MsgType::ExecutionReport) {
        return process_execution_report(message);
    }

    if (MsgType.value() == FIX_5_0SP2::field::MsgType::OrderCancelRequest) {
        return process_order_cancel_request(message);
    }

    if (MsgType.value() == FIX_5_0SP2::field::MsgType::OrderCancelReplaceRequest) {
        return process_order_cancel_replace_request(message);
    }

    if (MsgType.value() == FIX_5_0SP2::field::MsgType::OrderCancelReject) {
        return process_order_cancel_reject(message);
    }

    return { false, "unsupported MsgType = " + MsgType.value().value() };
}

order_book::process_result order_book::process_order_single(const message& message)
{
    try {
        auto order_single = order(message);
        auto [_, inserted] = m_orders.try_emplace(order_single.key(), order_single);
        if (!inserted) {
            return { false, "order book already contains an order with key = " + order_single.key() };
        }
        return { true, "" }; 
    }
    catch (std::exception& ex) {
        return { false, ex.what() };
    }
}

order_book::process_result order_book::process_execution_report(const message& execution_report)
{
    auto key = order::key_for_message(execution_report, true);

    auto order = m_orders.find(key);

    if (order == m_orders.end()) {
        return { false, "order book does not contain an order with key = " + key };
    }

    auto ExecType = execution_report.fields().try_get(FIX_5_0SP2::field::ExecType::Tag);

    if (ExecType.has_value()) {
        if (ExecType.value() == FIX_5_0SP2::field::ExecType::Replaced) {
            auto replacement = order->second.replace(execution_report);
            auto [_, inserted] = m_orders.try_emplace(replacement.key(), replacement);
            if (!inserted) {
                return { false, "order book already contains an order with key = " + replacement.key() };
            }
            return { true, "" }; 
        }
    }

    order->second.update(execution_report);
  
    return { true, "" };
}

order_book::process_result order_book::process_order_cancel_request(const message& order_cancel_request)
{
    auto key = order::key_for_message(order_cancel_request);

    auto order = m_orders.find(key);

    if (order == m_orders.end()) {
        return { false, "order book does not contain an order with key = " + key };
    }

    order->second.update(order_cancel_request);

    return { true, "" };
}

order_book::process_result order_book::process_order_cancel_replace_request(const message& order_cancel_replace_request)
{
    auto key = order::key_for_message(order_cancel_replace_request);

    auto order = m_orders.find(key);

    if (order == m_orders.end()) {
        return { false, "order book does not contain an order with key = " + key };
    }

    order->second.update(order_cancel_replace_request);

    return { true, "" };
}

order_book::process_result order_book::process_order_cancel_reject(const message& order_cancel_reject)
{
    auto key = order::key_for_message(order_cancel_reject, true);

    auto order = m_orders.find(key);

    if (order == m_orders.end()) {
        return { false, "order book does not contain an order with key = " + key };
    }

    order->second.rollback();

    return { true, "" };
}

}

std::ostream& operator<<(std::ostream& os, const crocofix::order_book& book) // NOLINT(readability-identifier-length)
{
    const std::string column_sender = "Sender";
    const std::string column_target = "Target";
    const std::string column_clordid = "ClOrdID";
    const std::string column_origclordid = "OrigClOrdID";
    const std::string column_symbol = "Symbol";
    const std::string column_ordstatus = "OrdStatus";
    const std::string column_ordtype = "OrdType";
    const std::string column_timeinforce = "TimeInForce";
    const std::string column_side = "Side";
    const std::string column_orderqty = "OrderQty";
    const std::string column_price = "Price";
    const std::string column_cumqty = "CumQty";
    const std::string column_avgpx = "AvgPx";

    crocofix::report report {
        { column_sender },
        { column_target },
        { column_clordid },
        { column_origclordid },
        { column_symbol },
        { column_ordstatus },
        { column_ordtype },
        { column_timeinforce },
        { column_side },
        { column_orderqty, crocofix::report::justification::right },
        { column_price, crocofix::report::justification::right },
        { column_cumqty, crocofix::report::justification::right },
        { column_avgpx, crocofix::report::justification::right }    
    };

    for (const auto& [key, order] : book.orders()) {

        const auto OrigClOrdID = order.fields().try_get(crocofix::FIX_5_0SP2::field::OrigClOrdID::Tag);
        const auto Symbol = order.fields().try_get(crocofix::FIX_5_0SP2::field::Symbol::Tag);
        const auto OrdStatus = order.fields().try_get(crocofix::FIX_5_0SP2::field::OrdStatus::Tag);
        const auto OrdType = order.fields().try_get(crocofix::FIX_5_0SP2::field::OrdType::Tag);
        const auto TimeInForce = order.fields().try_get(crocofix::FIX_5_0SP2::field::TimeInForce::Tag);
        const auto Side = order.fields().try_get(crocofix::FIX_5_0SP2::field::Side::Tag);
        const auto OrderQty = order.fields().try_get(crocofix::FIX_5_0SP2::field::OrderQty::Tag);
        const auto Price = order.fields().try_get(crocofix::FIX_5_0SP2::field::Price::Tag);
        const auto CumQty = order.fields().try_get(crocofix::FIX_5_0SP2::field::CumQty::Tag);
        const auto AvgPx = order.fields().try_get(crocofix::FIX_5_0SP2::field::AvgPx::Tag);

        report.rows().emplace_back(crocofix::report::row {
            order.SenderCompID(),
            order.TargetCompID(),
            order.ClOrdID().value(),
            OrigClOrdID.has_value() ? OrigClOrdID.value().value() : "",
            Symbol.has_value() ? Symbol.value().value() : "",
            OrdStatus.has_value() ? std::string(crocofix::FIX_5_0SP2::fields().name_of_value(crocofix::FIX_5_0SP2::field::OrdStatus::Tag, OrdStatus.value().value())) : "",
            OrdType.has_value() ? std::string(crocofix::FIX_5_0SP2::fields().name_of_value(crocofix::FIX_5_0SP2::field::OrdType::Tag, OrdType.value().value())) : "",
            TimeInForce.has_value() ? std::string(crocofix::FIX_5_0SP2::fields().name_of_value(crocofix::FIX_5_0SP2::field::TimeInForce::Tag, TimeInForce.value().value())) : "",
            Side.has_value() ? std::string(crocofix::FIX_5_0SP2::fields().name_of_value(crocofix::FIX_5_0SP2::field::Side::Tag, Side.value().value())) : "",
            OrderQty.has_value() ? OrderQty.value().value() : "",
            Price.has_value() ? Price.value().value() : "",
            CumQty.has_value() ? CumQty.value().value() : "",
            AvgPx.has_value() ? AvgPx.value().value() : ""
        });
    }

    os << report;

    return os;
}

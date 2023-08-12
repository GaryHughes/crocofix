#include "order_book.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

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


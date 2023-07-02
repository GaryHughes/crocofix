#include "order.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

namespace crocofix
{

order::order(const message& message)
:   mBeginString(message.fields().get(FIX_5_0SP2::field::BeginString::Tag).value()),
    mSenderCompID(message.fields().get(FIX_5_0SP2::field::SenderCompID::Tag).value()),
    mTargetCompID(message.fields().get(FIX_5_0SP2::field::TargetCompID::Tag).value()),
    mClOrdID(message.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value()),
    mOrigClOrdID(message.fields().try_get(FIX_5_0SP2::field::OrigClOrdID::Tag)),
    mSide(message.fields().get(FIX_5_0SP2::field::Side::Tag)),
    mSymbol(message.fields().get(FIX_5_0SP2::field::Symbol::Tag).value()),
    mOrdStatus(message.fields().try_get_or_default(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::PendingNew)),
    mOrdType(message.fields().try_get(FIX_5_0SP2::field::OrdType::Tag)),
    mTimeInForce(message.fields().try_get(FIX_5_0SP2::field::TimeInForce::Tag)),
    mOrderQty(message.fields().get(FIX_5_0SP2::field::OrderQty::Tag)),
    mPrice(message.fields().get(FIX_5_0SP2::field::Price::Tag)),
    mCumQty(message.fields().try_get_or_default(FIX_5_0SP2::field::CumQty::Tag, 0)),
    mAvgPx(message.fields().try_get_or_default(FIX_5_0SP2::field::AvgPx::Tag, 0))
{
    m_messages.emplace_back(message);
    m_key = create_key(mSenderCompID, mTargetCompID, mClOrdID);
}

std::string order::create_key(const std::string& SenderCompID, const std::string& TargetCompID, const std::string& ClOrdID)
{
    return SenderCompID + "-" + TargetCompID + "-" + ClOrdID;
}

std::string order::key_for_message(const message& message, bool reverse_comp_ids)
{
    auto SenderCompID = message.fields().get(FIX_5_0SP2::field::SenderCompID::Tag).value();
    auto TargetCompID = message.fields().get(FIX_5_0SP2::field::TargetCompID::Tag).value();

    auto ClOrdID = message.fields().try_get(FIX_5_0SP2::field::OrigClOrdID::Tag);

    if (!ClOrdID.has_value()) {
        ClOrdID = message.fields().get(FIX_5_0SP2::field::ClOrdID::Tag);
    }

    if (reverse_comp_ids) {
        return create_key(TargetCompID, SenderCompID, ClOrdID->value()); // NOLINT(readability-suspicious-call-argument)
    }

    return create_key(SenderCompID, TargetCompID, ClOrdID->value());
}

void order::update(const message& message)
{
    mOrdStatus = message.fields().try_get_or_default(FIX_5_0SP2::field::OrdStatus::Tag, mOrdStatus);
    mCumQty = message.fields().try_get_or_default(FIX_5_0SP2::field::CumQty::Tag, mCumQty);
    mAvgPx = message.fields().try_get_or_default(FIX_5_0SP2::field::AvgPx::Tag, mAvgPx);    

    if (mAvgPx.value() == "0") { // TODO - numeric type getters
        const auto LastPx = message.fields().try_get(FIX_5_0SP2::field::LastPx::Tag);
        if (LastPx.has_value()) {
            mAvgPx = LastPx.value();
        }
    }

    mPrice = message.fields().try_get_or_default(FIX_5_0SP2::field::Price::Tag, mPrice);
    mOrderQty = message.fields().try_get_or_default(FIX_5_0SP2::field::OrderQty::Tag, mOrderQty);

    if (message.MsgType() == FIX_5_0SP2::field::MsgType::OrderCancelRequest.value()) {
        mPreviousOrdStatus = mOrdStatus;
        mOrdStatus = field(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::PendingCancel);            
    }
    else if (message.MsgType() == FIX_5_0SP2::field::MsgType::OrderCancelReplaceRequest.value()) {
        mPreviousOrdStatus = mOrdStatus;
        mOrdStatus = field(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::PendingReplace);
    }
    else if (message.MsgType() == FIX_5_0SP2::field::MsgType::OrderCancelReject.value()) {
        if (mPreviousOrdStatus.has_value()) {
            mOrdStatus = mPreviousOrdStatus.value();
            mPreviousOrdStatus.reset();
        }
    }

    m_messages.emplace_back(message);
}

order order::replace(const message& execution_report)
{
    auto ClOrdID = execution_report.fields().try_get(FIX_5_0SP2::field::ClOrdID::Tag);

    if (!ClOrdID.has_value()) {
        throw std::runtime_error("ExecutionReport does not contain a ClOrdID"); 
    }

    order replacement = *this;
    replacement.update(execution_report);

    replacement.mClOrdID = ClOrdID.value().value();
    replacement.mOrdStatus = field(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::New); 
    replacement.m_key = create_key(replacement.mSenderCompID, replacement.mTargetCompID, replacement.mClOrdID); 

    mOrdStatus = field(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::Replaced);

    m_messages.emplace_back(execution_report);
  
    return replacement;
}

}
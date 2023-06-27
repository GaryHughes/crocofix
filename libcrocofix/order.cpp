#include "order.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

namespace crocofix
{

order::order(const message& message)
:   mBeginString(message.fields().get(FIX_5_0SP2::field::BeginString::Tag).value()),
    mSenderCompID(message.fields().get(FIX_5_0SP2::field::SenderCompID::Tag).value()),
    mTargetCompID(message.fields().get(FIX_5_0SP2::field::TargetCompID::Tag).value()),
    mClOrdID(message.fields().get(FIX_5_0SP2::field::ClOrdID::Tag).value()),
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

}
#include "order.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

namespace crocofix
{

const std::set<int> order::s_identity_fields = { 
    FIX_5_0SP2::field::BeginString::Tag,
    FIX_5_0SP2::field::SenderCompID::Tag,
    FIX_5_0SP2::field::TargetCompID::Tag,
    FIX_5_0SP2::field::ClOrdID::Tag,
    FIX_5_0SP2::field::OrigClOrdID::Tag
};

bool order::is_identity_field(const field& field)
{
    return s_identity_fields.contains(field.tag());
}

order::order(const message& message)
:   mBeginString(message.fields().get(FIX_5_0SP2::field::BeginString::Tag).value()),
    mSenderCompID(message.fields().get(FIX_5_0SP2::field::SenderCompID::Tag).value()),
    mTargetCompID(message.fields().get(FIX_5_0SP2::field::TargetCompID::Tag).value()),
    mClOrdID(message.fields().get(FIX_5_0SP2::field::ClOrdID::Tag)),
    mOrigClOrdID(message.fields().try_get(FIX_5_0SP2::field::OrigClOrdID::Tag)),
    m_fields(message.fields())
{
    m_messages.emplace_back(message);
    m_key = create_key(mSenderCompID, mTargetCompID, mClOrdID.value());
}

void order::update_pending_fields(const field_collection& fields)
{
    for (const auto& field : fields) {
        if (!is_identity_field(field)) {
            m_pending_fields.set(field, set_operation::replace_first_or_append);
        }
    }
}

void order::update_fields(const field_collection& fields)
{
    for (const auto& field : fields) {
        if (!is_identity_field(field)) {
            m_fields.set(field, set_operation::replace_first_or_append);
        }
    }
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
    m_messages.emplace_back(message);

    if (message.MsgType() == FIX_5_0SP2::field::MsgType::OrderCancelReplaceRequest.value()) {
        mPreviousOrdStatus = m_fields.try_get(FIX_5_0SP2::field::OrdStatus::Tag);
        mNewClOrdID = message.fields().get(FIX_5_0SP2::field::ClOrdID::Tag);
        update_pending_fields(message.fields());
        m_fields.set(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::PendingReplace);
        return;
    }

    if (message.MsgType() == FIX_5_0SP2::field::MsgType::OrderCancelRequest.value()) {
        mPreviousOrdStatus = m_fields.try_get(FIX_5_0SP2::field::OrdStatus::Tag);
        update_pending_fields(message.fields());
        m_fields.set(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::PendingCancel);
        return;
    }

    update_fields(message.fields());
}

order order::replace(const message& execution_report)
{
    order replacement = *this;
    replacement.update(execution_report);
    replacement.commit();
    rollback();

    if (mNewClOrdID.has_value()) {
        replacement.m_fields.set(mNewClOrdID.value(), set_operation::replace_first_or_append);
        replacement.m_fields.set(FIX_5_0SP2::field::OrigClOrdID::Tag, replacement.ClOrdID().value(), set_operation::replace_first_or_append);
        replacement.mOrigClOrdID = replacement.mClOrdID;
        replacement.mClOrdID = mNewClOrdID.value();
    }
    else {
        const auto ClOrdID = execution_report.fields().try_get(FIX_5_0SP2::field::ClOrdID::Tag);
        if (ClOrdID.has_value()) {
            replacement.m_fields.set(ClOrdID.value(), set_operation::replace_first_or_append);
            replacement.m_fields.set(FIX_5_0SP2::field::OrigClOrdID::Tag, replacement.mClOrdID.value(), set_operation::replace_first_or_append);
            replacement.mOrigClOrdID = replacement.mClOrdID;
            replacement.mClOrdID = ClOrdID.value();
        }
    }
  
    replacement.m_key = create_key(replacement.mSenderCompID, replacement.mTargetCompID, replacement.mClOrdID.value());
    replacement.m_fields.set(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::New);
    m_fields.set(FIX_5_0SP2::field::OrdStatus::Tag, FIX_5_0SP2::field::OrdStatus::Replaced);
  
    m_messages.emplace_back(execution_report);
  
    return replacement;
}

void order::commit()
{
    update_fields(m_pending_fields);
    m_pending_fields.clear();
}

void order::rollback()
{
    m_pending_fields.clear();

    if (mPreviousOrdStatus.has_value()) {
        m_fields.set(mPreviousOrdStatus.value());
        mPreviousOrdStatus.reset();
    }
}

}
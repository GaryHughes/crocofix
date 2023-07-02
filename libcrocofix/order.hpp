#ifndef crocofix_libcrocofix_order_hpp
#define crocofix_libcrocofix_order_hpp

#include "message.hpp"

namespace crocofix
{

class order
{
public:

    using message_collection = std::vector<message>;

    order(const message& message);

    void update(const message& message);
    order replace(const message& execution_report);

    const message_collection& messages() const { return m_messages; }
    const std::string& key() const { return m_key; }

    const std::string& BeginString() const { return mBeginString; }
    const std::string& SenderCompID() const { return mSenderCompID; }
    const std::string& TargetCompID() const { return mTargetCompID; }
    const std::string& ClOrdID() const { return mClOrdID; }
    const std::optional<field>& OrigClOrdID() const { return mOrigClOrdID; }
    const field& Side() const { return mSide; }
    const std::string& Symbol() const { return mSymbol; }
    const field& OrdStatus() const { return mOrdStatus; }
    const std::optional<field>& OrdType() const { return mOrdType; }
    const std::optional<field>& TimeInForce() const { return mTimeInForce; }
    const field& OrderQty() const { return mOrderQty; }
    const field& Price() const { return mPrice; }
    const field& CumQty() const { return mCumQty; }
    const field& AvgPx() const { return mAvgPx; }

    static std::string key_for_message(const message& message, bool reverse_comp_ids = false);

private:

    message_collection m_messages;
    std::string m_key;

    std::string mBeginString;
    std::string mSenderCompID;
    std::string mTargetCompID;
    std::string mClOrdID;
    std::optional<field> mOrigClOrdID;
    field mSide;
    std::string mSymbol;
    field mOrdStatus;
    std::optional<field> mOrdType;
    std::optional<field> mTimeInForce;
    field mOrderQty;
    field mPrice;
    field mCumQty;
    field mAvgPx;

    // When we send an order cancel or order cancel replace request we cache the current status here
    // and set OrdStatus to Pending???. If we get a successful reply we blat this value, if we get
    // rejected we replace OrdStatus with this value.
    std::optional<field> mPreviousOrdStatus;

    static std::string create_key(const std::string& SenderCompID, const std::string& TargetCompID, const std::string& ClOrdID);

};

}

#endif
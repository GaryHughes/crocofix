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

    const message_collection& messages() const { return m_messages; }
    const std::string& key() const { return m_key; }

    const std::string& BeginString() const { return mBeginString; }
    const std::string& SenderCompID() const { return mSenderCompID; }
    const std::string& TargetCompID() const { return mTargetCompID; }
    const std::string& ClOrdID() const { return mClOrdID; }
    const field& Side() const { return mSide; }
    const std::string& Symbol() const { return mSymbol; }
    const field& OrdStatus() const { return mOrdStatus; }
    const std::optional<field>& OrdType() const { return mOrdType; }
    const std::optional<field>& TimeInForce() const { return mTimeInForce; }
    const field& OrderQty() const { return mOrderQty; }
    const field& Price() const { return mPrice; }
    const field& CumQty() const { return mCumQty; }
    const field& AvgPx() const { return mAvgPx; }

private:

    message_collection m_messages;
    std::string m_key;

    std::string mBeginString;
    std::string mSenderCompID;
    std::string mTargetCompID;
    std::string mClOrdID;
    field mSide;
    std::string mSymbol;
    field mOrdStatus;
    std::optional<field> mOrdType;
    std::optional<field> mTimeInForce;
    field mOrderQty;
    field mPrice;
    field mCumQty;
    field mAvgPx;

    static std::string create_key(const std::string& SenderCompID, const std::string& TargetCompID, const std::string& ClOrdID);

};

}

#endif
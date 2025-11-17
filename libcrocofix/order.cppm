module;

#include <set>
#include <vector>
#include <string>
#include <optional>

export module crocofix:order;

import :message;
import :field;
import :field_collection;

export namespace crocofix
{

// This class is a simple representation of a single order and all fields must be unique. If you need to process
// an OrderList it must be broken into separate messages first.
class order
{
public:

    using message_collection = std::vector<message>;

    static const std::set<int> s_identity_fields;

    order(const message& message);

    void update(const message& message);
    order replace(const message& execution_report);

    const message_collection& messages() const { return m_messages; }
    const std::string& key() const { return m_key; }

    const std::string& BeginString() const { return mBeginString; }
    const std::string& SenderCompID() const { return mSenderCompID; }
    const std::string& TargetCompID() const { return mTargetCompID; }
    const field& ClOrdID() const { return mClOrdID; }
    const std::optional<field>& OrigClOrdID() const { return mOrigClOrdID; }

    const std::optional<field>& NewClOrdID() const { return mNewClOrdID; }

    static std::string key_for_message(const message& message, bool reverse_comp_ids = false);

    const field_collection& fields() const { return m_fields; }
    const field_collection& pending_fields() const { return m_pending_fields; }

    void commit();
    void rollback();

    static bool is_identity_field(const field& field);
    static bool is_identity_field(int tag);

private:

    message_collection m_messages;
    std::string m_key;

    // Store the fields that comprise the order book id directly.
    std::string mBeginString;
    std::string mSenderCompID;
    std::string mTargetCompID;
    field mClOrdID;

    // This isn't a part of the id but it's important for tracking cancel replace chains so track it directly.
    std::optional<field> mOrigClOrdID;

    // When we send an order cancel or order cancel replace request we cache the current status here
    // and set OrdStatus to Pending???. If we get a successful reply we blat this value, if we get
    // rejected we replace OrdStatus with this value.
    std::optional<field> mPreviousOrdStatus;

    // This is for replaced orders. 
    // 1. When we see the OrderCancelReplaceRequest we have ClOrdId=1 and OrigClOrdID=2  
    // 2. When we get the Pending ExecutionReport   we have ClOrdId=1 and OrigClOrdID=2
    // 3. When we get the Replaced ExecutionReport  we have ClOrdId=1 and OrigClOrdID=1
    // 4. At this point we want to set the previous order to Replaced and we want to clone it
    //    and give the new order ClOrdID=2. We walk back through the order list to find this
    //    ClOrdId in mNewClOrdId.
    std::optional<field> mNewClOrdID;

    field_collection m_fields;
    field_collection m_pending_fields;

    static std::string create_key(const std::string& SenderCompID, const std::string& TargetCompID, const std::string& ClOrdID);
    void update_pending_fields(const field_collection& fields);
    void update_fields(const field_collection& fields);

};

}

#ifndef crocofix_libcrocofix_order_book_hpp
#define crocofix_libcrocofix_order_book_hpp

#include <string>
#include <map>
#include "message.hpp"
#include "order.hpp"

namespace crocofix
{

class order_book
{
public:

    // TODO - make a specialised collection for this
    using order_collection = std::map<std::string, order>;

    struct process_result
    {
        bool processed;
        std::string reason;
    };

    process_result process(const message& message);

    const order_collection& orders() const { return m_orders; }

    void clear();

private:

    order_collection m_orders;

    process_result process_order_single(const message& message);
    process_result process_execution_report(const message& execution_report);
    process_result process_order_cancel_request(const message& order_cancel_request);
    process_result process_order_cancel_replace_request(const message& order_cancel_replace_request);
    process_result process_order_cancel_reject(const message& order_cancel_reject);

};

}

#endif
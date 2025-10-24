#ifndef crocofix_libcrocofix_order_book_hpp
#define crocofix_libcrocofix_order_book_hpp

#include <string>
#include <map>
#include <expected>
#include "message.hpp"
#include "order.hpp"

namespace crocofix
{

class order_book
{
public:

    // TODO - make a specialised collection for this
    using order_collection = std::map<std::string, order>;

    std::expected<void, std::string> process(const message& message);

    const order_collection& orders() const { return m_orders; }

    void clear();

private:

    order_collection m_orders;

    std::expected<void, std::string> process_order_single(const message& message);
    std::expected<void, std::string> process_execution_report(const message& execution_report);
    std::expected<void, std::string> process_order_cancel_request(const message& order_cancel_request);
    std::expected<void, std::string> process_order_cancel_replace_request(const message& order_cancel_replace_request);
    std::expected<void, std::string> process_order_cancel_reject(const message& order_cancel_reject);

};

}

#endif
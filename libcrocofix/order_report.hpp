#ifndef crocofix_libcrocofix_order_report_hpp
#define crocofix_libcrocofix_order_report_hpp

#include <vector>
#include "order_book.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

namespace crocofix
{

class order_report
{
public:

    order_report(std::initializer_list<dictionary::orchestration_field> fields = {
        FIX_5_0SP2::field::SenderCompID(), 
        FIX_5_0SP2::field::TargetCompID(), 
        FIX_5_0SP2::field::ClOrdID(),
        FIX_5_0SP2::field::OrigClOrdID(),
        FIX_5_0SP2::field::Symbol(),
        FIX_5_0SP2::field::OrdStatus(),
        FIX_5_0SP2::field::OrdType(),
        FIX_5_0SP2::field::TimeInForce(),
        FIX_5_0SP2::field::Side(),
        FIX_5_0SP2::field::OrderQty(),
        FIX_5_0SP2::field::Price(),
        FIX_5_0SP2::field::CumQty(),
        FIX_5_0SP2::field::AvgPx()
    });

    void print(std::ostream& os, const order_book& book);

private:

    std::vector<dictionary::orchestration_field> m_fields;

};

}

#endif
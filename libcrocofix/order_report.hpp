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

    static const std::vector<dictionary::orchestration_field> default_fields;

    order_report(const std::vector<dictionary::orchestration_field>& fields = default_fields);

    void print(std::ostream& os, const order_book& book);

private:

    std::vector<dictionary::orchestration_field> m_fields;

};

}

#endif
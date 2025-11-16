module;

#include <vector>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

export module crocofix:order_report;

import crocofix.utility;
import :order_book;

export namespace crocofix
{

class order_report
{
public:

    static const std::vector<dictionary::orchestration_field> default_fields;

    order_report(const std::vector<dictionary::orchestration_field>& fields = default_fields);

    void print(std::ostream& os, const order_book& book);

private:

    std::vector<dictionary::orchestration_field> m_fields;

    crocofix::report report;
    
};

}

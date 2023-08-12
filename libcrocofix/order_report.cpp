#include "order_report.hpp"
#include <libcrocofixutility/report.hpp>

namespace crocofix
{

order_report::order_report(std::initializer_list<dictionary::orchestration_field> fields)
:   m_fields(fields)
{
}

void order_report::print(std::ostream& os, const order_book& book) // NOLINT(readability-identifier-length)
{
    crocofix::report report;

    for (const auto& field : m_fields) {
        if (field.is_numeric()) {
            report.columns().emplace_back(std::string(field.name()), report::justification::right);
        }
        else {
            report.columns().emplace_back(std::string(field.name()), report::justification::left);
        }
    }    

    for (const auto& [key, order] : book.orders()) {
   
        crocofix::report::row row;
   
        for (const auto& field : m_fields) {
            auto value = order.fields().try_get_or_default(field.tag(), "");
            auto pending_value = order.pending_fields().try_get(field.tag());
            auto name_of_value = field.name_of_value(value.value());
            
            std::string cell = name_of_value.empty() ? value.value() : std::string(name_of_value);

            if (pending_value.has_value() && 
                value.value() != pending_value.value().value() && 
                !order::is_identity_field(field.tag())) 
            {
                auto name_of_pending_value = field.name_of_value(pending_value.value().value());
                if (name_of_pending_value.empty()) {
                    cell += " (" + pending_value.value().value() + ")";
                }
                else {
                    cell += " (" + std::string(name_of_pending_value) + ")";
                }
            }
            
            row.push_back(cell);
        }
        report.rows().push_back(row);
    }

    os << report;
}

}
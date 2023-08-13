#include "report.hpp"
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace crocofix
{

report::report(std::initializer_list<column> columns)
:   m_columns(columns)
{
}

report::column::column(const std::string& name, report::justification justification)
:   m_raw_name(name),
    m_justification(justification)
{
    boost::split(m_name, name, boost::is_any_of("\r\n"), boost::token_compress_on); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
}

}

std::ostream& operator<<(std::ostream& os, const crocofix::report& report) // NOLINT(readability-function-cognitive-complexity, readability-identifier-length)
{
    const std::ios::fmtflags flags(os.flags());

    // Calculate the column widths
    std::vector<int> column_widths(report.columns().size());

    size_t column_header_rows = 0;

    for (size_t column_index = 0; column_index < report.columns().size(); ++column_index) {
        
        const auto& column = report.columns()[column_index];
        int size = 0;
        const auto& name = column.name();

        column_header_rows = std::max(column_header_rows, name.size());

        for (const auto& name_component : name) {
            size = std::max(size, int(name_component.size()));
        }

        for (const auto & row : report.rows()) {
            if (column_index < row.size()) {
                size = std::max(size, int(row[column_index].size()));
            }
        }

        column_widths[column_index] = size;
    }

    int total_width = 0;
    
    if (!column_widths.empty()) {
        total_width = int(std::accumulate(column_widths.begin(), 
                                          column_widths.end(), 
                                          // Add 2 space characters between each column.
                                          (column_widths.size() - 1) * 2)); 
    }

    // Print a separator between the title column headers and column headers
    os << std::setfill('-') << std::setw(total_width) << "-" << std::setfill(' ');  
    os << '\n';
    //
    // Print the column headers
    //
    for (size_t header_index = 0; header_index < column_header_rows; ++header_index)
    {
        for (size_t index = 0; index < report.columns().size(); ++index)
        {
            const auto& column = report.columns()[index];
            const auto& name = column.name();

            if (column.justification() == crocofix::report::justification::right) {
                os.setf(std::ios::right, std::ios::adjustfield);
            }
            else {
                os.setf(std::ios::left, std::ios::adjustfield);
            }
        
            std::string value = " ";

            if (name.size() == column_header_rows) {
                value = name[header_index]; 
            }
            else {
                const size_t effective_max = column_header_rows - header_index;
                if (name.size() >= effective_max) {
                    value = name[header_index - (column_header_rows - name.size())];
                }
            }

            os << std::setw(column_widths[index]) << value;
            
            // print 2 spaces between each column
            if (index < report.columns().size() - 1) {
                os << "  ";
            }
        }

        os << '\n';
    }

    // Print a separator between the column headers and the data rows
    os << std::setfill('-') << std::setw(total_width) << "-" << std::setfill(' '); 
    os << '\n';
 
    // Print the data rows        
    os << std::setfill(' ');

    for (const auto& row : report.rows()) {
        for (size_t column_index = 0; column_index < report.columns().size(); ++column_index) {
            const auto& column = report.columns()[column_index];
            if (column.justification() == crocofix::report::justification::right) {
                os.setf(std::ios::right, std::ios::adjustfield);
            }
            else {
                os.setf(std::ios::left, std::ios::adjustfield);
            }

            os << std::setw(column_widths[column_index]);
            
            if (column_index < row.size()) {
                os << row[column_index];
            }
            else {
                os << "";
            }

            if (column_index < report.columns().size() - 1) {
                os << "  ";
            }
        }
        os << '\n';
    }

    // Print a footer.
    os << std::setfill('-') << std::setw(total_width) << "-"; 
    os << '\n';
    os << std::setfill(' ');
    
    os.flags(flags);

    return os;
}

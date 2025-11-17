module;

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <string>
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

module crocofix.utility;

namespace crocofix
{

report::report(std::initializer_list<column> columns)
:   m_columns(columns)
{
}

report::column::column(std::string name, report::justification justification)
:   m_raw_name(std::move(name)),
    m_justification(justification)
{
    boost::split(m_name, m_raw_name, boost::is_any_of("\r\n"), boost::token_compress_on); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks, clang-analyzer-security.ArrayBound)
}

}

std::ostream& operator<<(std::ostream& stream, const crocofix::report& report) // NOLINT(readability-function-cognitive-complexity)
{
    const std::ios::fmtflags flags(stream.flags());

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
    stream << std::setfill('-') << std::setw(total_width) << "-" << std::setfill(' ');  
    stream << '\n';
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
                stream.setf(std::ios::right, std::ios::adjustfield);
            }
            else {
                stream.setf(std::ios::left, std::ios::adjustfield);
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

            stream << std::setw(column_widths[index]) << value;
            
            // print 2 spaces between each column
            if (index < report.columns().size() - 1) {
                stream << "  ";
            }
        }

        stream << '\n';
    }

    // Print a separator between the column headers and the data rows
    stream << std::setfill('-') << std::setw(total_width) << "-" << std::setfill(' '); 
    stream << '\n';
 
    // Print the data rows        
    stream << std::setfill(' ');

    for (const auto& row : report.rows()) {
        for (size_t column_index = 0; column_index < report.columns().size(); ++column_index) {
            const auto& column = report.columns()[column_index];
            if (column.justification() == crocofix::report::justification::right) {
                stream.setf(std::ios::right, std::ios::adjustfield);
            }
            else {
                stream.setf(std::ios::left, std::ios::adjustfield);
            }

            stream << std::setw(column_widths[column_index]);
            
            if (column_index < row.size()) {
                stream << row[column_index];
            }
            else {
                stream << "";
            }

            if (column_index < report.columns().size() - 1) {
                stream << "  ";
            }
        }
        stream << '\n';
    }

    // Print a footer.
    stream << std::setfill('-') << std::setw(total_width) << "-"; 
    stream << '\n';
    stream << std::setfill(' ');
    
    stream.flags(flags);

    return stream;
}


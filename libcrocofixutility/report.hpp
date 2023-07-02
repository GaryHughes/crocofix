#ifndef crocofix_utilities_report_hpp
#define crocofix_utilities_report_hpp

#include <string>
#include <vector>

namespace crocofix
{

class report
{
public:

    enum class justification {
        left,
        right
    };

    class column
    {
    public:

        column(const std::string& name, report::justification justification = report::justification::left);

        const std::string& raw_name() const { return m_raw_name; }
        const std::vector<std::string>& name() const { return m_name; }
        report::justification justification() const { return m_justification; }

    private:

        std::string m_raw_name;  
        std::vector<std::string> m_name;
        report::justification m_justification;
        
    };

    using column_collection = std::vector<column>;
    using row = std::vector<std::string>;
    using row_collection = std::vector<row>;

    report() = default;
    report(std::initializer_list<column> columns);

    const column_collection& columns() const { return const_cast<report&>(*this).columns(); }
    column_collection& columns() { return m_columns; }
    
    const row_collection& rows() const { return const_cast<report&>(*this).rows(); }
    row_collection& rows() { return m_rows; }
    
private:

    column_collection m_columns;
    row_collection m_rows;

};

}

std::ostream& operator<<(std::ostream& os, const crocofix::report& report);

#endif
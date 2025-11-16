module;

#include <string>
#include <vector>
#include <cstdint>

export module crocofix.utility:report;

export namespace crocofix
{

class report
{
public:

    enum class justification : std::uint8_t {
        left,
        right
    };

    class column
    {
    public:

        column(std::string name, report::justification justification = report::justification::left);

        [[nodiscard]] const std::string& raw_name() const { return m_raw_name; }
        [[nodiscard]] const std::vector<std::string>& name() const { return m_name; }
        [[nodiscard]] report::justification justification() const { return m_justification; }

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

    [[nodiscard]] const column_collection& columns() const { return const_cast<report&>(*this).columns(); } // NOLINT(cppcoreguidelines-pro-type-const-cast)
    column_collection& columns() { return m_columns; }
    
    [[nodiscard]] const row_collection& rows() const { return const_cast<report&>(*this).rows(); } // NOLINT(cppcoreguidelines-pro-type-const-cast)
    row_collection& rows() { return m_rows; }
    
private:

    column_collection m_columns;
    row_collection m_rows;

};

}

export std::ostream& operator<<(std::ostream& stream, const crocofix::report& report);

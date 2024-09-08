#ifndef crocofix_libcrocofixpython_reader_hpp
#define crocofix_libcrocofixpython_reader_hpp

#include <pybind11/pybind11.h>
#include <libcrocofix/reader.hpp>
#include <iostream>

namespace py = pybind11;

void init_reader(py::module_& module);

class Reader : public crocofix::reader
{
public:

    Reader()
    {
        m_opened_connection = opened.connect([&]() { opened_callback(); });
        m_closed_connection = closed.connect([&]() { closed_callback(); });
        
        m_message_read_connection = message_read.connect(
            [&](crocofix::message& message) { 
                try {
                    message_read_callback(message);
                }
                catch (std::exception& ex)
                {
                    std::cout << "message_read_callback failed: " << ex.what() << "\n";
                } 
            } 
        );
    }

    void open() override
    {
        // TODO - can we do this to avoid crashes for not implemented methods?
        // pybind11::gil_scoped_acquire gil;  // Acquire the GIL while in this scope.
        // // Try to look up the overridden method on the Python side.
        // pybind11::function override = pybind11::get_override(this, "myMethod");
        // if (override) {  // method is found
        PYBIND11_OVERRIDE_PURE(void, Reader, open,);
    }

    void close() override 
    {
        PYBIND11_OVERRIDE_PURE(void, Reader, close,);
    }

    using operation_callback = std::function<void()>;
    using message_callback = std::function<void(crocofix::message&)>;

    operation_callback opened_callback;
    operation_callback closed_callback;
    message_callback message_read_callback;
    
private:

    Reader(const Reader&) = delete;
   
    boost::signals2::connection m_opened_connection;
    boost::signals2::connection m_closed_connection;
    boost::signals2::connection m_message_read_connection;

};

#endif
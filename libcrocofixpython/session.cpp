#include "session.hpp"
#include <pybind11/functional.h>
#include <libcrocofix/session.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>

class scheduler : public crocofix::scheduler
{
public:

    void run() override {}
    
    void schedule(task_type task) override {}
    
    cancellation_token schedule_relative_callback(std::chrono::milliseconds /*when*/, const scheduled_callback& /*callback*/) override 
    {
        return 0;
    }

    cancellation_token schedule_repeating_callback(std::chrono::milliseconds /*interval*/, const scheduled_callback& /*callback*/) override
    {
        return 0;
    }

    void cancel_callback(cancellation_token /*token*/) override {}
     
};

void init_session(py::module_& module)
{

    py::class_<crocofix::session>(module, "Session")

        .def(py::init([](crocofix::reader& reader, crocofix::writer& writer) {
            static scheduler sched;
            return std::make_shared<crocofix::session>(reader, writer, sched, crocofix::FIX_5_0SP2::orchestration());
        }))
        .def("open", &crocofix::session::open)
        .def("close", &crocofix::session::close)
    ;

}
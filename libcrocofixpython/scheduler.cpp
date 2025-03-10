#include "scheduler.hpp"
#include <libcrocofix/scheduler.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

#include <iostream>

namespace py = pybind11;

class Scheduler : public crocofix::scheduler
{
public:

    void run() override
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::scheduler, run,);
    }

    void schedule(task_type task) override
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::scheduler, schedule, task);
    }

    cancellation_token schedule_relative_callback(std::chrono::milliseconds when, const scheduled_callback& callback) override
    {
        PYBIND11_OVERRIDE_PURE(cancellation_token, crocofix::scheduler, schedule_relative_callback, when, callback);
    }

    cancellation_token schedule_repeating_callback(std::chrono::milliseconds interval, const scheduled_callback& callback) override
    {
        PYBIND11_OVERRIDE_PURE(cancellation_token, crocofix::scheduler, schedule_repeating_callback, interval, callback);
    }

    void cancel_callback(cancellation_token token) override
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::scheduler, cancel_callback, token);
    }
};

void init_scheduler(py::module_& module)
{
    py::class_<crocofix::scheduler, Scheduler>(module, "_Scheduler")
        .def(py::init<>())
        .def("run", &crocofix::scheduler::run)
        .def("schedule", &crocofix::scheduler::schedule)
        .def("schedule_relative_callback", &crocofix::scheduler::schedule_relative_callback)
        .def("schedule_repeating_callback", &crocofix::scheduler::schedule_repeating_callback)
        .def("cancel_callback", &crocofix::scheduler::cancel_callback)
    ;
}

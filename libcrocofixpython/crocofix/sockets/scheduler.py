from .._crocofix import _Scheduler

import sched, time

class Scheduler(_Scheduler):

    def __init__(self):
        super().__init__()
        self.scheduler = sched.scheduler(time.time, time.sleep)

    def run(self):
        while True:
            self.scheduler.run()
            time.sleep(0.2)

    def schedule(self, task):
        # This is used by test code in the C++ implementation so we don't need it here.
        raise NotImplementedError()
 
    def schedule_relative_callback(self, when, callback):
        print("SCHEDULE RELATIVE {}".format(when.total_seconds()))
        # self.scheduler.enter(when.total_seconds(), priority=0, action=callback)
        return 0
    
    def schedule_repeating_callback(self, interval, callback):
        print("SCHEDULE REPEATING {}".format(interval))
        # task = asyncio.create_task(self.repeating_callback(interval, callback))
        # # task = asyncio.create_task(task_coroutine(), name="MyTask")
        # # asyncio.tasks.all_tasks. // This is a set
        # self.tasks.add(task)
        # task.add_done_callback(self.tasks.discard)
        # TODO
        return 0
 
    def cancel_callback(self, token):
        # TODO - we need this for session shutdown
        raise NotImplementedError()
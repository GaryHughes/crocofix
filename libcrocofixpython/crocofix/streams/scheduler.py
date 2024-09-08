import asyncio

from .._crocofix import _Scheduler

class Scheduler(_Scheduler):

    def __init__(self):
        super().__init__()
        self.tasks = set()
  
    def run(self):
        # loop = asyncio.get_running_loop()
        # loop.run_forever() 
        pass
    
    def schedule(self, task):
        # This is used by test code in the C++ implementation so we don't need it here.
        raise NotImplementedError()
 
    def schedule_relative_callback(self, when, callback):
        loop = asyncio.get_running_loop()
        loop.call_later(when.total_seconds(), callback)
        # TODO
        return 0
    
    async def repeating_callback(self, interval, callback):
        while True:
            await asyncio.sleep(interval.total_seconds())
            callback() 
  
    def schedule_repeating_callback(self, interval, callback):
        task = asyncio.create_task(self.repeating_callback(interval, callback))
        # task = asyncio.create_task(task_coroutine(), name="MyTask")
        # asyncio.tasks.all_tasks. // This is a set
        self.tasks.add(task)
        task.add_done_callback(self.tasks.discard)
        # TODO
        return 0
 
    def cancel_callback(self, token):
        # TODO - we need this for session shutdown
        raise NotImplementedError()
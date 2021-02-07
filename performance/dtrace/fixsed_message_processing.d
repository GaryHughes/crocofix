#!/usr/sbin/dtrace -s

#pragma D option cleanrate=50hz


/*
	This program is intended to measure the time taken for fixsed to execute 
	the lua script for the messages passing through it.

    $ ./fixsed <OPTIONS> &
	$ sudo ./fixsed_message_processing.d -p $!

*/

fixsed$target:::script-call
{
	@message_counts[stringof(copyinstr(arg2))] = count();
	self->ts = timestamp;
}

fixsed$target:::script-return
/self->ts/
{
	@message_counts[stringof(copyinstr(arg2))] = count();
	time = (timestamp - self->ts) / 1000;
	@["Script Execution Times (microseconds)"] = quantize(time);
	@message_times_max[stringof(copyinstr(arg2))] = max(time);
	@message_times_avg[stringof(copyinstr(arg2))] = avg(time);
	@message_times_min[stringof(copyinstr(arg2))] = min(time);
	self->ts = 0;
}

dtrace:::END
{
	printf("\nMessages\n");
	printa(@message_counts);
	printf("\nMax Script Time (microseconds)\n");
	printa(@message_times_max);
	printf("\nAverage Script Time (microseconds)\n");
	printa(@message_times_avg);
	printf("\nMin Script Time (microseconds)\n");
	printa(@message_times_min);
}

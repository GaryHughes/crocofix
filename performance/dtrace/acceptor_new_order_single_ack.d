#!/usr/sbin/dtrace -s

#pragma D option cleanrate=50hz
/* #pragma D option dynvarsize=6000000 */


/*
	This program is intended to measure the time taken for the examples/acceptor program 
	to respond to NewOrderSingle messages with an ExecutionReport.

	$ ./acceptor
	$ sudo ./new_order_single_execution_report_echo.d -p $! &
	$ ./initiator
	$ <ctrl-c>

*/

crocofix$target:::session-message-read
/copyinstr(arg0) == "D"/ 				/* MsgType == NewOrderSingle */
{
	@message_counts[stringof(copyinstr(arg0))] = count();
	self->ts = timestamp;
}

crocofix$target:::session-message-write
/copyinstr(arg0) == "8" && self->ts/ 				/* MsgType == ExecutionReport */
{
	time = (timestamp - self->ts) / 1000;
	@["Processing Times (microseconds)"] = quantize(time);
	@message_times_max[stringof(copyinstr(arg0))] = max(time);
	@message_times_avg[stringof(copyinstr(arg0))] = avg(time);
	@message_times_min[stringof(copyinstr(arg0))] = min(time);
	self->ts = 0;
}

dtrace:::END
{
	printf("\nMessages\n");
	printa(@message_counts);
	printf("\nMax Processing Time (microseconds)\n");
	printa(@message_times_max);
	printf("\nAverage Processing Time (microseconds)\n");
	printa(@message_times_avg);
	printf("\nMin Processing Time (microseconds)\n");
	printa(@message_times_min);
}

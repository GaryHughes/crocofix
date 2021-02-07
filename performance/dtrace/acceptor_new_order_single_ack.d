#!/usr/sbin/dtrace -s

#pragma D option cleanrate=50hz
/* #pragma D option dynvarsize=6000000 */


/*
	This program is intended to measure the time taken for the examples/acceptor program 
	to respond to NewOrderSingle messages with an ExecutionReport.

	$ ./acceptor
	$ sudo ./acceptor_new_order_single_ack.d -p $! &
	$ ./initiator

*/

crocofix$target:::session-message-read
/copyinstr(arg2) == "D"/ 				/* MsgType == NewOrderSingle */
{
	@message_counts[stringof(copyinstr(arg2))] = count();
	self->ts = timestamp;
}

crocofix$target:::session-message-write
/copyinstr(arg2) == "8" && self->ts/ 				/* MsgType == ExecutionReport */
{
	@message_counts[stringof(copyinstr(arg2))] = count();
	time = (timestamp - self->ts) / 1000;
	@["Processing Times (microseconds)"] = quantize(time);
	@message_times_max[stringof(copyinstr(arg2))] = max(time);
	@message_times_avg[stringof(copyinstr(arg2))] = avg(time);
	@message_times_min[stringof(copyinstr(arg2))] = min(time);
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

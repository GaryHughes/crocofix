#!/usr/sbin/dtrace -s

#pragma D option cleanrate=50hz
/* #pragma D option dynvarsize=6000000 */ 


/*
	This program is intended to measure the time taken for the examples/initiator program 
	to send NewOrderSingle messages and recieve ExecutionReport ACK.

	NB: It relies on the acceptor processing messages in order and does no correlation of
	    message content. One and only one ExecutionReport must be sent in response to each 
		NewOrderSingle. 

	$ ./acceptor
	$ sudo ./initiator_new_order_single_ack.d -c ../../build-Release/examples/initiator/initiator

*/

int write_index;
int read_index;
int write_times[int];

BEGIN {
	write_index = 0;
	read_index = 0;
}

crocofix$target:::session-message-write
/copyinstr(arg0) == "D"/ 				/* MsgType == NewOrderSingle */
{
	@message_counts[stringof(copyinstr(arg0))] = count();
	write_times[write_index++] = timestamp;
}

crocofix$target:::session-message-read
/copyinstr(arg0) == "8"/ 				/* MsgType == ExecutionReport */
{
	@message_counts[stringof(copyinstr(arg0))] = count();
	write_time = write_times[read_index++];
	if (write_time) {
		time = (timestamp - write_time) / 1000;
		@["Processing Times (microseconds)"] = quantize(time);
		@message_times_max[stringof(copyinstr(arg0))] = max(time);
		@message_times_avg[stringof(copyinstr(arg0))] = avg(time);
		@message_times_min[stringof(copyinstr(arg0))] = min(time);
		write_times[read_index - 1] = 0;
	}
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

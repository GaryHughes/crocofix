# Performance

Crocofix has [USDT](http://dtrace.org/guide/chp-usdt.html) probes embedded to make measurement of various aspects of performance simple without expensive internal instrumentation. These probes can be consumed in [DTrace](http://dtrace.org/guide) scripts on macOS and [bpftrace](https://bpftrace.org) scripts on Linux.

The probes currently defined are as follows, any application that links libcrocofix will automatically include these probes.

```DTrace
provider crocofix 
{
	probe session__message__read(char* SenderCompID, char* TargetCompID, char* MsgType);
	probe session__message__write(char* SenderCompID, char* TargetCompID, char* MsgType);
};
```

In the performance subdirectory there are sample scripts for both dtrace and ebpf that to demonstrate the use of the probes with the examples/acceptor and examples/initiator programs. There are comments
in each of the scripts demonstrating their use.

## dtrace
```bash
#
# This script measures the time it takes the acceptor to process a NewOrderSingle and reply 
# with an ExecutionReport.
#
acceptor_new_order_single_ack.d
#
# This script measures the time it takes the initiator to send a NewOrderSingle to the acceptor
# and receive an ExecutionReport response.
#
initiator_new_order_single_ack.d
```

## bpftrace
```bash
#
# This script measures the time it takes the acceptor to process a NewOrderSingle and 
# reply with an ExecutionReport.
#
acceptor_new_order_single_ack.bt
```

This is sample output from the bpftrace acceptor_new_order_single_ack.bt script. Times are in microseconds.

```bash
$ sudo ./acceptor_new_order_single_ack.bt -p 96374
Attaching 2 probes...

@message_counts[8]: 99999
@message_counts[D]: 100000

@message_times_avg[8]: 3

@message_times_max[8]: 1076

@message_times_min[8]: 2

@processing_time: 
[2, 4)             87903 |@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@|
[4, 8)             10066 |@@@@@                                               |
[8, 16)              750 |                                                    |
[16, 32)             942 |                                                    |
[32, 64)             233 |                                                    |
[64, 128)             54 |                                                    |
[128, 256)            26 |                                                    |
[256, 512)            19 |                                                    |
[512, 1K)              5 |                                                    |
[1K, 2K)               1 |                                                    |

```

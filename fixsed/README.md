# fixsed

```sh
usage: fixsed [--help] [--log-level <level>] [--log-path <directory>] [--pretty] --in [address:]port --out address:port [--script-path <path>] --script <filename>
  --help                display usage
  --pretty              pretty print messages
  --in arg              listen [address:]port eg. hostname:8888 or 8888
  --out arg             remote address:port h eg. hostname:6666
  --bind arg            local address[:port] to bind for outgoing connection 
                        eg. hostname:6666
  --script-path arg     the directory to load scripts from - defaults to .
  --script arg          the filename of the lua script in --script-path to load
  --log-level arg       the minimum log severity to include in the logger 
                        output. (trace|debug|info|warning|error|critical)
  --log-path arg        the directory to write log files in
```

* fixsed does not participate in the FIX session protocol other than rewriting the BodyLength and Checksum fields of each message.
  * Be careful not to edit session fields such as MsgSeqNum unless you know what you're doing.
  * There is no mechanism to drop or reject messages - this would break the session protocol.

## Logging


```lua
function initiator_read()
    log_trace("initiator read")
    log_debug("initiator read")
    log_info("initiator read")
    log_warn("initiator read")
    log_error("initiator read")
    log_fatal("initiator read")
end

function acceptor_read()
    log_trace("initiator read")
    log_debug("initiator read")
    log_info("initiator read")
    log_warn("initiator read")
    log_error("initiator read")
    log_fatal("initiator read")
end
```


## Performance

fixsed is built open libcrocofix and so it contains the USDT probes described [here](https://github.com/GaryHughes/crocofix/blob/main/performance/README.md).

It also has it's own probes for measuring the execution time of the lua scripts it calls.

```dtrace
provider fixsed 
{
	/* 
	 * This pair of probes can be used to measure the time it takes a lua script to 
	 * process a message. 
	 */
	probe script__call(char* SenderCompID, char* TargetCompID, char* MsgType);
	probe script__return(char* SenderCompID, char* TargetCompID, char* MsgType);
};
```

In the crocofix/performance directory there are sample scripts for both dtrace and ebpf that demonstrate the use of the probes. There are comments in the scripts demonstrating their use.

## dtrace
```bash
#
# This script measures the time it takes fixsed to process each message that passes through it 
# with the configured lua script.
#
fixsed_message_processing.d
```

```bash
% sudo ./fixsed_message_processing.d -p 7461
Password:
dtrace: system integrity protection is on, some features will not be available

dtrace: script './fixsed_message_processing.d' matched 9 probes
^C
CPU     ID                    FUNCTION:NAME
  4      2                             :END 
Messages

  1                                                                 4
  A                                                                 4
  0                                                                12
  D                                                               122
  8                                                               688

Max Script Time (microseconds)

  1                                                                53
  A                                                                69
  0                                                               151
  8                                                               218
  D                                                               244

Average Script Time (microseconds)

  8                                                                43
  1                                                                47
  A                                                                57
  0                                                                77
  D                                                                77

Min Script Time (microseconds)

  8                                                                27
  0                                                                35
  1                                                                41
  A                                                                46
  D                                                                59


  Script Execution Times (microseconds)             
           value  ------------- Distribution ------------- count    
               8 |                                         0        
              16 |@@@@@@@@@                                92       
              32 |@@@@@@@@@@@@@@@@@@@@@@@@@@               269      
              64 |@@@@                                     42       
             128 |@                                        12       
             256 |                                         0        

```
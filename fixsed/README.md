## fixsed

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
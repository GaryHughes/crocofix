# fixcat

``` shell
usage: fixcat [--help] [--admin] [FILE]...
  --help                display usage
  --admin               include administrative messages
```

fixcat is modelled on the UNIX cat utility; it will print FIX messages in human readable format with message, field, and enumerated value descriptions.

It will read from standard input or a list of one or more filenames specified on the command line.

``` shell
less fixlog | fixcat
fixcat fixlog1.txt fixlog2.txt fixlog3.txt
```

The input is expected to be raw FIX messages, one per line, with an optional prefix which would typically be a timestamp etc as is typical in log files. The parser searches for **8=FIX.** and assumes that is the start of the message which continues until a CheckSum field or end of line is reached.

```
8=FIX.4.49=7235=A49=INITIATOR56=ACCEPTOR34=152=20190816-10:34:27.75298=0108=3010=013

2019-08-17 13:00:00.000 8=FIX.4.49=7235=A49=INITIATOR56=ACCEPTOR34=152=20190816-10:34:27.75298=0108=3010=013
```

Parsing either of these lines with fixcat will product the following output.

```
Logon
{
  BeginString   (8) FIX.4.4
   BodyLength   (9) 72
      MsgType  (35) A - Logon
 SenderCompID  (49) INITIATOR
 TargetCompID  (56) ACCEPTOR
    MsgSeqNum  (34) 1
  SendingTime  (52) 20190816-10:34:27.752
EncryptMethod  (98) 0 - None
   HeartBtInt (108) 30
     CheckSum  (10) 013
}
```
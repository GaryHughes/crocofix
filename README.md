# Overview

This repository contains a set of scripts for parsing [FIX protocol](https://www.fixtrading.org/online-specification/) standards representations, checking them for internal consistency, comparing them for accuracy, and converting between formats. It currently supports the [FIX Orchestra](https://www.fixtrading.org/standards/fix-orchestra/) and the [FIX Repository](https://www.fixtrading.org/standards/fix-repository/).

Orchestrations provided by the FIX Trading Community can be downloaded [here](https://github.com/FIXTradingCommunity/orchestrations).

The latest version of the repository can be downloaded [here](https://www.fixtrading.org/packages/fix-repository-2010/).

1. [fixorchestra](#fixorchestra)
1. [fixrepository](#fixrepository)
1. [fixaudit](#fixaudit)
1. [fixreptorc](#fixreptorc)

## fixorchestra
FIX Orchestration parser and utilities

```
$ ./orchestration.py --help
usage: orchestration.py [-h] --orchestration file [--dump-field (tag|name)] [--dump-message (msgtype|name)] [--list-messages] [--list-fields]

optional arguments:
  -h, --help            show this help message and exit
  --orchestration file  The orchestration to load
  --dump-field (tag|name)
                        Display the definition of a field
  --dump-message (msgtype|name)
                        Display the definition of a message
  --list-messages       List all the messages in this orchestration
  --list-fields         List all the fields in this orchestration
  --list-enumerated-fields
                        List all fields with an enumerated value
```

```
$ ./orchestration.py --orchestration FixRepository44.xml --dump-field 4
AdvSide {
    Id    = 4
    Type  = AdvSideCodeSet
    Added = FIX.2.7
    (Broker's side of advertised trade)
    Values {
        B (Buy, FIX.2.7, Buy)
        S (Sell, FIX.2.7, Sell)
        X (Cross, FIX.2.7, Cross)
        T (Trade, FIX.2.7, Trade)
    }
}
```
```
$ ./orchestration.py --orchestration FixRepository42.xml --dump-message 8
ExecutionReport {
    Id = 9
    MsgType = 8
    Category = SingleGeneralOrderHandling
    Added = FIX.2.7
    (The execution report message is used to:
         1. Confirm the receipt of an order
         2. Confirm changes to an existing order (i.e. accept cancel and replace requests)
         3. Relay order status information
         4. Relay fill information on working orders
         5. Reject orders
         6. Report post-trade fees calculations associated with a trade)
    References {
        StandardHeader (Id = 1001, Category = Session, Added = FIX.4.0, Presence = required) {
            BeginString (Id = 8, Type = String, Added = FIX.2.7, Presence = required)
            BodyLength (Id = 9, Type = int, Added = FIX.2.7, Presence = required)
            MsgType (Id = 35, Type = MsgTypeCodeSet, Added = FIX.2.7, Presence = required)
            SenderCompID (Id = 49, Type = String, Added = FIX.2.7, Presence = required)
            TargetCompID (Id = 56, Type = String, Added = FIX.2.7, Presence = required)
            OnBehalfOfCompID (Id = 115, Type = String, Added = FIX.4.0, Presence = None)
            
            ...SNIP...

            MessageEncoding (Id = 347, Type = MessageEncodingCodeSet, Added = FIX.4.2, Presence = None)
            LastMsgSeqNumProcessed (Id = 369, Type = int, Added = FIX.4.2, Presence = None)
            OnBehalfOfSendingTime (Id = 370, Type = UTCTimestamp, Added = FIX.4.2, Presence = None)
        }
        OrderID (Id = 37, Type = String, Added = FIX.2.7, Presence = required)
        SecondaryOrderID (Id = 198, Type = String, Added = FIX.4.1, Presence = None)
        ClOrdID (Id = 11, Type = String, Added = FIX.2.7, Presence = None)
        OrigClOrdID (Id = 41, Type = String, Added = FIX.2.7, Presence = None)
        ClientID (Id = 109, Type = String, Added = FIX.3.0, Presence = None)
        ExecBroker (Id = 76, Type = String, Added = FIX.2.7, Presence = None)
        ContraGrp (Id = 2012, Category = Common, Added = FIX.4.2, Presence  = None) {
            NoContraBrokers (Id = 382, Type = int, Added = FIX.4.2, Presence = None)
            ContraBroker (Id = 375, Type = String, Added = FIX.4.2, Presence = None)
            ContraTrader (Id = 337, Type = String, Added = FIX.4.2, Presence = None)
            ContraTradeQty (Id = 437, Type = Qty, Added = FIX.4.2, Presence = None)
            ContraTradeTime (Id = 438, Type = UTCTimestamp, Added = FIX.4.2, Presence = None)
        }
        ListID (Id = 66, Type = String, Added = FIX.2.7, Presence = None)
        ExecID (Id = 17, Type = String, Added = FIX.2.7, Presence = required)
        ExecTransType (Id = 20, Type = ExecTransTypeCodeSet, Added = FIX.2.7, Presence = required)
        ExecRefID (Id = 19, Type = String, Added = FIX.2.7, Presence = None)
        ExecType (Id = 150, Type = ExecTypeCodeSet, Added = FIX.4.1, Presence = required)
        OrdStatus (Id = 39, Type = OrdStatusCodeSet, Added = FIX.2.7, Presence = required)
        
        ...SNIP...

        OrderQty2 (Id = 192, Type = Qty, Added = FIX.4.1, Presence = None)
        ClearingFirm (Id = 439, Type = String, Added = FIX.4.2, Presence = None)
        ClearingAccount (Id = 440, Type = String, Added = FIX.4.2, Presence = None)
        MultiLegReportingType (Id = 442, Type = MultiLegReportingTypeCodeSet, Added = FIX.4.2, Presence = None)
        StandardTrailer (Id = 1002, Category = Session, Added = FIX.4.0, Presence = required) {
            SignatureLength (Id = 93, Type = int, Added = FIX.2.7, Presence = None)
            Signature (Id = 89, Type = data, Added = FIX.2.7, Presence = None)
            CheckSum (Id = 10, Type = String, Added = FIX.2.7, Presence = required)
        }
    }
}

```

## fixrepository
FIX Repository parser and utilities

```
$ ./repository.py --help
usage: repository.py [-h] --repository directory [--dump-field (tag|name)] [--dump-message (msgtype|name)] [--list-messages] [--list-fields]

optional arguments:
  -h, --help            show this help message and exit
  --repository directory
                        A directory containing a repository to load e.g. fix_repository_2010_edition_20200402/FIX.4.4/Base
  --dump-field (tag|name)
                        Display the definition of a field (name is not case sensitive)
  --dump-message (msgtype|name)
                        Display the definition of a message (name is not case sensitive
  --list-messages       List all the messages in this repository
  --list-fields         List all the fields in this repository
  --list-enumerated-fields
                        List all fields with an enumerated value
```

```
$ ./repository.py --repository FIX.4.4/Base --dump-field 4
AdvSide {
    Id   = 4
    Type  = char
    Added = FIX.2.7
    (Broker's side of advertised trade)
    Values {
        B (Buy, FIX.2.7, Buy)
        S (Sell, FIX.2.7, Sell)
        X (Cross, FIX.2.7, Cross)
        T (Trade, FIX.2.7, Trade)
    }
}
```

```
$ ./repository.py --repository FIX.4.2/Base --dump-message 8
ExecutionReport {
    ComponentId = 9
    MsgType = 8
    CategoryID = SingleGeneralOrderHandling
    SectionID = Trade
    Added = FIX.2.7
    (The execution report message is used to:
1. Confirm the receipt of an order
2. Confirm changes to an existing order (i.e. accept cancel and replace requests)
3. Relay order status information
4. Relay fill information on working orders
5. Reject orders
6. Report post-trade fees calculations associated with a trade)
    MsgContents {
        StandardHeader {
            BeginString (Id = 8, Type = String, Added = FIX.2.7, Required = 1)
            BodyLength (Id = 9, Type = int, Added = FIX.2.7, Required = 1)
            MsgType (Id = 35, Type = String, Added = FIX.2.7, Required = 1)
            SenderCompID (Id = 49, Type = String, Added = FIX.2.7, Required = 1)
            TargetCompID (Id = 56, Type = String, Added = FIX.2.7, Required = 1)
            OnBehalfOfCompID (Id = 115, Type = String, Added = FIX.4.0, Required = 0)
            
            ...SNIP...

            MessageEncoding (Id = 347, Type = String, Added = FIX.4.2, Required = 0)
            LastMsgSeqNumProcessed (Id = 369, Type = int, Added = FIX.4.2, Required = 0)
            OnBehalfOfSendingTime (Id = 370, Type = UTCTimestamp, Added = FIX.4.2, Required = 0)
        }
        OrderID (Id = 37, Type = String, Added = FIX.2.7, Required = 1)
        SecondaryOrderID (Id = 198, Type = String, Added = FIX.4.1, Required = 0)
        ClOrdID (Id = 11, Type = String, Added = FIX.2.7, Required = 0)
        OrigClOrdID (Id = 41, Type = String, Added = FIX.2.7, Required = 0)
        ClientID (Id = 109, Type = String, Added = FIX.3.0, Required = 0)
        ExecBroker (Id = 76, Type = String, Added = FIX.2.7, Required = 0)
        NoContraBrokers (Id = 382, Type = int, Added = FIX.4.2, Required = 0)
        ContraBroker (Id = 375, Type = String, Added = FIX.4.2, Required = 0)
        ContraTrader (Id = 337, Type = String, Added = FIX.4.2, Required = 0)
        ContraTradeQty (Id = 437, Type = Qty, Added = FIX.4.2, Required = 0)
        ContraTradeTime (Id = 438, Type = UTCTimestamp, Added = FIX.4.2, Required = 0)
        ListID (Id = 66, Type = String, Added = FIX.2.7, Required = 0)
        ExecID (Id = 17, Type = String, Added = FIX.2.7, Required = 1)
        ExecTransType (Id = 20, Type = char, Added = FIX.2.7, Required = 1)
        ExecRefID (Id = 19, Type = String, Added = FIX.2.7, Required = 0)
        ExecType (Id = 150, Type = char, Added = FIX.4.1, Required = 1)
        OrdStatus (Id = 39, Type = char, Added = FIX.2.7, Required = 1)
        
        ...SNIP...

        OrderQty2 (Id = 192, Type = Qty, Added = FIX.4.1, Required = 0)
        ClearingFirm (Id = 439, Type = String, Added = FIX.4.2, Required = 0)
        ClearingAccount (Id = 440, Type = String, Added = FIX.4.2, Required = 0)
        MultiLegReportingType (Id = 442, Type = char, Added = FIX.4.2, Required = 0)
        StandardTrailer {
            SignatureLength (Id = 93, Type = Length, Added = FIX.2.7, Required = 0)
            Signature (Id = 89, Type = data, Added = FIX.2.7, Required = 0)
            CheckSum (Id = 10, Type = String, Added = FIX.2.7, Required = 1)
        }
    }
}

```

## fixaudit
Utility to check the internal consistency of a FIX Orchestration or Repository and to compare a FIX Orchestration with a FIX Repository.

```
$ ./fixaudit.py --help
usage: fixaudit.py [-h] [--orchestration file] [--repository directory]

optional arguments:
  -h, --help            show this help message and exit
  --orchestration file  The orchestration to load
  --repository directory
                        A directory containing a repository to load e.g.
                        fix_repository_2010_edition_20200402/FIX.4.4/Base
```

```
$ ./fixaudit.py --repository fix_repository_2010_edition_20200402/FIX.4.2/Base
Validating repository
field tag=18 has type=MultipleValueString but there is no such data type defined
field tag=90 has type=Length but there is no such data type defined
field tag=93 has type=Length but there is no such data type defined
field tag=95 has type=Length but there is no such data type defined
field tag=212 has type=Length but there is no such data type defined
field tag=276 has type=MultipleValueString but there is no such data type defined
field tag=277 has type=MultipleValueString but there is no such data type defined
field tag=348 has type=Length but there is no such data type defined
field tag=350 has type=Length but there is no such data type defined
field tag=352 has type=Length but there is no such data type defined
field tag=354 has type=Length but there is no such data type defined
field tag=356 has type=Length but there is no such data type defined
field tag=358 has type=Length but there is no such data type defined
field tag=360 has type=Length but there is no such data type defined
field tag=362 has type=Length but there is no such data type defined
field tag=364 has type=Length but there is no such data type defined
field tag=445 has type=Length but there is no such data type defined
```

```
$ ./fixaudit.py --repository fix_repository_2010_edition_20200402/FIX.4.4/Base --orchestration orchestrations/FIX\ Standard/FixRepository44.xml
Fields Orchestration = 912 Repository = 912
All fields have the same Name and Added values in the repository and the orchestration
Messages Orchestration = 93 Repository = 93
The following 2 discrepancies were found
message MsgType = A has 46 fields in the repository and 43 fields in the orchestration
message MsgType = A repository has the following fields not in the corresponding orchestration message ['NoMsgTypes', 'MsgDirection', 'RefMsgType']```

```

## fixreptorc
Convert a FIX Repository to a FIX Orchestration

```
$ ./fixreptorc.py --help
usage: fixreptorc.py [-h] --repository directory

optional arguments:
  -h, --help            show this help message and exit
  --repository directory
                        A directory containing a repository to load e.g. fix_repository_2010_edition_20200402/FIX.4.4/Base
```

```
# Generate an orchestration from a repository
$ ./fixreptorc.py --repository fix_repository_2010_edition_20200402/FIX.4.4/Base > fix44_orchestration.xml

# Sanity check the result
$ ./fixaudit.py --repository fix_repository_2010_edition_20200402/FIX.4.4/Base --orchestration fix44_orchestration.xml
Fields Orchestration = 912 Repository = 912
All fields have the same Name and Added values in the repository and the orchestration
Messages Orchestration = 93 Repository = 93
All messages have the same Name values in the repository and the orchestration
```
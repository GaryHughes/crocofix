## lexicographer

```
uusage: lexicographer.py [-h] --namespace NAMESPACE --prefix PREFIX
                        --orchestration ORCHESTRATION

optional arguments:
  -h, --help                       Show this help message and exit
  --namespace NAMESPACE            The namespace to generate code in
  --prefix PREFIX                  The prefix for the generated filenames
  --orchestration ORCHESTRATION    THe orchestration filename to generate code for
```

The lexicographer generates 4 files of the following form in the working directory.

* `<PREFIX>`fields.hpp
* `<PREFIX>`fields.cpp
* `<PREFIX>`messages.hpp
* `<PREFIX>`messages.cpp

So this invocation `lexicographer.py --prefix FIX50SP2_ ...` will generate. 

* FIX50SP2_fields.hpp
* FIX50SP2_field.cpp
* FIX50SP2_messages.hpp
* FIX50SP2_messages.cpp

The following describes the generated code, for a more detailed decription of the usage of this code please see [libcrocofixdictionary](https://github.com/GaryHughes/crocofix/blob/master/libcrocofixdictionary/README.md).

## `<PREFIX>`fields.hpp

This file will contain a class declaration for each field defined in the orchestration, if the field has an enumerated value then the class will contain definitions for these values.

```
class Side : public crocofix::dictionary::version_field
{
public:

    constexpr static const int Tag = 54;

    Side();

    static constexpr crocofix::dictionary::field_value Buy = crocofix::dictionary::field_value("Buy", "1");
    static constexpr crocofix::dictionary::field_value Sell = crocofix::dictionary::field_value("Sell", "2");
    
    <SNIP>

};
```

A method will also be generated to support enumeration of all defined fields.

```
const crocofix::dictionary::version_field_collection& fields() noexcept;
```

## `<PREFIX>`field.cpp

The definition will initialise the base class with all the enumerated values for this field so they can be enumerated.

```
Side::Side()
: crocofix::dictionary::version_field(
    54, 
    "Side", 
    "SideCodeSet", 
    "FIX.2.7", 
    "Side of order (see Volume : 'Glossary' for value definitions)",
    {
        Buy,
        Sell,
       
        <SNIP>

    })
{{
}}
```

```
const crocofix::dictionary::version_field_collection& fields() noexcept
{
    static crocofix::dictionary::version_field_collection fields = {
        field::Account(),
        field::AdvId(),
        field::AdvRefID(),

        <SNIP>

    };

    return fields; 
}
```

## `<PREFIX>`messages.hpp

This file will contain a class declaration for each message defined in the orchestration.

```
class Heartbeat : public crocofix::dictionary::message
{
public:

    static constexpr const char* MsgType = "0";

    Heartbeat();

};
```

A method will also be generated to support enumeration of all defined messages.

```
const crocofix::dictionary::message_collection& messages() noexcept;
```

## `<PREFIX>`messages.cpp

The definition will initialise the base class with all the field definitions for this message so they can be enumerated.

```
Heartbeat::Heartbeat()
:    crocofix::dictionary::message(
         "Heartbeat", 
         "0", 
         "Session", 
         "FIX.2.7", 
         "The Heartbeat monitors the status of the communication link and identifies when the last of a string of messages was not received.",
         {
            crocofix::dictionary::message_field(field::BeginString(), 0),
            crocofix::dictionary::message_field(field::BodyLength(), 0),

            <SNIP>

         })
{
}
```


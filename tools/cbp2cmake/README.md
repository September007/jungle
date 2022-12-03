# cbp2cmake

## rule


* progress as delegation, responsible by agent
* dispatcher distribute delegation dedicated by proto-msg::type
* working agent has no context
* authorized data stored in cpp_interface::data, existing as table


## process

```cpp
CTC_Task_RAW :
   1. load CTC_Task_RAW.conf from command-line,json or whatever
   2. set the right type of CTC_Task_RAW.type
   3. Call CTC_Task_RAW.Parse() to parse out CTC_Task
      3.1 parse cbpfiles,agent
      3.2 construct CTC_Task
CTC_Task.Run()
   1. agent.Init()
   2. start message loop
      2.1 msg handler priority: agent>default_agent
      2.2 log down unhandled msg

```

## data

data store in cpp_interface::Data::Table, which is a recursible weak-type map(in some type)
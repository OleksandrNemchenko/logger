# Logger Library
This is flexible C++ library to output log information with different log levels, different log channels and different
log messages grouping.
 
## Features
- Different **log levels** are used : warning messages, debug, information, alert etc. You create and enable those levels
that appropriate to your application.
- Different **log channels** can be used : file (text, XML, JSON etc.), screen, network. Also you can select different
text coding : ASCII-characters or Unicode.
- Logger can work in a **thread safe mode**. Template parameter is used to enable this option.
- You can create "task". **Logger task** is the group of messages that are output at the task destruction. If task was
successful, only enabled log level messages will be output. In another case all messages will be output.

## Requirements
- **C++17** because of wide fold expression usage.
- **сmake** is used for library configuring. However, all targets except of avn_logger_test are interfaces, so you can
directly include them with your preferred make system.

## Installation
Clone repository :

```bash
git clone -b develop https://github.com/OleksandrNemchenko/logger
```

Logger library contains interface library only, so you can directly include headers and use them

## Roadmap
* version 1.1 :
    - create Doxygen documentation for all entities
    - create gh-pages description
* version 1.2 :
    - add library detailed description
    - add library usage examples
* version 1.3 :
    - make full test coverage

## History
* version 1.0 :
    - logger library with tasks and group entities.
# EasyHelpers Library

> [!IMPORTANT]\
> This library requires c++17
> see [extras](#extras)

This project supports the following boards:

- ESP32

## Installation

### Platformio (recommended)

You can install via the `Platformio Registry` by navigating to the `Libraries` section of `Platformio`.
The library is called `EasyHelpers` by `ZanzyTHEbar`.

if you like to install the bleading edge, in your `platformio.ini` file add the following:

```ini
lib_deps = 
    https://github.com/ZanzyTHEbar/EasyHelper.git
```

#### Dependencies used in this project

- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## Headers

- `EasyHelpers.h` - Main header file for the library, includes the `EasyHelpers.hpp` file
- `helpers/helpers.hpp` - Main helpers file, containing various string helpers
- `helpers/iter_queue.hpp` - A queue that can be iterated over
- `helpers/logger.hpp` - A logger class that can be used to log messages
- `helpers/make_unique.hpp` - A helper function to create unique pointers (not really needed in c++17 - it's legacy)
- `helpers/observer.hpp` - A class for the observer pattern
- `helpers/strategy.hpp` - A class for the strategy pattern
- `helpers/visitor.hpp` - A class for the visitor pattern
- `events/event.hpp` - A class for event managment using the `iter_queue`
- `events/event_interface.hpp` - An interface for the `event` class
- `EasyHelpers.hpp` - A header file that includes all the headers above

## Usage

> [!IMPORTANT]\
> Coming Soon

For basic usage please see the [examples](/EasyHelpers/examples) folder.

## Configuration

> [!WARNING]\
> It is **required** to add a build flag to your setup for the code to function properly.

For `platformio` add the following to your `platformio.ini` file:

```ini
build_flags = 
    -std=gnu++17
build_unflags = -std=gnu++11
```

> [!WARNING]\
> This library is still in development, if there are any bugs please report them in the issues section.

## Extras

To see any of the `log` statements used in this library - you need to add this to your `platformio.ini`:

```ini
build_flags = 
  -DASYNCWEBSERVER_REGEX # add regex support to AsyncWebServer
  -DUSE_WEBMANAGER # enable wifimanager
  -DCORE_DEBUG_LEVEL=4 # add debug logging in serial monitor
  -std=gnu++17
build_unflags = -std=gnu++11

; other build parameters
monitor_filters = 
 esp32_exception_decoder
build_type = debug
lib_ldf_mode = deep+
```

If you want to build in debug mode add this (it's not a build flag):

```ini
build_type = debug
```

## License

This library is licensed under the MIT License.

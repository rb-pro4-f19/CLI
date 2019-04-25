# Changelog
All notable changes to this project will be documented in this file.

### [Unreleased]

#### Known Issues
- `cli::log_insert()` breaks when input line not in view / scroll overflow.
- `callback_thread.detach()` in `UART_RX_STATE::VALIDATE` might break if same callback overlaps.

#### Todo
- `.write_spi()` method.
- Centralized `cli::log()` method with log level, colors etc.
- `constexpr` for expressions, common default file.
- Improved error handling and messages.

#### Added
- Add `.set_pid()` + command entry "set pid".
- Add `sys::set_pid_param()`.
- Add `.set_slew()` + command entry "set slew".
- Add `.set_bound()` + command entry "set bound".
- Add `enum` `CMD_ID` to match with MCU `CMD_TABLE`.

# Changed
- Updated semantics of set/get methods; e.g. `motor` -> `mot_id`, `uart_id` -> `cmd_id`.
- Removed `enum class SYSTEM_MODE`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.7.0] - 2019-04-20

#### Added
- Add `.set_gui()` + command entry "set gui".
- Add `.set_msg()` + command entry "set msg".
- Add `STREAM_FRAME_SIZE` `enum`.
- Add `sys::stream_handler()`.
- Add `sstr()` in `uart.h`; templated fold expression.
- Add buffer overflow flushing after `UART_BUFFER_FLUSH_TH` number of bytes.
- Add `mode_str` literal to `pan-tilt-gui`.

#### Changed
- Updated `sys::gui()` method to accommodate DPI when resizing window (untested) + updated variable names.
- Removed `string` passing by-ref from `set` & `get` methods.
- Updated `shm_def.h` structs to match `MCU` data types.
- Set `UART` baudrate to `921600`.
- Update `UART_MAX_PAYLOAD_SIZE` to `31 bytes`.
- Disabled frame transmission logging; only showing errors now.
- Updated `uart::buffer::queue_size()` to properly show bytes in queue.
- Removed `500 ms` sleep in `UART_RX_STATE::RESET` state.
- Cast struct elements to proper types for `cout` in `pan-tilt-gui`.
- Removed `gt` command entry.

#### Fixed
- Removed `reciever::callback_ack = nullptr` from `UART_RX_STATE::VALIDATE` state; used to sometimes result in dangling pointer.

<!-- ----------------------------------------------------------------------------------------- -->

### [1.6.0] - 2019-04-15

#### Added
- `shm_def.h` file for common shared memory definitions.
- `namespace shm` to `sys.h` for shared memory functionality.
- `pan-tilt-gui` project with `gui.h`.
- `sys::gui()` method + command entry "gui" for launching `pan-tilt-gui` executable + shared memory configuration; still needs complete MCU/UART implementation.

#### Changed
- Renamed solution to `pan-tilt-sys`; separated into `pan-tilt-cli` and `pan-tilt-gui`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.5.0] - 2019-04-12

#### Added
- Add `.write_array()` + command entry "write array".
- Add `.set_freq()` + command entry "set freq".
- Add `.set_mode()` + command entry "set mode".
- Add `.get_hal()` + command entry "get hal".

#### Changed
- Implemented `.queue_size()`

#### Fixed
- Size check in `.set_pwm()` and `.get_enc()` fixed; had wrong logic operator.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.4.0] - 2019-03-25

#### Added

- Add `.set_pwm()` + command entry "set pwm".
- Add `.get_enc()` + command entry "get enc".
- Add `cmd_func(x)` define for shorter lambdas; currently unused.
- 500ms delay in UART `EXCEPTION` handler state.
- Add `.split_str()` method.
- Add `.write_array()` prototype and placeholder + CMD table entry.
- Add `.write_spi()` prototype and placeholder + CMD table entry.

#### Changed

- Changed `.callback_msg()` thread lambda clause to `by value`.
- Changed `.msgbox()` to take arguments by-value.
- Changed `.msgbox()` thread lambda clause to `by value`.
- Beautified CMD table; rename define `newcmd` to `subcmd`.
- Update changelog layout to match that of `MCU/docs`.
- Cleaned up layout.
- Changed `.write()` to `.write_byte()` + updated CMD table.

#### Fixed
- Messages from CLI occasionally showed gibberish.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.3.0] - 2019-03-17

#### Added
- `cli::msgbox()` for dialog boxes.
- `cli::log_insert()` for inserting lines from thread.
- `sys::echo()` for test with MCU.
- Added proper changelog.

#### Changed
- Added some `const ...&` (by-ref) optimizations.

#### Fixed
- `uart::reciever`'s callbacks were `static`; moved definition into `.c` and updated to `extern` in `.h`.
- Callback lambda from `uart::send()` could be called too late, trying to access out memory; now reset to `nullptr` every time.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-03-15

#### Changed
- Made CLI a static class/namespace (`cli.h`).
- Made SYS a static class/namespace (`system.h`).

#### Fixed
- Remaining args had extra space (" COM3") in `cli::parse_input()`.
- 8-bit checksum generator included checksum field in iterations.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-14

#### Added
- UART static class/namespace (`uart.h`).
- Include `chksum.h` from embedded C.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2017-03-05

#### Added
- Created project.
- Basic command parser structs and methods.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.7.0]: #changelog
[1.6.0]: #changelog
[1.5.0]: #changelog
[1.4.0]: #changelog
[1.3.0]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog

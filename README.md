# **Memhole**

`memhole` is a Linux kernel module designed to intentionally de-escalate memory security. It allows any user-level (sudo or non-sudo) process to edit and read the memory of any process running on the system at kernel level, meaning **this is dangerous to install on your machine.**

## Contents
1. Dependencies
1. Build
1. Installation
1. Utilization & Developer's Guide
1. Limitations
1. Feedback and Contributing
1. License
1. Changelog

## Dependencies
The build process requires `Make`.

This module has been tested on Ubuntu 20.04 and 22.04. It may not work on other versions of Linux. It certainly does not work on Windows.

## Build
To build `memhole`, run `make` from the root repo directory.

## Installation
**NOTE:** You must install `memhole` each time you reboot. For safety, `memhole` is not automatically activated upon boot.

To install `memhole`, run `sudo ./memhole_load.sh` from the [root repo directory](.). 

If you get an error similar to:
>insmod: ERROR: could not insert module memhole.ko: Operation not permitted

you need to disable Secure Boot in your motherboard's BIOS in order for the installation process to succeed, as `memhole` is not signed.

## Utilization & Developer's Guide
The best way to interact with `memhole` is through the C wrapper located in [./wrappers](./wrappers), or the [Python wrapper](https://github.com/Wayzware/memhole-python/tree/master/memhole) (which itself is a wrapper on the C wrapper). Specific documentation and function names can be found in the well-documented source code for the wrappers.

Regardless of wrapper used, in general, `memhole` is operated by:
1. Connecting to the memhole device
    * Note: only 1 connection is allowed to `memhole` at a time.
2. Sending the PID of the process to attach to
3. Sending the memory address to start reading/writing from
4. Setting the internal buffer size (ex. 4 KB for a 4 KB read)
    * This step is handled automatically by the wrappers
5. Read or write call, reading or modifying the process's memory at kernel level with user-level permissions
    * Note: the memory position is **NOT** increased after a read/write call.
    * Under the hood, for a read, memory is transfered from target program -> kernel buffer -> program harnessing `memhole`. For writes, the process is reversed.

## Limitations
`memhole` can only have 1 active connection at a time, meaning there can only be 1 handle on the memhole device (`/dev/memhole`) active at a time. Using the C wrapper, it is possible to make calls to `memhole` from multiple threads using the safe seekmode argument (of type `memhole_mode_t`, found in [the C wrapper](wrappers/C/memhole.h)), but the system simply institutes a queue, meaning true multithreading or parallel actions are not currently supported with `memhole`.

At the moment, error checking is very limited, and most existing error checking occurs at the wrapper level. For this reason, it is highly recommended to only interact with `memhole` using a wrapper. If an invalid argument is supplied to `memhole`, undefined behavior will happen.

In versions prior to 1.3 (i.e. <= v1.2.x), `memhole` was limited to a buffer size equivalent to the system's page size (likely 4 KB). In version 1.3, the `kmalloc()` was exchanged for a `vmalloc()`, increasing the read/write limits to the amount of RAM available to the system.

## Feedback and Contributing
Memhole's git repo can be found [here](https://github.com/Wayzware/memhole).

## License
The `memhole` kernel module and all other files in this repo, with the exception of files in [./wrappers](./wrappers), are licensed under the [GPL-3.0 license](./LICENSE.md). The `memhole` kernel module is legally bound to this license due to utilizing several GPL-licensed functions. Due to this, any derivatives of `memhole` must also be licensed using GPL.

Files in [./wrappers](./wrappers) are to be used in user-space programs, and as such are not subject to the GPL license requirements that the `memhole` kernel module is bound to. Files in [./wrappers](./wrappers) are licensed under the MIT license, and as such it is not required to release source code for any programs that utilize the `memhole` wrappers.

## Changelog
The changelog can be found [here](./changelog.md)

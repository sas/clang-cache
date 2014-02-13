clang-cache [![Build Status](https://travis-ci.org/sas/clang-cache.png?branch=master)](https://travis-ci.org/sas/clang-cache)
===========

Clang caching daemon

## Building

Just type `make` :)

Or, for an out-of-tree build:
```
mkdir build
cd build
make -f ../Makefile
```

You will need GNU make, a gcc-compatible C++ compiler (clang++ works), a thrift
compiler, and the thrift runtime library for C++.

## Installing

`make install` will install everything to `/usr/local`. You can specify the
prefix with `make PREFIX=/usr install`.

## Usage

Take a look at the man page [here](doc/clang-cache.1.md).

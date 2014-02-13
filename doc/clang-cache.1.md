clang-cache
===========

## NAME

clang-cache - clang caching daemon

## SYNOPSIS

`clang-cache [ -c COMPILER | -s | -k | -f ] -- COMPILER_OPTIONS...`

## DESCRIPTION

Clang caching daemon

### -c COMPILER

Specify compiler to use (defaults to clang).

### -s

Start the daemon

### -k

Kill the daemon

### -f

Foreground mode (to be used with `-s`).

## EXAMPLES

* `clang-cache -- -c foo.c -o foo.o`
* `clang-cache -s`
* `clang-cache -k`
* `clang-cache -sf`
* `clang-cache -c g++ -- -c bar.cpp -o bar.o`

## AUTHORS

Written by Stephane Sezer `<sas@cd80.net>` and Xavier Deguillard
`<deguilx@gmail.com>`.

## COPYRIGHT

This tool is published under the BSD 3-clause license.

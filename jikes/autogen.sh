#!/bin/sh

# Run this program like so ./autogen.sh after making changes
# to the configure.in script or the aclocal.m4 file or any
# of the macros mentioned in the aclocal.m4 file. It will
# generate a new ./configure script and a new src/config.h.in

#aclocal
autoheader
autoconf
#automake

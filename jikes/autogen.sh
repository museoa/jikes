#!/bin/sh

# Run this program (./autogen.sh) after changing any of
# the files that are used to automatically generate
# other files. This includes:
# Makefile.am
# acinclude.m4
# configure.in
# any of the files in src/m4

aclocal
autoheader
autoconf
automake --ignore-deps --foreign --add-missing --force-missing

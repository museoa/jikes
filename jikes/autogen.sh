#!/bin/sh
# This software is subject to the terms of the IBM Jikes
# Compiler License Agreement available at the following URL:
# http://ibm.com/developerworks/opensource/jikes.
# Copyright (C) 2000, 2004 IBM Corporation and others.  All Rights Reserved.
# You must accept the terms of that agreement to use this software.

# Run this program (./autogen.sh) after changing any of
# the files that are used to automatically generate
# other files. This includes:
# Makefile.am
# configure.ac
# any of the files in src/m4

aclocal -I src/m4
autoconf
autoheader
automake --ignore-deps --foreign --add-missing --force-missing --copy

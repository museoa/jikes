// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//

#include "config.h"
#include <iostream.h>
#include <assert.h>
#include "control.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int return_code;

    SetNewHandler();

    FloatingPointCheck();

    ArgumentExpander *arguments = new ArgumentExpander(argc, argv);

    Option *option = new Option(*arguments);

    if (option -> first_file_index < arguments -> argc)
    {
        Control *control = new Control(*arguments, *option);
        return_code = control -> return_code;
#ifdef NO_LEAKS
        delete control;
#endif
    }
    else
    {
        fprintf(stderr,
        "\nJikes Compiler"
        "\n(C) Copyright IBM Corp. 1997, 2000.\n"
        "- Licensed Materials - Program Property of IBM - All Rights Reserved.\n\n");
        fprintf(stderr, "%s", StringConstant::U8S_command_format);
        fprintf(stderr,
        "\n\n"
        "-classpath path    use path for CLASSPATH\n"
        "-d dir             write class files in directory dir\n"
        "-debug             no effect (recognized for compatibility)\n"
        "-depend | -Xdepend recompile all used classes\n"
        "-deprecation       report uses of deprecated features\n"
        "-encoding encoding use specified encoding to read source files\n"
        "-g                 debug (generate LocalVariableTable)\n"
        "-nowarn            do not issue warning messages\n"
        "-nowrite           do not write any class files\n"
        "-O                 do not write LineNumberTable\n"
        "-verbose           list files read and written\n"
        "-Xstdout           redirect output listings to stdout\n"
        "++                 compile in incremental mode\n"
        "+B                 do not invoke bytecode generator\n"
        "+c                 do not discard comments from lexer output\n"
        "+CSO               search for both java and classfile in classpath\n"
        "+D                 report errors immediately in emacs-form without buffering\n"
        "+DR=filename       generate dependence report in filename\n"
        "+E                 list errors in emacs-form\n"
        "+F                 do full dependence check except for Zip and Jar files\n"
        "+Kname=TypeKeyWord map name to type keyword\n"
        "+M                 generate makefile dependencies\n"
        "+P                 pedantic compilation - issues lots of warnings\n"
        "+Td...d            set value of tab d...d spaces; each d is a decimal digit\n"
        "+U                 do full dependence check including Zip and Jar files\n"
        "+Z                 treat cautions as errors\n"
#ifdef TEST
        "Debugging options:\n"
        "+A                 dump AST to standard output\n"
        "+C                 dump bytecodes to standard output\n"
        "+L                 dump lexer output (stream of tokens)\n"
        "+O numbytes        call no-op op_trap() for bytecodes of the given length\n"
        "+u                 unparse AST; produces Java code for the AST\n"
        "+ud                unparse AST, with extra debugging information\n"
#endif
        "\nVersion 1.11 (10 Jan 2000)\n"
        "Originally written by Philippe Charles and David Shields \n"
        "of IBM Research, Jikes is now maintained and refined by the\n"
        "Jikes Project at http://ibm.com/developerworks/opensource.\n"
        "Please consult this URL for more information and to learn \n"
        "how to report problems.\n");

        return_code = 1;
    }

#ifdef NO_LEAKS
    delete arguments;
    delete option;
#endif

    return return_code;
}

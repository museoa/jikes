// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
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

#ifdef EBCDIC
    Code::Conversion();
#endif
    
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
        //
        // Be quiet about +A and +C. Those who know, know; those who don't, don't.
        //
        cerr << "\nIBM Research Jikes Compiler"
             << "\n(C) Copyright IBM Corp. 1997, 1998.\n"
             << "- Licensed Materials - Program Property of IBM - All Rights Reserved.\n"
             << "\nuse: jikes [-classpath path][-d dir][-debug][-depend][-deprecation]\n"
             << "  [-g][-nowarn][-nowrite][-O][-verbose]"
             << "[+1.0][+$][++][+B][+D][+E][+F]][+M][+U][+V][+Z]" 
             << "  file.java...\n\n"
             << "-classpath path    use path for CLASSPATH\n"
             << "-d dir             write class files in directory dir\n"
             << "-debug             no effect (recognized for compatibility)\n"
             << "-depend            recompile all used classes\n"
             << "-deprecation       report uses of deprecated features\n"
             << "-g                 debug (generate LocalVariableTable)\n"
             << "-nowarn            do not issue warning messages\n"
             << "-nowrite           do not write any class files\n"
             << "-O                 do not write LineNumberTable\n"
             << "-verbose           list files read and written\n"
             << "+1.0               recognize only 1.0.2 language\n"
             << "+$                 allow use of $ in 1.1 identifiers\n"
             << "++                 compile in incremental mode\n"
             << "+B                 do not invoke bytecode generator\n"
             << "+D                 report errors immediately in emacs-form without buffering\n"
             << "+E                 list errors in emacs-form\n"
             << "+F                 do full dependence check except for Zip and Jar files\n"
             << "+Kname=TypeKeyWord map name to type keyword\n"
             << "+M                 generate makefile dependencies\n"
             << "+U                 do full dependence check including Zip and Jar files\n"
             << "+V                 generate veracious listing\n"
             << "+Z                 treat cautions as errors\n"
             << "\nVersion 0.41 (01 Dec 98)"
             << " by Philippe Charles and David Shields, IBM Research.\n";
        cerr << "Please report problems to shields@watson.ibm.com.\n" ;
    cerr << "or via browser at http://www.ibm.com/research/jikes\n";

        return_code = 1;
    }

#ifdef NO_LEAKS
    delete arguments;
    delete option;
#endif

    return return_code;
}

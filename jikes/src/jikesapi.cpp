/*
 * $Id$
 */

#include "jikesapi.h"

#include "control.h"

JikesOption::~JikesOption()
{
    delete [] classpath ;
    delete [] directory ;
    delete [] encoding  ;
}

JikesOption::JikesOption():
    classpath(NULL),
    directory(NULL),
    encoding(NULL),
    nowrite(false),
    deprecation(false),
    O(false),
    g(false),
    verbose(false),
    depend(false),
    nowarn(false),
    classpath_search_order(false),
    zero_defect(false)
{
}

JikesAPI * JikesAPI::instance = NULL;

JikesAPI::JikesAPI():option(NULL)
{
    SetNewHandler();
    FloatingPointCheck();
    instance = this;
}

JikesAPI * JikesAPI::getInstance()
{
    return instance;
}


JikesAPI::~JikesAPI()
{
#ifdef NO_LEAKS
    delete option;
#endif
}

char ** JikesAPI::parseOptions(int argc, char **argv)
{

#ifdef NO_LEAKS
    delete option;
#endif

    ArgumentExpander *args=new ArgumentExpander(argc, argv);
    Option *opt = new Option(*args);
    option = opt;
    int n=args->argc - opt->first_file_index;

    if(n <= 0)
    {
        return NULL;
    }
    else
    {
        char **res=new char*[n+1];
        for(int i=0;i<n;i++)
        {
            const char *o=argv[opt->first_file_index+i];
            if(o)
            {
                res[i] = new char[strlen(o)+1];
                strcpy(res[i],o);
            } else
            {
                res[i] = NULL;
                break;
            }
        }
        res[n] = NULL;
        return res;
    }
}

JikesOption *JikesAPI::getOptions()
{
    return option;
}

/**
 * Compile given list of files.
 */
int JikesAPI::compile(char **filenames)
{
    Control *control = new Control(filenames , *((Option*)option));
    int return_code = control -> return_code;

#ifdef NO_LEAKS
    delete control;
#endif

    return return_code;
}

/**
 * This method will be called for each error reported.
 */
void JikesAPI::reportError(JikesError *error)
{
    Coutput << error->getErrorReport();
    Coutput.flush();
}

const char *JikesError::getSeverityString() 
{
    switch(getSeverity())
    {
    case ERROR  : return "Error"   ;
    case WARNING: return "Warning" ;
    case CAUTION: return "Caution" ;
    default: "Unknown";
    }
}



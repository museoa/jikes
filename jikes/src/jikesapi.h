/*
 * $Id$
 */

#ifndef _JIKES_API_H_FLAG_
#define _JIKES_API_H_FLAG_

class JikesOption
{
    
public:
    
    char *classpath    ;
    char *directory    ;
    char *encoding     ;
    
    bool nowrite;
    bool deprecation;
    bool O;
    bool g;
    bool verbose;
    bool depend;
    bool nowarn;
    bool classpath_search_order;
    bool zero_defect;

    virtual ~JikesOption();

 protected:
    
    JikesOption();
};

class JikesError
{

 public:

    enum JikesErrorSeverity
    {
        ERROR,
        CAUTION,
        WARNING,
    };
        
    virtual JikesErrorSeverity getSeverity() = 0 ;
    virtual const char *getFileName() = 0 ;
    
    virtual int getLeftLineNo      () = 0 ;
    virtual int getLeftColumnNo    () = 0 ;
    virtual int getRightLineNo     () = 0 ;
    virtual int getRightColumnNo   () = 0 ;

    /**
     * Returns message describing error.
     */
    virtual const wchar_t *getErrorMessage() = 0;

    /**
     * Returns formatter error report. 
     */
    virtual const wchar_t *getErrorReport() = 0;

 protected:

    const char *getSeverityString();
};

/**
 * API to jikes compiler.
 */
class JikesAPI
{
 public:

    JikesAPI();
    
    virtual ~JikesAPI();
       
    /**
     * Returns instance of current compiler options.
     * returned pointer can be used to modify current
     * compiler options.
     */
    virtual JikesOption *getOptions();
    
    /**
     * Creates instance of compiler options,
     * corresponding to given command line parameters.
     *
     * @return list of java file names found on command line
     */
    virtual char ** parseOptions(int argc, char **argv) ;

    /**
     * Compile given list ofiles using current compiler options.
     */
    virtual int compile(char ** filenames);

    /*
     * Jikes API implements singelton pattern.
     * This is a way to get instance of it.
     */
    static JikesAPI * getInstance();

    /**
     * This method will be called for each error reported.
     */
    virtual void reportError(JikesError *error);
    
 protected:
    
/*    virtual void stat(struct stat &status, const char *filename) = 0; */
/*    virtual const char * read(const char *filename)              = 0; */
/*    virtual void write(const char *filename, const char *data, size_t bytes, int n) = 0; */
    
 private:

    JikesOption *option;
    static JikesAPI *instance;
    
};

#endif // _JIKES_API_H_FLAG_

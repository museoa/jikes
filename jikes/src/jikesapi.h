/*
 * $Id$
 */

#ifndef _JIKES_API_H_FLAG_
#define _JIKES_API_H_FLAG_


#include "stdlib.h"

#ifdef UNIX_FILE_SYSTEM
#include <stdio.h>
#elif defined( WIN32_FILE_SYSTEM )
#include <windows.h>
#endif


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
        JIKES_ERROR,
        JIKES_CAUTION,
        JIKES_WARNING
    } ;
        
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

	  /*
	   * Define the virtual abse class for all ReadObjects.
	   * A pointer to an object of this type is returned by JikesAPI::read()
	   */
      class FileReader
      {
         public:
            virtual  ~FileReader()  {}

            virtual const char     *GetBuffer()      = 0;	// If the file is unreadable an object should still be created but GetBuffer() should return NULL.
            virtual       size_t    GetBufferSize()  = 0;	// If the file is unreadable GetBufferSize() is undefined.
      };

	  /*
	   * A default implamentation of ReadObject that read from the file sysytem.
	   */ 
      class DefaultFileReader: public FileReader
      {
         public:      DefaultFileReader(const char *fileName);
            virtual  ~DefaultFileReader();

            virtual const char     *GetBuffer()      {return(buffer);}
            virtual       size_t    GetBufferSize()  {return(size);}
         private:
            const char     *buffer;
                  size_t    size;
#ifdef   WIN32_FILE_SYSTEM
                  HANDLE    srcfile;
                  HANDLE    mapfile;
#endif 
      };

      /*
       * Define the virtual base class for all WriteObjects.
       * A pointer to an object of this type is returned by JikesAPI::write()
       */
      class FileWriter
      {
         public:      FileWriter(size_t mS):   maxSize(mS) {} 
            virtual  ~FileWriter() {}

            size_t    Write(const unsigned char *data,size_t size);
            virtual  bool      Valid()                                 = 0;
         private:
            virtual  size_t    DoWrite(const unsigned char *data,size_t size)   = 0;	// Garanteed not to be called with a combined total of more than maxSize bytes during the lifespan of the object.
            size_t   maxSize;
      };

	  /*
	   * A default implamentaion of WriteObject that writes to the file system.
	   */
      class DefaultFileWriter: public FileWriter
      {
         public:      DefaultFileWriter(const char *fileName,size_t maxSize);
            virtual  ~DefaultFileWriter();

            virtual  bool      Valid();
         private:
            virtual  size_t    DoWrite(const unsigned char *data,size_t size);

                     bool      valid;
#ifdef UNIX_FILE_SYSTEM
                     FILE     *file;
#elif defined(WIN32_FILE_SYSTEM)
                     HANDLE    file;
                     HANDLE    mapfile;
                     u1       *string_buffer;
                     size_t    dataWritten;
#endif
      };

	  // These functions are used internally by Jikes to retrieve file information
	  // from the JikesAPI.

      virtual	FileReader * readerFactory(const char *fileName);
      virtual	FileWriter * writterFactory(const char *fileName, size_t bytes);

  public:
      virtual	  int stat(const char *filename,struct stat *status);
      
      FileReader  * read  (const char *filename              );
      FileWriter  * write (const char *filename, size_t bytes);
      
 private:
    JikesOption *option;
    static JikesAPI *instance;
};

#endif // _JIKES_API_H_FLAG_

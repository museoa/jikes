// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef stream_INCLUDED
#define stream_INCLUDED

#include "config.h"
#include <sys/stat.h>
#include <limits.h>
#include <iostream.h>
#include <stddef.h>
#include <stdio.h>
#include "javadef.h"
#include "javasym.h"
#include "tuple.h"
#include "bool.h"
#include "tab.h"
#include "lookup.h"
#include "unicode.h"

class Control;
class Input_info;
class Scanner;
class Symbol;
class FileSymbol;
class ZipFile;

class StreamError
{
public:
    enum StreamErrorKind
    {
        BAD_TOKEN,
        BAD_OCTAL_CONSTANT,
        EMPTY_CHARACTER_CONSTANT,
        UNTERMINATED_CHARACTER_CONSTANT,
        UNTERMINATED_COMMENT,
        UNTERMINATED_STRING_CONSTANT,
        INVALID_HEX_CONSTANT,
        INVALID_FLOATING_CONSTANT_EXPONENT,
        INVALID_UNICODE_ESCAPE
    };

    unsigned start_location,
             end_location;
    StreamErrorKind kind;

    void Initialize(StreamErrorKind kind_, unsigned start_location_, unsigned end_location_)
    {
        kind = kind_;
        start_location = start_location_;
        end_location = end_location_;

        return;
    }
};


//
// LexStream holds a stream of tokens generated from an input and 
// provides methods to retrieve information from the stream.
//
class LexStream
{
public:
    typedef int TypeIndex;
    typedef int TokenIndex;
    typedef int CommentIndex;
    enum { INFINITY = INT_MAX }; // the largest possible value for TokenIndex

    FileSymbol *file_symbol;
 
    inline TokenIndex Next(TokenIndex i)
         { return (++i < token_stream.Length() ? i : token_stream.Length() - 1); }

    inline TokenIndex Previous(TokenIndex i) { return (i <= 0 ? 0 : i - 1); }

    inline TokenIndex Peek() { return Next(index); }

    inline void Reset(TokenIndex i = 1) { index = Previous(i); }

    inline TokenIndex Gettoken() { return index = Next(index); }

    inline TokenIndex Gettoken(TokenIndex end_token)
         { return index = (index < end_token ? Next(index) : token_stream.Length() - 1); }

    inline TokenIndex Badtoken() { return 0; }

    inline unsigned Kind(TokenIndex i) { return tokens[i].Kind(); }

    inline unsigned Location(TokenIndex i) { return tokens[i].Location(); }

    inline unsigned Line(TokenIndex i) { return FindLine(tokens[i].Location()); }

    inline unsigned Column(TokenIndex i) { return columns ? columns[i] : (input_buffer ? FindColumn(tokens[i].Location()) : 0); }

    inline bool AfterEol(TokenIndex i) { return (i < 1 ? true : Line(i - 1) < Line(i)); }

    inline TokenIndex MatchingBrace(TokenIndex i) { return tokens[i].additional_info.right_brace; }

    inline Symbol *NameSymbol(TokenIndex i) { return (Kind(i) != TK_LBRACE ? tokens[i].additional_info.symbol : (Symbol *) NULL); }

    wchar_t *Name(TokenIndex i)
    {
        return (NameSymbol(i) ? tokens[i].additional_info.symbol -> Name() : KeywordName(tokens[i].Kind()));
    }

    int NameLength(TokenIndex i)
    {
        return (NameSymbol(i) ? tokens[i].additional_info.symbol -> NameLength() : wcslen(KeywordName(tokens[i].Kind())));
    }

    char *FileName();
    int FileNameLength();

    inline int LineLength(unsigned line_no) { return locations[line_no + 1] - locations[line_no]; }
    inline int LineStart(unsigned line_no)  { return locations[line_no]; }
    inline int LineEnd(unsigned line_no)    { return locations[line_no + 1] - 1; }

    inline int LineSegmentLength(TokenIndex i)
        { return Tab::Wcslen(input_buffer, tokens[i].Location(), LineEnd(Line(i))); }

    wchar_t *InputBuffer() { return input_buffer; }
    size_t InputBufferLength() { return input_buffer_length; }

    CommentIndex FirstComment(TokenIndex);

    inline int NumTypes() { return type_index.Length(); }
    inline TokenIndex Type(int i) { return types[i]; }

    inline int NumTokens() { return token_stream.Length(); }

    inline int NumComments() { return comment_stream.Length(); }

    inline TokenIndex PrecedingToken(CommentIndex i) { return comments[i].previous_token; }

    inline unsigned CommentLocation(CommentIndex i)  { return comments[i].location; }

    inline wchar_t *CommentString(CommentIndex i)    { return comments[i].string; }

    inline int CommentStringLength(CommentIndex i)   { return comments[i].length; }

    inline int NumBadTokens() { return bad_tokens.Length(); }

#ifdef TEST
    int file_read;
#endif

    //*
    //* Constructors and Destructor.
    //*
    LexStream(Control &control_, FileSymbol *file_symbol_) : control(control_),
                                                             file_symbol(file_symbol_),
                                                             initial_reading_of_input(true),
                                                             input_buffer(NULL),
                                                             input_buffer_length(0),
                                                             comment_buffer(NULL),
                                                             token_stream(12, 16),
                                                             tokens(NULL),
                                                             columns(NULL),
                                                             comment_stream(10, 8),
                                                             comments(NULL),
                                                             line_location(12, 8),
                                                             locations(NULL)
#ifdef TEST
                                                           , file_read(0)
#endif
    {}

    bool ComputeColumns()
    {
        RereadInput();
        if (input_buffer)
            InitializeColumns();

        DestroyInput();

        return (columns != NULL);
    }

    void RereadInput();
    ~LexStream();


    void DestroyInput()
    {
        delete [] input_buffer;
        input_buffer = NULL;

        delete [] comment_buffer;
        comment_buffer = NULL;
    }

    void SortMessages();
    void PrintMessages();
    void PrintEmacsMessage(int);
    void PrintSmallSource(int);
    void PrintLargeSource(int);
    void PrintMessage(StreamError::StreamErrorKind);

    void SetUpComments()
    {
        RereadInput();
        //
        // Calculate the length of the string required to save the comments.
        // Allocate the buffer, save the comments in the buffer and update their
        // respective "string" pointer.
        //
        int length = 0,
            i;

        for (i = 1; i < comment_stream.Length(); i++)
            length += (comments[i].length + 1);
        comment_buffer = new wchar_t[length];
        wchar_t *ptr = comment_buffer;
        for (i = 1; i < comment_stream.Length(); i++)
        {
            memmove(ptr, &(input_buffer[comments[i].location]), comments[i].length * sizeof(wchar_t));
            comments[i].string = ptr;
            ptr += comments[i].length;
            *ptr++ = U_NULL;
        }

        return;
    }

#ifdef TEST
void LexStream::Dump(); // temporary function used to dump token stream.
#endif

    //
    // Return the total size of space allocated for the tokens.
    //
    size_t TokenSpaceAllocated(void)
    {
        return token_stream.Length() * sizeof(Token);
    }

    //
    // Return the total size of space allocated for the comments.
    //
    size_t CommentSpaceAllocated(void)
    {
        return comment_stream.Length() * sizeof(Comment);
    }

private:

    friend class Scanner;

    class Token
    {
    public:
        unsigned info;
        union
        {
            Symbol   *symbol;
            TokenIndex right_brace;
        } additional_info;

        inline void SetLocation(unsigned location) { info = (info & 0x000000FF) | (location << 8); }
        inline unsigned Location()                 { return (info >> 8); }
        inline void SetKind(unsigned kind)         { info = (info & 0xFFFFFF80) | kind; }
        inline unsigned Kind()                     { return (info & 0x0000007F); }
        
        //
        // Note that the bit 0x00000080 is currently unused. It might be used in the future
        // either to record some boolean fact about a token, To allow the number of token
        // kinds to be expanded from 127 to 255 or to allow the number of input locations
        // to be expanded from 16M to 32M.
        //

        inline void SetSymbol(Symbol *symbol)             { additional_info.symbol = symbol; }
        inline void SetRightBrace(TokenIndex right_brace) { additional_info.right_brace = right_brace; }
    };

    class Comment
    {
    public:
        TokenIndex previous_token;
        unsigned   location;
        unsigned   length;
        wchar_t   *string;
    };

    Tuple<StreamError> bad_tokens;

    TokenIndex index;
    Token *tokens;
    unsigned short *columns;
    ConvertibleArray<Token> token_stream;
    Comment *comments;
    ConvertibleArray<Comment> comment_stream;
    unsigned *locations;
    ConvertibleArray<unsigned> line_location;
    TokenIndex *types;
    ConvertibleArray<TokenIndex> type_index;

    void InitializeColumns();
    void CompressSpace();

    bool initial_reading_of_input;

    wchar_t *input_buffer;
    size_t input_buffer_length;
    wchar_t *comment_buffer;

    Control &control;

    void ReadInput();
    void ProcessInput(char *, long);

    wchar_t *KeywordName(int);

    unsigned FindLine(unsigned location);

    unsigned FindColumn(unsigned location)
    {
assert(locations);
        return Tab::Wcslen(input_buffer, locations[FindLine(location)], location);
    }
};

#endif

// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef class_INCLUDED
#define class_INCLUDED

#include "config.h"
#include <stdio.h>
#include "semantic.h"
#include "access.h"
#include "unicode.h"
#include "tuple.h"

class cp_info
{
public:
    u1 tag;

    cp_info(u1 _tag) : tag(_tag) {}

    virtual ~cp_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
         assert("trying to put unsupported attribute kind" == NULL);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << (int) tag;
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << (int) tag;
    }
#endif
};


class CONSTANT_Class_info : public cp_info
{
public:
    /* u1 tag; */
    u2 name_index;

    CONSTANT_Class_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Class_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(name_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Class_info: name_index " << name_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "Class:";  constant_pool[name_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_Double_info : public cp_info
{
public:
    /* u1 tag; */
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Double_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Double_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB4(high_bytes);
        output_buffer.PutB4(low_bytes);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Float_info: bytes  \n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "D:";
//        constant_pool[string_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_Fieldref_info : public cp_info
{
public:
    /* u1 tag; */
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_Fieldref_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Fieldref_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
         output_buffer.PutB1(tag);
         output_buffer.PutB2(class_index);
         output_buffer.PutB2(name_and_type_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Fieldref_info: class_index: " << class_index
             << ", name_and_type_index: " << name_and_type_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        constant_pool[class_index]->describe(constant_pool); cout << ".";
        constant_pool[name_and_type_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_Float_info : public cp_info
{
public:
    /* u1 tag; */
    u4 bytes;

    CONSTANT_Float_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Float_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB4(bytes);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Float_info: bytes " << (float) bytes << "\n"; //DSDouble
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "F:";
//        constant_pool[string_index]->describe(constant_pool);
    }
#endif    
};


class CONSTANT_Integer_info : public cp_info
{
public:
    /* u1 tag; */
    u4 bytes;

    CONSTANT_Integer_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Integer_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB4(bytes);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
    int val;
    val = ((bytes>>24)&0xff)<<24 | ((bytes>>16)&0xff)<<16
        | ((bytes>>8)&0xff)<<8 | (bytes&0xff);
//        cout << "CONSTANT_Integer_info: bytes " <<  value << "\n";
        cout << "CONSTANT_Integer_info: bytes " << val << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "I:";
//        constant_pool[string_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_InterfaceMethodref_info : public cp_info
{
public:
    /* u1 tag; */
    u2 class_index;
    u2 name_and_type_index;

    CONSTANT_InterfaceMethodref_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_InterfaceMethodref_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(class_index);
        output_buffer.PutB2(name_and_type_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_InterfaceMethodref_info: class_index: " << class_index
             << ", name_and_type_index: " << name_and_type_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        constant_pool[class_index]->describe(constant_pool); cout << ".";
        constant_pool[name_and_type_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_Long_info : public cp_info
{
public:
    /* u1 tag; */
    u4 high_bytes;
    u4 low_bytes;

    CONSTANT_Long_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Long_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB4(high_bytes);
        output_buffer.PutB4(low_bytes);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Long_info: bytes \n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "L:";
//        constant_pool[string_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_Methodref_info : public cp_info
{
public:
    /* u1 tag; */
    u2 class_index;
    u2 name_and_type_index;


    CONSTANT_Methodref_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_Methodref_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(class_index);
        output_buffer.PutB2(name_and_type_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_Methodref_info: class_index: " << class_index
             << ", name_and_type_index: " << name_and_type_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        constant_pool[class_index]->describe(constant_pool); cout << ".";
        constant_pool[name_and_type_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_NameAndType_info : public cp_info
{
public:
    /* u1 tag; */
    u2 name_index;
    u2 descriptor_index;

    CONSTANT_NameAndType_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_NameAndType_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(name_index);
        output_buffer.PutB2(descriptor_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_NameAndType_info: name_index: " << name_index
             << ", descriptor_index: " << descriptor_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        constant_pool[name_index]->describe(constant_pool);
        cout << " ";
        constant_pool[descriptor_index]->describe(constant_pool);
    }
#endif    
};

class CONSTANT_String_info : public cp_info
{
public:
    /* u1 tag; */
    u2 string_index;

    CONSTANT_String_info(u1 _tag) : cp_info(_tag) {}
    virtual ~CONSTANT_String_info() {}

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(string_index);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "CONSTANT_String_info: string_index: " << string_index << "\n";
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        constant_pool[string_index]->describe(constant_pool);
    }
#endif
};

class CONSTANT_Utf8_info : public cp_info
{
public:
    /* u1 tag; */
    u2 length() { return length_; }
    char *bytes; /* bytes[length+1] ... after input a '\0' will be added. */

    CONSTANT_Utf8_info(u1 _tag) : cp_info(_tag) {}

    virtual ~CONSTANT_Utf8_info()
    {
        delete [] bytes;
    }

    virtual void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB1(tag);
        output_buffer.PutB2(length());
        for (int i = 0; i < length(); i++)
            output_buffer.PutB1(bytes[i]);
    }

public:
    u2 length_;
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        
        cout << "CONSTANT_Utf8_info: length: " << length_<< " ";
        for (int i=0;i<length_;i++) Unicode::Cout(bytes[i]);
        cout << "\n";
        // should only do packing when actually write the string out!!
        // so have same common internal form on input and output.
    }
    virtual void describe(Tuple<cp_info *>& constant_pool) {
        cout << "\"";
        for (int i=0;i<length_;i++) Unicode::Cout(bytes[i]);
        cout << "\"";
//        Unicode::Cout(bytes);
    }
#endif    
};

// field_info and method_infoshould be defined here, but they contain attributes, so it is necessary
// to define the attributes first.


class attribute_info
{
public:
    enum 
    {
        Generic,
        Code,
        ConstantValue,
        Deprecated,
        Exceptions,
        InnerClasses,
        LineNumberTable,
        LocalVariableTable,
        SourceFile,
        Synthetic
    };

    u1 tag;
    u2 attribute_name_index;
    u4 attribute_length;

    attribute_info(u1 _tag) : tag(_tag) {}
    attribute_info(u1 _tag, u2 _name_index, u4 _length) :
            tag(_tag), attribute_name_index(_name_index),
            attribute_length(_length) {}

    virtual ~attribute_info() {};

    virtual void put(OutputBuffer &output_buffer)
    {
        assert(0);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "print for attribute info tag " << tag << " not defined\n";
    }
#endif    
};

class GenericAttribute_info : public attribute_info
{
public:
//    u2 attribute_name_index;
    u4 attribute_length() { return info.Length(); }
    Tuple<u1> info; /* info[attribute_length] */

    GenericAttribute_info(u2 &_name_index, u4 &_length) : attribute_info(Generic, _name_index, _length),
                                                          info(8, 4)
    {}

    virtual ~GenericAttribute_info() { }

    virtual void put(OutputBuffer &output_error)
    {
        assert(0);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "print for attribute info tag " << tag << " not defined\n";
    }
#endif    
};

class Code_attribute : public attribute_info
{
public:
    Code_attribute() : attribute_info(Code,0,0),
                       max_stack(0),
                       max_locals(0),
                       code(8, 4),
                       exception_table(6, 16),
                       attributes(6, 16)
    {}

    Code_attribute(u2 &_name_index, u4 &_length) : attribute_info(Code,_name_index,_length),
                                                   code(8, 4),
                                                   exception_table(6, 16),
                                                   attributes(6, 16)
    {}

    virtual ~Code_attribute()
    {
        for (int i = 0; i < attributes.Length(); i++)
            delete attributes[i];
    }

//    u2 attribute_name_index;
//    u4 attribute_length;
    u2 max_stack;
    u2 max_locals;
    /* u4 code_length; */
    Tuple<u1> code; /* code[code_length] */

    u2 exception_table_length() { return exception_table.Length(); }
    struct exception_element
    {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
    };
    Tuple<exception_element> exception_table; /* exceptiontable[exception_table_length] */

    u2 attributes_count() { return attributes.Length(); }
    Tuple<attribute_info *> attributes; /* attributes[attributes_count] */

    virtual void put(OutputBuffer &output_buffer)
    {
        int i;
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(max_stack);
        output_buffer.PutB2(max_locals); 
        output_buffer.PutB4(code.Length());

        for (i = 0; i < code.Length(); i++) 
            output_buffer.PutB1(code[i]);
        output_buffer.PutB2(exception_table.Length());

        for (i = 0; i < exception_table.Length(); i++)
        {
            output_buffer.PutB2(exception_table[i].start_pc);
            output_buffer.PutB2(exception_table[i].end_pc);
            output_buffer.PutB2(exception_table[i].handler_pc);
            output_buffer.PutB2(exception_table[i].catch_type);
        }

        output_buffer.PutB2(attributes.Length());
        for (i = 0; i < attributes.Length(); i++)
            attributes[i] -> put(output_buffer);

        return;
    }
                
#ifdef TEST
    virtual void  print(Tuple<cp_info *>& constant_pool)
    {

        void  opdmp(Tuple<cp_info *>& constant_pool, Tuple<u1>& code);
        int i;
        cout << "Code_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length << "\n";
        cout << " max_stack " << max_stack <<
            " max_locals "  << max_locals <<
            " code_length "  << code.Length() << "\n";
        if (exception_table.Length()) {
            cout << " exception_table: " << exception_table.Length() << " entries\n";
            for (i=0; i<exception_table.Length(); i++) {
                cout <<
                "  start_pc " << exception_table[i].start_pc <<
                "  end_pc " << exception_table[i].end_pc <<
                "  handler_pc " << exception_table[i].handler_pc <<
                "  catch_type " << exception_table[i].catch_type << "\n";
//                const_string(cp,exception_table[i].catch_type);
            }
        }
        opdmp(constant_pool,code);
        cout << "  \n";
        for (i=0;i<attributes.Length();i++) {
            attributes[i] -> print(constant_pool);
        }
    }
#endif    

};

class ConstantValue_attribute : public attribute_info
{
public:
    ConstantValue_attribute(u2 &_name_index, u4 &_length) : attribute_info(ConstantValue,_name_index,_length)
    {}

//    u2 attribute_name_index;
//    u4 attribute_length; /* must be 2 */
    u2 constantvalue_index;

    virtual ~ConstantValue_attribute() {}

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(constantvalue_index);
    }
                
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "ConstantValue_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length
             << " constantvalue_index " << constantvalue_index << "\n";
    }
#endif    
};

class Deprecated_attribute : public attribute_info
{
public:
//    u2 attribute_name_index;
//    u4 attribute_length; /* must be 0 */

    Deprecated_attribute(u2 &_name_index, u4 &_length) : attribute_info(Deprecated,_name_index,_length)
    {}

    virtual ~Deprecated_attribute() {}
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "Deprecated_attribute attribute_name_index " << attribute_name_index
             << " length " << attribute_length
             << "\n";
    }
#endif    
};

class Exceptions_attribute : public attribute_info
{
public:
//    u2 attribute_name_index;
//    u4 attribute_length; /* must be 2 */
     u2 number_of_exceptions() { return exception_index_table.Length(); }
    Tuple<u2> exception_index_table; /* exception_index_table[number_of_exceptions] */

    Exceptions_attribute(u2 &_name_index, u4 &_length)
              : attribute_info(Exceptions,_name_index, _length) {}
     virtual ~Exceptions_attribute() { }

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(exception_index_table.Length());
        for (int i = 0; i< exception_index_table.Length(); i++)
            output_buffer.PutB2(exception_index_table[i]);
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "Exceptions_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length << "\n";
        for (int i=0; i<exception_index_table.Length(); i++) {
            cout << "    " << exception_index_table[i];
        }
        cout << "\n";
    }
#endif    
};

class InnerClasses_attribute : public attribute_info
{
public:
    InnerClasses_attribute(u2 &_name_index, u4 &_length) : attribute_info(InnerClasses,_name_index,_length),
                                                           inner_classes(6, 16)
    {}

    virtual ~InnerClasses_attribute() {}

//    u2 attribute_name_index;
//    u4 attribute_length;
     u2 inner_classes_length() { return inner_classes.Length();}
    struct inner_classes_element
    {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;
    };
    Tuple<inner_classes_element> inner_classes; /* inner_classes_table[inner_classes_table_length] */

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(inner_classes.Length());
        for (int i = 0; i < inner_classes.Length(); i++)
        {
            output_buffer.PutB2(inner_classes[i].inner_class_info_index);
            output_buffer.PutB2(inner_classes[i].outer_class_info_index);
            output_buffer.PutB2(inner_classes[i].inner_name_index);
            output_buffer.PutB2(inner_classes[i].inner_class_access_flags);
        }
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "InnerClasses_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length << "\n";
        cout << " inner_classes_length " << inner_classes.Length() <<"\n";
        for (int i=0; i<inner_classes.Length(); i++) {
            cout << "     " << i << "  inner_class_info_index " << inner_classes[i].inner_class_info_index
                 << "  outer_class_info_index " << inner_classes[i].outer_class_info_index
                 << "  inner_name_index " << inner_classes[i].inner_name_index
                 << "  inner_class_access_flags " << inner_classes[i].inner_class_access_flags
                 << "\n";
        }
    }
#endif    
};

class LineNumberTable_attribute : public attribute_info
{
public:
    LineNumberTable_attribute(u2 &_name_index, u4 &_length) : attribute_info(LineNumberTable,_name_index,_length),
                                                              line_number_table(6, 16)
    {}

//    u2 attribute_name_index;
//    u4 attribute_length;
     u2 line_number_table_length() { return line_number_table.Length();}
    struct line_number_element
    {
        u2 start_pc;
        u2 line_number;
    };
    Tuple<line_number_element> line_number_table; /* line_number_table[line_number_table_length] */

    virtual ~LineNumberTable_attribute() {}

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(line_number_table.Length());
        for (int i = 0; i < line_number_table.Length(); i++)
        {
            output_buffer.PutB2(line_number_table[i].start_pc);
            output_buffer.PutB2(line_number_table[i].line_number);
        }
    }

#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "LineNumberTable_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length << "\n";
        cout << " line_number_table_length " << line_number_table.Length() <<"\n";
        for (int i=0; i<line_number_table.Length(); i++) {
            cout << "     " << i << "  start_pc " << line_number_table[i].start_pc
                    << "  line_number " << line_number_table[i].line_number << "\n";
        }
    }
#endif    
};


class LocalVariableTable_attribute : public attribute_info
{
public:
    LocalVariableTable_attribute(u2 &_name_index, u4 &_length) : attribute_info(LocalVariableTable,_name_index,_length)
    {}

//    u2 attribute_name_index;
//    u4 attribute_length;
     u2 local_variable_table_length() { return local_variable_table.Length();}
    struct local_variable_element
    {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
    };
    Tuple<local_variable_element> local_variable_table; /* local_variable_table[local_variable_table_length] */

    virtual ~LocalVariableTable_attribute() { }

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(local_variable_table.Length());
        for (int i = 0; i < local_variable_table.Length(); i++)
        {
            output_buffer.PutB2(local_variable_table[i].start_pc);
            output_buffer.PutB2(local_variable_table[i].length);
            output_buffer.PutB2(local_variable_table[i].name_index);
            output_buffer.PutB2(local_variable_table[i].descriptor_index);
            output_buffer.PutB2(local_variable_table[i].index);
        }
    }
                
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "LocalVariableTable_attribute attribute_name_index " << attribute_name_index
             << " attribute_length " << attribute_length << "\n";
        cout << " local_variable_table_length " << local_variable_table.Length() <<"\n";
        for (int i=0; i<local_variable_table.Length(); i++) {
            cout << "     " << i << "  start_pc " << local_variable_table[i].start_pc
                 << "  length " << local_variable_table[i].length
                 << "  name_index " << local_variable_table[i].name_index
                 << "  descriptor_index " << local_variable_table[i].descriptor_index
                 << "  index " << local_variable_table[i].index << "\n";
        }
    }
#endif    
};

class SourceFile_attribute : public attribute_info
{
public:
//    u2 attribute_name_index;
//    u4 attribute_length; /* must be 2 */
    u2 sourcefile_index;

    SourceFile_attribute(u2 &_name_index, u4 &_length) : attribute_info(SourceFile,_name_index,_length)
    {}

    virtual ~SourceFile_attribute() {}

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
        output_buffer.PutB2(sourcefile_index);
    }
                
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "SourceFile_attribute attribute_name_index " << attribute_name_index
             << " length " << attribute_length
             << " sourcefile_index " << sourcefile_index << "\n";
    }
#endif    
};

class Synthetic_attribute : public attribute_info
{
public:
//    u2 attribute_name_index;
//    u4 attribute_length; /* must be 0 */

    Synthetic_attribute(u2 &_name_index, u4 &_length) : attribute_info(Synthetic,_name_index,_length)
    {}

    virtual ~Synthetic_attribute() {}

    virtual void put(OutputBuffer &output_buffer)
    {
assert(attribute_name_index != 0);

        output_buffer.PutB2(attribute_name_index);
        output_buffer.PutB4(attribute_length);
    }
                
#ifdef TEST
    virtual void print(Tuple<cp_info *>& constant_pool) {
        cout << "Synthetic_attribute attribute_name_index " << attribute_name_index
             << " length " << attribute_length
             << "\n";
    }
#endif    
};


class field_info : public AccessFlags
{
public:
    /* u2 access_flags; */
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count() { return attributes.Length();}
    Tuple<attribute_info *> attributes; /* attributes[attributes_count] */

     ~field_info()
     {
         for (int i = 0; i < attributes.Length(); i++)
             delete attributes[i];
     }

    inline void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB2(access_flags);
        output_buffer.PutB2(name_index);
        output_buffer.PutB2(descriptor_index);
        output_buffer.PutB2(attributes.Length());

        for (int ai = 0; ai < attributes.Length(); ai++)
            attributes[ai] -> put(output_buffer);
    }

#ifdef TEST
    void print(Tuple<cp_info *>& constant_pool) {
        cout << "field_info  name_index " << name_index
             << "  descriptor_index " << descriptor_index << "\n";
        AccessFlags::Print();
        for (int i=0; i<attributes.Length(); i++) {
            attributes[i]->print(constant_pool);
        }
        cout << "\n";
    }
#endif    
};


class method_info : public AccessFlags
{
public:
    /* u2 access_flags; */
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count() { return attributes.Length(); }
    Tuple<attribute_info *> attributes; /* attributes[attributes_count] */

    ~method_info()
    {
        for (int i = 0; i < attributes.Length(); i++)
            delete attributes[i];
    }

    inline void put(OutputBuffer &output_buffer)
    {
        output_buffer.PutB2(access_flags);
        output_buffer.PutB2(name_index);
        output_buffer.PutB2(descriptor_index);
        output_buffer.PutB2(attributes.Length());

        for (int ai = 0; ai < attributes.Length(); ai++)
            attributes[ai] -> put(output_buffer);
    }

#ifdef TEST
     void print(Tuple<cp_info *>& constant_pool) {
        cout << "method_info  name_index " << name_index
             << "  descriptor_index " << descriptor_index << "\n";
        AccessFlags::Print();
        for (int i=0; i<attributes.Length(); i++) {
            attributes[i]->print(constant_pool);
        }
        cout << "\n";
    }
#endif    
};


class ClassFile : public AccessFlags
{
public:
    enum 
    {
        CONSTANT_Class              = 7,
        CONSTANT_Fieldref           = 9,
        CONSTANT_Methodref          = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_String             = 8,
        CONSTANT_Integer            = 3,
        CONSTANT_Float              = 4,
        CONSTANT_Long               = 5,
        CONSTANT_Double             = 6,
        CONSTANT_NameAndType        = 12,
        CONSTANT_Utf8               = 1
    };

    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count() { return constant_pool.Length(); }
    Tuple<cp_info *> constant_pool; /* cp_info[constant_pool_count] */
/*    u2 access_flags; */
    u2 this_class;
    u2 super_class;
    u2 interfaces_count() { return interfaces.Length(); }
    Tuple<u2> interfaces; /* interfaces[interfaces_count] */
    u2 fields_count() { return fields.Length(); }
    Tuple<field_info> fields; /* fields[fields_count] */
    u2 methods_count() { return methods.Length();}
    Tuple<method_info> methods; /* methods[methods_count] */
    u2 attributes_count() { return attributes.Length(); }
    Tuple<attribute_info *> attributes; /* attributes[attributes_count] */

    ClassFile(TypeSymbol *type_) : type(type_),
                                   constant_pool(8, 4),
                                   fields(6, 16),
                                   methods(6, 16)
    {}

    ~ClassFile()
    {
        for (int i = 1; i < constant_pool.Length(); i++)
            delete constant_pool[i];

        for (int j = 0; j < attributes.Length(); j++)
            delete attributes[j];

        return;
    }

    void Write()
    {
        Control &control = type -> semantic_environment -> sem -> control;

        if (! control.option.nowrite)
        {
            output_buffer.PutB4(magic);
            output_buffer.PutB2(minor_version);
            output_buffer.PutB2(major_version);

            output_buffer.PutB2(constant_pool.Length());
            for (int i = 1; i < constant_pool.Length(); i++)
            {
                constant_pool[i] -> put(output_buffer);
                if (constant_pool[i] -> tag  ==  CONSTANT_Long || constant_pool[i] -> tag  ==  CONSTANT_Double)
                    i++;; // skip the next entry for eight-byte constants
            }

            output_buffer.PutB2(access_flags);
            output_buffer.PutB2(this_class);
            output_buffer.PutB2(super_class);

            output_buffer.PutB2(interfaces.Length());
            for (int j = 0; j < interfaces.Length(); j++)
                output_buffer.PutB2(interfaces[j]);

            output_buffer.PutB2(fields.Length());
            for (int k = 0; k < fields.Length(); k++)
                fields[k].put(output_buffer);

            output_buffer.PutB2(methods.Length());
            for (int l = 0; l < methods.Length(); l++)
                methods[l].put(output_buffer);

            output_buffer.PutB2(attributes.Length());
            for (int m = 0; m < attributes.Length(); m++)
                attributes[m] -> put(output_buffer);

            char *class_file_name = type -> ClassName();
            if (control.option.verbose) {
                cout << "[write ";
                Unicode::Cout(class_file_name);
                cout << "]\n";
            }

            if (! output_buffer.WriteToFile(class_file_name))
            {
                int length = strlen(class_file_name);
                wchar_t *name = new wchar_t[length + 1];
                for (int i = 0; i < length; i++)
                    name[i] = class_file_name[i];
                name[length] = U_NULL;
                control.system_semantic -> ReportSemError(SemanticError::CANNOT_WRITE_FILE,
                                                          0,
                                                          0,
                                                          name);
                delete [] name;
            }
        }

        return;
    }

protected:
    TypeSymbol *type;
    OutputBuffer output_buffer;
};
#endif

// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#ifndef table_INCLUDED
#define table_INCLUDED

#include "config.h"
#include "symbol.h"

class VariableShadowSymbol
{
public:
    VariableSymbol *variable_symbol;

    VariableShadowSymbol(VariableSymbol *variable_symbol_) : variable_symbol(variable_symbol_),
                                                             conflict(NULL)
    {}

    ~VariableShadowSymbol()
    {
        delete conflict;
    }

    VariableSymbol *Conflict(int i) { return (*conflict)[i]; }

    inline int NumConflicts()
    {
        return (conflict ? conflict -> Length() : 0);
    }
    inline void AddConflict(VariableSymbol *conflict_symbol)
    {
        if ((variable_symbol != conflict_symbol) && (! Find(conflict_symbol)))
            conflict -> Next() = conflict_symbol;
        return;
    }
    inline void CompressSpace()
    {
        if (conflict)
            (void) conflict -> Array();
    }

private:
    friend class ExpandedFieldTable;
    VariableShadowSymbol *next;

    ConvertibleArray<VariableSymbol *> *conflict;

    bool Find(VariableSymbol *conflict_symbol)
    {
        if (! conflict)
            conflict = new ConvertibleArray<VariableSymbol *>(4);
        for (int k = 0; k < conflict -> Length(); k++)
            if ((*conflict)[k] == conflict_symbol)
                return true;
        return false;
    }
};


class MethodShadowSymbol
{
public:
    MethodSymbol *method_symbol;
    MethodShadowSymbol *next_method;

    MethodShadowSymbol(MethodSymbol *method_symbol_) : method_symbol(method_symbol_),
                                                       conflict(NULL)
    {}

    ~MethodShadowSymbol()
    {
        delete conflict;
    }

    MethodSymbol *Conflict(const int i) { return (*conflict)[i]; }

    inline int NumConflicts()
    {
        return (conflict ? conflict -> Length() : 0);
    }
    inline void AddConflict(MethodSymbol *conflict_symbol)
    {
        if ((method_symbol != conflict_symbol) && (! Find(conflict_symbol)))
            conflict -> Next() = conflict_symbol;
        return;
    }
    inline void RemoveConflicts()
    {
        delete conflict;
        conflict = NULL;
    }

    inline void CompressSpace()
    {
        if (conflict)
            (void) conflict -> Array();
    }

private:
    friend class ExpandedMethodTable;
    MethodShadowSymbol *next;

    ConvertibleArray<MethodSymbol *> *conflict;

    bool Find(MethodSymbol *conflict_symbol)
    {
        if (! conflict)
            conflict = new ConvertibleArray<MethodSymbol *>(4);
        for (int k = 0; k < conflict -> Length(); k++)
            if ((*conflict)[k] == conflict_symbol)
                return true;
        return false;
    }
};


class TypeShadowSymbol
{
public:
    TypeSymbol *type_symbol;

    TypeShadowSymbol(TypeSymbol *type_symbol_) : type_symbol(type_symbol_),
                                                 conflict(NULL)
    {}

    ~TypeShadowSymbol()
    {
        delete conflict;
    }

    TypeSymbol *Conflict(int i) { return (*conflict)[i]; }

    inline int NumConflicts()
    {
        return (conflict ? conflict -> Length() : 0);
    }

    inline void AddConflict(TypeSymbol *conflict_symbol)
    {
        if ((type_symbol != conflict_symbol) && (! Find(conflict_symbol)))
            conflict -> Next() = conflict_symbol;
        return;
    }

    inline void CompressSpace()
    {
        if (conflict)
            (void) conflict -> Array();
    }

private:
    friend class ExpandedTypeTable;
    TypeShadowSymbol *next;

    ConvertibleArray<TypeSymbol *> *conflict;

    bool Find(TypeSymbol *conflict_symbol)
    {
        if (! conflict)
            conflict = new ConvertibleArray<TypeSymbol *>(4);
        for (int k = 0; k < conflict -> Length(); k++)
            if ((*conflict)[k] == conflict_symbol)
                return true;
        return false;
    }
};


class ExpandedTypeTable
{
public:
    enum
    {
        DEFAULT_HASH_SIZE = 251,
        MAX_HASH_SIZE = 509
    };

    ConvertibleArray<TypeShadowSymbol *> symbol_pool;

    inline void CompressSpace()
    {
        hash_size = symbol_pool.Length();
        hash_size = (hash_size <= 0 ? 1 : (hash_size > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size));
        delete [] base;
        base = (TypeShadowSymbol **) memset(new TypeShadowSymbol *[hash_size], 0, hash_size * sizeof(TypeShadowSymbol *));

        TypeShadowSymbol **array = symbol_pool.Array();
        for (int i = 0; i < symbol_pool.Length(); i++)
        {
            array[i] -> CompressSpace();

            int k = array[i] -> type_symbol -> name_symbol -> index % hash_size;
            array[i] -> next = base[k];
            base[k] = array[i];
        }
    }

    ExpandedTypeTable(int hash_size_ = DEFAULT_HASH_SIZE) : symbol_pool(10, 4)
    {
        hash_size = (hash_size_ <= 0 ? 1 : (hash_size_ > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size_));
        base = (TypeShadowSymbol **) memset(new TypeShadowSymbol *[hash_size], 0, hash_size * sizeof(TypeShadowSymbol *));
    }

    ~ExpandedTypeTable()
    {
        for (int k = 0; k < symbol_pool.Length(); k++)
            delete symbol_pool[k];
        delete [] base;
    }

    inline TypeShadowSymbol *InsertTypeShadowSymbol(TypeSymbol *type_symbol)
    {
        int i = type_symbol -> name_symbol -> index % hash_size;
        TypeShadowSymbol *p = new TypeShadowSymbol(type_symbol);
        p -> next = base[i];
        base[i] = p;
        symbol_pool.Next() = p;

        return p;
    }

    inline TypeShadowSymbol *FindTypeShadowSymbol(NameSymbol *name_symbol)
    {
        TypeShadowSymbol *p;
        for (p = base[name_symbol -> index % hash_size]; p; p = p -> next)
             if (p -> type_symbol -> name_symbol == name_symbol)
                 break;
        return p;
    }

private:
    TypeShadowSymbol **base;
    int hash_size;
};

class ExpandedFieldTable
{
public:
    enum
    {
        DEFAULT_HASH_SIZE = 251,
        MAX_HASH_SIZE = 509
    };

    ConvertibleArray<VariableShadowSymbol *> symbol_pool;

    inline void CompressSpace()
    {
        hash_size = symbol_pool.Length();
        hash_size = (hash_size <= 0 ? 1 : (hash_size > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size));
        delete [] base;
        base = (VariableShadowSymbol **) memset(new VariableShadowSymbol *[hash_size], 0, hash_size * sizeof(VariableShadowSymbol *));

        VariableShadowSymbol **array = symbol_pool.Array();
        for (int i = 0; i < symbol_pool.Length(); i++)
        {
            array[i] -> CompressSpace();

            int k = array[i] -> variable_symbol -> name_symbol -> index % hash_size;
            array[i] -> next = base[k];
            base[k] = array[i];
        }
    }

    ExpandedFieldTable(int hash_size_ = DEFAULT_HASH_SIZE) : symbol_pool(10, 4)
    {
        hash_size = (hash_size_ <= 0 ? 1 : (hash_size_ > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size_));
        base = (VariableShadowSymbol **) memset(new VariableShadowSymbol *[hash_size], 0, hash_size * sizeof(VariableShadowSymbol *));
    }
    ~ExpandedFieldTable()
    {
        for (int i = 0; i < symbol_pool.Length(); i++)
            delete symbol_pool[i];
        delete [] base;
    }

    inline VariableShadowSymbol *InsertVariableShadowSymbol(VariableSymbol *variable_symbol)
    {
        int i = variable_symbol -> name_symbol -> index % hash_size;
        VariableShadowSymbol *p = new VariableShadowSymbol(variable_symbol);
        p -> next = base[i];
        base[i] = p;
        symbol_pool.Next() = p;

        return p;
    }

    inline VariableShadowSymbol *FindVariableShadowSymbol(NameSymbol *name_symbol)
    {
        VariableShadowSymbol *p;
        for (p = base[name_symbol -> index % hash_size]; p; p = p -> next)
            if (p -> variable_symbol -> name_symbol == name_symbol)
                break;
        return p;
    }

private:
    VariableShadowSymbol **base;
    int hash_size;
};


class ExpandedMethodTable
{
public:
    enum
    {
        DEFAULT_HASH_SIZE = 251,
        MAX_HASH_SIZE = 509
    };

    ConvertibleArray<MethodShadowSymbol *> symbol_pool;

    inline void CompressSpace()
    {
        hash_size = symbol_pool.Length();
        hash_size = (hash_size <= 0 ? 1 : (hash_size > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size));
        delete [] base;
        base = (MethodShadowSymbol **) memset(new MethodShadowSymbol *[hash_size], 0, hash_size * sizeof(MethodShadowSymbol *));

        MethodShadowSymbol **array = symbol_pool.Array();
        for (int i = 0; i < symbol_pool.Length(); i++)
        {
            array[i] -> CompressSpace();

            NameSymbol *name_symbol = array[i] -> method_symbol -> name_symbol;
            MethodShadowSymbol *base_shadow = FindMethodShadowSymbol(name_symbol);
            if (! base_shadow)
            {
                int k = name_symbol -> index % hash_size;
                array[i] -> next = base[k];
                base[k] = array[i];
                array[i] -> next_method = NULL;
            }
            else
            {
                array[i] -> next_method = base_shadow -> next_method;
                base_shadow -> next_method = array[i];
            }
        }
    }

    ExpandedMethodTable(int hash_size_ = DEFAULT_HASH_SIZE) : symbol_pool(10, 4)
    {
        hash_size = (hash_size_ <= 0 ? 1 : (hash_size_ > MAX_HASH_SIZE ? MAX_HASH_SIZE : hash_size_));
        base = (MethodShadowSymbol **) memset(new MethodShadowSymbol *[hash_size], 0, hash_size * sizeof(MethodShadowSymbol *));
    }
    ~ExpandedMethodTable()
    {
        for (int i = 0; i < symbol_pool.Length(); i++)
            delete symbol_pool[i];
       delete [] base;
    }

    inline MethodShadowSymbol *FindMethodShadowSymbol(NameSymbol *name_symbol)
    {
        MethodShadowSymbol *p;
        for (p = base[name_symbol -> index % hash_size]; p; p = p -> next)
            if (p -> method_symbol -> name_symbol == name_symbol)
                break;
        return p;
    }

    inline void InsertMethodShadowSymbol(MethodSymbol *method_symbol)
    {
        int i = method_symbol -> name_symbol -> index % hash_size;
        MethodShadowSymbol *p = new MethodShadowSymbol(method_symbol);
        p -> next_method = NULL;
        p -> next = base[i];
        base[i] = p;
        symbol_pool.Next() = p;

        return;
    }

    inline void Overload(MethodShadowSymbol *base_shadow, MethodSymbol *overload_method)
    {
        MethodShadowSymbol *shadow = new MethodShadowSymbol(overload_method);
        symbol_pool.Next() = shadow;
        shadow -> next_method = base_shadow -> next_method;
        base_shadow -> next_method = shadow;
    }

    inline void Overload(MethodSymbol *overload_method)
    {
        MethodShadowSymbol *base_shadow = FindMethodShadowSymbol(overload_method -> name_symbol);
        if (! base_shadow)
             InsertMethodShadowSymbol(overload_method);
        else Overload(base_shadow, overload_method);
    }

    MethodShadowSymbol *FindOverloadMethodShadow(MethodSymbol *overload_method, Semantic *sem, LexStream::TokenIndex tok)
    {
        if (! overload_method -> IsTyped())
            overload_method -> ProcessMethodSignature(sem, tok);

        for (MethodShadowSymbol *method_shadow = FindMethodShadowSymbol(overload_method -> name_symbol);
             method_shadow;
             method_shadow = method_shadow -> next_method)
        {
            MethodSymbol *method = method_shadow -> method_symbol;

            if (overload_method == method)
                return method_shadow;

            if (! method -> IsTyped())
                method -> ProcessMethodSignature(sem, tok);

            if (overload_method -> NumFormalParameters() == method -> NumFormalParameters())
            {
                int i;
                for (i = method -> NumFormalParameters() - 1; i >= 0; i--)
                {
                    if (method -> FormalParameter(i) -> Type() != overload_method -> FormalParameter(i) -> Type())
                        break;
                }

                if (i < 0)
                    return method_shadow;
            }
        }

        return (MethodShadowSymbol *) NULL;
    }

private:
    MethodShadowSymbol **base;
    int hash_size;
};
#endif



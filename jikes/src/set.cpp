// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "set.h"
#include "config.h"

int SymbolSet::primes[] = {DEFAULT_HASH_SIZE, 101, 401, MAX_HASH_SIZE};

void SymbolSet::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (ShadowSymbol **) memset(new ShadowSymbol *[hash_size], 0, hash_size * sizeof(ShadowSymbol *));

    for (int k = 0; k < symbol_pool.Length(); k++)
    {
        ShadowSymbol *shadow = symbol_pool[k];
        int i = shadow -> Identity() -> index % hash_size;
        shadow -> next = base[i];
        base[i] = shadow;
    }

    return;
}


SymbolSet::~SymbolSet()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (ShadowSymbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying a set with base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (ShadowSymbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
    SetEmpty();
    delete [] base;
}


bool SymbolSet::operator==(SymbolSet& rhs)
{
    if (this != &rhs)
    {
        if (symbol_pool.Length() != rhs.symbol_pool.Length())
            return false;

        for (int i = 0; i < symbol_pool.Length(); i++)
        {
            ShadowSymbol *shadow = symbol_pool[i];
            Symbol *symbol = shadow -> symbol;
            for (int k = 0; symbol; symbol = (Symbol *) (k < shadow -> NumConflicts() ? shadow -> Conflict(k++) : NULL))
            {
                if (! rhs.IsElement(symbol))
                    return false;
            }
        }
    }

    return true;
}


//
// Union the set in question with the set passed as argument: "set"
//
void SymbolSet::Union(SymbolSet &set)
{
    if (this != &set)
    {
        for (int i = 0; i < set.symbol_pool.Length(); i++)
        {
            ShadowSymbol *shadow = set.symbol_pool[i];
            Symbol *symbol = shadow -> symbol;
            for (int k = 0; symbol; symbol = (Symbol *) (k < shadow -> NumConflicts() ? shadow -> Conflict(k++) : NULL))
                AddElement(symbol);
        }
    }
 
    return;
}


//
// Intersect the set in question with the set passed as argument: "set"
//
void SymbolSet::Intersection(SymbolSet &set)
{
    if (this != &set)
    {
        Tuple<Symbol *> old_symbol_pool(this -> symbol_pool.Length());
        for (int i = 0; i < this -> symbol_pool.Length(); i++)
        {
            ShadowSymbol *shadow = this -> symbol_pool[i];
            Symbol *symbol = shadow -> symbol;
            for (int k = 0; symbol; symbol = (Symbol *) (k < shadow -> NumConflicts() ? shadow -> Conflict(k++) : NULL))
                old_symbol_pool.Next() = symbol;
        }

        this -> SetEmpty();

        for (int j = 0; j < old_symbol_pool.Length(); j++)
        {
            if (set.IsElement(old_symbol_pool[j]))
                AddElement(old_symbol_pool[j]);
        }
    }
 
    return;
}


//
// Return a bolean value indicating whether or not the set in question intersects the set passed as argument: "set"
// i.e., is there at least one element of set that is also an element of "this" set.
//
bool SymbolSet::Intersects(SymbolSet &set)
{
    for (int i = 0; i < set.symbol_pool.Length(); i++)
    {
        ShadowSymbol *shadow = set.symbol_pool[i];
        Symbol *symbol = shadow -> symbol;
        for (int k = 0; symbol; symbol = (Symbol *) (k < shadow -> NumConflicts() ? shadow -> Conflict(k++) : NULL))
            if (IsElement(symbol))
                return true;
    }
 
    return false;
}


//
// Remove element from the set
//
void SymbolSet::RemoveElement(Symbol *element)
{
    NameSymbol *name_symbol = element -> Identity();
    int i = name_symbol -> index % hash_size;
    ShadowSymbol *previous = NULL,
                 *shadow;
    for (shadow = base[i]; shadow; previous = shadow, shadow = shadow -> next)
    {
        if (shadow -> Identity() == name_symbol)
        {
            Symbol *symbol = shadow -> symbol;
            int k;
            for (k = 0; symbol; symbol = (Symbol *) (k < shadow -> NumConflicts() ? shadow -> Conflict(k++) : NULL))
            {
                if (symbol == element)
                    break;
            }

            if (symbol)
            {
                if (shadow -> NumConflicts() == 0)
                    break;
                shadow -> RemoveConflict(k - 1);
            }

            return;
        }
    }

    if (shadow) // element is the only object contained in shadow
    {
        if (previous == NULL)
             base[i] = shadow -> next;
        else previous -> next = shadow -> next;

        int last_index = symbol_pool.Length() - 1;
        if (shadow -> pool_index != last_index)
        {// move last element to position previously occupied by element being deleted
            symbol_pool[last_index] -> pool_index = shadow -> pool_index;
            symbol_pool[shadow -> pool_index] = symbol_pool[last_index];
        }

        symbol_pool.Reset(last_index); // remove last slot in symbol_pool

        delete shadow;
    }

    return;
}


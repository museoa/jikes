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
#include "lookup.h"
#include "symbol.h"
#include "code.h"
#include "ast.h"
#include "case.h"

int IntLiteralTable::decimal_limit = 0x7FFFFFFF / 10;
LongInt LongLiteralTable::decimal_limit = LongInt(0x7FFFFFFF, 0xFFFFFFFF) / 10;

int DirectoryTable::primes[] = {DEFAULT_HASH_SIZE, 2039, 4093, MAX_HASH_SIZE};

DirectoryTable::DirectoryTable(int estimate) : entry_pool(estimate),
                                               prime_index(0),
                                               hash_size(primes[0])
{
    base = (DirectoryEntry **) memset(new DirectoryEntry *[hash_size], 0, hash_size * sizeof(DirectoryEntry *));
}

DirectoryTable::~DirectoryTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the Name table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
#ifdef TEST
    for (int i = 0; i < entry_pool.Length(); i++)
        delete entry_pool[i];
    delete [] base;
#endif
}


time_t DirectoryEntry::Mtime()
{
    if (mtime_ == 0)
    {
        char *dirname = this -> directory -> DirectoryName();
        int length = this -> directory -> DirectoryNameLength() + this -> length + 1; // +1 for '/'
        char *file_name = new char[length + 1];
        strcpy(file_name, dirname);
        if (dirname[this -> directory -> DirectoryNameLength() - 1] != U_SLASH)
            strcat(file_name, StringConstant::U8S__SL_);
        strcat(file_name, this -> name);

        struct stat status;
        if (::SystemStat(file_name, &status) == 0)
            mtime_ = status.st_mtime;
else
assert("Cannot compute system time stamp\n");

        delete [] file_name;
    }

    return mtime_;
}


DirectoryEntry *DirectoryTable::FindEntry(char *str, int len)
{
    int k = Hash(str, len) % hash_size;
    DirectoryEntry *entry;
    for (entry = base[k]; entry; entry = entry -> next)
    {
        if (len == entry -> length && memcmp(entry -> name, str, len * sizeof(char)) == 0)
            return (entry -> IsDummy() ? (DirectoryEntry *) NULL : entry);
    }

    return NULL;
}


void DirectoryTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (DirectoryEntry **) memset(new DirectoryEntry *[hash_size], 0, hash_size * sizeof(DirectoryEntry *));

    for (int i = 0; i < entry_pool.Length(); i++)
    {
        DirectoryEntry *e = entry_pool[i];
        int k = Hash(e -> name, e -> length) % hash_size;
        e -> next = base[k];
        base[k] = e;
    }

    return;
}


DirectoryEntry *DirectoryTable::InsertEntry(DirectorySymbol *directory_symbol, char *str, int len)
{
    int k = Hash(str, len) % hash_size;
    DirectoryEntry *entry;
    for (entry = base[k]; entry; entry = entry -> next)
    {
        if (len == entry -> length && memcmp(entry -> name, str, len * sizeof(char)) == 0)
            return entry;
    }

    entry = new DirectoryEntry();
    entry_pool.Next() = entry;
    entry -> Initialize(directory_symbol, str, len);

    entry -> next = base[k];
    base[k] = entry;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (entry_pool.Length() > (hash_size << 1)))
        Rehash();

    return entry;
}


#ifdef WIN32_FILE_SYSTEM
DirectoryEntry *DirectoryTable::FindCaseInsensitiveEntry(char *name, int length)
{
    char *lower_name = new char[length + 1];
    for (int i = 0; i < length; i++)
        lower_name[i] = Case::ToAsciiLower(name[i]);
    lower_name[length] = U_NULL;

    DirectoryEntry *entry = FindEntry(lower_name, length);
    delete [] lower_name;

    return (entry ? entry -> Image() : entry);
}

void DirectoryTable::InsertCaseInsensitiveEntry(DirectoryEntry *image)
{
    int length = image -> length;
    char *lower_name = new char[length + 1];
    for (int i = 0; i < length; i++)
        lower_name[i] = Case::ToAsciiLower(image -> name[i]);
    lower_name[length] = U_NULL;

    int k = Hash(lower_name, length) % hash_size;
    DirectoryEntry *entry;
    for (entry = base[k]; entry; entry = entry -> next)
    {
        if (length == entry -> length && memcmp(entry -> name, lower_name, length * sizeof(char)) == 0)
            break;
    }

    if (! entry)
    {
        FoldedDirectoryEntry *folded_entry = new FoldedDirectoryEntry(image);
        entry_pool.Next() = folded_entry;
        folded_entry -> Initialize(image, lower_name, length);

        folded_entry -> next = base[k];
        base[k] = folded_entry;

        //
        // If the number of unique elements in the hash table exceeds 2 times
        // the size of the base, and we have not yet reached the maximum
        // allowable size for a base, reallocate a larger base and rehash
        // the elements.
        //
        if ((hash_size < MAX_HASH_SIZE) && (entry_pool.Length() > (hash_size << 1)))
            Rehash();
    }

    delete [] lower_name;

    return;
}
#endif


int NameLookupTable::primes[] = {DEFAULT_HASH_SIZE, 8191, 16411, MAX_HASH_SIZE};

NameLookupTable::NameLookupTable(int estimate) : symbol_pool(estimate),
                                                 prime_index(0),
                                                 hash_size(primes[0])
{
    base = (NameSymbol **) memset(new NameSymbol *[hash_size], 0, hash_size * sizeof(NameSymbol *));
}

NameLookupTable::~NameLookupTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the Name table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int LiteralLookupTable::primes[] = {DEFAULT_HASH_SIZE, 2039, 4093, MAX_HASH_SIZE};

LiteralLookupTable::LiteralLookupTable() : symbol_pool(16384),
                                           prime_index(0),
                                           hash_size(primes[0])
{
    base = (LiteralSymbol **) memset(new LiteralSymbol *[hash_size], 0, hash_size * sizeof(LiteralSymbol *));
}

LiteralLookupTable::~LiteralLookupTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the Literal table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (Symbol *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int IntLiteralTable::primes[] = {DEFAULT_HASH_SIZE, 8191, 16411, MAX_HASH_SIZE};

IntLiteralTable::IntLiteralTable(LiteralValue *bad_value_) : symbol_pool(16384),
                                                             bad_value(bad_value_),
                                                             prime_index(0),
                                                             hash_size(primes[0])
{
    base = (IntLiteralValue **) memset(new IntLiteralValue *[hash_size], 0, hash_size * sizeof(IntLiteralValue *));
}

IntLiteralTable::~IntLiteralTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the integer table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/

#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int LongLiteralTable::primes[] = {DEFAULT_HASH_SIZE, 2039, 4093, MAX_HASH_SIZE};

LongLiteralTable::LongLiteralTable(LiteralValue *bad_value_) : symbol_pool(16384),
                                                               bad_value(bad_value_),
                                                               prime_index(0),
                                                               hash_size(primes[0])
{
    base = (LongLiteralValue **) memset(new LongLiteralValue *[hash_size], 0, hash_size * sizeof(LongLiteralValue *));
}

LongLiteralTable::~LongLiteralTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the long table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/

#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int FloatLiteralTable::primes[] = {DEFAULT_HASH_SIZE, 2039, 4093, MAX_HASH_SIZE};

FloatLiteralTable::FloatLiteralTable(LiteralValue *bad_value_) : symbol_pool(16384),
                                                                 bad_value(bad_value_),
                                                                 prime_index(0),
                                                                 hash_size(primes[0])
{
    base = (FloatLiteralValue **) memset(new FloatLiteralValue *[hash_size], 0, hash_size * sizeof(FloatLiteralValue *));
}

FloatLiteralTable::~FloatLiteralTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the float table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/

#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int DoubleLiteralTable::primes[] = {DEFAULT_HASH_SIZE, 2039, 4093, MAX_HASH_SIZE};

DoubleLiteralTable::DoubleLiteralTable(LiteralValue *bad_value_) : symbol_pool(16384),
                                                                   bad_value(bad_value_),
                                                                   prime_index(0),
                                                                   hash_size(primes[0])
{
    base = (DoubleLiteralValue **) memset(new DoubleLiteralValue *[hash_size], 0, hash_size * sizeof(DoubleLiteralValue *));
}

DoubleLiteralTable::~DoubleLiteralTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the double table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


int Utf8LiteralTable::primes[] = {DEFAULT_HASH_SIZE, 8191, 16411, MAX_HASH_SIZE};

Utf8LiteralTable::Utf8LiteralTable(LiteralValue *bad_value_) : symbol_pool(16384),
                                                               bad_value(bad_value_),
                                                               prime_index(0),
                                                               hash_size(primes[0])
{
    base = (Utf8LiteralValue **) memset(new Utf8LiteralValue *[hash_size], 0, hash_size * sizeof(Utf8LiteralValue *));
}


Utf8LiteralTable::~Utf8LiteralTable()
{
/*
int n;
int num_slots = 0;
int total = 0;
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
{
num_slots++;
total += num;
}
}

if (num_slots > 0)
{
cout << "\nDestroying the Utf8 table with " << total
     << " elements and base size " << hash_size << " containing " << num_slots << " non-empty slots\n";
for (n = 0; n < hash_size; n++)
{
int num = 0;
for (LiteralValue *s = base[n]; s; s = s -> next)
    num++;
if (num > 0)
cout << "    slot " << n << " contains " << num << " element(s)\n";
}
}
if (hash_size < total)
    total = total;
*/
#ifdef TEST
    for (int i = 0; i < symbol_pool.Length(); i++)
        delete symbol_pool[i];
    delete [] base;
#endif
}


void NameLookupTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (NameSymbol **) memset(new NameSymbol *[hash_size], 0, hash_size * sizeof(NameSymbol *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        NameSymbol *ns = symbol_pool[i];
        int k = ns -> hash_address % hash_size;
        ns -> next = base[k];
        base[k] = ns;
    }

    return;
}


NameSymbol *NameLookupTable::FindOrInsertName(wchar_t *str, int len)
{
    unsigned hash_address = Hash(str, len);
    int k = hash_address % hash_size;
    NameSymbol *symbol;
    for (symbol = base[k]; symbol; symbol = (NameSymbol *) symbol -> next)
    {
        if (len == symbol -> NameLength() && memcmp(symbol -> Name(), str, len * sizeof(wchar_t)) == 0)
            return symbol;
    }

    int index = symbol_pool.Length(); // index of the next element
    symbol = new NameSymbol();
    symbol_pool.Next() = symbol;
    symbol -> Initialize(str, len, hash_address, index);

    symbol -> next = base[k];
    base[k] = symbol;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return symbol;
}


void LiteralLookupTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (LiteralSymbol **) memset(new LiteralSymbol *[hash_size], 0, hash_size * sizeof(LiteralSymbol *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        LiteralSymbol *ls = symbol_pool[i];
        int k = ls -> hash_address % hash_size;
        ls -> next = base[k];
        base[k] = ls;
    }

    return;
}


LiteralSymbol *LiteralLookupTable::FindOrInsertLiteral(wchar_t *str, int len)
{
    unsigned hash_address = Hash(str, len);
    int k = hash_address % hash_size;
    LiteralSymbol *symbol;
    for (symbol = base[k]; symbol; symbol = (LiteralSymbol *) symbol -> next)
    {
        if (len == symbol -> NameLength() && memcmp(symbol -> Name(), str, len * sizeof(wchar_t)) == 0)
            return symbol;
    }

    symbol = new LiteralSymbol();
    symbol_pool.Next() = symbol;
    symbol -> Initialize(str, hash_address, len);

    symbol -> next = base[k];
    base[k] = symbol;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return symbol;
}


LiteralValue *IntLiteralTable::FindOrInsertChar(LiteralSymbol *literal)
{
    wchar_t *name = literal -> Name();

    if (literal -> NameLength() == 1) // an isolated quote
         return literal -> value = bad_value;
    else if (literal -> NameLength() <= 3) // a regular character of the form:  quote + char
                                           // or                                quote + char + quote
         return literal -> value = FindOrInsert((int) name[1]);

    int value;

    if (name[1] != U_BACKSLASH)
         value = -1;
    else if (name[2] == U_b && name[3] == U_SINGLE_QUOTE)
         value = U_BACKSPACE;
    else if (name[2] == U_t && name[3] == U_SINGLE_QUOTE)
         value = U_HORIZONTAL_TAB;
    else if (name[2] == U_n && name[3] == U_SINGLE_QUOTE)
         value = U_LINE_FEED;
    else if (name[2] == U_f && name[3] == U_SINGLE_QUOTE)
         value = U_FORM_FEED;
    else if (name[2] == U_r && name[3] == U_SINGLE_QUOTE)
         value = U_CARRIAGE_RETURN;
    else if (name[2] == U_DOUBLE_QUOTE && name[3] == U_SINGLE_QUOTE)
         value = U_DOUBLE_QUOTE;
    else if (name[2] == U_SINGLE_QUOTE && name[3] == U_SINGLE_QUOTE)
         value = U_SINGLE_QUOTE;
    else if (name[2] == U_BACKSLASH && name[3] == U_SINGLE_QUOTE)
         value = U_BACKSLASH;
    else if (Code::IsDigit(name[2]))
    {
        wchar_t *p = &name[2];

        int d1 = *p++ - U_0;
        value = (d1 < 8 ? d1 : -1);

        if (value >= 0 && Code::IsDigit(*p))
        {
            int d2 = *p++ - U_0;
            value = (d2 < 8 ? value * 8 + d2 : -1);

            if (value >= 0 && Code::IsDigit(*p))
            {
                int d3 = *p++ - U_0;
                value = (d3 < 8 && d1 < 4 ? value * 8 + d3 : -1);
            }
        }

        if (*p != U_NULL && *p != U_SINGLE_QUOTE)
            value = -1;
    }
    else value = -1;

    return literal -> value = (value < 0 || value > 65535 ? bad_value : FindOrInsert(value));
}


LiteralValue *IntLiteralTable::FindOrInsertHexInt(LiteralSymbol *literal)
{
    wchar_t *head = literal -> Name() + 1, // point to X
            *tail = &literal -> Name()[literal -> NameLength() - 1];

    u4 uvalue = 0;

    //
    // According to the JLS 3.10.1, "A compile-time error occurs if ... a
    // hexadecimal or octal int literal does not fit in 32 bits". 
    // So, strictly speaking, we should not skip leading zeroes. However,
    // there are many publicly available code out there with leading zeroes
    // that don't compile with jikes, if ...
    //
    {
        for (++head; tail > head && *head == U_0; head++) // skip leading zeroes
            ;
        head--;
    }

    for (int i = 0; i < 32 && tail > head; i += 4, tail--)
    {
        u4 d = *tail - (Code::IsDigit(*tail) ? U_0 : (Code::IsLower(*tail) ? U_a : U_A) - 10);
        uvalue |= (d << i);
    }

    return (tail > head ? bad_value : FindOrInsert((int) uvalue));
}


LiteralValue *IntLiteralTable::FindOrInsertOctalInt(LiteralSymbol *literal)
{
    wchar_t *head = literal -> Name(), // point to initial '0'
            *tail = &head[literal -> NameLength() - 1];

    u4 uvalue = 0;
    //
    // According to the JLS 3.10.1, "A compile-time error occurs if ... a
    // hexadecimal or octal int literal does not fit in 32 bits". 
    // So, strictly speaking, we should not skip leading zeroes. However,
    // there are many publicly available code out there with leading zeroes
    // that don't compile with jikes,...
    //
    {
        for (++head; tail > head && *head == U_0; head++) // skip leading zeroes
            ;
        head--;
    }

    for (int i = 0; i < 30 && tail > head; i += 3, tail--)
    {
        u4 d = *tail - U_0;
        uvalue |= (d << i);
    }

    if (tail > head)
    {
        u4 d = *tail - U_0;

        if (d <= 3) // max number that can fit in 2 bits
        {
            tail--;
            uvalue |= (d << 30);
        }
    }

    return (tail > head ? bad_value : FindOrInsert((int) uvalue));
}


LiteralValue *IntLiteralTable::FindOrInsertInt(LiteralSymbol *literal)
{
    wchar_t *name = literal -> Name();

    if (name[0] == U_0)
        literal -> value = (name[1] == U_x || name[1] == U_X ? FindOrInsertHexInt(literal) : FindOrInsertOctalInt(literal));
    else
    {
        int value = 0;

        wchar_t *p;
        for (p = name; *p; p++)
        {
            int digit = *p - U_0;
            if (value > decimal_limit || (value == decimal_limit && digit > 7))
                break;
            value = value * 10 + digit;
        }

        literal -> value = (*p ? bad_value : FindOrInsert(value));
    }

    return literal -> value;
}


LiteralValue *IntLiteralTable::FindOrInsertNegativeInt(LiteralSymbol *literal)
{
    if (literal -> value && literal -> value != bad_value) // a positive value already exists
    {
        IntLiteralValue *int_literal = (IntLiteralValue *) literal -> value;
        return FindOrInsert(- int_literal -> value);
    }

    wchar_t *name = literal -> Name();

    //
    // We can assert that the name of a literal contains at least two characters:
    // at least one digit and the terminating '\0'.
    //
    if (name[0] == U_0)
    {
        IntLiteralValue *int_literal = (IntLiteralValue *) (name[1] == U_x || name[1] == U_X
                                                                     ? FindOrInsertHexInt(literal)
                                                                     : FindOrInsertOctalInt(literal));
        return FindOrInsert(- int_literal -> value);
    }

    int value = 0;

    wchar_t *p;
    for (p = name; *p; p++)
    {
        int digit = *p - U_0;
        if (value > decimal_limit || (value == decimal_limit && digit > 8))
            break;
        value = value * 10 + digit;
    }

    return (*p ? bad_value : FindOrInsert(-value));
}


void IntLiteralTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (IntLiteralValue **) memset(new IntLiteralValue *[hash_size], 0, hash_size * sizeof(IntLiteralValue *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        IntLiteralValue *ilv = symbol_pool[i];
        int k = ((unsigned) ilv -> value) % hash_size; // The unsigned casting turns the negative values into positive values
        ilv -> next = base[k];
        base[k] = ilv;
    }

    return;
}


IntLiteralValue *IntLiteralTable::FindOrInsert(int value)
{
    int k = ((unsigned) value) % hash_size; // The unsigned casting turns the negative values into positive values

    IntLiteralValue *lit;
    for (lit = base[k]; lit; lit = (IntLiteralValue *) lit -> next)
    {
        if (lit -> value == value)
            return lit;
    }

    lit = new IntLiteralValue();
    symbol_pool.Next() = lit;
    lit -> Initialize(value);

    lit -> next = base[k];
    base[k] = lit;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return lit;
}


LiteralValue *LongLiteralTable::FindOrInsertHexLong(LiteralSymbol *literal)
{
    u4 high = 0,
       low = 0;

    wchar_t *head = literal -> Name() + 1, // point to X
            *tail = &literal -> Name()[literal -> NameLength() - 2]; // -2 to skip the 'L' suffix

    //
    // According to the JLS 3.10.1, "A compile-time error occurs if ... a
    // hexadecimal or octal int literal does not fit in 32 bits". 
    // So, strictly speaking, we should not skip leading zeroes. However,
    // there are many publicly available code out there with leading zeroes
    // that don't compile with jikes,...
    //
    {
        for (++head; tail > head && *head == U_0; head++) // skip leading zeroes
            ;
        head--;
    }

    for (int i = 0; i < 32 && tail > head; i += 4, tail--)
    {
        u4 d = *tail - (Code::IsDigit(*tail) ? U_0 : (Code::IsLower(*tail) ? U_a : U_A) - 10);
        low |= (d << i);
    }

    for (int j = 0; j < 32 && tail > head; j += 4, tail--)
    {
        u4 d = *tail - (Code::IsDigit(*tail) ? U_0 : (Code::IsLower(*tail) ? U_a : U_A) - 10);
        high |= (d << j);
    }

    return (tail > head ? bad_value : FindOrInsert(LongInt(high, low)));
}


LiteralValue *LongLiteralTable::FindOrInsertOctalLong(LiteralSymbol *literal)
{
    wchar_t *head = literal -> Name(), // point to initial '0'
            *tail = &head[literal -> NameLength() - 2]; // -2 to skip the 'L' suffix

    ULongInt uvalue = 0;
    //
    // According to the JLS 3.10.1, "A compile-time error occurs if ... a
    // hexadecimal or octal int literal does not fit in 32 bits". 
    // So, strictly speaking, we should not skip leading zeroes. However,
    // there are many publicly available code out there with leading zeroes
    // that don't compile with jikes,...
    //
    {
        for (++head; tail > head && *head == U_0; head++) // skip leading zeroes
            ;
        head--;
    }

    for (int i = 0; i < 63 && tail > head; i += 3, tail--)
    {
        ULongInt d = (u4) (*tail - U_0);
        uvalue |= (d << i);
    }

    if (tail > head)
    {
        u4 d = *tail - U_0;

        if (d <= 1) // max number that can fit in 1 bit
        {
            tail--;
            uvalue.HighWord() |= (d << 31);
        }
    }

    return (tail > head ? bad_value : FindOrInsert((LongInt) uvalue));
}


LiteralValue *LongLiteralTable::FindOrInsertLong(LiteralSymbol *literal)
{
    wchar_t *name = literal -> Name();

    //
    // We can assert that the name of a literal contains at least two characters:
    // at least one digit and the terminating '\0'.
    //
    if (name[0] == U_0)
        literal -> value = (name[1] == U_x || name[1] == U_X ? FindOrInsertHexLong(literal) : FindOrInsertOctalLong(literal));
    else
    {
        LongInt value = 0;

        wchar_t *p;
        for (p = name; *p != U_L && *p != U_l; p++)
        {
            u4 digit = *p - U_0;
            if (value > decimal_limit || (value == decimal_limit && digit > 7))
                break;
            value = value * 10 + digit;
        }

        literal -> value = (*p != U_L && *p != U_l ? bad_value : FindOrInsert(value));
    }

    return literal -> value;
}


LiteralValue *LongLiteralTable::FindOrInsertNegativeLong(LiteralSymbol *literal)
{
    if (literal -> value && literal -> value != bad_value) // a positive value already exists
    {
        LongLiteralValue *long_literal = (LongLiteralValue *) literal -> value;
        return FindOrInsert(- long_literal -> value);
    }

    wchar_t *name = literal -> Name();
    //
    // We can assert that the name of a literal contains at least two characters:
    // at least one digit and the terminating '\0'.
    //
    if (name[0] == U_0)
    {
        LongLiteralValue *long_literal = (LongLiteralValue *) (name[1] == U_x || name[1] == U_X
                                                                               ? FindOrInsertHexLong(literal)
                                                                               : FindOrInsertOctalLong(literal));
        return FindOrInsert(- long_literal -> value);
    }

    LongInt value = 0;

    wchar_t *p;
    for (p = name; *p != U_L && *p != U_l && value >= 0; p++)
    {
        u4 digit = *p - U_0;
        if (value > decimal_limit || (value == decimal_limit && digit > 8))
            break;
        value = value * 10 + digit;
    }

    return (*p != U_L && *p != U_l ? bad_value : FindOrInsert(-value));
}


void LongLiteralTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (LongLiteralValue **) memset(new LongLiteralValue *[hash_size], 0, hash_size * sizeof(LongLiteralValue *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        LongLiteralValue *llv = symbol_pool[i];
        int k = (((ULongInt) llv -> value) % ULongInt(0, hash_size)).LowWord(); // The ULongInt turns negative values positive
        llv -> next = base[k];
        base[k] = llv;
    }

    return;
}


LongLiteralValue *LongLiteralTable::FindOrInsert(LongInt value)
{
    int k = (((ULongInt) value) % ULongInt(0, hash_size)).LowWord();  // The ULongInt cast turns negative values into positive values

    LongLiteralValue *lit;
    for (lit = base[k]; lit; lit = (LongLiteralValue *) lit -> next)
    {
        if (lit -> value == value)
            return lit;
    }

    lit = new LongLiteralValue();
    symbol_pool.Next() = lit;
    lit -> Initialize(value);

    lit -> next = base[k];
    base[k] = lit;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return lit;
}


LiteralValue *FloatLiteralTable::FindOrInsertFloat(LiteralSymbol *literal)
{
    char *name = new char[literal -> NameLength() + 1];
    for (int i = 0; i < literal -> NameLength(); i++)
        name[i] = (char) literal -> Name()[i];
    name[literal -> NameLength()] = U_NULL;

    IEEEfloat value = IEEEfloat(name);

    literal -> value = FindOrInsert(value);

    delete [] name;

    return literal -> value;
}


void FloatLiteralTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (FloatLiteralValue **) memset(new FloatLiteralValue *[hash_size], 0, hash_size * sizeof(FloatLiteralValue *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        FloatLiteralValue *flv = symbol_pool[i];
        int k = Hash(flv -> value) % hash_size; // the hash function for float values is cheap so we don't need to save it.
        flv -> next = base[k];
        base[k] = flv;
    }

    return;
}


FloatLiteralValue *FloatLiteralTable::FindOrInsert(IEEEfloat value)
{
    int k = Hash(value) % hash_size;

    FloatLiteralValue *lit;
    for (lit = base[k]; lit; lit = (FloatLiteralValue *) lit -> next)
    {
        if (lit -> value == value)
            return lit;
    }

    lit = new FloatLiteralValue();
    symbol_pool.Next() = lit;
    lit -> Initialize(value);

    lit -> next = base[k];
    base[k] = lit;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return lit;
}


LiteralValue *DoubleLiteralTable::FindOrInsertDouble(LiteralSymbol *literal)
{
    char *name = new char[literal -> NameLength() + 1];
    for (int i = 0; i < literal -> NameLength(); i++)
        name[i] = (char) literal -> Name()[i];
    name[literal -> NameLength()] = U_NULL;

    IEEEdouble value = IEEEdouble(name);

    literal -> value = FindOrInsert(value);

    delete [] name;

    return literal -> value;
}


void DoubleLiteralTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (DoubleLiteralValue **) memset(new DoubleLiteralValue *[hash_size], 0, hash_size * sizeof(DoubleLiteralValue *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        DoubleLiteralValue *dlv = symbol_pool[i];
        int k = Hash(dlv -> value) % hash_size; // the hash function for double values is cheap so we don't need to save it.
        dlv -> next = base[k];
        base[k] = dlv;
    }

    return;
}


DoubleLiteralValue *DoubleLiteralTable::FindOrInsert(IEEEdouble value)
{
    int k = Hash(value) % hash_size;

    DoubleLiteralValue *lit;
    for (lit = base[k]; lit; lit = (DoubleLiteralValue *) lit -> next)
    {
        if (lit -> value == value)
            return lit;
    }

    lit = new DoubleLiteralValue();
    symbol_pool.Next() = lit;
    lit -> Initialize(value);

    lit -> next = base[k];
    base[k] = lit;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return lit;
}


LiteralValue *Utf8LiteralTable::FindOrInsertString(LiteralSymbol *literal)
{
    wchar_t *name = literal -> Name();

    int literal_length = literal -> NameLength();

    char *value = new char[literal_length * 3]; // should be big enough for the worst case
    int len = 0,
        i;
    for (i = 1; i < literal_length && name[i] != U_DOUBLE_QUOTE; i++)  // start at 1 to skip the initial \"
    {
        int ch = name[i];

        if (ch == U_BACKSLASH)
        {
            if (name[i + 1] == U_b)
            {
                i++;
                ch = U_BACKSPACE;
            }
            else if (name[i + 1] == U_t)
            {
                i++;
                ch = U_HORIZONTAL_TAB;
            }
            else if (name[i + 1] == U_n)
            {
                i++;
                ch = U_LINE_FEED;
            }
            else if (name[i + 1] == U_f)
            {
                i++;
                ch = U_FORM_FEED;
            }
            else if (name[i + 1] == U_r)
            {
                i++;
                ch = U_CARRIAGE_RETURN;
            }
            else if (name[i + 1] == U_DOUBLE_QUOTE)
            {          
                i++;
                ch = U_DOUBLE_QUOTE;
            }
            else if (name[i + 1] == U_SINGLE_QUOTE)
            {          
                i++;
                ch = U_SINGLE_QUOTE;
            }
            else if (name[i + 1] == U_BACKSLASH)
            {
                i++;
                ch = U_BACKSLASH;
            }
            else if (Code::IsDigit(name[i + 1]))
            {
                int d1 = name[i + 1] - U_0;
                ch = (d1 < 8 ? d1 : -1);

                if (ch >= 0)
                {
                    i++;
                    if (Code::IsDigit(name[i + 1]))
                    {
                        int d2 = name[i + 1] - U_0;

                        if (d2 < 8)
                        {
                            i++;
                            ch = ch * 8 + d2;

                            if (Code::IsDigit(name[i + 1]))
                            {
                                int d3 = name[i + 1] - U_0;

                                if (d3 < 8)
                                {
                                    i++;
                                    ch = (d1 < 4 ? ch * 8 + d3 : -1);
                                }
                            }
                        }
                    }
                }
            }
            else ch = -1;
        }

        if (ch < 0)
             break;
        else if (ch == 0)
        {
             value[len++] = (char) 0xC0;
             value[len++] = (char) 0x80;
        }
        else if (ch <= 0x007F)
             value[len++] = (char) ch;
        else if (ch <= 0x07FF)
        {
             value[len++] = (char) ((char) 0xC0 | (char) ((ch >> 6) & 0x001F)); // bits 6-10
             value[len++] = (char) ((char) 0x80 | (char) (ch & 0x003F));        // bits 0-5
        }
        else
        {
             value[len++] = (char) ((char) 0xE0 | (char) ((ch >> 12) & 0x000F));
             value[len++] = (char) ((char) 0x80 | (char) ((ch >> 6) & 0x003F));
             value[len++] = (char) ((char) 0x80 | (char) (ch & 0x3F));
        }
    }

    value[len] = U_NULL;
    literal -> value = (i < literal_length && name[i] != U_DOUBLE_QUOTE ? bad_value : FindOrInsert(value, len));

    delete [] value;
    return literal -> value;
}


Utf8LiteralValue *Utf8LiteralTable::FindOrInsert(wchar_t ch)
{
    int len = 0;
    char str[4];

    if (ch == 0)
    {
         str[len++] = (char) 0xC0;
         str[len++] = (char) 0x80;
    }
    else if (ch <= 0x007F)
         str[len++] = (char) ch;
    else if (ch <= 0x07FF)
    {
         str[len++] = (char) ((char) 0xC0 | (char) ((ch >> 6) & 0x001F)); // bits 6-10
         str[len++] = (char) ((char) 0x80 | (char) (ch & 0x003F));        // bits 0-5
    }
    else
    {
         str[len++] = (char) ((char) 0xE0 | (char) ((ch >> 12) & 0x000F));
         str[len++] = (char) ((char) 0x80 | (char) ((ch >> 6) & 0x003F));
         str[len++] = (char) ((char) 0x80 | (char) (ch & 0x3F));
    }

    str[len] = U_NULL;

    return FindOrInsert(str, len);
}


void Utf8LiteralTable::Rehash()
{
    hash_size = primes[++prime_index];

    delete [] base;
    base = (Utf8LiteralValue **) memset(new Utf8LiteralValue *[hash_size], 0, hash_size * sizeof(Utf8LiteralValue *));

    for (int i = 0; i < symbol_pool.Length(); i++)
    {
        Utf8LiteralValue *ulv = symbol_pool[i];
        int k = ulv -> hash_address % hash_size;
        ulv -> next = base[k];
        base[k] = ulv;
    }

    return;
}


Utf8LiteralValue *Utf8LiteralTable::FindOrInsert(char *str, int len)
{
    unsigned hash_address = Hash(str, len);
    int k = hash_address % hash_size;

    Utf8LiteralValue *lit;
    for (lit = base[k]; lit; lit = (Utf8LiteralValue *) lit -> next)
    {
        if (len == lit -> length)
        {
            if (memcmp(lit -> value, str, len * sizeof(char)) == 0)
                 return lit;
        }
    }

    lit = new Utf8LiteralValue();
    symbol_pool.Next() = lit;
    lit -> Initialize(str, len, hash_address);

    lit -> next = base[k];
    base[k] = lit;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((hash_size < MAX_HASH_SIZE) && (symbol_pool.Length() > (hash_size << 1)))
        Rehash();

    return lit;
}


bool Utf8LiteralTable::IsConstant(AstExpression *expression)
{
    //
    // Recall that addition if left-recursive
    //
    while (! expression -> IsConstant())
    {
        AstBinaryExpression *binary_expression;
        AstCastExpression *cast_expression;
        AstParenthesizedExpression *parenthesized_expression;

        if (binary_expression = expression -> BinaryExpressionCast())
        {
             AstExpression *right = binary_expression -> right_expression;
             if (! IsConstant(right))
                 return false;
             expression = binary_expression -> left_expression;
        }
        else if (cast_expression = expression -> CastExpressionCast())
             expression = cast_expression -> expression;
        else if (parenthesized_expression = expression -> ParenthesizedExpressionCast())
             expression = parenthesized_expression -> expression;
        else return false; // Not a constant String expression
    }

    if (expression -> IsConstant())
        expr -> Next() = expression;

    return expression -> IsConstant();
}


void Utf8LiteralTable::CheckStringConstant(AstExpression *expression)
{
    expr = new Tuple<AstExpression *>(256);

    if (IsConstant(expression))
    {
        Utf8LiteralValue *literal;
        int length = 0;
        for (int i = 0; i < expr -> Length(); i++)
        {
            literal = (Utf8LiteralValue *) (*expr)[i] -> value;
            length += literal -> length;
        }
        char *str = new char[length + 1]; // +1 for '\0'

        //
        // Since we started on the right, we have to run over the list in reverse order.
        //
        int index = 0;
        for (int k = expr -> Length() - 1; k >= 0; k--)
        {
            literal = (Utf8LiteralValue *) (*expr)[k] -> value;
            memmove(&str[index], literal -> value, literal -> length * sizeof(char));
            index += literal -> length;
        }
        str[length] = U_NULL;

        expression -> value = FindOrInsert(str, length);

        delete [] str;
    }

    delete expr;

    return;
}

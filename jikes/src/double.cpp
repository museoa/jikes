// $Id$
//
// This software is subject to the terms of the IBM Jikes Compiler Open
// Source License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1998, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.
//
#include "config.h"
#include <iostream.h>
#include "double.h"
#include "long.h"
IEEEfloat::IEEEfloat(float d)
{
    FloatValue() = d;
}

IEEEfloat::IEEEfloat(u4 a)
{
    Word() = a;
}

IEEEfloat::IEEEfloat(i4 a)
{
    FloatValue() = a;
}

IEEEfloat::IEEEfloat(char * name)
{
    FloatValue() = atof(name);
}

int IEEEfloat::IntValue()
{
    return (int) FloatValue();
}
int IEEEfloat::LongValue()
{
    return (long) FloatValue();
}

IEEEdouble IEEEdouble::min_long = IEEEdouble(0xc3e00000, 0x00000000);

IEEEdouble::IEEEdouble(double d)
{
    DoubleValue() = d;
}

IEEEdouble::IEEEdouble(u4 a, u4 b)
{
    HighWord() = a;
    LowWord() = b;
}

IEEEdouble::IEEEdouble(IEEEfloat a)
{
    DoubleValue() = a.FloatValue();
}

IEEEdouble::IEEEdouble(i4 a)
{
    DoubleValue() = a;
}


IEEEdouble::IEEEdouble(u4 a)
{
    HighWord() = 0;
    LowWord() = a;
}

IEEEdouble::IEEEdouble(char * name)
{
    DoubleValue()  = atof(name);
}

bool IEEEdouble::operator== (IEEEdouble op)
{
    return DoubleValue() == op.DoubleValue();
}

bool IEEEdouble::operator!= (IEEEdouble op)
{
    return DoubleValue() != op.DoubleValue();
}

IEEEdouble IEEEdouble::operator+ (IEEEdouble op)
{
    return IEEEdouble(DoubleValue() + op.DoubleValue());
}

IEEEdouble& IEEEdouble::operator+= (IEEEdouble op)
{
    *this = *this + op;
    return *this;
}

IEEEdouble IEEEdouble::operator- ()
{
    return IEEEdouble(- this -> DoubleValue());
}

IEEEdouble IEEEdouble::operator- (IEEEdouble op)
{
    return IEEEdouble(DoubleValue() + (-op.DoubleValue()));
}

IEEEdouble& IEEEdouble::operator-= (IEEEdouble op)
{
    *this = *this - op;
    return *this;
}


IEEEdouble IEEEdouble::operator* (IEEEdouble op)
{
    return IEEEdouble(DoubleValue() * op.DoubleValue());
}

IEEEdouble& IEEEdouble::operator*= (IEEEdouble op)
{
    *this = *this * op;
    return *this;
}

IEEEdouble IEEEdouble::operator/ (IEEEdouble op)
{
    return IEEEdouble(DoubleValue() / op.DoubleValue());
}

IEEEdouble& IEEEdouble::operator/= (IEEEdouble op)
{
    *this = *this / op;
    return *this;
}

bool IEEEdouble::operator< (IEEEdouble op)
{
    return (DoubleValue() < op.DoubleValue() ? 1 : 0);
}

bool IEEEdouble::operator<= (IEEEdouble op)
{
    return (DoubleValue() <= op.DoubleValue() ? 1 : 0);
}

bool IEEEdouble::operator> (IEEEdouble op)
{
    return (DoubleValue() > op.DoubleValue() ? 1 : 0);
}

bool IEEEdouble::operator>= (IEEEdouble op)
{
    return (DoubleValue() >= op.DoubleValue() ? 1 : 0);
}

bool IEEEfloat::operator== (IEEEfloat op)
{
    return FloatValue() == op.FloatValue();
}

bool IEEEfloat::operator!= (IEEEfloat op)
{
    return FloatValue() != op.FloatValue();
}


bool IEEEfloat::operator< (IEEEfloat op)
{
    return (FloatValue() < op.FloatValue() ? 1 : 0);
}

bool IEEEfloat::operator<= (IEEEfloat op)
{
    return (FloatValue() <= op.FloatValue() ? 1 : 0);
}

bool IEEEfloat::operator> (IEEEfloat op)
{
    return (FloatValue() > op.FloatValue() ? 1 : 0);
}

bool IEEEfloat::operator>= (IEEEfloat op)
{
    return (FloatValue() >= op.FloatValue() ? 1 : 0);
}


IEEEfloat IEEEfloat::operator+ (IEEEfloat op)
{
    return IEEEfloat(FloatValue() + op.FloatValue());
}

IEEEfloat& IEEEfloat::operator+= (IEEEfloat op)
{
    *this = *this + op;
    return *this;
}


IEEEfloat IEEEfloat::operator- ()
{
    return IEEEfloat( - this -> FloatValue());

}

IEEEfloat IEEEfloat::operator- (IEEEfloat op)
{
    return *this + (-op);
}

IEEEfloat& IEEEfloat::operator-= (IEEEfloat op)
{
    *this = *this - op;
    return *this;
}

IEEEfloat IEEEfloat::operator* (IEEEfloat op)
{
    return IEEEfloat(FloatValue() * op.FloatValue());
}

IEEEfloat& IEEEfloat::operator*= (IEEEfloat op)
{
    *this = *this * op;
    return *this;
}

IEEEfloat IEEEfloat::operator/ (IEEEfloat op)
{
    return IEEEfloat(FloatValue() / op.FloatValue());
}

IEEEfloat& IEEEfloat::operator/= (IEEEfloat op)
{
    *this = *this / op;
    return *this;
}


void IEEEdouble::Fmodulus(IEEEdouble a, IEEEdouble b, IEEEdouble& result)
{
     result.DoubleValue() = fmod(a.DoubleValue(), b.DoubleValue());
}

void IEEEdouble::Divide(IEEEdouble dividend, IEEEdouble divisor, IEEEdouble &quotient)
{
    quotient = dividend.DoubleValue() / divisor.DoubleValue();
    return;
}

IEEEfloat::IEEEfloat(IEEEdouble a)
{
    FloatValue() = a.DoubleValue();
}

IEEEdouble::IEEEdouble(LongInt& a)
{
    DoubleValue() = a.Double();
}

void IEEEfloat::Fmodulus(IEEEfloat a, IEEEfloat b, IEEEfloat& result)
{
    result.FloatValue() = (float) fmod((double) a.FloatValue(), (double) b.FloatValue());
}

void IEEEfloat::String(char * str)
{
    // format value into character string
    sprintf(str, "%E", FloatValue());
}
void IEEEdouble::String(char * str)
{
    // format value into character string
    sprintf(str, "%E", DoubleValue());
}

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
#ifndef IEEE_DIV_0
    return IEEEdouble(DoubleValue() / op.DoubleValue());
#else /* IEEE_DIV_0 */
    if (op.DoubleValue() == 0.0)
    {
	if (DoubleValue() < 0.0)
	    return NEGATIVE_INFINITY();
	else if (DoubleValue() == 0.0)
	    return NaN();
	else
	    return POSITIVE_INFINITY();
    }
    else
	return IEEEdouble(DoubleValue() / op.DoubleValue());
#endif /* IEEE_DIV_0 */
}

IEEEdouble& IEEEdouble::operator/= (IEEEdouble op)
{
#ifndef IEEE_DIV_0
    *this = *this / op;
#else /* IEEE_DIV_0 */
    if (op.DoubleValue() == 0.0)
    {
	if (*this < 0.0)
	    *this = NEGATIVE_INFINITY();
	else if (*this == 0.0)
	    *this = NaN();
	else
	    *this = POSITIVE_INFINITY();
    }
    else
	*this = *this / op;
#endif /* IEEE_DIV_0 */
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
#ifndef IEEE_DIV_0
    return IEEEfloat(FloatValue() / op.FloatValue());
#else /* IEEE_DIV_0 */
    if (op.FloatValue() == 0.0)
    {
	if (FloatValue() < 0.0)
	    return IEEEfloat(IEEEdouble::NEGATIVE_INFINITY());
	else if (FloatValue() == 0.0)
	    return IEEEfloat(IEEEdouble::NaN());
	else
	    return IEEEfloat(IEEEdouble::POSITIVE_INFINITY());
    }
    else
	return IEEEfloat(FloatValue() / op.FloatValue());
#endif /* IEEE_DIV_0 */
}

IEEEfloat& IEEEfloat::operator/= (IEEEfloat op)
{
#ifndef IEEE_DIV_0
    *this = *this / op;
#else /* IEEE_DIV_0 */
    if (op.FloatValue() == 0.0)
    {
	if (*this < (float)0.0)
	    *this = IEEEdouble::NEGATIVE_INFINITY();
	else if (*this == (float)0.0)
	    *this = IEEEdouble::NaN();
	else
	    *this = IEEEdouble::POSITIVE_INFINITY();
    }
    else
	*this = *this / op;
#endif /* IEEE_DIV_0 */
    return *this;
}


void IEEEdouble::Fmodulus(IEEEdouble a, IEEEdouble b, IEEEdouble& result)
{
#ifndef IEEE_DIV_0
     result.DoubleValue() = fmod(a.DoubleValue(), b.DoubleValue());
#else /* IEEE_DIV_0 */
    if (b.DoubleValue() == 0)
	result.DoubleValue() = NaN().DoubleValue();
    else
	result.DoubleValue() = fmod(a.DoubleValue(), b.DoubleValue());
#endif /* IEEE_DIV_0 */
}

void IEEEdouble::Divide(IEEEdouble dividend, IEEEdouble divisor, IEEEdouble &quotient)
{
#ifndef IEEE_DIV_0
    quotient = dividend.DoubleValue() / divisor.DoubleValue();
#else /* IEEE_DIV_0 */
    if (divisor.DoubleValue() == 0)
    {
	if (dividend.DoubleValue() < 0.0)
	    quotient = NEGATIVE_INFINITY();
	else if (dividend.DoubleValue() == 0.0)
	    quotient = NaN();
	else
	    quotient = POSITIVE_INFINITY();
    }
    else
	quotient = dividend.DoubleValue() / divisor.DoubleValue();
#endif /* IEEE_DIV_0 */
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
#ifndef IEEE_DIV_0
    result.FloatValue() = (float) fmod((double) a.FloatValue(), (double) b.FloatValue());
#else /* IEEE_DIV_0 */
    if (b.FloatValue() == 0.0)
	result.FloatValue() = (IEEEdouble::NaN()).DoubleValue();
    else
	result.FloatValue() = fmod((double) a.FloatValue(), (double) b.FloatValue());
#endif /* IEEE_DIV_0 */
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

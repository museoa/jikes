dnl @synopsis AC_CHECK_MATHLIB
dnl
dnl This check will check for the math.h include file and see if the
dnl math library -lm needs to be added to the compiler LIBS.
dnl
dnl @version $Id$
dnl @author Mo DeJong <mdejong@cygnus.com>
dnl
AC_DEFUN([AC_CHECK_MATHLIB],
[
AC_CHECK_HEADERS(math.h)
AC_CACHE_CHECK(if math lib is required, ac_cv_need_math_lib,
[
  AC_TRY_LINK([
#ifdef HAVE_MATH_H
#include <math.h>
#endif
], [ floor(0.0); ],
  ac_cv_need_math_lib=no,
  ac_cv_need_math_lib=yes)
])

if test "$ac_cv_need_math_lib" = "yes" ; then
    AC_CHECK_LIB(m, floor, , AC_MSG_ERROR([Could not find floor function in math library]) )

    dnl Double check that we can link to the math lib
    AC_TRY_LINK([
#ifdef HAVE_MATH_H
#include <math.h>
#endif
], [ floor(0.0); ], , AC_MSG_ERROR([Could not link program that uses math library]))

fi
]
)


dnl @synopsis AC_CXX_CHECK_OSTREAM_CONST_UNSIGNED_CHAR_PTR
dnl
dnl Check to see if calling ostream << const unsigned char *
dnl causes a compiler error.
dnl
dnl @version $Id$
dnl @author Eric Blake <ebb9@email.byu.edu>
dnl
AC_DEFUN([AC_CXX_CHECK_OSTREAM_CONST_UNSIGNED_CHAR_PTR],
[AC_CACHE_CHECK(for presence of ostream << const unsigned char *,
ac_cv_cxx_ostream_const_unsigned_char_ptr,
[AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <iostream.h>
void foo(const unsigned char *arg) { cerr << arg; }
],[])],
 [ac_cv_cxx_ostream_const_unsigned_char_ptr=yes],
 [ac_cv_cxx_ostream_const_unsigned_char_ptr=no])
])
if test "$ac_cv_cxx_ostream_const_unsigned_char_ptr" = "yes"; then
  AC_DEFINE(HAVE_OSTREAM_CONST_UNSIGNED_CHAR_PTR, ,
    [compiler has ostream << const unsigned char *] )
fi
])


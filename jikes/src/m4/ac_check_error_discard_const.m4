dnl @synopsis AC_CHECK_ERROR_DISCARD_CONST
dnl
dnl Check to see if calling a non const function with a const
dnl argument causes a compiler error. If it does then discarding
dnl a const qualifier is treated as an error by this compiler.
dnl
dnl @version $Id$
dnl @author Mo DeJong <mdejong@cygnus.com>
dnl
AC_DEFUN([AC_CHECK_ERROR_DISCARD_CONST],
[AC_CACHE_CHECK(for compiler error on discard of const qualifier,
ac_cv_error_discard_const,
[AC_TRY_COMPILE([
void foo(char * arg) {}
void const_foo(const char * arg) { foo(arg); }
],[],
 ac_cv_error_discard_const=no, ac_cv_error_discard_const=yes)
])
if test "$ac_cv_error_discard_const" = "yes"; then
  AC_DEFINE(HAVE_ERROR_DISCARD_CONST, ,
    [compiler will generate an error when discarding a const qualifier] )
fi
])

dnl @synopsis AC_CHECK_ICONV
dnl
dnl Check to see if the iconv functions and libs are present
dnl @version $Id$
dnl @author Mo DeJong <mdejong@redhat.com>
dnl
AC_DEFUN(AC_CHECK_ICONV,
[
dnl	Check for the iconv header file
AC_CHECK_HEADERS(iconv.h)

dnl	Check if iconv library is present
AC_CHECK_LIB(iconv, iconv)

iconv_includes="
#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif
#ifdef HAVE_ICONV_H
# include <iconv.h>
#endif
"

dnl	Make sure we can compile a simple call to iconv
AC_CACHE_CHECK(simple call to iconv,
  ac_cv_call_iconv,
[
AC_TRY_COMPILE($iconv_includes, [iconv(0, NULL, NULL, NULL, NULL)],
  ac_cv_call_iconv=yes,
  ac_cv_call_iconv=no)
])

dnl No additional testing is possible if regular call does not work.
dnl In this case, we just assume iconv can not be used by the application

if test "$ac_cv_call_iconv" = "yes" ; then

dnl	Check for declaration of iconv without const qualifier in 2nd argument
AC_CACHE_CHECK(for error calling iconv with const 2nd argument,
  ac_cv_error_call_iconv_const,
[AC_TRY_COMPILE($iconv_includes, [
const char ** s = NULL;
iconv(0, s, NULL, NULL, NULL);
],
 ac_cv_error_call_iconv_const=no, ac_cv_error_call_iconv_const=yes)
])

if test "$ac_cv_error_call_iconv_const" = "yes"; then

  dnl define symbol so we know when to cast the argument to iconv
  dnl from const char ** to char *

  AC_DEFINE(HAVE_ERROR_CALL_ICONV_CONST, ,
    [Defined when the compiler would generate an error on a call to iconv with a non const 2nd argument. This is a known problem on IRIX systems.])

  dnl Double check that our suggested fix works.

  AC_CACHE_CHECK(fix for calling iconv with non const argument,
    ac_cv_call_iconv_non_const,
  [AC_TRY_COMPILE($iconv_includes, [
  const wchar_t * s = NULL;
  iconv( 0,
  #ifdef HAVE_ERROR_CALL_ICONV_CONST
           (char **)
  #endif
         s, NULL, NULL, NULL);
  ],
    ac_cv_call_iconv_non_const=yes,
    AC_MSG_ERROR([Could not compile with iconv const cast fix])
  )])

fi

fi

]
)

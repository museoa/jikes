dnl @synopsis AC_CXX_CHECK_SET_NEW_HANDLER
dnl
dnl Check the type of the method passed to set_new_handler
dnl the standard call for pointer to "void f()" but VC++
dnl requires a pointer to "int f(size_t)". If we do not
dnl find a VC++ style impl, then double check that the
dnl standard impl works as expected. This method will
dnl define HAVE_VCPP_SET_NEW_HANDLER if a VC++ style
dnl implementation is found, otherwise it does nothing.
dnl This method also checks for the C++ standard include
dnl <new> and the old style <new.h>.
dnl
dnl You include file could look like this:
dnl
dnl #ifdef HAVE_NEW
dnl # include <new>
dnl # ifdef HAVE_NAMESPACES
dnl   using namespace std;
dnl # endif
dnl #else
dnl # ifdef HAVE_NEW_H
dnl #  include <new.h>
dnl # endif
dnl #endif
dnl
dnl Your source code should look something like this:
dnl
dnl #ifdef HAVE_VCPP_SET_NEW_HANDLER
dnl int OutOfMemory(size_t)
dnl #else
dnl void OutOfMemory()
dnl #endif
dnl {
dnl   fprintf(stderr, "***System Failure: Out of memory\n");
dnl   exit(1);
dnl 
dnl #ifdef HAVE_VCPP_SET_NEW_HANDLER
dnl   return 0;
dnl #endif
dnl }
dnl
dnl void SetNewHandler()
dnl {
dnl #ifdef  HAVE_VCPP_SET_NEW_HANDLER
dnl   _set_new_handler(OutOfMemory);
dnl #else
dnl   set_new_handler(OutOfMemory);
dnl #endif
dnl }
dnl
dnl @version $Id$
dnl @author Mo DeJong <mdejong@cygnus.com>
dnl

AC_DEFUN(AC_CXX_CHECK_SET_NEW_HANDLER,
[
AC_REQUIRE([AC_CXX_HAVE_STD])

dnl If new style headers do not work, then just test for new.h
if test "$ac_cv_cxx_have_std" = yes; then
  AC_CHECK_HEADERS(new)
fi
AC_CHECK_HEADERS(new.h)

AC_CACHE_CHECK(for VC++ style set_new_handler, ac_cv_vcpp_set_new_handler,
[
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_LINK([
#ifdef HAVE_NEW
# include <new>
# ifdef HAVE_NAMESPACES
    using namespace std;
# endif
#else
# ifdef HAVE_NEW_H
#  include <new.h>
# endif
#endif

int OutOfMemory(size_t) { return 0; }
], [ _set_new_handler(OutOfMemory); ],
  ac_cv_vcpp_set_new_handler=yes,
  ac_cv_vcpp_set_new_handler=no)
  AC_LANG_RESTORE
])

if test "$ac_cv_vcpp_set_new_handler" = "yes" ; then
    AC_DEFINE(HAVE_VCPP_SET_NEW_HANDLER, ,[Use visual C++ version of set_new_handler])
else

dnl Double check that the standard set_new_handler actually works.
AC_CACHE_CHECK(for standard set_new_handler, ac_cv_standard_set_new_handler,
    AC_TRY_COMPILE([
#ifdef HAVE_NEW
# include <new>
# ifdef HAVE_NAMESPACES
    using namespace std;
# endif
#else
# ifdef HAVE_NEW_H
#  include <new.h>
# endif
#endif

void OutOfMemory() { return; }
],
[ set_new_handler(OutOfMemory); ],
   ac_cv_standard_set_new_handler=yes,
   ac_cv_standard_set_new_handler=no)
)

if test "$ac_cv_standard_set_new_handler" != "yes" ; then
    AC_MSG_ERROR([Cound not find standard set_new_handler function])
fi

fi

]
)


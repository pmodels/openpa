dnl/*D
dnl PAC_FUNC_NEEDS_DECL - Set NEEDS_<funcname>_DECL if a declaration is needed
dnl
dnl Synopsis:
dnl PAC_FUNC_NEEDS_DECL(headerfiles,funcname)
dnl
dnl Output Effect:
dnl Sets 'NEEDS_<funcname>_DECL' if 'funcname' is not declared by the
dnl headerfiles.
dnl
dnl Approach:
dnl Attempt to assign library function to function pointer.  If the function
dnl is not declared in a header, this will fail.  Use a non-static global so
dnl the compiler does not warn about an unused variable.
dnl
dnl Simply calling the function is not enough because C89 compilers allow
dnl calls to implicitly-defined functions.  Re-declaring a library function
dnl with an incompatible prototype is also not sufficient because some
dnl compilers (notably clang-3.2) only produce a warning in this case.
dnl
dnl D*/
AC_DEFUN([PAC_FUNC_NEEDS_DECL],[
AC_CACHE_CHECK([whether $2 needs a declaration],
pac_cv_func_decl_$2,[
AC_TRY_COMPILE([$1
void (*fptr)(void) = (void(*)(void))$2;],[],
pac_cv_func_decl_$2=no,pac_cv_func_decl_$2=yes)])
if test "$pac_cv_func_decl_$2" = "yes" ; then
changequote(<<,>>)dnl
define(<<PAC_FUNC_NAME>>, translit(NEEDS_$2_DECL, [a-z *], [A-Z__]))dnl
changequote([, ])dnl
    AC_DEFINE_UNQUOTED(PAC_FUNC_NAME,1,[Define if $2 needs a declaration])
undefine([PAC_FUNC_NAME])
fi
])

SUBDIRS = doc src

# Include the RPM spec file in the .tar ball

EXTRA_DIST = jikes.spec

# The aclocal tool does not follow the includes in acinclude.m4, so
# we manually add in the dependence.
INCLUDED_M4_FILES = \
$(srcdir)/src/m4/ac_check_error_discard_const.m4 \
$(srcdir)/src/m4/ac_check_iconv.m4 \
$(srcdir)/src/m4/ac_check_mathlib.m4 \
$(srcdir)/src/m4/ac_check_wcs_funcs.m4 \
$(srcdir)/src/m4/ac_cxx_bool.m4 \
$(srcdir)/src/m4/ac_cxx_check_ostream_const_unsigned_char_ptr.m4 \
$(srcdir)/src/m4/ac_cxx_check_set_new_handler.m4 \
$(srcdir)/src/m4/ac_cxx_const_cast.m4 \
$(srcdir)/src/m4/ac_cxx_dynamic_cast.m4 \
$(srcdir)/src/m4/ac_cxx_exceptions.m4 \
$(srcdir)/src/m4/ac_cxx_explicit.m4 \
$(srcdir)/src/m4/ac_cxx_have_std.m4 \
$(srcdir)/src/m4/ac_cxx_member_constants.m4 \
$(srcdir)/src/m4/ac_cxx_namespaces.m4 \
$(srcdir)/src/m4/ac_cxx_reinterpret_cast.m4 \
$(srcdir)/src/m4/ac_cxx_rtti.m4 \
$(srcdir)/src/m4/ac_cxx_static_cast.m4 \
$(srcdir)/src/m4/ac_cxx_type_equivalence.m4 \
$(srcdir)/src/m4/ac_define_integer_bits.m4 \
$(srcdir)/src/m4/ac_find_encoding.m4 \
$(srcdir)/src/m4/ac_try_encoding.m4 \
$(srcdir)/src/m4/acx_check_pathname_style.m4

$(srcdir)/acinclude.m4: @MAINTAINER_MODE_TRUE@ $(INCLUDED_M4_FILES)
	$(TOUCH) $(srcdir)/acinclude.m4

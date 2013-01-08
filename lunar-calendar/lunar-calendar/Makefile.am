CLEANFILES =

INCLUDES =                                              \
	-I$(top_srcdir)					\
        -I$(srcdir) 					\
	$(LUNAR_CALENDAR_CFLAGS)				\
	$(WARN_CFLAGS)

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

if OS_WIN32
lunar_calendar_def = lunar-calendar.def
lunar_calendar_win32_symbols = -export-symbols $(srcdir)/lunar-calendar.def

lunar_calendar_win32_res = lunar-calendar-win32-res.o
lunar_calendar_win32_res_ldflag = -Wl,lunar-calendar-win32-res.o

lunar-calendar-win32-res.o : lunar-calendar-win32.rc
	$(WINDRES) lunar-calendar-win32.rc $@

install-def-file: lunar-calendar.def
	$(INSTALL) $(srcdir)/lunar-calendar.def $(DESTDIR)$(libdir)/lunar-calendar-win32-2.0.def

uninstall-def-file:
	-rm $(DESTDIR)$(libdir)/lunar-calendar-win32-2.0.def
else
install-def-file:
uninstall-def-file:
endif

if MS_LIB_AVAILABLE
noinst_DATA = lunar-calendar-2.0.lib

lunar-calendar-2.0.lib: liblunar-calendar-2.0.la lunar-calendar.def
	lib -machine:@LIB_EXE_MACHINE_FLAG@ -name:liblunar-calendar-2.0-$(LT_CURRENT_MINUS_AGE).dll -def:lunar-calendar.def -out:$@

install_ms_lib:
	$(INSTALL) lunar-calendar-2.0.lib $(DESTDIR)$(libdir)

uninstall_ms_lib:
	-rm $(DESTDIR)$(libdir)/lunar-calendar-2.0.lib
else
install-ms-lib:
uninstall-ms-lib:
endif

# This places the generated .def file in srcdir, since it is expected to be there.
# (The one from a tarball is)
lunar-calendar.def: lunar-calendar.symbols
	$(AM_V_GEN) (echo -e EXPORTS; $(CPP) -P -DINCLUDE_VARIABLES -DINCLUDE_INTERNAL_SYMBOLS -DG_OS_WIN32 -DALL_FILES - <$(srcdir)/lunar-calendar.symbols | sed -e '/^$$/d' -e 's/^/	/' -e 's/G_GNUC_[^ ]*//g') > $(srcdir)/lunar-calendar.def

deps =

libtool_opts =							\
  -version-info $(LT_VERSION_INFO)				\
  -export-dynamic $(no_undefined) $(LIBTOOL_EXPORT_OPTIONS)	\
  -rpath $(libdir) $(liblunar_calendar_target_ldflags)


source_h = lunar-calendar.h 
source_c = lunar-calendar.c

lib_LTLIBRARIES = liblunar-calendar-2.0.la liblunar-calendar-preload-2.0.la

liblunar_calendar_2_0_includedir = $(includedir)/liblunar-2.0/lunar-calendar
liblunar_calendar_2_0_include_HEADERS =	$(source_h)
liblunar_calendar_2_0_la_SOURCES = $(source_c)
liblunar_calendar_2_0_la_LIBADD = $(LUNAR_CALENDAR_LIBS)
liblunar_calendar_2_0_la_LDFLAGS = $(libtool_opts)
liblunar_calendar_2_0_la_DEPENDENCIES = $(deps)

if OS_WIN32
liblunar_calendar_target_ldflags = $(lunar_calendar_win32_res_ldflag) $(lunar_calendar_win32_symbols)
liblunar_calendar_2_0_la_DEPENDENCIES += $(lunar_calendar_def) $(lunar_calendar_win32_res)
endif

install-calendar-local: install-ms-lib install-def-file

liblunar_calendar_preload_2_0_includedir = $(includedir)/liblunar-2.0/lunar-calendar
liblunar_calendar_preload_2_0_include_HEADERS = $(source_h)
liblunar_calendar_preload_2_0_la_SOURCES = lunar-calendar-preload.c
liblunar_calendar_preload_2_0_la_LDFLAGS =  -version-info $(LT_VERSION_INFO)
liblunar_calendar_preload_2_0_la_LIBADD = 	\
	$(LUNAR_CALENDAR_LIBS)	\
	liblunar-calendar-2.0.la		\
	-ldl			\
	$(NULL)

EXTRA_DIST =  lunar-calendar-win32.rc.in lunar-calendar.symbols

-include $(top_srcdir)/git.mk

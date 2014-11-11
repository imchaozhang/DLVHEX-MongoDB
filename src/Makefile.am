AUTOMAKE_OPTIONS = gnu

plugin_LTLIBRARIES = libdlvhexplugin-mongo.la

AM_CPPFLAGS= -I$(top_srcdir)/include \
             $(DEBUG_FLAGS) \
             $(BOOST_CPPFLAGS) \
             $(MONGO_INCLUDES) \
             $(DLVHEX_CFLAGS)

libdlvhexplugin_mongo_la_SOURCES = MongoPlugin.cpp \
				   MongoQueryAtom.cpp \
				   MongoInsertAtom.cpp \
				   MongoArrayAtom.cpp \
				   MongoJSONAtom.cpp \
				   MongoMapReduceAtom.cpp \
				   ConfigFile.cpp

libdlvhexplugin_mongo_la_LDFLAGS =-version-info="" -avoid-version -module $(MONGO_LIBS)

libdlvhexplugin-mongo-static.la: $(libdlvhexplugin_mongo_la_OBJECTS)
	$(CXXLINK) -avoid-version -module -rpath $(plugindir) $(libdlvhexplugin_mongo_la_OBJECTS)

install-static: libdlvhexplugin-mongo-static.la
	$(LIBTOOL) --mode=install $(INSTALL) -s libdlvhexplugin-mongo-static.la $(DESTDIR)$(plugindir)/libdlvhexplugin-mongo-static.la

uninstall-static:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(plugindir)/libdlvhexplugin-mongo-static.la

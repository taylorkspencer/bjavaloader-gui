## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/bjavaloader_gui/ui
ui_DATA = bjavaloader_gui.ui

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(BJAVALOADER_GUI_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = bjavaloader_gui

bjavaloader_gui_SOURCES = \
	main.cc          \
	main.h

bjavaloader_gui_LDFLAGS = 

bjavaloader_gui_LDADD = $(BJAVALOADER_GUI_LIBS)

EXTRA_DIST = $(ui_DATA) 

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/bjavaloader_gui

#
#   "$Id: Makefile,v 1.13.2.8 2001/12/20 15:55:20 mike Exp $"
#
#   Makefile for HTMLDOC, an HTML document processing program.
#
#   Copyright 1997-2001 by Easy Software Products.
#
#   These coded instructions, statements, and computer programs are the
#   property of Easy Software Products and are protected by Federal
#   copyright law.  Distribution and use rights are outlined in the file
#   "COPYING.txt" which should have been included with this file.  If this
#   file is missing or damaged please contact Easy Software Products
#   at:
#
#       Attn: ESP Licensing Information
#       Easy Software Products
#       44141 Airport View Drive, Suite 204
#       Hollywood, Maryland 20636-3111 USA
#
#       Voice: (301) 373-9600
#       EMail: info@easysw.com
#         WWW: http://www.easysw.com
#

#
# Include common definitions...
#

include ../Makedefs


#
# Object files...
#

OBJS	=	gui.o file.o html.o htmldoc.o htmllib.o http.o \
		image.o iso8859.o md5.o progress.o ps-pdf.o rc4.o \
		snprintf.o string.o toc.o util.o


#
# Make everything...
#

all:	htmldoc

#
# Install everything...
#

install:	all
	if [ ! -d $(bindir) ]; then\
		$(MKDIR) $(bindir);\
	fi
	cp htmldoc $(bindir)
	chmod ugo+rx $(bindir)/htmldoc


#
# Clean out object and executable files...
#

clean:
	rm -f $(OBJS) htmldoc

#
# htmldoc
#

htmldoc:	$(OBJS) ../Makedefs
	echo Linking $@...
	$(CXX) $(LDFLAGS) -o htmldoc $(OBJS) $(LIBS)
	$(POST) htmldoc

htmldoc.purify:	$(OBJS) ../Makedefs
	echo Linking $@...
	purify $(CXX) $(LDFLAGS) -o htmldoc.purify $(OBJS) $(LIBS)

file.o:		file.h http.h md5.h progress.h hdstring.h
gui.o:		gui.h html.h htmldoc.h image.h iso8859.h progress.h \
		types.h htmldoc.xbm htmldoc.xpm
html.o:		html.h htmldoc.h image.h iso8859.h progress.h types.h
htmldoc.o:	gui.h html.h htmldoc.h image.h iso8859.h progress.h types.h
htmllib.o:	debug.h htmldoc.h html.h image.h iso8859.h progress.h types.h
http.o:		http.h hdstring.h
image.o:	debug.h htmldoc.h image.h progress.h types.h
iso8859.o:	iso8859.h types.h
md5.o:		md5.h
ps-pdf.o:	debug.h html.h htmldoc.h image.h iso8859.h md5.h progress.h \
		rc4.h types.h
rc4.o:		rc4.h
string.o:	hdstring.h
toc.o:		html.h htmldoc.h image.h iso8859.h types.h
util.o:		htmldoc.h types.h

#
# testhtml
#

testhtml:	testhtml.o file.o gui.o htmllibd.o http.o image.o iso8859.o \
		md5.o rc4.o progress.o ps-pdf.o toc.o util.o html.o ../Makedefs
	echo Linking $@...
	$(CXX) $(LDFLAGS) -o testhtml testhtml.o file.o gui.o htmllibd.o \
		http.o image.o iso8859.o progress.o ps-pdf.o toc.o util.o \
		md5.o rc4.o html.o $(LIBS)

testhtml.o:	gui.h html.h htmldoc.h image.h iso8859.h progress.h types.h
htmllibd.o:	htmllib.cxx ../config.h ../Makedefs debug.h html.h htmldoc.h \
		image.h iso8859.h types.h
	echo Compiling $<...
	$(CXX) $(CXXFLAGS) -DDEBUG -o htmllibd.o -c htmllib.cxx

$(OBJS):	../Makedefs ../config.h


#
# End of "$Id: Makefile,v 1.13.2.8 2001/12/20 15:55:20 mike Exp $".
#

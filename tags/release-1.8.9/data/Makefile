#
# "$Id: Makefile,v 1.4 2000/01/04 13:52:22 mike Exp $"
#
#   Makefile for HTMLDOC data files.
#
#   Copyright 1997-2000 by Easy Software Products.
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
# Character set/glyph files...
#

FILES	=	8859-1 8859-14 8859-15 8859-2 8859-3 8859-4 \
		8859-5 8859-6 8859-7 8859-8 8859-9 psglyphs


#
# Make everything...
#

all:


#
# Install everything...
#

install:
	if [ ! -d $(datadir)/htmldoc/data ]; then\
		$(MKDIR) $(datadir)/htmldoc/data;\
	fi
	$(CP) $(FILES) $(datadir)/htmldoc/data
	$(CHMOD) ugo+r $(datadir)/htmldoc/data/*


#
# Clean out object and library files...
#

clean:

#
# End of "$Id: Makefile,v 1.4 2000/01/04 13:52:22 mike Exp $".
#

#
# "$Id: Makefile,v 1.5.2.1 2001/02/02 15:11:04 mike Exp $"
#
#   JPEG library Makefile for the Common UNIX Printing System (CUPS).
#
#   Copyright 1997-2001 by Easy Software Products, all rights reserved.
#
#   These coded instructions, statements, and computer programs are the
#   property of Easy Software Products and are protected by Federal
#   copyright law.  Distribution and use rights are outlined in the file
#   "LICENSE.txt" which should have been included with this file.  If this
#   file is missing or damaged please contact Easy Software Products
#   at:
#
#       Attn: CUPS Licensing Information
#       Easy Software Products
#       44141 Airport View Drive, Suite 204
#       Hollywood, Maryland 20636-3111 USA
#
#       Voice: (301) 373-9603
#       EMail: cups-info@cups.org
#         WWW: http://www.cups.org
#

include ../Makedefs

#
# Object files...
#

OBJS	=	jmemnobs.o \
		jcapimin.o jcapistd.o jccoefct.o jccolor.o jcdctmgr.o \
		jchuff.o jcinit.o jcmainct.o jcmarker.o jcmaster.o jcomapi.o \
		jcparam.o jcphuff.o jcprepct.o jcsample.o jctrans.o \
		jdapimin.o jdapistd.o jdatadst.o jdatasrc.o jdcoefct.o \
		jdcolor.o jddctmgr.o jdhuff.o jdinput.o jdmainct.o jdmarker.o \
		jdmaster.o jdmerge.o jdphuff.o jdpostct.o jdsample.o \
		jdtrans.o jerror.o jfdctflt.o jfdctfst.o jfdctint.o \
		jidctflt.o jidctfst.o jidctint.o jidctred.o jquant1.o \
		jquant2.o jutils.o jmemmgr.o

#
# Make all targets...
#

all:	libjpeg.a

#
# Clean all targets and object files...
#

clean:
	$(RM) $(OBJS)
	$(RM) libjpeg.a

#
# libjpeg.a
#

libjpeg.a:	$(OBJS)
	echo Archiving $@...
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

$(OBJS):	jchuff.h jdhuff.h jdct.h jerror.h jinclude.h jmemsys.h \
		jmorecfg.h jpegint.h jpeglib.h jversion.h ../Makedefs

#
# End of "$Id: Makefile,v 1.5.2.1 2001/02/02 15:11:04 mike Exp $".
#

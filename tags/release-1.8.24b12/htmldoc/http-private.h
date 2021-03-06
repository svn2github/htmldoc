/*
 * "$Id: http-private.h,v 1.1.2.2 2004/02/06 03:51:09 mike Exp $"
 *
 *   Private HTTP definitions for the Common UNIX Printing System (CUPS).
 *
 *   Copyright 1997-2004 by Easy Software Products, all rights reserved.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Easy Software Products and are protected by Federal
 *   copyright law.  Distribution and use rights are outlined in the file
 *   "LICENSE.txt" which should have been included with this file.  If this
 *   file is missing or damaged please contact Easy Software Products
 *   at:
 *
 *       Attn: CUPS Licensing Information
 *       Easy Software Products
 *       44141 Airport View Drive, Suite 204
 *       Hollywood, Maryland 20636-3111 USA
 *
 *       Voice: (301) 373-9603
 *       EMail: cups-info@cups.org
 *         WWW: http://www.cups.org
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 */

#ifndef _CUPS_HTTP_PRIVATE_H_
#  define _CUPS_HTTP_PRIVATE_H_

/*
 * Include necessary headers...
 */

#  include <config.h>

#  include "http.h"

#  if defined HAVE_LIBSSL
/*
 * The OpenSSL library provides its own SSL/TLS context structure for its
 * IO and protocol management...
 */

#    include <openssl/err.h>
#    include <openssl/rand.h>
#    include <openssl/ssl.h>

typedef SSL http_tls_t;

#  elif defined HAVE_GNUTLS
/*
 * The GNU TLS library is more of a "bare metal" SSL/TLS library...
 */
#    include <gnutls/gnutls.h>

typedef struct
{
  gnutls_session	session;	/* GNU TLS session object */
  void			*credentials;	/* GNU TLS credentials object */
} http_tls_t;

#  elif defined(HAVE_CDSASSL)
/*
 * Darwin's Security framework provides its own SSL/TLS context structure
 * for its IO and protocol management...
 */

#    include <Security/SecureTransport.h>

typedef SSLConnectionRef http_tls_t;

#  endif /* HAVE_LIBSSL */

/*
 * Some OS's don't have hstrerror(), most notably Solaris...
 */

#  ifndef HAVE_HSTRERROR
extern const char *hd_hstrerror(int error);
#    define hstrerror hd_hstrerror
#  elif defined(_AIX) || defined(__osf__)
/*
 * AIX and Tru64 UNIX don't provide a prototype but do provide the function...
 */
extern const char *hstrerror(int error);
#  endif /* !HAVE_HSTRERROR */

#endif /* !_CUPS_HTTP_PRIVATE_H_ */

/*
 * End of "$Id: http-private.h,v 1.1.2.2 2004/02/06 03:51:09 mike Exp $".
 */

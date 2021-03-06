/*
 * "$Id$"
 *
 *   Hyper-Text Transport Protocol definitions for the Common UNIX Printing
 *   System (CUPS).
 *
 *   Copyright 1997-2005 by Easy Software Products, all rights reserved.
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
 *       Hollywood, Maryland 20636 USA
 *
 *       Voice: (301) 373-9600
 *       EMail: cups-info@cups.org
 *         WWW: http://www.cups.org
 *
 *   This file is subject to the Apple OS-Developed Software exception.
 */

#ifndef _CUPS_HTTP_H_
#  define _CUPS_HTTP_H_

/*
 * Include necessary headers...
 */

#  include <string.h>
#  include <time.h>
#  include <sys/types.h>
#  ifdef WIN32
#    include <winsock2.h>
#    include <ws2tcpip.h>
#  else
#    include <unistd.h>
#    include <sys/time.h>
#    include <sys/socket.h>
#    include <netdb.h>
#    include <netinet/in.h>
#    include <arpa/inet.h>
#    include <netinet/in_systm.h>
#    include <netinet/ip.h>
#    if !defined(__APPLE__) || !defined(TCP_NODELAY)
#      include <netinet/tcp.h>
#    endif /* !__APPLE__ || !TCP_NODELAY */
#    ifdef AF_LOCAL
#      include <sys/un.h>
#    endif /* AF_LOCAL */
#  endif /* WIN32 */

#  include "md5.h"


/*
 * C++ magic...
 */

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */


/*
 * Oh, the wonderful world of IPv6 compatibility.  Apparently some
 * implementations expose the (more logical) 32-bit address parts
 * to everyone, while others only expose it to kernel code...  To
 * make supporting IPv6 even easier, each vendor chose different
 * core structure and union names, so the same defines or code
 * can't be used on all platforms.
 *
 * The following will likely need tweaking on new platforms that
 * support IPv6 - the "s6_addr32" define maps to the 32-bit integer
 * array in the in6_addr union, which is named differently on various
 * platforms.
 */

#if defined(AF_INET6) && !defined(s6_addr32)
#  if defined(__sun)
#    define s6_addr32	_S6_un._S6_u32
#  elif defined(__FreeBSD__) || defined(__APPLE__)
#    define s6_addr32	__u6_addr.__u6_addr32
#  elif defined(WIN32)
/*
 * Windows only defines byte and 16-bit word members of the union and
 * requires special casing of all raw address code...
 */
#    define s6_addr32	error_need_win32_specific_code
#  endif /* __sun */
#endif /* AF_INET6 && !s6_addr32 */


/*
 * Limits...
 */

#  define HTTP_MAX_URI		1024	/* Max length of URI string */
#  define HTTP_MAX_HOST		256	/* Max length of hostname string */
#  define HTTP_MAX_BUFFER	1024	/* Max length of data buffer */
#  define HTTP_MAX_VALUE	256	/* Max header field value length */


/*
 * Types and structures...
 */

typedef enum http_auth_e		/**** HTTP authentication types ****/
{
  HTTP_AUTH_NONE,			/* No authentication in use */
  HTTP_AUTH_BASIC,			/* Basic authentication in use */
  HTTP_AUTH_MD5,			/* Digest authentication in use */
  HTTP_AUTH_MD5_SESS,			/* MD5-session authentication in use */
  HTTP_AUTH_MD5_INT,			/* Digest authentication in use for body */
  HTTP_AUTH_MD5_SESS_INT		/* MD5-session authentication in use for body */
} http_auth_t;

typedef enum http_encoding_e		/**** HTTP transfer encoding values ****/
{
  HTTP_ENCODE_LENGTH,			/* Data is sent with Content-Length */
  HTTP_ENCODE_CHUNKED			/* Data is chunked */
} http_encoding_t;

typedef enum http_encryption_e		/**** HTTP encryption values ****/
{
  HTTP_ENCRYPT_IF_REQUESTED,		/* Encrypt if requested (TLS upgrade) */
  HTTP_ENCRYPT_NEVER,			/* Never encrypt */
  HTTP_ENCRYPT_REQUIRED,		/* Encryption is required (TLS upgrade) */
  HTTP_ENCRYPT_ALWAYS			/* Always encrypt (SSL) */
} http_encryption_t;

typedef enum http_field_e		/**** HTTP field names ****/
{
  HTTP_FIELD_UNKNOWN = -1,		/* Unknown field */
  HTTP_FIELD_ACCEPT_LANGUAGE,		/* Accept-Language field */
  HTTP_FIELD_ACCEPT_RANGES,		/* Accept-Ranges field */
  HTTP_FIELD_AUTHORIZATION,		/* Authorization field */
  HTTP_FIELD_CONNECTION,		/* Connection field */
  HTTP_FIELD_CONTENT_ENCODING,		/* Content-Encoding field */
  HTTP_FIELD_CONTENT_LANGUAGE,		/* Content-Language field */
  HTTP_FIELD_CONTENT_LENGTH,		/* Content-Length field */
  HTTP_FIELD_CONTENT_LOCATION,		/* Content-Location field */
  HTTP_FIELD_CONTENT_MD5,		/* Content-MD5 field */
  HTTP_FIELD_CONTENT_RANGE,		/* Content-Range field */
  HTTP_FIELD_CONTENT_TYPE,		/* Content-Type field */
  HTTP_FIELD_CONTENT_VERSION,		/* Content-Version field */
  HTTP_FIELD_DATE,			/* Date field */
  HTTP_FIELD_HOST,			/* Host field */
  HTTP_FIELD_IF_MODIFIED_SINCE,		/* If-Modified-Since field */
  HTTP_FIELD_IF_UNMODIFIED_SINCE,	/* If-Unmodified-Since field */
  HTTP_FIELD_KEEP_ALIVE,		/* Keep-Alive field */
  HTTP_FIELD_LAST_MODIFIED,		/* Last-Modified field */
  HTTP_FIELD_LINK,			/* Link field */
  HTTP_FIELD_LOCATION,			/* Location field */
  HTTP_FIELD_RANGE,			/* Range field */
  HTTP_FIELD_REFERER,			/* Referer field */
  HTTP_FIELD_RETRY_AFTER,		/* Retry-After field */
  HTTP_FIELD_TRANSFER_ENCODING,		/* Transfer-Encoding field */
  HTTP_FIELD_UPGRADE,			/* Upgrade field */
  HTTP_FIELD_USER_AGENT,		/* User-Agent field */
  HTTP_FIELD_WWW_AUTHENTICATE,		/* WWW-Authenticate field */
  HTTP_FIELD_MAX			/* Maximum field index */
} http_field_t;

typedef enum http_keepalive_e		/**** HTTP keep-alive values ****/
{
  HTTP_KEEPALIVE_OFF = 0,		/* No keep alive support */
  HTTP_KEEPALIVE_ON			/* Use keep alive */
} http_keepalive_t;

typedef enum http_state_e		/**** HTTP state values; states
					 **** are server-oriented...
					 ****/
{
  HTTP_WAITING,				/* Waiting for command */
  HTTP_OPTIONS,				/* OPTIONS command, waiting for blank line */
  HTTP_GET,				/* GET command, waiting for blank line */
  HTTP_GET_SEND,			/* GET command, sending data */
  HTTP_HEAD,				/* HEAD command, waiting for blank line */
  HTTP_POST,				/* POST command, waiting for blank line */
  HTTP_POST_RECV,			/* POST command, receiving data */
  HTTP_POST_SEND,			/* POST command, sending data */
  HTTP_PUT,				/* PUT command, waiting for blank line */
  HTTP_PUT_RECV,			/* PUT command, receiving data */
  HTTP_DELETE,				/* DELETE command, waiting for blank line */
  HTTP_TRACE,				/* TRACE command, waiting for blank line */
  HTTP_CLOSE,				/* CLOSE command, waiting for blank line */
  HTTP_STATUS				/* Command complete, sending status */
} http_state_t;

typedef enum http_status_e		/**** HTTP status codes ****/
{
  HTTP_ERROR = -1,			/* An error response from httpXxxx() */

  HTTP_CONTINUE = 100,			/* Everything OK, keep going... */
  HTTP_SWITCHING_PROTOCOLS,		/* HTTP upgrade to TLS/SSL */

  HTTP_OK = 200,			/* OPTIONS/GET/HEAD/POST/TRACE command was successful */
  HTTP_CREATED,				/* PUT command was successful */
  HTTP_ACCEPTED,			/* DELETE command was successful */
  HTTP_NOT_AUTHORITATIVE,		/* Information isn't authoritative */
  HTTP_NO_CONTENT,			/* Successful command, no new data */
  HTTP_RESET_CONTENT,			/* Content was reset/recreated */
  HTTP_PARTIAL_CONTENT,			/* Only a partial file was recieved/sent */

  HTTP_MULTIPLE_CHOICES = 300,		/* Multiple files match request */
  HTTP_MOVED_PERMANENTLY,		/* Document has moved permanently */
  HTTP_MOVED_TEMPORARILY,		/* Document has moved temporarily */
  HTTP_SEE_OTHER,			/* See this other link... */
  HTTP_NOT_MODIFIED,			/* File not modified */
  HTTP_USE_PROXY,			/* Must use a proxy to access this URI */

  HTTP_BAD_REQUEST = 400,		/* Bad request */
  HTTP_UNAUTHORIZED,			/* Unauthorized to access host */
  HTTP_PAYMENT_REQUIRED,		/* Payment required */
  HTTP_FORBIDDEN,			/* Forbidden to access this URI */
  HTTP_NOT_FOUND,			/* URI was not found */
  HTTP_METHOD_NOT_ALLOWED,		/* Method is not allowed */
  HTTP_NOT_ACCEPTABLE,			/* Not Acceptable */
  HTTP_PROXY_AUTHENTICATION,		/* Proxy Authentication is Required */
  HTTP_REQUEST_TIMEOUT,			/* Request timed out */
  HTTP_CONFLICT,			/* Request is self-conflicting */
  HTTP_GONE,				/* Server has gone away */
  HTTP_LENGTH_REQUIRED,			/* A content length or encoding is required */
  HTTP_PRECONDITION,			/* Precondition failed */
  HTTP_REQUEST_TOO_LARGE,		/* Request entity too large */
  HTTP_URI_TOO_LONG,			/* URI too long */
  HTTP_UNSUPPORTED_MEDIATYPE,		/* The requested media type is unsupported */
  HTTP_UPGRADE_REQUIRED = 426,		/* Upgrade to SSL/TLS required */

  HTTP_SERVER_ERROR = 500,		/* Internal server error */
  HTTP_NOT_IMPLEMENTED,			/* Feature not implemented */
  HTTP_BAD_GATEWAY,			/* Bad gateway */
  HTTP_SERVICE_UNAVAILABLE,		/* Service is unavailable */
  HTTP_GATEWAY_TIMEOUT,			/* Gateway connection timed out */
  HTTP_NOT_SUPPORTED			/* HTTP version not supported */
} http_status_t;

typedef enum http_uri_status_e		/**** URI separation status @since CUPS1.2@ ****/
{
  HTTP_URI_OVERFLOW = -8,		/* URI buffer for httpAssembleURI is too small */
  HTTP_URI_BAD_ARGUMENTS = -7,		/* Bad arguments to function (error) */
  HTTP_URI_BAD_RESOURCE = -6,		/* Bad resource in URI (error) */
  HTTP_URI_BAD_PORT = -5,		/* Bad port number in URI (error) */
  HTTP_URI_BAD_HOSTNAME = -4,		/* Bad hostname in URI (error) */
  HTTP_URI_BAD_USERNAME = -3,		/* Bad username in URI (error) */
  HTTP_URI_BAD_SCHEME = -2,		/* Bad scheme in URI (error) */
  HTTP_URI_BAD_URI = -1,		/* Bad/empty URI (error) */
  HTTP_URI_OK = 0,			/* URI decoded OK */
  HTTP_URI_MISSING_SCHEME,		/* Missing scheme in URI (warning) */
  HTTP_URI_UNKNOWN_SCHEME,		/* Unknown scheme in URI (warning) */
  HTTP_URI_MISSING_RESOURCE		/* Missing resource in URI (warning) */
} http_uri_status_t;

typedef enum http_version_e		/**** HTTP version numbers ****/
{
  HTTP_0_9 = 9,				/* HTTP/0.9 */
  HTTP_1_0 = 100,			/* HTTP/1.0 */
  HTTP_1_1 = 101			/* HTTP/1.1 */
} http_version_t;

typedef union http_addr_u		/**** Socket address union, which
					 **** makes using IPv6 and other
					 **** address types easier and
					 **** more portable. @since CUPS 1.2@
					 ****/
{
  struct sockaddr	addr;		/* Base structure for family value */
  struct sockaddr_in	ipv4;		/* IPv4 address */
#ifdef AF_INET6
  struct sockaddr_in6	ipv6;		/* IPv6 address */
#endif /* AF_INET6 */
#ifdef AF_LOCAL
  struct sockaddr_un	un;		/* Domain socket file */
#endif /* AF_LOCAL */
  char			pad[256];	/* Padding to ensure binary compatibility */
} http_addr_t;

typedef struct http_addrlist_s		/**** Socket address list, which is
					 **** used to enumerate all of the
					 **** addresses that are associated
					 **** with a hostname. @since CUPS 1.2@
					 ****/
{
  struct http_addrlist_s *next;		/* Pointer to next address in list */
  http_addr_t		addr;		/* Address */
} http_addrlist_t;

typedef struct http_s			/**** HTTP connection structure. ****/
{
  int			fd;		/* File descriptor for this socket */
  int			blocking;	/* To block or not to block */
  int			error;		/* Last error on read */
  time_t		activity;	/* Time since last read/write */
  http_state_t		state;		/* State of client */
  http_status_t		status;		/* Status of last request */
  http_version_t	version;	/* Protocol version */
  http_keepalive_t	keep_alive;	/* Keep-alive supported? */
  struct sockaddr_in	_hostaddr;	/* Address of connected host @deprecated@ */
  char			hostname[HTTP_MAX_HOST],
  					/* Name of connected host */
			fields[HTTP_FIELD_MAX][HTTP_MAX_VALUE];
					/* Field values */
  char			*data;		/* Pointer to data buffer */
  http_encoding_t	data_encoding;	/* Chunked or not */
  int			_data_remaining;/* Number of bytes left @deprecated@ */
  int			used;		/* Number of bytes used in buffer */
  char			buffer[HTTP_MAX_BUFFER];
					/* Buffer for incoming data */
  char			wbuffer[HTTP_MAX_BUFFER];
					/* Buffer for outgoing data */
  int			auth_type;	/* Authentication in use */
  md5_state_t		md5_state;	/* MD5 state */
  char			nonce[HTTP_MAX_VALUE];
					/* Nonce value */
  int			nonce_count;	/* Nonce count */
  void			*tls;		/* TLS state information */
  http_encryption_t	encryption;	/* Encryption requirements */
  /**** New in CUPS 1.1.19 ****/
  fd_set		*input_set;	/* select() set for httpWait() @since CUPS 1.1.19@ */
  http_status_t		expect;		/* Expect: header @since CUPS 1.1.19@ */
  char			*cookie;	/* Cookie value(s) @since CUPS 1.1.19@ */
  /**** New in CUPS 1.1.20 ****/
  char			authstring[HTTP_MAX_VALUE],
					/* Current Authentication value @since CUPS 1.1.20@ */
			userpass[HTTP_MAX_VALUE];
					/* Username:password string @since CUPS 1.1.20@ */
  int			digest_tries;	/* Number of tries for digest auth @since CUPS 1.1.20@ */
  /**** New in CUPS 1.2 ****/
  http_addr_t		*hostaddr;	/* Current host address and port @since CUPS 1.2@ */
  http_addrlist_t	*addrlist;	/* List of valid addresses @since CUPS 1.2@ */
  int			wused;		/* Write buffer bytes used @since CUPS 1.2@ */
  off_t			data_remaining;	/* Number of bytes left @since CUPS 1.2@ */
} http_t;


/*
 * Prototypes...
 */

#  define		httpBlocking(http,b)	(http)->blocking = (b)
extern int		httpCheck(http_t *http);
#  define		httpClearFields(http)	memset((http)->fields, 0, sizeof((http)->fields)),\
						httpSetField((http), HTTP_FIELD_HOST, (http)->hostname)
extern void		httpClose(http_t *http);
extern http_t		*httpConnect(const char *host, int port);
extern http_t		*httpConnectEncrypt(const char *host, int port,
			                    http_encryption_t encryption);
extern int		httpDelete(http_t *http, const char *uri);
extern int		httpEncryption(http_t *http, http_encryption_t e);
#  define		httpError(http) ((http)->error)
extern void		httpFlush(http_t *http);
extern int		httpGet(http_t *http, const char *uri);
extern char		*httpGets(char *line, int length, http_t *http);
extern const char	*httpGetDateString(time_t t);
extern time_t		httpGetDateTime(const char *s);
#  define		httpGetField(http,field)	(http)->fields[field]
extern struct hostent	*httpGetHostByName(const char *name);
extern char		*httpGetSubField(http_t *http, http_field_t field,
			                 const char *name, char *value);
extern int		httpHead(http_t *http, const char *uri);
extern void		httpInitialize(void);
extern int		httpOptions(http_t *http, const char *uri);
extern int		httpPost(http_t *http, const char *uri);
extern int		httpPrintf(http_t *http, const char *format, ...)
#  ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 2, 3)))
#  endif /* __GNUC__ */
;
extern int		httpPut(http_t *http, const char *uri);
extern int		httpRead(http_t *http, char *buffer, int length);
extern int		httpReconnect(http_t *http);
extern void		httpSeparate(const char *uri, char *method,
			             char *username, char *host, int *port,
				     char *resource);
extern void		httpSetField(http_t *http, http_field_t field,
			             const char *value);
extern const char	*httpStatus(http_status_t status);
extern int		httpTrace(http_t *http, const char *uri);
extern http_status_t	httpUpdate(http_t *http);
extern int		httpWrite(http_t *http, const char *buffer, int length);
extern char		*httpEncode64(char *out, const char *in);
extern char		*httpDecode64(char *out, const char *in);
extern int		httpGetLength(http_t *http);
extern char		*httpMD5(const char *, const char *, const char *,
			         char [33]);
extern char		*httpMD5Final(const char *, const char *, const char *,
			              char [33]);
extern char		*httpMD5String(const unsigned char *, char [33]);

/**** New in CUPS 1.1.19 ****/
extern void		httpClearCookie(http_t *http);
#define httpGetCookie(http) ((http)->cookie)
extern void		httpSetCookie(http_t *http, const char *cookie);
extern int		httpWait(http_t *http, int msec);

/**** New in CUPS 1.1.21 ****/
extern char		*httpDecode64_2(char *out, int *outlen, const char *in);
extern char		*httpEncode64_2(char *out, int outlen, const char *in,
			                int inlen);
extern void		httpSeparate2(const char *uri,
			              char *method, int methodlen,
			              char *username, int usernamelen,
				      char *host, int hostlen, int *port,
				      char *resource, int resourcelen);

/**** New in CUPS 1.2 ****/
extern int		httpAddrAny(const http_addr_t *addr);
extern http_addrlist_t	*httpAddrConnect(http_addrlist_t *addrlist, int *sock);
extern int		httpAddrEqual(const http_addr_t *addr1,
			              const http_addr_t *addr2);
extern void		httpAddrFreeList(http_addrlist_t *addrlist);
extern http_addrlist_t	*httpAddrGetList(const char *hostname, int family,
			                 const char *service);
extern int		httpAddrLength(const http_addr_t *addr);
extern int		httpAddrLocalhost(const http_addr_t *addr);
extern char		*httpAddrLookup(const http_addr_t *addr,
                                        char *name, int namelen);
extern char		*httpAddrString(const http_addr_t *addr,
			                char *s, int slen);
extern http_uri_status_t httpAssembleURI(char *uri, int urilen,
			        	 const char *scheme,
					 const char *username,
					 const char *host, int port,
					 const char *resource);
extern http_uri_status_t httpAssembleURIf(char *uri, int urilen,
			        	  const char *scheme,
					  const char *username,
					  const char *host, int port,
					  const char *resourcef, ...);
extern int		httpFlushWrite(http_t *http);
extern const char	*httpGetDateString2(time_t t, char *s, int slen);
extern const char	*httpGetHostname(char *s, int slen);
extern off_t		httpGetLength2(http_t *http);
extern char		*httpGetSubField2(http_t *http, http_field_t field,
			                  const char *name, char *value,
					  int valuelen);
extern http_uri_status_t httpSeparateURI(const char *uri,
			        	 char *scheme, int schemelen,
			        	 char *username, int usernamelen,
					 char *host, int hostlen, int *port,
					 char *resource, int resourcelen);
extern void		httpSetLength(http_t *http, off_t length);


/*
 * C++ magic...
 */

#  ifdef __cplusplus
}
#  endif /* __cplusplus */
#endif /* !_CUPS_HTTP_H_ */

/*
 * End of "$Id$".
 */

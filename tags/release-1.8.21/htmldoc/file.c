/*
 * "$Id: file.c,v 1.13.2.38 2002/08/14 19:44:58 mike Exp $"
 *
 *   Filename routines for HTMLDOC, a HTML document processing program.
 *
 *   Copyright 1997-2002 by Easy Software Products.
 *
 *   These coded instructions, statements, and computer programs are the
 *   property of Easy Software Products and are protected by Federal
 *   copyright law.  Distribution and use rights are outlined in the file
 *   "COPYING.txt" which should have been included with this file.  If this
 *   file is missing or damaged please contact Easy Software Products
 *   at:
 *
 *       Attn: ESP Licensing Information
 *       Easy Software Products
 *       44141 Airport View Drive, Suite 204
 *       Hollywood, Maryland 20636-3111 USA
 *
 *       Voice: (301) 373-9600
 *       EMail: info@easysw.com
 *         WWW: http://www.easysw.com
 *
 * Contents:
 *
 *   file_basename()    - Return the base filename without directory or target.
 *   file_cleanup()     - Close an open HTTP connection and remove
 *                        temporary files...
 *   file_directory()   - Return the directory without filename or target.
 *   file_extension()   - Return the extension of a file without the target.
 *   file_find_check()  - Check to see if the specified file or URL exists...
 *   file_find()        - Find a file in one of the path directories.
 *   file_gets()        - Read a line from a file terminated with CR, LF,
 *                        or CR LF.
 *   file_localize()    - Localize a filename for the new working directory.
 *   file_method()      - Return the method for a filename or URL.
 *   file_nolocal()     - Disable access to local files.
 *   file_proxy()       - Set the proxy host for all HTTP requests.
 *   file_target()      - Return the target of a link.
 *   file_temp()        - Create and open a temporary file.
 */

/*
 * Include necessary headers.
 */

#include "file.h"
#include "http.h"
#include "progress.h"
#include "debug.h"

#if defined(WIN32)
#  include <io.h>
#else
#  include <unistd.h>
#endif /* WIN32 */

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>



/*
 * Temporary file definitions...
 */

#ifdef WIN32
#  define getpid	GetCurrentProcessId
#  define TEMPLATE	"%s%08x.%06d.tmp"
#  define OPENMODE	(_O_CREAT | _O_RDWR | _O_TRUNC | _O_BINARY)
#  define OPENPERM	(_S_IREAD | _S_IWRITE)
#else
#  define TEMPLATE	"%s/%06d.%06d.tmp"
#  define OPENMODE	(O_CREAT | O_RDWR | O_EXCL | O_TRUNC)
#  define OPENPERM	0600
#endif /* WIN32 */


/*
 * Cache file structure...
 */

typedef struct		/* Cache for all temporary files */
{
  char	*name;				/* Temporary filename */
  char	*url;				/* URL */
} cache_t;


/*
 * Local globals...
 */

char	proxy_host[HTTP_MAX_URI] = "";	/* Proxy hostname */
int	proxy_port = 0;			/* Proxy port */
http_t	*http = NULL;			/* Connection to remote server */
int	web_files = 0,			/* Number of temporary files */
	web_alloc = 0;			/* Number of allocated files */
cache_t	*web_cache = NULL;		/* Cache array */
int	no_local = 0;			/* Non-zero to disable local files */


/*
 * 'file_basename()' - Return the base filename without directory or target.
 */

const char *			/* O - Base filename */
file_basename(const char *s)	/* I - Filename or URL */
{
  const char	*basename;	/* Pointer to directory separator */
  static char	buf[1024];	/* Buffer for files with targets */


  if (s == NULL)
    return (NULL);

  if ((basename = strrchr(s, '/')) != NULL)
    basename ++;
  else if ((basename = strrchr(s, '\\')) != NULL)
    basename ++;
#ifdef MAC
  else if ((basename = strrchr(s, ':')) != NULL)
    basename ++;
#endif /* MAC */
  else
    basename = (char *)s;

  if (basename[0] == '#')
    return (NULL);

  if (strchr(basename, '#') == NULL)
    return (basename);

  strncpy(buf, basename, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  *(char *)strchr(buf, '#') = '\0';

  return (buf);
}


/*
 * 'file_cleanup()' - Close an open HTTP connection and remove temporary files...
 */

void
file_cleanup(void)
{
  int		i;			/* Looping var */
  char		filename[1024];		/* Temporary file */
#ifdef WIN32
  char		tmpdir[1024];		/* Temporary directory */
#else
  const char	*tmpdir;		/* Temporary directory */
#endif /* WIN32 */
  const char	*debug;			/* HTMLDOC_DEBUG env var */


  if (http)
  {
    httpClose(http);
    http = NULL;
  }

#ifdef WIN32
  GetTempPath(sizeof(tmpdir), tmpdir);
#else
  if ((tmpdir = getenv("TMPDIR")) == NULL)
    tmpdir = "/var/tmp";
#endif /* WIN32 */

 /*
  * Check to see if we want to leave the temporary files around for
  * debugging...
  */

  if ((debug = getenv("HTMLDOC_DEBUG")) != NULL &&
      (strstr(debug, "all") != NULL || strstr(debug, "tempfiles") != NULL))
  {
   /*
    * Yes, leave the files, but show the mapping from filename to URL...
    */

    progress_error(HD_ERROR_NONE, "DEBUG: Temporary File Summary");
    progress_error(HD_ERROR_NONE, "DEBUG:");
    progress_error(HD_ERROR_NONE, "DEBUG: URL                             Filename");
    progress_error(HD_ERROR_NONE, "DEBUG: ------------------------------- ---------------------");

    for (i = 0; i < web_files; i ++)
    {
      snprintf(filename, sizeof(filename), TEMPLATE, tmpdir,
               getpid(), i + 1);
      progress_error(HD_ERROR_NONE, "DEBUG: %-31.31s %s\n",
                     web_cache[i].url ? web_cache[i].url : "none", filename);
    }

    progress_error(HD_ERROR_NONE, "DEBUG:");

    return;
  }

  while (web_files > 0)
  {
    snprintf(filename, sizeof(filename), TEMPLATE, tmpdir,
             getpid(), web_files);

    if (unlink(filename))
      progress_error(HD_ERROR_DELETE_ERROR,
                     "Unable to delete temporary file \"%s\": %s",
                     filename, strerror(errno));

    web_files --;

    if (web_cache[web_files].name)
      free(web_cache[web_files].name);
    if (web_cache[web_files].url)
      free(web_cache[web_files].url);
  }

  if (web_alloc)
  {
    free(web_cache);

    web_alloc = 0;
    web_cache = NULL;
  }
}


/*
 * 'file_directory()' - Return the directory without filename or target.
 */

const char *			/* O - Directory for file */
file_directory(const char *s)	/* I - Filename or URL */
{
  char		*dir;		/* Pointer to directory separator */
  static char	buf[1024];	/* Buffer for files with targets */


  if (s == NULL)
    return (NULL);

  if (strncmp(s, "http://", 7) == 0 || strncmp(s, "https://", 8) == 0)
  {
   /*
    * Handle URLs...
    */

    char	method[HTTP_MAX_URI],
		username[HTTP_MAX_URI],
		hostname[HTTP_MAX_URI],
		resource[HTTP_MAX_URI];
    int		port;


    httpSeparate(s, method, username, hostname, &port, resource);
    if ((dir = strrchr(resource, '/')) != NULL)
      *dir = '\0';

    if (username[0])
      snprintf(buf, sizeof(buf), "%s://%s@%s:%d%s", method, username, hostname,
               port, resource);
    else
      snprintf(buf, sizeof(buf), "%s://%s:%d%s", method, hostname, port,
               resource);
  }
  else
  {
   /*
    * Normal stuff...
    */

    strncpy(buf, s, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    if ((dir = strrchr(buf, '/')) != NULL)
      *dir = '\0';
    else if ((dir = strrchr(buf, '\\')) != NULL)
      *dir = '\0';
#ifdef MAC
    else if ((dir = strrchr(buf, ':')) != NULL)
      *dir = '\0';
#endif /* MAC */
    else
      return (".");

    if (strncmp(buf, "file:", 5) == 0)
      strcpy(buf, buf + 5);

    if (!buf[0])
      strcpy(buf, "/");
  }

  return (buf);
}


/*
 * 'file_extension()' - Return the extension of a file without the target.
 */

const char *			/* O - File extension */
file_extension(const char *s)	/* I - Filename or URL */
{
  const char	*extension;	/* Pointer to directory separator */
  static char	buf[1024];	/* Buffer for files with targets */


  if (s == NULL)
    return (NULL);

  if ((extension = strrchr(s, '/')) != NULL)
    extension ++;
  else if ((extension = strrchr(s, '\\')) != NULL)
    extension ++;
#ifdef MAC
  else if ((extension = strrchr(s, ':')) != NULL)
    extension ++;
#endif /* MAC */
  else
    extension = s;

  if ((extension = strrchr(extension, '.')) == NULL)
    return ("");
  else
    extension ++;

  if (strchr(extension, '#') == NULL)
    return (extension);

  strncpy(buf, extension, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';

  *(char *)strchr(buf, '#') = '\0';

  return (buf);
}


/*
 * 'file_find_check()' - Check to see if the specified file or URL exists...
 */

static const char *			/* O - Pathname or NULL */
file_find_check(const char *filename)	/* I - File or URL */
{
  int		i;			/* Looping var */
  int		retry;			/* Current retry */
  char		method[HTTP_MAX_URI],	/* Method/scheme */
		username[HTTP_MAX_URI],	/* Username:password */
		hostname[HTTP_MAX_URI],	/* Hostname */
		resource[HTTP_MAX_URI];	/* Resource */
  int		port;			/* Port number */
  const char	*connhost;		/* Host to connect to */
  int		connport;		/* Port to connect to */
  char		connpath[HTTP_MAX_URI],	/* Path for GET */
		connauth[HTTP_MAX_VALUE];/* Auth string */
  http_status_t	status;			/* Status of request... */
  FILE		*fp;			/* Web file */
  int		bytes,			/* Bytes read */
		count,			/* Number of bytes so far */
		total;			/* Total bytes in file */
  char		tempname[HTTP_MAX_URI];	/* Temporary filename */


  DEBUG_printf(("file_find_check(filename=\"%s\")\n", filename));

  if (strncmp(filename, "http:", 5) == 0 || strncmp(filename, "//", 2) == 0)
    strcpy(method, "http");
#ifdef HAVE_LIBSSL
  else if (strncmp(filename, "https:", 6) == 0)
    strcpy(method, "https");
#endif /* HAVE_LIBSSL */
  else
    strcpy(method, "file");

  if (strcmp(method, "file") == 0)
  {
   /*
    * Return immediately if we aren't allowing access to local files...
    */

    if (no_local)
      return (NULL);

   /*
    * If the filename exists, return the filename...
    */

    if (!access(filename, 0))
    {
      DEBUG_printf(("file_find_check: Returning \"%s\"!\n", filename));
      return (filename);
    }
  }
  else
  {
   /*
    * Remote file; look it up in the web cache, and then try getting it
    * from the remote system...
    */

    for (i = 0; i < web_files; i ++)
      if (web_cache[i].url && strcmp(web_cache[i].url, filename) == 0)
      {
        DEBUG_printf(("file_find_check: Returning \"%s\" for \"%s\"!\n",
	              web_cache[i].name, filename));
        return (web_cache[i].name);
      }

    httpSeparate(filename, method, username, hostname, &port, resource);

    for (status = HTTP_ERROR, retry = 0;
         status == HTTP_ERROR && retry < 5;
         retry ++)
    {
      if (proxy_port)
      {
       /*
        * Send request to proxy host...
        */

        connhost = proxy_host;
        connport = proxy_port;
        snprintf(connpath, sizeof(connpath), "%s://%s:%d%s", method,
                 hostname, port, resource);
      }
      else
      {
       /*
        * Send request to host directly...
        */

        connhost = hostname;
        connport = port;
        strncpy(connpath, resource, sizeof(connpath) - 1);
	connpath[sizeof(connpath) - 1] = '\0';
      }

      if (http != NULL && strcasecmp(http->hostname, hostname) != 0)
      {
        httpClose(http);
        http = NULL;
      }

      if (http == NULL)
      {
        progress_show("Connecting to %s...", connhost);
        atexit(file_cleanup);

#ifdef HAVE_LIBSSL
        if (strcmp(method, "http") == 0)
          http = httpConnect(connhost, connport);
	else
          http = httpConnectEncrypt(connhost, connport, HTTP_ENCRYPT_ALWAYS);
#else
        http = httpConnect(connhost, connport);
#endif /* HAVE_LIBSSL */

        if (http == NULL)
	{
          progress_hide();
          progress_error(HD_ERROR_NETWORK_ERROR,
	                 "Unable to connect to %s!", connhost);
          return (NULL);
        }
      }

      progress_show("Getting %s...", connpath);

      httpClearFields(http);
      httpSetField(http, HTTP_FIELD_HOST, hostname);
      httpSetField(http, HTTP_FIELD_USER_AGENT, "HTMLDOC v" SVERSION);
      httpSetField(http, HTTP_FIELD_CONNECTION, "Keep-Alive");

      if (username[0])
      {
        strcpy(connauth, "Basic ");
        httpEncode64(connauth + 6, username);
        httpSetField(http, HTTP_FIELD_AUTHORIZATION, connauth);
      }

      if (!httpGet(http, connpath))
      {
	while ((status = httpUpdate(http)) == HTTP_CONTINUE);
      }
      else
	status = HTTP_ERROR;

      if (status >= HTTP_MOVED_PERMANENTLY && status <= HTTP_SEE_OTHER)
      {
       /*
        * Flush text...
	*/

	httpFlush(http);

       /*
        * Grab new location from HTTP data...
	*/

        httpSeparate(httpGetField(http, HTTP_FIELD_LOCATION), method, username,
	             hostname, &port, resource);
        status = HTTP_ERROR;
      }
    }

    if (status != HTTP_OK)
    {
      progress_hide();
      progress_error((HDerror)status, "%s", httpStatus(status));
      httpFlush(http);
      return (NULL);
    }

    if ((fp = file_temp(tempname, sizeof(tempname))) == NULL)
    {
      progress_hide();
      progress_error(HD_ERROR_WRITE_ERROR,
                     "Unable to create temporary file \"%s\": %s", tempname,
                     strerror(errno));
      httpFlush(http);
      return (NULL);
    }

    if ((total = atoi(httpGetField(http, HTTP_FIELD_CONTENT_LENGTH))) == 0)
      total = 8192;

    count = 0;
    while ((bytes = httpRead(http, resource, sizeof(resource))) > 0)
    {
      count += bytes;
      progress_update((100 * count / total) % 101);
      fwrite(resource, 1, bytes, fp);
    }

    progress_hide();

    fclose(fp);

    web_cache[web_files - 1].name = strdup(tempname);
    web_cache[web_files - 1].url  = strdup(filename);

    DEBUG_printf(("file_find_check: Returning \"%s\" for \"%s\"!\n",
		  tempname, filename));

    return (web_cache[web_files - 1].name);
  }

  return (NULL);
}


/*
 * 'file_find()' - Find a file in one of the path directories.
 */

const char *				/* O - Pathname or NULL */
file_find(const char *path,		/* I - Path "dir;dir;dir" */
          const char *s)		/* I - File to find */
{
  int		i;			/* Looping var */
  char		*temp;			/* Current position in filename */
  const char	*sptr;			/* Pointer into "s" */
  int		ch;			/* Quoted character */
  char		basename[HTTP_MAX_URI];	/* Base (unquoted) filename */
  const char	*realname;		/* Real filename */
  static char	filename[HTTP_MAX_URI];	/* Current filename */


 /*
  * If the filename is NULL, return NULL...
  */

  if (s == NULL)
    return (NULL);

  DEBUG_printf(("file_find(path=\"%s\", s=\"%s\")\n", path ? path : "(null)", s));

 /*
  * See if this is a cached remote file...
  */

  for (i = 0; i < web_files; i ++)
    if (strcmp(s, web_cache[i].name) == 0)
    {
      DEBUG_printf(("file_find: Returning cache file \"%s\"!\n", s));
      return (s);
    }

  DEBUG_printf(("file_find: \"%s\" not in web cache of %d files...\n",
                s, web_files));

 /*
  * Make sure the filename is not quoted...
  */

  if (strchr(s, '%') == NULL)
  {
    strncpy(basename, s, sizeof(basename) - 1);
    basename[sizeof(basename) - 1] = '\0';
  }
  else
  {
    for (sptr = s, temp = basename;
	 *sptr && temp < (basename + sizeof(basename) - 1);)
      if (*sptr == '%' && isxdigit(sptr[1]) && isxdigit(sptr[2]))
      {
       /*
	* Dequote %HH...
	*/

	if (isalpha(sptr[1]))
	  ch = (tolower(sptr[1]) - 'a' + 10) << 4;
	else
	  ch = (sptr[1] - '0') << 4;

	if (isalpha(sptr[2]))
	  ch |= tolower(sptr[2]) - 'a' + 10;
	else
	  ch |= sptr[2] - '0';

	*temp++ = ch;

	sptr += 3;
      }
      else
	*temp++ = *sptr++;

    *temp = '\0';
  }

 /*
  * If we got a complete URL, we don't use the path...
  */

  if (path != NULL && !path[0])
  {
    DEBUG_puts("file_find: Resetting path to NULL since path is empty...");
    path = NULL;
  }

  if (strncmp(s, "http:", 5) == 0 ||
      strncmp(s, "https:", 6) == 0 ||
      strncmp(s, "//", 2) == 0)
  {
    DEBUG_puts("file_find: Resetting path to NULL since filename is a URL...");
    path = NULL;
  }

 /*
  * Loop through the path as needed...
  */

  if (path != NULL)
  {
    filename[sizeof(filename) - 1] = '\0';

    while (*path != '\0')
    {
     /*
      * Copy the path directory...
      */

      temp = filename;

      while (*path != ';' && *path && temp < (filename + sizeof(filename) - 1))
	*temp++ = *path++;

      if (*path == ';')
	path ++;

     /*
      * Append a slash as needed, then the filename...
      */

      if (temp > filename && temp < (filename + sizeof(filename) - 1) &&
          basename[0] != '/')
	*temp++ = '/';

      strncpy(temp, basename, sizeof(filename) - (temp - filename) - 1);

     /*
      * See if the file or URL exists...
      */

      if ((realname = file_find_check(filename)) != NULL)
	return (realname);
    }
  }

  return (file_find_check(s));
}


/*
 * 'file_gets()' - Read a line from a file terminated with CR, LF, or CR LF.
 */

char *				/* O - Line from file or NULL on EOF */
file_gets(char  *buf,		/* I - Line buffer */
	  int   buflen,		/* I - Length of buffer */
	  FILE  *fp)		/* I - File to read from */
{
  int		ch;		/* Character from file */
  char		*ptr,		/* Current position in line buffer */
		*end;		/* End of line buffer */


 /*
  * Range check everything...
  */

  if (fp == NULL || buf == NULL || buflen < 2)
    return (NULL);

 /*
  * Now loop until we have a valid line...
  */

  ptr = buf;
  end = buf + buflen - 1;

  for (;;)
  {
    if ((ch = getc(fp)) == EOF)
      break;
    else if (ch == '\r')
    {
     /*
      * See if we have CR or CR LF...
      */

      int nextch = getc(fp);

      if (nextch == EOF || nextch == '\n')
        break;

     /*
      * No LF, so save the next char for later...
      */

      ungetc(nextch, fp);

      break;
    }
    else if (ch == '\n')
      break;
    else if (ch == '\\')
    {
     /*
      * Handle \ escapes, to continue to multiple lines...
      */

      int nextch = getc(fp);

      if (nextch == EOF)
        break;
      else if (nextch == '\r')
      {
        nextch = getc(fp);

	if (nextch == EOF)
	  break;
	else if (nextch != '\n')
	  ungetc(nextch, fp);
      }
      else if (nextch != '\n' && ptr < end)
        *ptr++ = nextch;
    }
    else if (ptr < end)
      *ptr++ = ch;
  }

  *ptr = '\0';

  if (ch != EOF || ptr > buf)
    return (buf);
  else
    return (NULL);
}


/*
 * 'file_localize()' - Localize a filename for the new working directory.
 */

const char *				/* O - New filename */
file_localize(const char *filename,	/* I - Filename */
              const char *newcwd)	/* I - New directory */
{
  const char	*newslash;		/* Directory separator */
  char		*slash;			/* Directory separator */
  char		cwd[1024];		/* Current directory */
  char		temp[1024];		/* Temporary pathname */
  static char	newfilename[1024];	/* New filename */


  if (filename[0] == '\0')
    return ("");

  getcwd(cwd, sizeof(cwd));
  if (newcwd == NULL)
    newcwd = cwd;

#if defined(WIN32) || defined(__EMX__)
  if (filename[0] != '/' &&
      filename[0] != '\\' &&
      !(isalpha(filename[0]) && filename[1] == ':'))
#else
  if (filename[0] != '/')
#endif /* WIN32 || __EMX__ */
  {
    for (newslash = filename; strncmp(newslash, "../", 3) == 0; newslash += 3)
#if defined(WIN32) || defined(__EMX__)
    {
      if ((slash = strrchr(cwd, '/')) == NULL)
        slash = strrchr(cwd, '\\');
      if (slash != NULL)
        *slash = '\0';
    }
#else
      if ((slash = strrchr(cwd, '/')) != NULL)
        *slash = '\0';
#endif /* WIN32 || __EMX__ */

    sprintf(temp, "%s/%s", cwd, newslash);
  }
  else
  {
    strncpy(temp, filename, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
  }

  for (slash = temp, newslash = newcwd;
       *slash != '\0' && *newslash != '\0';
       slash ++, newslash ++)
    if ((*slash == '/' || *slash == '\\') &&
        (*newslash == '/' || *newslash == '\\'))
      continue;
    else if (*slash != *newslash)
      break;

  while (*slash != '/' && *slash != '\\' && slash > temp)
    slash --;

  if (*slash == '/' || *slash == '\\')
    slash ++;

#if defined(WIN32) || defined(__EMX__)
  if (isalpha(slash[0]) && slash[1] == ':')
    return ((char *)filename); /* Different drive letter... */
#endif /* WIN32 || __EMX__ */

  if (*newslash != '\0')
    while (*newslash != '/' && *newslash != '\\' && newslash > newcwd)
      newslash --;

  newfilename[0] = '\0';

  while (*newslash != '\0')
  {
    if (*newslash == '/' || *newslash == '\\')
      strcat(newfilename, "../");
    newslash ++;
  }

  strcat(newfilename, slash);

  return (newfilename);
}


/*
 * 'file_method()' - Return the method for a filename or URL.
 *
 * Returns NULL if the URL is a local file.
 */

const char *			/* O - Method string ("http", "ftp", etc.) */
file_method(const char *s)	/* I - Filename or URL */
{
  if (strncmp(s, "http:", 5) == 0)
    return ("http");
  else if (strncmp(s, "https:", 6) == 0)
    return ("https");
  else if (strncmp(s, "ftp:", 4) == 0)
    return ("ftp");
  else if (strncmp(s, "mailto:", 7) == 0)
    return ("mailto");
  else
    return (NULL);
}


/*
 * 'file_nolocal()' - Disable access to local files.
 */

void
file_nolocal()
{
  no_local = 1;
}


/*
 * 'file_proxy()' - Set the proxy host for all HTTP requests.
 */

void
file_proxy(const char *url)	/* I - URL of proxy server */
{
   char	method[HTTP_MAX_URI],	/* Method name (must be HTTP) */
	username[HTTP_MAX_URI],	/* Username:password information */
	hostname[HTTP_MAX_URI],	/* Hostname */
	resource[HTTP_MAX_URI];	/* Resource name */
  int	port;			/* Port number */


  if (url == NULL || url[0] == '\0')
  {
    proxy_host[0] = '\0';
    proxy_port    = 0;
  }
  else
  {
    httpSeparate(url, method, username, hostname, &port, resource);

    if (strcmp(method, "http") == 0)
    {
      strncpy(proxy_host, hostname, sizeof(proxy_host) - 1);
      proxy_host[sizeof(proxy_host) - 1] = '\0';
      proxy_port = port;
    }
  }
}


/*
 * 'file_target()' - Return the target of a link.
 */

const char *			/* O - Target name */
file_target(const char *s)	/* I - Filename or URL */
{
  const char	*basename;	/* Pointer to directory separator */
  const char	*target;	/* Pointer to target */


  if (s == NULL)
    return (NULL);

  if ((basename = strrchr(s, '/')) != NULL)
    basename ++;
  else if ((basename = strrchr(s, '\\')) != NULL)
    basename ++;
#ifdef MAC
  else if ((basename = strrchr(s, ':')) != NULL)
    basename ++;
#endif /* MAC */
  else
    basename = s;

  if ((target = strchr(basename, '#')) != NULL)
    return (target + 1);
  else
    return (NULL);
}


/*
 * 'file_temp()' - Create and open a temporary file.
 */

FILE *					/* O - Temporary file */
file_temp(char *name,			/* O - Filename */
          int  len)			/* I - Length of filename buffer */
{
  cache_t	*temp;			/* Pointer to cache entry */
  FILE		*fp;			/* File pointer */
  int		fd;			/* File descriptor */
#ifdef WIN32
  char		tmpdir[1024];		/* Buffer for temp dir */
#else
  const char	*tmpdir;		/* Temporary directory */
#endif /* WIN32 */


 /*
  * Allocate memory for the file cache as needed...
  */

  if (web_files >= web_alloc)
  {
    web_alloc += ALLOC_FILES;
    if (web_files == 0)
      temp = (cache_t *)malloc(sizeof(cache_t) * web_alloc);
    else
      temp = (cache_t *)realloc(web_cache, sizeof(cache_t) * web_alloc);

    if (temp == NULL)
    {
      progress_error(HD_ERROR_OUT_OF_MEMORY,
                     "Unable to allocate memory for %d file entries - %s",
                     web_alloc, strerror(errno));
      web_alloc -= ALLOC_FILES;
      return (NULL);
    }

    web_cache = temp;
  }

 /*
  * Clear a new file cache entry...
  */

  temp = web_cache + web_files;

  temp->name = NULL;
  temp->url  = NULL;
  web_files ++;

#ifdef WIN32
  GetTempPath(sizeof(tmpdir), tmpdir);
#else
  if ((tmpdir = getenv("TMPDIR")) == NULL)
    tmpdir = "/var/tmp";
#endif /* WIN32 */

  snprintf(name, len, TEMPLATE, tmpdir, getpid(), web_files);

  if ((fd = open(name, OPENMODE, OPENPERM)) >= 0)
    fp = fdopen(fd, "w+b");
  else
    fp = NULL;

  if (!fp)
    web_files --;

  return (fp);
}


/*
 * End of "$Id: file.c,v 1.13.2.38 2002/08/14 19:44:58 mike Exp $".
 */

/*
 * "$Id: htmldoc.h,v 1.18.2.4 2001/02/28 01:46:18 mike Exp $"
 *
 *   Header file for HTMLDOC, a HTML document processing program.
 *
 *   Copyright 1997-2001 by Easy Software Products.
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
 */

/*
 * Include necessary headers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "html.h"
#include "image.h"
#include "debug.h"
#include "progress.h"

#ifdef HAVE_LIBFLTK
#  include "gui.h"
#endif /* HAVE_LIBFLTK */

#ifdef WIN32	    /* Include all 8 million Windows header files... */
#  include <windows.h>
#endif /* WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Macro to get rid of "unreferenced variable xyz" warnings...
 */

#define REF(x)		(void)x;


/*
 * PDF constants...
 */

enum	/* PDF page mode */
{
  PDF_DOCUMENT,
  PDF_OUTLINE,
  PDF_FULLSCREEN
};

enum	/* PDF page layout */
{
  PDF_SINGLE,
  PDF_ONE_COLUMN,
  PDF_TWO_COLUMN_LEFT,
  PDF_TWO_COLUMN_RIGHT
};

enum	/* PDF first page */
{
  PDF_PAGE_1,
  PDF_TOC,
  PDF_CHAPTER_1
};

enum	/* PDF transition effect */
{
  PDF_NONE,
  PDF_BOX_INWARD,
  PDF_BOX_OUTWARD,
  PDF_DISSOLVE,
  PDF_GLITTER_DOWN,
  PDF_GLITTER_DOWN_RIGHT,
  PDF_GLITTER_RIGHT,
  PDF_HORIZONTAL_BLINDS,
  PDF_HORIZONTAL_SWEEP_INWARD,
  PDF_HORIZONTAL_SWEEP_OUTWARD,
  PDF_VERTICAL_BLINDS,
  PDF_VERTICAL_SWEEP_INWARD,
  PDF_VERTICAL_SWEEP_OUTWARD,
  PDF_WIPE_DOWN,
  PDF_WIPE_LEFT,
  PDF_WIPE_RIGHT,
  PDF_WIPE_UP
};

enum	/* PDF document permissions */
{
  PDF_PERM_PRINT = 4,
  PDF_PERM_MODIFY = 8,
  PDF_PERM_COPY = 16,
  PDF_PERM_ANNOTATE = 32
};


/*
 * Globals...
 */

#ifdef _HTMLDOC_C_
#  define VAR
#  define VALUE(x)	=x
#else
#  define VAR		extern
#  define VALUE(x)
#endif /* _HTML_DOC_C */

VAR int		Verbosity	VALUE(0);	/* Verbosity */
VAR int		Errors		VALUE(0);	/* Number of errors */
VAR int		Compression	VALUE(1);	/* Non-zero means compress PDFs */
VAR int		TitlePage	VALUE(1),	/* Need a title page */
		TocLevels	VALUE(3),	/* Number of table-of-contents levels */
		TocLinks	VALUE(1),	/* Generate links */
		TocNumbers	VALUE(0),	/* Generate heading numbers */
		TocDocCount	VALUE(0);	/* Number of chapters */
VAR int		OutputBook	VALUE(1);	/* Output a "book" */
VAR char	OutputPath[255]	VALUE("");	/* Output directory/name */
VAR int		OutputFiles	VALUE(0),	/* Generate multiple files? */
		OutputColor	VALUE(1);	/* Output color images */
VAR int		OutputJPEG	VALUE(0);	/* JPEG compress images? */
VAR float	PDFVersion	VALUE(1.3);	/* Version of PDF to support */
VAR int		PDFPageMode	VALUE(PDF_OUTLINE),
						/* PageMode attribute */
		PDFPageLayout	VALUE(PDF_SINGLE),
						/* PageLayout attribute */
		PDFFirstPage	VALUE(PDF_CHAPTER_1),
						/* First page */
		PDFEffect	VALUE(PDF_NONE);/* Page transition effect */
VAR float	PDFEffectDuration VALUE(1.0),	/* Page effect duration */
		PDFPageDuration	VALUE(10.0);	/* Page duration */
VAR int		Encryption	VALUE(0),	/* Encrypt the PDF file? */
		Permissions	VALUE(-4);	/* File permissions? */
VAR char	OwnerPassword[33] VALUE(""),	/* Owner password */
		UserPassword[33] VALUE("");	/* User password */
VAR int		TrueType	VALUE(0);	/* Use TrueType fonts (PDF)? */
VAR int		PSLevel		VALUE(2),	/* Language level (0 for PDF) */
		PSCommands	VALUE(0);	/* Output PostScript commands? */
VAR int		PageWidth	VALUE(595),	/* Page width in points */
		PageLength	VALUE(792),	/* Page length in points */
		PageLeft	VALUE(72),	/* Left margin */
		PageRight	VALUE(36),	/* Right margin */
		PageTop		VALUE(36),	/* Top margin */
		PageBottom	VALUE(36),	/* Bottom margin */
		PagePrintWidth,			/* Printable width */
		PagePrintLength,		/* Printable length */
		PageDuplex	VALUE(0),	/* Adjust margins/pages for duplexing? */
		Landscape	VALUE(0);	/* Landscape orientation? */

VAR typeface_t	HeadFootType	VALUE(TYPE_HELVETICA);
						/* Typeface for header & footer */
VAR style_t	HeadFootStyle	VALUE(STYLE_NORMAL);
						/* Type style */
VAR float	HeadFootSize	VALUE(11.0f);	/* Size of header & footer */

VAR char	Header[4]	VALUE(".t."),	/* Header for regular pages */
		TocHeader[4]	VALUE(".t."),	/* Header for TOC pages */
		Footer[4] 	VALUE("h.1"),	/* Regular page footer */
		TocFooter[4]	VALUE("..i"),	/* Footer for TOC pages */
		TocTitle[1024]	VALUE("Table of Contents");
						/* TOC title string */

VAR char	TitleImage[255]	VALUE(""),	/* Title page image */
		LogoImage[255]	VALUE(""),	/* Logo image */
		BodyColor[255]	VALUE(""),	/* Body color */
		BodyImage[255]	VALUE(""),	/* Body image */
		LinkColor[255]	VALUE("");	/* Link color */
VAR int		LinkStyle	VALUE(1);	/* 1 = underline, 0 = plain */
VAR int		Links		VALUE(1);	/* 1 = generate links, 0 = no links */
VAR char	Path[2048]	VALUE(""),	/* Search path */
		Proxy[1024]	VALUE("");	/* Proxy URL */

VAR char	*PDFModes[3]			/* Mode strings */
#  ifdef _HTMLDOC_C_
= { "document", "outline", "fullscreen" }
#  endif /* _HTMLDOC_C_ */
;
VAR char	*PDFLayouts[4]			/* Layout strings */
#  ifdef _HTMLDOC_C_
= { "single", "one", "twoleft", "tworight" }
#  endif /* _HTMLDOC_C_ */
;
VAR char	*PDFPages[3]			/* First page strings */
#  ifdef _HTMLDOC_C_
= { "p1", "toc", "c1" }
#  endif /* _HTMLDOC_C_ */
;
VAR char	*PDFEffects[17]			/* Effect strings */
#  ifdef _HTMLDOC_C_
= { "none", "bi", "bo", "d", "gd", "gdr", "gr", "hb", "hsi", "hso",
    "vb", "vsi", "vso", "wd", "wl", "wr", "wu" }
#  endif /* _HTMLDOC_C_ */
;

#ifdef HAVE_LIBFLTK
VAR GUI		*BookGUI	VALUE(NULL);	/* GUI for book files */
#  ifdef WIN32					/* Editor for HTML files */
VAR char	HTMLEditor[1024] VALUE("notepad.exe %s");
#  else
VAR char	HTMLEditor[1024] VALUE("nedit %s");
#  endif /* WIN32 */
#endif /* HAVE_LIBFLTK */


/*
 * Prototypes...
 */

extern int	pspdf_export(tree_t *document, tree_t *toc);

extern int	html_export(tree_t *document, tree_t *toc);

extern tree_t	*toc_build(tree_t *tree);

extern void	get_color(const uchar *c, float *rgb, int defblack = 1);
extern int	get_measurement(const char *s);
extern void	set_page_size(const char *size);

extern void	prefs_load(void);
extern void	prefs_save(void);

extern char	*format_number(int n, char f);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*
 * End of "$Id: htmldoc.h,v 1.18.2.4 2001/02/28 01:46:18 mike Exp $".
 */

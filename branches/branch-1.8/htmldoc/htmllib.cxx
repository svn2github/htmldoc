/*
 * "$Id: htmllib.cxx,v 1.41.2.43 2001/12/13 19:04:09 mike Exp $"
 *
 *   HTML parsing routines for HTMLDOC, a HTML document processing program.
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
 *
 * Contents:
 *
 *   htmlReadFile()      - Read a file for HTML markup codes.
 *   write_file()        - Write a tree entry to a file...
 *   htmlWriteFile()     - Write an HTML markup tree to a file.
 *   htmlAddTree()       - Add a tree node to the parent.
 *   htmlDeleteTree()    - Free all memory associated with a tree...
 *   htmlInsertTree()    - Insert a tree node to the parent.
 *   htmlNewTree()       - Create a new tree node for the parent.
 *   get_text()          - Get all text from the given tree.
 *   htmlGetText()       - Get all text from the given tree.
 *   htmlGetMeta()       - Get document "meta" data...
 *   htmlGetStyle()      - Get a style value from a node's STYLE attribute.
 *   htmlGetVariable()   - Get a variable value from a markup entry.
 *   htmlSetVariable()   - Set a variable for a markup entry.
 *   htmlSetBaseSize()   - Set the font sizes and spacings...
 *   htmlSetCharSet()    - Set the character set for output.
 *   htmlSetTextColor()  - Set the default text color.
 *   compare_variables() - Compare two markup variables.
 *   compare_markups()   - Compare two markup strings...
 *   delete_node()       - Free all memory associated with a node...
 *   insert_space()      - Insert a whitespace character before the
 *                         specified node.
 *   parse_markup()      - Parse a markup string.
 *   parse_variable()    - Parse a markup variable string.
 *   compute_size()      - Compute the width and height of a tree entry.
 *   compute_color()     - Compute the red, green, blue color from the given
 *   get_alignment()     - Get horizontal & vertical alignment values.
 *   fix_filename()      - Fix a filename to be relative to the base directory.
 */

/*
 * Include necessary headers.
 */

#include "htmldoc.h"
#include <ctype.h>


/*
 * Markup strings...
 */

const char	*_htmlMarkups[] =
		{
		  "",		/* MARKUP_NONE */
		  "!--",	/* MARKUP_COMMENT */
		  "!DOCTYPE",
		  "A",
		  "ACRONYM",
		  "ADDRESS",
		  "APPLET",
		  "AREA",
		  "B",
		  "BASE",
		  "BASEFONT",
		  "BIG",
		  "BLINK",
		  "BLOCKQUOTE",
		  "BODY",
		  "BR",
		  "CAPTION",
		  "CENTER",
		  "CITE",
		  "CODE",
		  "COL",
		  "COLGROUP",
		  "DD",
		  "DEL",
		  "DFN",
		  "DIR",
		  "DIV",
		  "DL",
		  "DT",
		  "EM",
		  "EMBED",
		  "FONT",
		  "FORM",
		  "FRAME",
		  "FRAMESET",
		  "H1",
		  "H2",
		  "H3",
		  "H4",
		  "H5",
		  "H6",
		  "HEAD",
		  "HR",
		  "HTML",
		  "I",
		  "IMG",
		  "INPUT",
		  "INS",
		  "ISINDEX",
		  "KBD",
		  "LI",
		  "LINK",
		  "MAP",
		  "MENU",
		  "META",
		  "MULTICOL",
		  "NOBR",
		  "NOFRAMES",
		  "OL",
		  "OPTION",
		  "P",
		  "PRE",
		  "S",
		  "SAMP",
		  "SCRIPT",
		  "SELECT",
		  "SMALL",
		  "SPACER",
		  "STRIKE",
		  "STRONG",
		  "STYLE",
		  "SUB",
		  "SUP",
		  "TABLE",
		  "TBODY",
		  "TD",
		  "TEXTAREA",
		  "TFOOT",
		  "TH",
		  "THEAD",
		  "TITLE",
		  "TR",
		  "TT",
		  "U",
		  "UL",
		  "VAR",
		  "WBR"
		};

const char	*_htmlData = HTML_DATA;	/* Data directory */
float		_htmlPPI = 80.0f;	/* Image resolution */
int		_htmlGrayscale = 0;	/* Grayscale output? */
uchar		_htmlTextColor[255] =	/* Default text color */
		{ 0 };
float		_htmlBrowserWidth = 680.0f;
					/* Browser width for pixel scaling */
float		_htmlSizes[8] =		/* Point size for each HTML size */
		{ 6.0f, 8.0f, 9.0f, 11.0f, 14.0f, 17.0f, 20.0f, 24.0f };
float		_htmlSpacings[8] =	/* Line height for each HTML size */
		{ 7.2f, 9.6f, 10.8f, 13.2f, 16.8f, 20.4f, 24.0f, 28.8f };
typeface_t	_htmlBodyFont = TYPE_TIMES,
		_htmlHeadingFont = TYPE_HELVETICA;

int		_htmlInitialized = 0;	/* Initialized glyphs yet? */
char		_htmlCharSet[256] = "";	/* Character set name */
float		_htmlWidths[4][4][256];	/* Character widths of fonts */
const char	*_htmlGlyphsAll[65536];	/* Character glyphs for Unicode */
const char	*_htmlGlyphs[256];	/* Character glyphs for charset */
const char	*_htmlFonts[4][4] =
		{
		  {
		    "Courier",
		    "Courier-Bold",
		    "Courier-Oblique",
		    "Courier-BoldOblique"
		  },
		  {
		    "Times-Roman",
		    "Times-Bold",
		    "Times-Italic",
		    "Times-BoldItalic"
		  },
		  {
		    "Helvetica",
		    "Helvetica-Bold",
		    "Helvetica-Oblique",
		    "Helvetica-BoldOblique"
		  },
		  {
		    "Symbol",
		    "Symbol",
		    "Symbol",
		    "Symbol"
		  }
		};


/*
 * Local functions.
 */

extern "C" {
typedef int	(*compare_func_t)(const void *, const void *);
}

static int	write_file(tree_t *t, FILE *fp, int col);
static int	compare_variables(var_t *v0, var_t *v1);
static int	compare_markups(uchar **m0, uchar **m1);
static void	delete_node(tree_t *t);
static void	insert_space(tree_t *parent, tree_t *t);
static int	parse_markup(tree_t *t, FILE *fp);
static int	parse_variable(tree_t *t, FILE *fp);
static int	compute_size(tree_t *t);
static int	compute_color(tree_t *t, uchar *color);
static int	get_alignment(tree_t *t);
static const char *fix_filename(char *path, char *base);

#define issuper(x)	((x) == MARKUP_CENTER || (x) == MARKUP_DIV ||\
			 (x) == MARKUP_BLOCKQUOTE)
#define isblock(x)	((x) == MARKUP_ADDRESS || \
			 (x) == MARKUP_P || (x) == MARKUP_PRE ||\
			 ((x) >= MARKUP_H1 && (x) <= MARKUP_H6) ||\
			 (x) == MARKUP_HR || (x) == MARKUP_TABLE)
#define islist(x)	((x) == MARKUP_DL || (x) == MARKUP_OL ||\
			 (x) == MARKUP_UL || (x) == MARKUP_DIR ||\
			 (x) == MARKUP_MENU)
#define islentry(x)	((x) == MARKUP_LI || (x) == MARKUP_DD ||\
			 (x) == MARKUP_DT)
#define istable(x)	((x) == MARKUP_TBODY || (x) == MARKUP_THEAD ||\
			 (x) == MARKUP_TFOOT || (x) == MARKUP_TR)
#define istentry(x)	((x) == MARKUP_TD || (x) == MARKUP_TH)

#ifdef DEBUG
static uchar	indent[255] = "";
#endif /* DEBUG */


/*
 * 'htmlReadFile()' - Read a file for HTML markup codes.
 */

tree_t *			/* O - Pointer to top of file tree */
htmlReadFile(tree_t     *parent,/* I - Parent tree entry */
             FILE       *fp,	/* I - File pointer */
	     const char *base)	/* I - Base directory for file */
{
  int		ch;		/* Character from file */
  uchar		*ptr,		/* Pointer in string */
		glyph[16],	/* Glyph name (&#nnn; or &name;) */
		*glyphptr;	/* Pointer in glyph string */
  tree_t	*tree,		/* "top" of this tree */
		*t,		/* New tree entry */
		*prev,		/* Previous tree entry */
		*temp;		/* Temporary looping var */
  int		pos;		/* Current file position */
  FILE		*embed;		/* File pointer for EMBED */
  char		newbase[1024];	/* New base directory for EMBED */
  uchar		*filename,	/* Filename for EMBED tag */
		*face,		/* Typeface for FONT tag */
		*color,		/* Color for FONT tag */
		*size;		/* Size for FONT tag */
  int		sizeval;	/* Size value from FONT tag */
  static uchar	s[10240];	/* String from file */
  static int	have_whitespace = 0;
  				/* Non-zero if there was leading whitespace */


#ifdef DEBUG
  strcat((char *)indent, "    ");
#endif /* DEBUG */

 /*
  * Start off with no previous tree entry...
  */

  prev = NULL;
  tree = NULL;

 /*
  * Parse data until we hit end-of-file...
  */

  while ((ch = getc(fp)) != EOF)
  {
   /*
    * Ignore leading whitespace...
    */

    if (parent == NULL || !parent->preformatted)
    {
      while (isspace(ch))
      {
        have_whitespace = 1;
        ch              = getc(fp);
      }

      if (ch == EOF)
        break;
    }

   /*
    * Allocate a new tree entry - use calloc() to get zeroed data...
    */

    t = (tree_t *)calloc(sizeof(tree_t), 1);
    if (t == NULL)
    {
#ifndef DEBUG
      progress_error(HD_ERROR_OUT_OF_MEMORY,
                     "Unable to allocate memory for HTML tree node!");
#endif /* !DEBUG */
      break;
    }

   /*
    * Set/copy font characteristics...
    */

    if (parent == NULL)
    {
      t->halignment   = ALIGN_LEFT;
      t->valignment   = ALIGN_MIDDLE;
      t->typeface     = _htmlBodyFont;
      t->size         = SIZE_P;

      compute_color(t, _htmlTextColor);
    }
    else
    {
      t->link          = parent->link;
      t->halignment    = parent->halignment;
      t->valignment    = parent->valignment;
      t->typeface      = parent->typeface;
      t->size          = parent->size;
      t->style         = parent->style;
      t->superscript   = parent->superscript;
      t->subscript     = parent->subscript;
      t->preformatted  = parent->preformatted;
      t->indent        = parent->indent;
      t->red           = parent->red;
      t->green         = parent->green;
      t->blue          = parent->blue;
      t->underline     = parent->underline;
      t->strikethrough = parent->strikethrough;
    }

   /*
    * See what the character was...
    */

    if (ch == '<')
    {
     /*
      * Markup char; grab the next char to see if this is a /...
      */

      pos = ftell(fp) - 1;

      ch = getc(fp);
      if (isspace(ch) || ch == '=' || ch == '<')
      {
       /*
        * Sigh...  "<" followed by anything but an element name is
	* invalid HTML, but so many people have asked for this to
	* be supported that we have added this hack...
	*/

	ptr = s;
	if (have_whitespace)
	{
          *ptr++ = ' ';
	  have_whitespace = 0;
	}

        *ptr++ = '<';
	if (ch == '=')
	  *ptr++ = '=';
	else if (ch == '<')
	  ungetc(ch, fp);
	else
	  have_whitespace = 1;

	*ptr++ = '\0';

	t->markup = MARKUP_NONE;
	t->data   = (uchar *)strdup((char *)s);
      }
      else
      {
       /*
        * Start of a markup...
	*/

	if (ch != '/')
          ungetc(ch, fp);

	if (parse_markup(t, fp) == MARKUP_ERROR)
	{
  #ifndef DEBUG
          progress_error(HD_ERROR_READ_ERROR,
                         "Unable to parse HTML element at %d!", pos);
  #endif /* !DEBUG */

          delete_node(t);
          break;
	}

       /*
	* Eliminate extra whitespace...
	*/

	if (issuper(t->markup) || isblock(t->markup) ||
            islist(t->markup) || islentry(t->markup) ||
            istable(t->markup) || istentry(t->markup) ||
	    t->markup == MARKUP_TITLE)
          have_whitespace = 0;

       /*
	* If this is the matching close mark, or if we are starting the same
	* markup, or if we've completed a list, we're done!
	*/

	if (ch == '/')
	{
	 /*
          * Close markup; find matching markup...
          */

          for (temp = parent; temp != NULL; temp = temp->parent)
            if (temp->markup == t->markup)
              break;
	    else if (temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
              break;
	    }
	}
	else if (t->markup == MARKUP_EMBED)
	{
	 /*
          * Close any text blocks...
	  */

          for (temp = parent; temp != NULL; temp = temp->parent)
            if (isblock(temp->markup) || islentry(temp->markup))
              break;
	    else if (istentry(temp->markup) || islist(temp->markup) ||
	             issuper(temp->markup) || temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
	      break;
	    }
	}
	else if (issuper(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
	    if (istentry(temp->markup) || temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
              break;
	    }
	}
	else if (islist(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
            if (isblock(temp->markup))
	      break;
	    else if (islentry(temp->markup) || istentry(temp->markup) ||
	             issuper(temp->markup) || temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
              break;
	    }

	}
	else if (islentry(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
            if (islentry(temp->markup) || isblock(temp->markup))
              break;
	    else if (islist(temp->markup) || issuper(temp->markup) ||
	             istentry(temp->markup) || temp->markup == MARKUP_EMBED)
            {
	      temp = NULL;
	      break;
	    }
	}
	else if (isblock(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
            if (isblock(temp->markup) || islentry(temp->markup))
              break;
	    else if (istentry(temp->markup) || islist(temp->markup) ||
	             issuper(temp->markup) || temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
	      break;
	    }
	}
	else if (istable(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
            if (istable(temp->markup))
	      break;
	    else if (temp->markup == MARKUP_TABLE || temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
              break;
	    }
	}
	else if (istentry(t->markup))
	{
          for (temp = parent; temp != NULL; temp = temp->parent)
            if (istentry(temp->markup))
              break;
	    else if (temp->markup == MARKUP_TABLE || istable(temp->markup) ||
	             temp->markup == MARKUP_EMBED)
	    {
	      temp = NULL;
              break;
	    }
	}
	else
          temp = NULL;

	if (temp != NULL)
	{
          DEBUG_printf(("%s>>>> Auto-ascend <<<\n", indent));

          delete_node(t);

	 /*
          * If the markup doesn't start with a slash, or if it does but
          * doesn't match up with the parent (i.e. <UL><LI>...<LI>...</UL>)
          * then seek back so the parent entry gets a copy...
          */

          if (ch != '/' || temp != parent)
            fseek(fp, pos, SEEK_SET);	/* Make sure parent gets this markup... */
          break;
	}
	else if (ch == '/')
	{
          delete_node(t);
	  continue;
	}
      }
    }
    else if (t->preformatted)
    {
     /*
      * Read a pre-formatted string into the current tree entry...
      */

      ptr = s;
      while (ch != '<' && ch != EOF && ptr < (s + sizeof(s) - 1))
      {
        if (ch == '&')
        {
          for (glyphptr = glyph;
               (ch = getc(fp)) != EOF && (glyphptr - glyph) < 15;
               glyphptr ++)
            if (ch == ';' || isspace(ch))
              break;
            else
              *glyphptr = ch;

          *glyphptr = '\0';
          ch = iso8859(glyph);
        }

        if (ch != 0)
          *ptr++ = ch;

        if (ch == '\n')
          break;

        ch = getc(fp);
      }

      *ptr = '\0';

      if (ch == '<')
        ungetc(ch, fp);

      t->markup = MARKUP_NONE;
      t->data   = (uchar *)strdup((char *)s);

      DEBUG_printf(("%sfragment \"%s\"\n", indent, s));
    }
    else
    {
     /*
      * Read the next string fragment...
      */

      ptr = s;
      if (have_whitespace)
      {
        *ptr++ = ' ';
	have_whitespace = 0;
      }

      while (!isspace(ch) && ch != '<' && ch != EOF && ptr < (s + sizeof(s) - 1))
      {
        if (ch == '&')
        {
          for (glyphptr = glyph;
               (ch = getc(fp)) != EOF && (glyphptr - glyph) < 15;
               glyphptr ++)
            if (ch == ';' || isspace(ch))
              break;
            else
              *glyphptr = ch;

          *glyphptr = '\0';
          ch = iso8859(glyph);
        }

        if (ch != 0)
          *ptr++ = ch;

        ch = getc(fp);
      }

      if (isspace(ch))
        have_whitespace = 1;

      *ptr = '\0';

      if (ch == '<')
        ungetc(ch, fp);

      t->markup = MARKUP_NONE;
      t->data   = (uchar *)strdup((char *)s);

      DEBUG_printf(("%sfragment \"%s\"\n", indent, s));
    }

   /*
    * If the parent tree pointer is not null and this is the first
    * entry we've read, set the child pointer...
    */

    if (parent != NULL && prev == NULL)
      parent->child = t;

    if (parent != NULL)
      parent->last_child = t;

   /*
    * Do the prev/next links...
    */

    t->parent = parent;
    t->prev   = prev;
    if (prev != NULL)
      prev->next = t;
    else
      tree = t;

    prev = t;

   /*
    * Do markup-specific stuff...
    */

    switch (t->markup)
    {
      case MARKUP_BODY :
         /*
	  * Update the text color as necessary...
	  */

          if ((color = htmlGetVariable(t, (uchar *)"TEXT")) != NULL)
            compute_color(t, color);
	  else
            compute_color(t, _htmlTextColor);

          if ((color = htmlGetVariable(t, (uchar *)"BGCOLOR")) != NULL &&
	      !BodyColor[0])
	    strcpy(BodyColor, (char *)color);

          // Update the background image as necessary...
          if ((filename = htmlGetVariable(t, (uchar *)"BACKGROUND")) != NULL)
	    htmlSetVariable(t, (uchar *)"BACKGROUND",
	                    (uchar *)fix_filename((char *)filename,
			                          (char *)base));

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_IMG :
          if (have_whitespace)
	  {
	    // Insert a space before this image...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          // Get the image alignment...
          t->valignment = ALIGN_TOP;
          get_alignment(t);

          // Update the image source as necessary...
          if ((filename = htmlGetVariable(t, (uchar *)"SRC")) != NULL)
	    htmlSetVariable(t, (uchar *)"REALSRC",
	                    (uchar *)fix_filename((char *)filename,
			                          (char *)base));

      case MARKUP_BR :
      case MARKUP_NONE :
      case MARKUP_SPACER :
	 /*
	  * Figure out the width & height of this markup...
	  */

          compute_size(t);
	  break;

      case MARKUP_H1 :
      case MARKUP_H2 :
      case MARKUP_H3 :
      case MARKUP_H4 :
      case MARKUP_H5 :
      case MARKUP_H6 :
          get_alignment(t);

          t->typeface      = _htmlHeadingFont;
          t->size          = SIZE_H1 - t->markup + MARKUP_H1;
          t->subscript     = 0;
          t->superscript   = 0;
          t->strikethrough = 0;
          t->preformatted  = 0;
          t->style         = STYLE_BOLD;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_P :
          get_alignment(t);

          t->typeface      = _htmlBodyFont;
          t->size          = SIZE_P;
          t->style         = STYLE_NORMAL;
          t->subscript     = 0;
          t->superscript   = 0;
          t->strikethrough = 0;
          t->preformatted  = 0;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_PRE :
          t->typeface      = TYPE_COURIER;
          t->size          = SIZE_PRE;
          t->style         = STYLE_NORMAL;
          t->subscript     = 0;
          t->superscript   = 0;
          t->strikethrough = 0;
          t->preformatted  = 1;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_BLOCKQUOTE :
      case MARKUP_DIR :
      case MARKUP_MENU :
      case MARKUP_UL :
      case MARKUP_OL :
      case MARKUP_DL :
          t->indent ++;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_DIV :
          get_alignment(t);

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_HR :
          t->halignment = ALIGN_CENTER;
          get_alignment(t);
          break;

      case MARKUP_DOCTYPE :
      case MARKUP_AREA :
      case MARKUP_COMMENT :
      case MARKUP_INPUT :
      case MARKUP_ISINDEX :
      case MARKUP_LINK :
      case MARKUP_META :
      case MARKUP_WBR :
          break;

      case MARKUP_EMBED :
          if ((filename = htmlGetVariable(t, (uchar *)"SRC")) != NULL)
	  {
	    filename = (uchar *)fix_filename((char *)filename,
	                                     (char *)base);

            if ((embed = fopen((char *)filename, "r")) != NULL)
            {
	      strcpy(newbase, file_directory((char *)filename));

              htmlReadFile(t, embed, newbase);
              fclose(embed);
            }
#ifndef DEBUG
	    else
	      progress_error(HD_ERROR_FILE_NOT_FOUND,
                             "Unable to embed \"%s\" - %s", filename,
	                     strerror(errno));
#endif /* !DEBUG */
	  }
          break;

      case MARKUP_TH :
          if (htmlGetVariable(t->parent, (uchar *)"ALIGN") != NULL)
	    t->halignment = t->parent->halignment;
	  else
            t->halignment = ALIGN_CENTER;

          get_alignment(t);

          t->style = STYLE_BOLD;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_TD :
          if (htmlGetVariable(t->parent, (uchar *)"ALIGN") != NULL)
	    t->halignment = t->parent->halignment;
	  else
            t->halignment = ALIGN_LEFT;

	  get_alignment(t);

          t->style = STYLE_NORMAL;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_FONT :
          if ((face = htmlGetVariable(t, (uchar *)"FACE")) != NULL)
          {
            for (ptr = face; *ptr != '\0'; ptr ++)
              *ptr = tolower(*ptr);

            if (strstr((char *)face, "arial") != NULL ||
                strstr((char *)face, "helvetica") != NULL)
              t->typeface = TYPE_HELVETICA;
            else if (strstr((char *)face, "times") != NULL)
              t->typeface = TYPE_TIMES;
            else if (strstr((char *)face, "courier") != NULL)
	    {
              t->typeface = TYPE_COURIER;

              if (have_whitespace)
	      {
		// Insert a space before monospaced text...
		insert_space(parent, t);

		have_whitespace = 0;
	      }
            }
	    else if (strstr((char *)face, "symbol") != NULL)
              t->typeface = TYPE_SYMBOL;
          }

          if ((color = htmlGetVariable(t, (uchar *)"COLOR")) != NULL)
            compute_color(t, color);

          if ((size = htmlGetVariable(t, (uchar *)"SIZE")) != NULL)
          {
            if (have_whitespace)
	    {
	      // Insert a space before sized text...
	      insert_space(parent, t);

	      have_whitespace = 0;
	    }

	    if (isdigit(size[0]))
	      sizeval = atoi((char *)size);
	    else
              sizeval = t->size + atoi((char *)size);

            if (sizeval < 0)
              t->size = 0;
            else if (sizeval > 7)
              t->size = 7;
            else
              t->size = sizeval;
          }

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_BIG :
          if (have_whitespace)
	  {
	    // Insert a space before big text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          if (t->size < 6)
            t->size += 2;
          else
            t->size = 7;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_SMALL :
          if (have_whitespace)
	  {
	    // Insert a space before small text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          if (t->size > 2)
            t->size -= 2;
          else
            t->size = 0;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_SUP :
          if (have_whitespace)
	  {
	    // Insert a space before superscript text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          t->superscript = 1;

          if ((sizeval = t->size + SIZE_SUP) < 0)
	    t->size = 0;
	  else
	    t->size = sizeval;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_SUB :
          if (have_whitespace)
	  {
	    // Insert a space before subscript text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          t->subscript = 1;

          if ((sizeval = t->size + SIZE_SUB) < 0)
	    t->size = 0;
	  else
	    t->size = sizeval;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_KBD :
          t->style    = STYLE_BOLD;

      case MARKUP_TT :
      case MARKUP_CODE :
      case MARKUP_SAMP :
          if (isspace(ch = getc(fp)))
	    have_whitespace = 1;
	  else
	    ungetc(ch, fp);

          if (have_whitespace)
	  {
	    // Insert a space before monospaced text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          t->typeface = TYPE_COURIER;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_B :
          t->style = (style_t)(t->style | STYLE_BOLD);

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_DD :
          t->indent ++;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_VAR :
          t->style = (style_t)(t->style | STYLE_ITALIC);
      case MARKUP_DFN :
          t->typeface = TYPE_HELVETICA;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_STRONG :
          t->style = (style_t)(t->style | STYLE_BOLD);
      case MARKUP_CITE :
      case MARKUP_DT :
      case MARKUP_EM :
      case MARKUP_I :
          t->style = (style_t)(t->style | STYLE_ITALIC);

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_U :
      case MARKUP_INS :
          if (have_whitespace)
	  {
	    // Insert a space before underlined text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          t->underline = 1;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_STRIKE :
      case MARKUP_S :
      case MARKUP_DEL :
          if (have_whitespace)
	  {
	    // Insert a space before struck-through text...
	    insert_space(parent, t);

	    have_whitespace = 0;
	  }

          t->strikethrough = 1;

          htmlReadFile(t, fp, base);
          break;

      case MARKUP_CENTER :
          t->halignment = ALIGN_CENTER;

          htmlReadFile(t, fp, base);
          break;

      default :
         /*
          * All other markup types should be using <MARK>...</MARK>
          */

          get_alignment(t);

          htmlReadFile(t, fp, base);
          break;
    }
  }  

#ifdef DEBUG
  indent[strlen((char *)indent) - 4] = '\0';
#endif /* DEBUG */

  return (tree);
}


/*
 * 'write_file()' - Write a tree entry to a file...
 */

static int			/* I - New column */
write_file(tree_t *t,		/* I - Tree entry */
           FILE   *fp,		/* I - File to write to */
           int    col)		/* I - Current column */
{
  int	i;			/* Looping var */
  uchar	*ptr;			/* Character pointer */


  while (t != NULL)
  {
    if (t->markup == MARKUP_NONE)
    {
      if (t->preformatted)
      {
        for (ptr = t->data; *ptr != '\0'; ptr ++)
          fputs((char *)iso8859(*ptr), fp);

	if (t->data[strlen((char *)t->data) - 1] == '\n')
          col = 0;
	else
          col += strlen((char *)t->data);
      }
      else
      {
	if ((col + strlen((char *)t->data)) > 72 && col > 0)
	{
          putc('\n', fp);
          col = 0;
	}

        for (ptr = t->data; *ptr != '\0'; ptr ++)
          fputs((char *)iso8859(*ptr), fp);

	col += strlen((char *)t->data);

	if (col > 72)
	{
          putc('\n', fp);
          col = 0;
	}
      }
    }
    else if (t->markup == MARKUP_COMMENT)
      fprintf(fp, "\n<!--%s-->\n", t->data);
    else if (t->markup > 0)
    {
      switch (t->markup)
      {
        case MARKUP_AREA :
        case MARKUP_BR :
        case MARKUP_CENTER :
        case MARKUP_COMMENT :
        case MARKUP_DD :
        case MARKUP_DL :
        case MARKUP_DT :
        case MARKUP_H1 :
        case MARKUP_H2 :
        case MARKUP_H3 :
        case MARKUP_H4 :
        case MARKUP_H5 :
        case MARKUP_H6 :
        case MARKUP_HEAD :
        case MARKUP_HR :
        case MARKUP_LI :
        case MARKUP_MAP :
        case MARKUP_OL :
        case MARKUP_P :
        case MARKUP_PRE :
        case MARKUP_TABLE :
        case MARKUP_TITLE :
        case MARKUP_TR :
        case MARKUP_UL :
	case MARKUP_DIR :
	case MARKUP_MENU :
            if (col > 0)
            {
              putc('\n', fp);
              col = 0;
            }
        default :
            break;
      }

      col += fprintf(fp, "<%s", _htmlMarkups[t->markup]);
      for (i = 0; i < t->nvars; i ++)
      {
	if (col > 72 && !t->preformatted)
	{
          putc('\n', fp);
          col = 0;
	}

        if (col > 0)
        {
          putc(' ', fp);
          col ++;
        }

	if (t->vars[i].value == NULL)
          col += fprintf(fp, "%s", t->vars[i].name);
	else if (strchr((char *)t->vars[i].value, '\"') != NULL)
          col += fprintf(fp, "%s=\'%s\'", t->vars[i].name, t->vars[i].value);
	else
          col += fprintf(fp, "%s=\"%s\"", t->vars[i].name, t->vars[i].value);
      }

      putc('>', fp);
      col ++;

      if (col > 72 && !t->preformatted)
      {
	putc('\n', fp);
	col = 0;
      }

      if (t->child != NULL)
      {
	col = write_file(t->child, fp, col);

	if (col > 72 && !t->preformatted)
	{
	  putc('\n', fp);
	  col = 0;
	}
	
        col += fprintf(fp, "</%s>", _htmlMarkups[t->markup]);
        switch (t->markup)
        {
          case MARKUP_AREA :
          case MARKUP_BR :
          case MARKUP_CENTER :
          case MARKUP_COMMENT :
          case MARKUP_DD :
          case MARKUP_DL :
          case MARKUP_DT :
          case MARKUP_H1 :
          case MARKUP_H2 :
          case MARKUP_H3 :
          case MARKUP_H4 :
          case MARKUP_H5 :
          case MARKUP_H6 :
          case MARKUP_HEAD :
          case MARKUP_HR :
          case MARKUP_LI :
          case MARKUP_MAP :
          case MARKUP_OL :
          case MARKUP_P :
          case MARKUP_PRE :
          case MARKUP_TABLE :
          case MARKUP_TITLE :
          case MARKUP_TR :
          case MARKUP_UL :
          case MARKUP_DIR :
          case MARKUP_MENU :
              putc('\n', fp);
              col = 0;
          default :
	      break;
        }
      }
    }

    t = t->next;
  }

  return (col);
}
        
  
/*
 * 'htmlWriteFile()' - Write an HTML markup tree to a file.
 */

int				/* O - Write status: 0 = success, -1 = fail */
htmlWriteFile(tree_t *parent,	/* I - Parent tree entry */
              FILE   *fp)	/* I - File to write to */
{
  if (write_file(parent, fp, 0) < 0)
    return (-1);
  else
    return (0);
}


/*
 * 'htmlAddTree()' - Add a tree node to the parent.
 */

tree_t *			/* O - New entry */
htmlAddTree(tree_t   *parent,	/* I - Parent entry */
            markup_t markup,	/* I - Markup code */
            uchar    *data)	/* I - Data/text */
{
  tree_t	*t;		/* New tree entry */


  if ((t = htmlNewTree(parent, markup, data)) == NULL)
    return (NULL);

 /*
  * Add the tree entry to the end of the chain of children...
  */

  if (parent != NULL)
  {
    if (parent->last_child != NULL)
    {
      parent->last_child->next = t;
      t->prev                  = parent->last_child;
    }
    else
      parent->child = t;

    parent->last_child = t;
  }

  return (t);
}


/*
 * 'htmlDeleteTree()' - Free all memory associated with a tree...
 */

int				/* O - 0 for success, -1 for failure */
htmlDeleteTree(tree_t *parent)	/* I - Parent to delete */
{
  tree_t	*next;		/* Next tree entry */


  if (parent == NULL)
    return (-1);

  while (parent != NULL)
  {
    next = parent->next;

    if (parent->child != NULL)
      if (htmlDeleteTree(parent->child))
        return (-1);

    delete_node(parent);

    parent = next;
  }

  return (0);
}


/*
 * 'htmlInsertTree()' - Insert a tree node under the parent.
 */

tree_t *			/* O - New entry */
htmlInsertTree(tree_t   *parent,/* I - Parent entry */
               markup_t markup,	/* I - Markup code */
               uchar    *data)	/* I - Data/text */
{
  tree_t	*t;		/* New tree entry */


  if ((t = htmlNewTree(parent, markup, data)) == NULL)
    return (NULL);

 /*
  * Insert the tree entry at the beginning of the chain of children...
  */

  if (parent != NULL)
  {
    if (parent->child != NULL)
    {
      parent->child->prev = t;
      t->next             = parent->child;
    }
    else
      parent->last_child = t;

    parent->child = t;
  }

  return (t);
}


/*
 * 'htmlNewTree()' - Create a new tree node for the parent.
 */

tree_t *			/* O - New entry */
htmlNewTree(tree_t   *parent,	/* I - Parent entry */
            markup_t markup,	/* I - Markup code */
            uchar    *data)	/* I - Data/text */
{
  tree_t	*t;		/* New tree entry */


 /*
  * Allocate a new tree entry - use calloc() to get zeroed data...
  */

  t = (tree_t *)calloc(sizeof(tree_t), 1);
  if (t == NULL)
    return (NULL);

 /*
  * Set the markup code and copy the data if necessary...
  */

  t->markup = markup;
  if (data != NULL)
    t->data = (uchar *)strdup((char *)data);

 /*
  * Set/copy font characteristics...
  */

  if (parent == NULL)
  {
    t->halignment = ALIGN_LEFT;
    t->valignment = ALIGN_MIDDLE;
    t->typeface   = _htmlBodyFont;
    t->size       = SIZE_P;

    compute_color(t, _htmlTextColor);
  }
  else
  {
    t->link          = parent->link;
    t->halignment    = parent->halignment;
    t->valignment    = parent->valignment;
    t->typeface      = parent->typeface;
    t->size          = parent->size;
    t->style         = parent->style;
    t->preformatted  = parent->preformatted;
    t->indent        = parent->indent;
    t->red           = parent->red;
    t->green         = parent->green;
    t->blue          = parent->blue;
    t->underline     = parent->underline;
    t->strikethrough = parent->strikethrough;
  }

  switch (t->markup)
  {
    case MARKUP_NONE :
    case MARKUP_IMG :
       /*
	* Figure out the width & height of this fragment...
	*/

        compute_size(t);
	break;

    case MARKUP_H1 :
    case MARKUP_H2 :
    case MARKUP_H3 :
    case MARKUP_H4 :
    case MARKUP_H5 :
    case MARKUP_H6 :
        get_alignment(t);

        t->typeface      = _htmlHeadingFont;
        t->size          = SIZE_H1 - t->markup + MARKUP_H1;
        t->subscript     = 0;
        t->superscript   = 0;
        t->strikethrough = 0;
        t->preformatted  = 0;
        t->style         = STYLE_BOLD;
        break;

    case MARKUP_P :
        get_alignment(t);

        t->typeface      = _htmlBodyFont;
        t->size          = SIZE_P;
        t->style         = STYLE_NORMAL;
        t->subscript     = 0;
        t->superscript   = 0;
        t->strikethrough = 0;
        t->preformatted  = 0;
        break;

    case MARKUP_PRE :
        t->typeface      = TYPE_COURIER;
        t->size          = SIZE_PRE;
        t->style         = STYLE_NORMAL;
        t->subscript     = 0;
        t->superscript   = 0;
        t->strikethrough = 0;
        t->preformatted  = 1;
        break;

    case MARKUP_DIV :
        get_alignment(t);
        break;

    case MARKUP_BLOCKQUOTE :
        t->style = STYLE_ITALIC;

    case MARKUP_UL :
    case MARKUP_DIR :
    case MARKUP_MENU :
    case MARKUP_OL :
    case MARKUP_DL :
        t->indent ++;
        break;

    case MARKUP_AREA :
    case MARKUP_BR :
    case MARKUP_COMMENT :
    case MARKUP_HR :
    case MARKUP_INPUT :
    case MARKUP_ISINDEX :
    case MARKUP_META :
    case MARKUP_WBR :
        break;

    case MARKUP_TH :
        t->style = STYLE_BOLD;
    case MARKUP_TD :
        get_alignment(t);
        break;

    case MARKUP_SUP :
        t->superscript = 1;
        t->size        = SIZE_P + SIZE_SUP;
        break;

    case MARKUP_SUB :
        t->subscript = 1;
        t->size      = SIZE_P + SIZE_SUB;
        break;

    case MARKUP_B :
        t->style = (style_t)(t->style | STYLE_BOLD);
        break;

    case MARKUP_DD :
        t->indent ++;
        break;

    case MARKUP_DT :
    case MARKUP_I :
        t->style = (style_t)(t->style | STYLE_ITALIC);
        break;

    case MARKUP_U :
    case MARKUP_INS :
        t->underline = 1;
        break;

    case MARKUP_STRIKE :
    case MARKUP_DEL :
        t->strikethrough = 1;
        break;

    default :
        break;
  }

  t->parent = parent;

  return (t);
}


/*
 * 'get_text()' - Get all text from the given tree.
 */

static uchar *		/* O - Pointer to last char set */
get_text(tree_t *tree,	/* I - Tree to pick */
         uchar  *buf)	/* I - Buffer to store text in */
{
  while (tree != NULL)
  {
    if (tree->child != NULL)
      buf = get_text(tree->child, buf);
    else if (tree->markup == MARKUP_NONE && tree->data != NULL)
    {
      strcpy((char *)buf, (char *)tree->data);
      buf += strlen((char *)buf);
    }
    else if (tree->markup == MARKUP_BR)
    {
      strcat((char *)buf, " ");
      buf ++;
    }

    tree = tree->next;
  }

  return (buf);
}


/*
 * 'htmlGetText()' - Get all text from the given tree.
 */

uchar *				/* O - String containing text nodes */
htmlGetText(tree_t *tree)	/* I - Tree to pick */
{
  uchar	buf[10240];		/* String buffer */


  buf[0] = '\0';
  get_text(tree, buf);

  return ((uchar *)strdup((char *)buf));
}


/*
 * 'htmlGetMeta()' - Get document "meta" data...
 */

uchar *				/* O - Content string */
htmlGetMeta(tree_t *tree,	/* I - Document tree */
            uchar  *name)	/* I - Metadata name */
{
  uchar	*tname,			/* Name value from tree entry */
	*tcontent;		/* Content value from tree entry */


  while (tree != NULL)
  {
   /*
    * Check this tree entry...
    */

    if (tree->markup == MARKUP_META &&
        (tname = htmlGetVariable(tree, (uchar *)"NAME")) != NULL &&
        (tcontent = htmlGetVariable(tree, (uchar *)"CONTENT")) != NULL)
      if (strcasecmp((char *)name, (char *)tname) == 0)
        return (tcontent);

   /*
    * Check child entries...
    */

    if (tree->child != NULL)
      if ((tcontent = htmlGetMeta(tree->child, name)) != NULL)
        return (tcontent);

   /*
    * Next tree entry...
    */

    tree = tree->next;
  }

  return (NULL);
}


/*
 * 'htmlGetStyle()' - Get a style value from a node's STYLE attribute.
 */

uchar *				// O - Value or NULL
htmlGetStyle(tree_t *t,		// I - Node
             uchar  *name)	// I - Name (including ":")
{
  uchar		*ptr,		// Pointer in STYLE attribute
		*bufptr;	// Pointer in buffer
  int		ptrlen,		// Length of STYLE attribute
		namelen;	// Length of name
  static uchar	buffer[1024];	// Buffer for value


  // See if we have a STYLE attribute...
  if ((ptr = htmlGetVariable(t, (uchar *)"STYLE")) == NULL)
    return (NULL);

  // Loop through the STYLE attribute looking for the name...
  for (namelen = strlen((char *)name), ptrlen = strlen((char *)ptr);
       ptrlen > namelen;
       ptr ++, ptrlen --)
    if (strncasecmp((char *)name, (char *)ptr, namelen) == 0)
    {
      for (ptr += namelen; isspace(*ptr); ptr ++);

      for (bufptr = buffer;
           *ptr && *ptr != ';' && bufptr < (buffer + sizeof(buffer) - 1);
	   *bufptr++ = *ptr++);

      *bufptr = '\0';

      return (buffer);
    }

  return (NULL);
}


/*
 * 'htmlGetVariable()' - Get a variable value from a markup entry.
 */

uchar *				/* O - Value or NULL if variable does not exist */
htmlGetVariable(tree_t *t,	/* I - Tree entry */
                uchar  *name)	/* I - Variable name */
{
  var_t	*v,			/* Matching variable */
	key;			/* Search key */


  if (t == NULL || name == NULL || t->nvars == 0)
    return (NULL);

  key.name = name;

  v = (var_t *)bsearch(&key, t->vars, t->nvars, sizeof(var_t),
                       (compare_func_t)compare_variables);
  if (v == NULL)
    return (NULL);
  else if (v->value == NULL)
    return ((uchar *)"");
  else
    return (v->value);
}


/*
 * 'htmlSetVariable()' - Set a variable for a markup entry.
 */

int				/* O - Set status: 0 = success, -1 = fail */
htmlSetVariable(tree_t *t,	/* I - Tree entry */
                uchar  *name,	/* I - Variable name */
                uchar  *value)	/* I - Variable value */
{
  var_t	*v,			/* Matching variable */
	key;			/* Search key */


  DEBUG_printf(("%shtmlSetVariable(%08x, \"%s\", \"%s\")\n", indent, t, name,
                value ? (const char *)value : "(null)"));

  if (t->nvars == 0)
    v = NULL;
  else
  {
    key.name = name;

    v = (var_t *)bsearch(&key, t->vars, t->nvars, sizeof(var_t),
        	         (compare_func_t)compare_variables);
  }

  if (v == NULL)
  {
    if (t->nvars == 0)
      v = (var_t *)malloc(sizeof(var_t));
    else
      v = (var_t *)realloc(t->vars, sizeof(var_t) * (t->nvars + 1));

    if (v == NULL)
    {
      DEBUG_printf(("%s==== MALLOC/REALLOC FAILED! ====\n", indent));

      return (-1);
    }

    t->vars  = v;
    v        += t->nvars;
    t->nvars ++;
    v->name  = (uchar *)strdup((char *)name);
    if (value != NULL)
      v->value = (uchar *)strdup((char *)value);
    else
      v->value = NULL;

    if (strcasecmp((char *)name, "HREF") == 0)
    {
      DEBUG_printf(("%s---- Set link to %s ----\n", indent, value));
      t->link = t;
    }

    if (t->nvars > 1)
      qsort(t->vars, t->nvars, sizeof(var_t),
            (compare_func_t)compare_variables);
  }
  else if (v->value != value)
  {
    if (v->value != NULL)
      free(v->value);
    if (value != NULL)
      v->value = (uchar *)strdup((char *)value);
    else
      v->value = NULL;
  }

  return (0);
}


/*
 * 'htmlSetBaseSize()' - Set the font sizes and spacings...
 */

void
htmlSetBaseSize(float p,	/* I - Point size of paragraph font */
                float s)	/* I - Spacing */
{
  int	i;			/* Looping var */


  p /= 1.2 * 1.2 * 1.2;
  for (i = 0; i < 8; i ++, p *= 1.2)
  {
    _htmlSizes[i]    = p;
    _htmlSpacings[i] = p * s;
  }
}


/*
 * 'htmlSetCharSet()' - Set the character set for output.
 */

void
htmlSetCharSet(const char *cs)	/* I - Character set file to load */
{
  int		i, j;		/* Looping vars */
  char		filename[1024];	/* Filenames */
  FILE		*fp;		/* Files */
  int		ch, unicode;	/* Character values */
  float		width;		/* Width value */
  char		glyph[64];	/* Glyph name */
  char		line[1024];	/* Line from AFM file */
  int		chars[256];	/* Character encoding array */


  strcpy(_htmlCharSet, cs);

  if (!_htmlInitialized)
  {
   /*
    * Load the PostScript glyph names for all of Unicode...
    */

    memset(_htmlGlyphsAll, 0, sizeof(_htmlGlyphsAll));

    snprintf(line, sizeof(line), "%s/data/psglyphs", _htmlData);
    if ((fp = fopen(line, "r")) != NULL)
    {
      while (fscanf(fp, "%x%63s", &unicode, glyph) == 2)
        _htmlGlyphsAll[unicode] = strdup(glyph);

      fclose(fp);

      _htmlInitialized = 1;
    }
#ifndef DEBUG
    else
      progress_error(HD_ERROR_FILE_NOT_FOUND,
                     "Unable to open psglyphs data file!");
#endif /* !DEBUG */
  }

  memset(_htmlGlyphs, 0, sizeof(_htmlGlyphs));

  if (strncmp(cs, "8859-", 5) == 0)
    snprintf(filename, sizeof(filename), "%s/data/iso-%s", _htmlData, cs);
  else
    snprintf(filename, sizeof(filename), "%s/data/%s", _htmlData, cs);

  if ((fp = fopen(filename, "r")) == NULL)
  {
   /*
    * Can't open charset file; use ISO-8859-1...
    */

#ifndef DEBUG
    progress_error(HD_ERROR_FILE_NOT_FOUND,
                   "Unable to open character set file %s!", cs);
#endif /* !DEBUG */

    for (i = 0; i < 256; i ++)
      chars[i] = i;

   /*
    * Hardcode characters 128 to 159 for Microsoft's version of ISO-8859-1...
    */

    chars[0x80] = 0x20ac; /* Euro */
    chars[0x82] = 0x201a;
    chars[0x83] = 0x0192;
    chars[0x84] = 0x201e;
    chars[0x85] = 0x2026;
    chars[0x86] = 0x2020;
    chars[0x87] = 0x2021;
    chars[0x88] = 0x02c6;
    chars[0x89] = 0x2030;
    chars[0x8a] = 0x0160;
    chars[0x8b] = 0x2039;
    chars[0x8c] = 0x0152;
    chars[0x91] = 0x2018;
    chars[0x92] = 0x2019;
    chars[0x93] = 0x201c;
    chars[0x94] = 0x201d;
    chars[0x95] = 0x2022;
    chars[0x96] = 0x2013;
    chars[0x97] = 0x2014;
    chars[0x98] = 0x02dc;
    chars[0x99] = 0x2122;
    chars[0x9a] = 0x0161;
    chars[0x9b] = 0x203a;
    chars[0x9c] = 0x0153;
    chars[0x9f] = 0x0178;
  }
  else
  {
   /*
    * Read the <char> <unicode> lines from the file...
    */

    memset(chars, 0, sizeof(chars));

    while (fscanf(fp, "%x%x", &ch, &unicode) == 2)
      chars[ch] = unicode;

    fclose(fp);
  }

 /*
  * Build the glyph array...
  */

  for (i = 0; i < 256; i ++)
    if (chars[i] == 0)
      _htmlGlyphs[i] = NULL;
    else
      _htmlGlyphs[i] = _htmlGlyphsAll[chars[i]];

 /*
  * Now read all of the font widths...
  */

  for (i = 0; i < 4; i ++)
    for (j = 0; j < 4; j ++)
    {
      for (ch = 0; ch < 256; ch ++)
        _htmlWidths[i][j][ch] = 0.6f;

      snprintf(filename, sizeof(filename), "%s/afm/%s", _htmlData,
               _htmlFonts[i][j]);
      if ((fp = fopen(filename, "r")) == NULL)
      {
#ifndef DEBUG
        progress_error(HD_ERROR_FILE_NOT_FOUND,
                       "Unable to open font width file %s!", _htmlFonts[i][j]);
#endif /* !DEBUG */
        continue;
      }

      while (fgets(line, sizeof(line), fp) != NULL)
      {
        if (strncmp(line, "C ", 2) != 0)
	  continue;

        if (i < 3)
	{
	 /*
	  * Handle encoding of Courier, Times, and Helvetica using
	  * assigned charset...
	  */

          if (sscanf(line, "%*s%*s%*s%*s%f%*s%*s%s", &width, glyph) != 2)
	    continue;

          for (ch = 0; ch < 256; ch ++)
	    if (_htmlGlyphs[ch] && strcmp(_htmlGlyphs[ch], glyph) == 0)
	      break;

          if (ch < 256)
	    _htmlWidths[i][j][ch] = width * 0.001f;
	}
	else
	{
	 /*
	  * Symbol font uses its own encoding...
	  */

          if (sscanf(line, "%*s%d%*s%*s%f", &ch, &width) != 2)
	    continue;

          if (ch < 256)
	    _htmlWidths[i][j][ch] = width * 0.001f;
	}
      }

      fclose(fp);
    }
}


/*
 * 'htmlSetTextColor()' - Set the default text color.
 */

void
htmlSetTextColor(uchar *color)	/* I - Text color */
{
  strncpy((char *)_htmlTextColor, (char *)color, sizeof(_htmlTextColor));
  _htmlTextColor[sizeof(_htmlTextColor) - 1] = '\0';
}


/*
 * 'compare_variables()' - Compare two markup variables.
 */

static int			/* O - -1 if v0 < v1, 0 if v0 == v1, 1 if v0 > v1 */
compare_variables(var_t *v0,	/* I - First variable */
                  var_t *v1)	/* I - Second variable */
{
  return (strcasecmp((char *)v0->name, (char *)v1->name));
}


/*
 * 'compare_markups()' - Compare two markup strings...
 */

static int			/* O - -1 if m0 < m1, 0 if m0 == m1, 1 if m0 > m1 */
compare_markups(uchar **m0,	/* I - First markup */
                uchar **m1)	/* I - Second markup */
{
  return (strcasecmp((char *)*m0, (char *)*m1));
}


/*
 * 'delete_node()' - Free all memory associated with a node...
 */

static void
delete_node(tree_t *t)		/* I - Node to delete */
{
  int		i;		/* Looping var */
  var_t		*var;		/* Current variable */


  if (t == NULL)
    return;

  if (t->data != NULL)
    free(t->data);

  for (i = t->nvars, var = t->vars; i > 0; i --, var ++)
  {
    free(var->name);
    if (var->value != NULL)
      free(var->value);
  }

  if (t->vars != NULL)
    free(t->vars);

  free(t);
}


//
// 'insert_space()' - Insert a whitespace character before the
//                    specified node.
//

static void
insert_space(tree_t *parent,	// I - Parent node
             tree_t *t)		// I - Node to insert before
{
  tree_t	*space;		// Space node


  // Allocate memory for the whitespace...
  space = (tree_t *)calloc(sizeof(tree_t), 1);
  if (space == NULL)
  {
#ifndef DEBUG
    progress_error(HD_ERROR_OUT_OF_MEMORY,
                   "Unable to allocate memory for HTML tree node!");
#endif /* !DEBUG */
    return;
  }

  // Set/copy font characteristics...
  if (parent)
  {
    space->typeface = parent->typeface;
    space->size     = parent->size;
    space->style    = parent->style;
  }
  else
  {
    space->typeface = _htmlBodyFont;
    space->size     = SIZE_P;
  }

  // Initialize element data...
  space->markup = MARKUP_NONE;
  space->data   = (uchar *)strdup(" ");

  // Set tree pointers...
  space->parent = parent;
  space->prev   = t->prev;
  space->next   = t;

  if (space->prev)
    space->prev->next = space;
  else if (parent)
    parent->child = space;

  t->prev = space;

  compute_size(space);
}


/*
 * 'parse_markup()' - Parse a markup string.
 */

static int			/* O - -1 on error, MARKUP_nnnn otherwise */
parse_markup(tree_t *t,		/* I - Current tree entry */
             FILE   *fp)	/* I - Input file */
{
  int	ch, ch2;		/* Characters from file */
  uchar	markup[255],		/* Markup string... */
	*mptr,			/* Current character... */
	comment[10240],		/* Comment string */
	*cptr,			/* Current char... */
	**temp;			/* Markup variable entry */


  mptr = markup;

  while ((ch = getc(fp)) != EOF && mptr < (markup + sizeof(markup) - 1))
    if (ch == '>' || isspace(ch))
      break;
    else
    {
      *mptr++ = ch;

      // Handle comments without whitespace...
      if ((mptr - markup) == 3 && strncmp((const char *)markup, "!--", 3) == 0)
      {
        ch = getc(fp);
        break;
      }
    }

  *mptr = '\0';

  if (ch == EOF)
    return (MARKUP_ERROR);

  mptr = markup;
  temp = (uchar **)bsearch(&mptr, _htmlMarkups,
                           sizeof(_htmlMarkups) / sizeof(_htmlMarkups[0]),
                           sizeof(_htmlMarkups[0]),
                           (compare_func_t)compare_markups);

  if (temp == NULL)
  {
   /*
    * Unrecognized markup stuff...
    */

    t->markup = MARKUP_COMMENT;
    strcpy((char *)comment, (char *)markup);
    cptr = comment + strlen((char *)comment);

    DEBUG_printf(("%s%s (unrecognized!)\n", indent, markup));
  }
  else
  {
    t->markup = (markup_t)((const char **)temp - _htmlMarkups);
    cptr      = comment;

    DEBUG_printf(("%s%s\n", indent, markup));
  }

  if (t->markup == MARKUP_COMMENT)
  {
    while (ch != EOF && cptr < (comment + sizeof(comment) - 1))
    {
      if (ch == '>' && temp == NULL)
        break;

      *cptr++ = ch;

      if (ch == '-')
      {
        if ((ch2 = getc(fp)) == '>')
	{
	  // Erase trailing -->
	  cptr --;
	  if (*cptr == '-' && cptr > comment)
	    cptr --;

	  break;
        }
	else
	  ch = ch2;
      }
      else
        ch = getc(fp);
    }

    *cptr = '\0';
    t->data = (uchar *)strdup((char *)comment);
  }
  else
  {
    while (ch != EOF && ch != '>')
    {
      if (!isspace(ch))
      {
        ungetc(ch, fp);
        parse_variable(t, fp);
      }

      ch = getc(fp);
    }
  }

  return (t->markup);
}


/*
 * 'parse_variable()' - Parse a markup variable string.
 */

static int			/* O - -1 on error, 0 on success */
parse_variable(tree_t *t,	/* I - Current tree entry */
               FILE   *fp)	/* I - Input file */
{
  uchar	name[1024],		/* Name of variable */
	value[10240],		/* Value of variable */
	*ptr;			/* Temporary pointer */
  int	ch;			/* Character from file */


  ptr = name;
  while ((ch = getc(fp)) != EOF)
    if (isspace(ch) || ch == '=' || ch == '>' || ch == '\r')
      break;
    else if (ptr < (name + sizeof(name) - 1))
      *ptr++ = ch;

  *ptr = '\0';

  while (isspace(ch) || ch == '\r')
    ch = getc(fp);

  switch (ch)
  {
    default :
        ungetc(ch, fp);
        return (htmlSetVariable(t, name, NULL));
    case EOF :
        return (-1);
    case '=' :
        ptr = value;
        ch  = getc(fp);

        while (isspace(ch) || ch == '\r')
          ch = getc(fp);

        if (ch == EOF)
          return (-1);

        if (ch == '\'')
        {
          while ((ch = getc(fp)) != EOF)
            if (ch == '\'')
              break;
            else if (ptr < (value + sizeof(value) - 1) &&
	             ch != '\n' && ch != '\r')
              *ptr++ = ch;

          *ptr = '\0';
        }
        else if (ch == '\"')
        {
          while ((ch = getc(fp)) != EOF)
            if (ch == '\"')
              break;
            else if (ptr < (value + sizeof(value) - 1) &&
	             ch != '\n' && ch != '\r')
              *ptr++ = ch;

          *ptr = '\0';
        }
        else
        {
          *ptr++ = ch;
          while ((ch = getc(fp)) != EOF)
            if (isspace(ch) || ch == '>' || ch == '\r')
              break;
            else if (ptr < (value + sizeof(value) - 1))
              *ptr++ = ch;

          *ptr = '\0';
          if (ch == '>')
            ungetc(ch, fp);
        }

        return (htmlSetVariable(t, name, value));
  }
}


/*
 * 'compute_size()' - Compute the width and height of a tree entry.
 */

static int			/* O - 0 = success, -1 = failure */
compute_size(tree_t *t)		/* I - Tree entry */
{
  uchar		*ptr;		/* Current character */
  float		width,		/* Current width */
		max_width;	/* Maximum width */
  uchar		*width_ptr,	/* Pointer to width string */
		*height_ptr,	/* Pointer to height string */
		*size_ptr,	/* Pointer to size string */
		*type_ptr;	/* Pointer to spacer type string */
  image_t	*img;		/* Image */
  char		number[255];	/* Width or height value */


  if (!_htmlInitialized)
    htmlSetCharSet("8859-1");

  if (t->markup == MARKUP_IMG)
  {
    width_ptr  = htmlGetVariable(t, (uchar *)"WIDTH");
    height_ptr = htmlGetVariable(t, (uchar *)"HEIGHT");

    img = image_load((char *)htmlGetVariable(t, (uchar *)"REALSRC"),
                     _htmlGrayscale);

    if (width_ptr != NULL && height_ptr != NULL)
    {
      t->width  = atoi((char *)width_ptr) / _htmlPPI * 72.0f;
      t->height = atoi((char *)height_ptr) / _htmlPPI * 72.0f;

      return (0);
    }

    if (img == NULL)
      return (-1);

    if (width_ptr != NULL)
    {
      t->width  = atoi((char *)width_ptr) / _htmlPPI * 72.0f;
      t->height = t->width * img->height / img->width;

      sprintf(number, "%d",
              atoi((char *)width_ptr) * img->height / img->width);
      if (strchr((char *)width_ptr, '%') != NULL)
        strcat(number, "%");
      htmlSetVariable(t, (uchar *)"HEIGHT", (uchar *)number);
    }
    else if (height_ptr != NULL)
    {
      t->height = atoi((char *)height_ptr) / _htmlPPI * 72.0f;
      t->width  = t->height * img->width / img->height;

      sprintf(number, "%d",
              atoi((char *)height_ptr) * img->width / img->height);
      if (strchr((char *)height_ptr, '%') != NULL)
        strcat(number, "%");
      htmlSetVariable(t, (uchar *)"WIDTH", (uchar *)number);
    }
    else
    {
      t->width  = img->width / _htmlPPI * 72.0f;
      t->height = img->height / _htmlPPI * 72.0f;

      sprintf(number, "%d", img->width);
      htmlSetVariable(t, (uchar *)"WIDTH", (uchar *)number);

      sprintf(number, "%d", img->height);
      htmlSetVariable(t, (uchar *)"HEIGHT", (uchar *)number);
    }

    return (0);
  }
  else if (t->markup == MARKUP_SPACER)
  {
    width_ptr  = htmlGetVariable(t, (uchar *)"WIDTH");
    height_ptr = htmlGetVariable(t, (uchar *)"HEIGHT");
    size_ptr   = htmlGetVariable(t, (uchar *)"SIZE");
    type_ptr   = htmlGetVariable(t, (uchar *)"TYPE");

    if (width_ptr != NULL)
      t->width = atoi((char *)width_ptr) / _htmlPPI * 72.0f;
    else if (size_ptr != NULL)
      t->width = atoi((char *)size_ptr) / _htmlPPI * 72.0f;
    else
      t->width = 1.0f;

    if (height_ptr != NULL)
      t->height = atoi((char *)height_ptr) / _htmlPPI * 72.0f;
    else if (size_ptr != NULL)
      t->height = atoi((char *)size_ptr) / _htmlPPI * 72.0f;
    else
      t->height = 1.0f;

    if (type_ptr == NULL)
      return (0);

    if (strcasecmp((char *)type_ptr, "horizontal") == 0)
      t->height = 0.0;
    else if (strcasecmp((char *)type_ptr, "vertical") == 0)
      t->width = 0.0;

    return (0);
  }
  else if (t->markup == MARKUP_BR)
  {
    t->width  = 0.0;
    t->height = _htmlSizes[t->size];

    return (0);
  }
  else if (t->preformatted && t->data)
  {
    for (max_width = 0.0, width = 0.0, ptr = t->data; *ptr != '\0'; ptr ++)
      if (*ptr == '\n')
      {
        if (width > max_width)
          max_width = width;
      }
      else if (*ptr == '\t')
        width = (float)(((int)width + 7) & ~7);
      else
        width += _htmlWidths[t->typeface][t->style][*ptr];

   if (width < max_width)
     width = max_width;
  }
  else if (t->data)
    for (width = 0.0, ptr = t->data; *ptr != '\0'; ptr ++)
      width += _htmlWidths[t->typeface][t->style][*ptr];
  else
    width = 0.0f;

  t->width  = width * _htmlSizes[t->size];
  t->height = _htmlSizes[t->size];

  DEBUG_printf(("%swidth = %.1f, height = %.1f\n",
                indent, t->width, t->height));

  return (0);
}


/*
 * 'compute_color()' - Compute the red, green, blue color from the given
 *                     string.
 */

static int
compute_color(tree_t *t,	/* I - Tree entry */
              uchar  *color)	/* I - Color string */
{
  float	rgb[3];			/* RGB color */


  get_color(color, rgb);

  t->red   = (uchar)(rgb[0] * 255.0f + 0.5f);
  t->green = (uchar)(rgb[1] * 255.0f + 0.5f);
  t->blue  = (uchar)(rgb[2] * 255.0f + 0.5f);

  return (0);
}


/*
 * 'get_alignment()' - Get horizontal & vertical alignment values.
 */

static int			/* O - 0 for success, -1 for failure */
get_alignment(tree_t *t)	/* I - Tree entry */
{
  uchar	*align;			/* Alignment string */


  if ((align = htmlGetVariable(t, (uchar *)"ALIGN")) == NULL)
    align = htmlGetStyle(t, (uchar *)"text-align");

  if (align != NULL)
  {
    if (strcasecmp((char *)align, "left") == 0)
      t->halignment = ALIGN_LEFT;
    else if (strcasecmp((char *)align, "center") == 0)
      t->halignment = ALIGN_CENTER;
    else if (strcasecmp((char *)align, "right") == 0)
      t->halignment = ALIGN_RIGHT;
    else if (strcasecmp((char *)align, "justify") == 0)
      t->halignment = ALIGN_JUSTIFY;
    else if (strcasecmp((char *)align, "top") == 0)
      t->valignment = ALIGN_TOP;
    else if (strcasecmp((char *)align, "middle") == 0)
      t->valignment = ALIGN_MIDDLE;
    else if (strcasecmp((char *)align, "bottom") == 0)
      t->valignment = ALIGN_BOTTOM;
  }

  if ((align = htmlGetVariable(t, (uchar *)"VALIGN")) == NULL)
    align = htmlGetStyle(t, (uchar *)"vertical-align");

  if (align != NULL)
  {
    if (strcasecmp((char *)align, "top") == 0)
      t->valignment = ALIGN_TOP;
    else if (strcasecmp((char *)align, "middle") == 0)
      t->valignment = ALIGN_MIDDLE;
    else if (strcasecmp((char *)align, "center") == 0)
      t->valignment = ALIGN_MIDDLE;
    else if (strcasecmp((char *)align, "bottom") == 0)
      t->valignment = ALIGN_BOTTOM;
  }

  return (0);
}


/*
 * 'fix_filename()' - Fix a filename to be relative to the base directory.
 */

static const char *				/* O - Fixed filename */
fix_filename(char *filename,		/* I - Original filename */
             char *base)		/* I - Base directory */
{
  char		*slash;			/* Location of slash */
  static char	newfilename[1024];	/* New filename */


  if (filename == NULL)
    return (NULL);

  if (strcmp(base, ".") == 0 || strstr(filename, "//") != NULL)
    return (file_find(Path, filename));

#ifdef MAC
  //
  // Convert UNIX/DOS/WINDOWS slashes to colons for MacOS...
  //
  // Question: WHY doesn't the Mac standard C library do this for
  // you???
  //

  for (slash = strchr(filename, '/'); slash != NULL; slash = strchr(slash + 1, '/'))
    *slash = ':';

  for (slash = strchr(filename, '\\'); slash != NULL; slash = strchr(slash + 1, '\\'))
    *slash = ':';
#endif // MAC

  if (strncmp(filename, "./", 2) == 0 ||
      strncmp(filename, ".\\", 2) == 0)
    filename += 2;

  if (strncmp(base, "http://", 7) == 0)
  {
    strcpy(newfilename, base);
    base = newfilename + 7;

    if (filename[0] == '/')
    {
      if ((slash = strchr(base, '/')) != NULL)
        strcpy(slash, filename);
      else
        strcat(newfilename, filename);

      return (newfilename);
    }
    else if ((slash = strchr(base, '/')) == NULL)
      strcat(newfilename, "/");
  }
  else
  {
    if (filename[0] == '/' || filename[0] == '\\' || base == NULL ||
	base[0] == '\0' || (isalpha(filename[0]) && filename[1] == ':'))
      return (file_find(Path, filename)); /* No change needed for absolute path */

    strcpy(newfilename, base);
    base = newfilename;
  }

#ifdef MAC
  //
  // Convert UNIX/DOS/WINDOWS slashes to colons for MacOS...
  //
  // Question: WHY doesn't the Mac standard C library do this for
  // you???
  //

  for (slash = strchr(newfilename, '/'); slash != NULL; slash = strchr(slash + 1, '/'))
    *slash = ':';

  for (slash = strchr(newfilename, '\\'); slash != NULL; slash = strchr(slash + 1, '\\'))
    *slash = ':';
#endif // MAC

#if defined(WIN32) || defined(__EMX__)
  while (strncmp(filename, "../", 3) == 0 ||
         strncmp(filename, "..\\", 3) == 0)
#elif defined(MAC)
  while (strncmp(filename, "..:", 3) == 0)
#else
  while (strncmp(filename, "../", 3) == 0)
#endif // WIN32 || __EMX__
  {
    filename += 3;
#if defined(WIN32) || defined(__EMX__)
    if ((slash = strrchr(base, '/')) != NULL)
      *slash = '\0';
    else if ((slash = strrchr(base, '\\')) != NULL)
      *slash = '\0';
#elif defined(MAC)
    if ((slash = strrchr(base, ':')) != NULL)
      *slash = '\0';
#else
    if ((slash = strrchr(base, '/')) != NULL)
      *slash = '\0';
#endif // WIN32 || __EMX__
    else
    {
      filename -= 3;
      break;
    }
  }

#ifdef MAC
  strcat(newfilename, ":");
#else
  strcat(newfilename, "/");
#endif // MAC
  strcat(newfilename, filename);

  return (file_find(Path, newfilename));
}


/*
 * End of "$Id: htmllib.cxx,v 1.41.2.43 2001/12/13 19:04:09 mike Exp $".
 */

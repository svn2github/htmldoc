/*
 * "$Id: toc.cxx,v 1.19 2004/03/31 09:51:27 mike Exp $"
 *
 *   Table of contents generator for HTMLDOC, a HTML document processing
 *   program.
 *
 *   Copyright 1997-2004 by Easy Software Products.
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
 *       Hollywood, Maryland 20636-3142 USA
 *
 *       Voice: (301) 373-9600
 *       EMail: info@easysw.com
 *         WWW: http://www.easysw.com
 *
 * Contents:
 *
 *   toc_build()   - Build a table of contents of the given HTML tree.
 *   add_heading() - Add heading records to the given toc entry...
 *   parse_tree()  - Parse headings from the given tree...
 */

/*
 * Include necessary headers.
 */

#include "htmldoc.h"


/*
 * Local functions...
 */

static void	parse_tree(hdTree *t);


/*
 * Local globals...
 */

static int	heading_numbers[15];
static uchar	heading_types[15] =
		{
		  '1', '1', '1', '1', '1', '1', '1', '1',
		  '1', '1', '1', '1', '1', '1', '1'
		};
static int	last_level;
static hdTree	*heading_parents[15];


/*
 * 'toc_build()' - Build a table of contents of the given HTML tree.
 */

hdTree *			/* O - Table of contents tree */
toc_build(hdTree *tree)		/* I - Document tree */
{
  hdTree	*toc,		/* TOC tree pointer */
		*title,		/* Title entry */
		*link;		/* Link entry */


  TocDocCount        = 0;
  last_level         = 0;	/* Currently at the "top" level */
  heading_numbers[0] = 0;	/* Start at 1 (see below) */

  toc = htmlAddTree(NULL, HD_ELEMENT_BODY, NULL);

  title = htmlAddTree(toc, HD_ELEMENT_H1, NULL);
  htmlSetVariable(title, (uchar *)"ALIGN", (uchar *)"CENTER");
  link = htmlAddTree(title, HD_ELEMENT_A, NULL);
  htmlSetVariable(link, (uchar *)"NAME", (uchar *)"CONTENTS");
  htmlAddTree(link, HD_ELEMENT_NONE, (uchar *)TocTitle);

  heading_parents[0]  = toc;
  heading_parents[1]  = toc;
  heading_parents[2]  = toc;
  heading_parents[3]  = toc;
  heading_parents[4]  = toc;
  heading_parents[5]  = toc;
  heading_parents[6]  = toc;
  heading_parents[7]  = toc;
  heading_parents[8]  = toc;
  heading_parents[9]  = toc;
  heading_parents[10] = toc;
  heading_parents[11] = toc;
  heading_parents[12] = toc;
  heading_parents[13] = toc;
  heading_parents[14] = toc;

  parse_tree(tree);

  return (toc);
}


/*
 * 'add_heading()' - Add heading records to the given toc entry...
 */

void
add_heading(hdTree *toc,	/* I - Table of contents */
            hdTree *heading)	/* I - Heading entry */
{
  while (heading != NULL)
  {
    if (heading->child != NULL)
      add_heading(toc, heading->child);
    else if (heading->markup == HD_ELEMENT_NONE && heading->data != NULL)
      htmlAddTree(toc, HD_ELEMENT_NONE, heading->data);

    heading = heading->next;
  }
}


/*
 * 'parse_tree()' - Parse headings from the given tree...
 *
 * Note: We also add anchor points and numbers as necessary...
 */

static void			/* O - Tree of TOC entries */
parse_tree(hdTree *t)		/* I - Document tree */
{
  hdTree	*parent;	/* Parent of toc entry (DD or LI) */
  hdTree	*target,	/* Link target */
		*temp;		/* Looping var */
  uchar		heading[255],	/* Heading numbers */
		link[255],	/* Actual link */
		baselink[255],	/* Base link (numbered) */
		*existing;	/* Existing link string */
  int		i, level;	/* Header level */
  uchar		*var;		/* Starting value/type for this level */
  static const char *ones[10] =
		{
		  "",	"i",	"ii",	"iii",	"iv",
		  "v",	"vi",	"vii",	"viii",	"ix"
		},
		*tens[10] =
		{
		  "",	"x",	"xx",	"xxx",	"xl",
		  "l",	"lx",	"lxx",	"lxxx",	"xc"
		},
		*hundreds[10] =
		{
		  "",	"c",	"cc",	"ccc",	"cd",
		  "d",	"dc",	"dcc",	"dccc",	"cm"
		},
		*ONES[10] =
		{
		  "",	"I",	"II",	"III",	"IV",
		  "V",	"VI",	"VII",	"VIII",	"IX"
		},
		*TENS[10] =
		{
		  "",	"X",	"XX",	"XXX",	"XL",
		  "L",	"LX",	"LXX",	"LXXX",	"XC"
		},
		*HUNDREDS[10] =
		{
		  "",	"C",	"CC",	"CCC",	"CD",
		  "D",	"DC",	"DCC",	"DCCC",	"CM"
		};


  while (t != NULL)
  {
    switch (t->markup)
    {
      case HD_ELEMENT_H1 :
      case HD_ELEMENT_H2 :
      case HD_ELEMENT_H3 :
      case HD_ELEMENT_H4 :
      case HD_ELEMENT_H5 :
      case HD_ELEMENT_H6 :
      case HD_ELEMENT_H7 :
      case HD_ELEMENT_H8 :
      case HD_ELEMENT_H9 :
      case HD_ELEMENT_H10 :
      case HD_ELEMENT_H11 :
      case HD_ELEMENT_H12 :
      case HD_ELEMENT_H13 :
      case HD_ELEMENT_H14 :
      case HD_ELEMENT_H15 :
          level = t->markup - HD_ELEMENT_H1;

	  if ((level - last_level) > 1)
	  {
	   /*
	    * This step necessary to keep page numbers synced up...
	    */

	    level     = last_level + 1;
	    t->markup = (hdElement)(HD_ELEMENT_H1 + level);
	  }

          if ((var = htmlGetVariable(t, (uchar *)"VALUE")) != NULL)
            heading_numbers[level] = atoi((char *)var);
          else
            heading_numbers[level] ++;

          if (level == 0)
            TocDocCount ++;

          if ((var = htmlGetVariable(t, (uchar *)"TYPE")) != NULL)
            heading_types[level] = var[0];

          for (i = level + 1; i < 15; i ++)
            heading_numbers[i] = 0;

          heading[0]  = '\0';
	  baselink[0] = '\0';

          for (i = 0; i <= level; i ++)
          {
            if (i == 0)
              sprintf((char *)baselink + strlen((char *)baselink), "%d", TocDocCount);
            else
              sprintf((char *)baselink + strlen((char *)baselink), "%d", heading_numbers[i]);

            switch (heading_types[i])
            {
              case '1' :
                  sprintf((char *)heading + strlen((char *)heading), "%d", heading_numbers[i]);
                  break;
              case 'a' :
                  if (heading_numbers[i] > 26)
                    sprintf((char *)heading + strlen((char *)heading), "%c%c",
                            'a' + (heading_numbers[i] / 26) - 1,
                            'a' + (heading_numbers[i] % 26) - 1);
                  else
                    sprintf((char *)heading + strlen((char *)heading), "%c",
                            'a' + heading_numbers[i] - 1);
                  break;
              case 'A' :
                  if (heading_numbers[i] > 26)
                    sprintf((char *)heading + strlen((char *)heading), "%c%c",
                            'A' + (heading_numbers[i] / 26) - 1,
                            'A' + (heading_numbers[i] % 26) - 1);
                  else
                    sprintf((char *)heading + strlen((char *)heading), "%c",
                            'A' + heading_numbers[i] - 1);
                  break;
              case 'i' :
                  sprintf((char *)heading + strlen((char *)heading), "%s%s%s",
                          hundreds[heading_numbers[i] / 100],
                          tens[(heading_numbers[i] / 10) % 10],
                          ones[heading_numbers[i] % 10]);
                  break;
              case 'I' :
                  sprintf((char *)heading + strlen((char *)heading), "%s%s%s",
                          HUNDREDS[heading_numbers[i] / 100],
                          TENS[(heading_numbers[i] / 10) % 10],
                          ONES[heading_numbers[i] % 10]);
                  break;
            }

            if (i < level)
            {
              strcat((char *)heading, ".");
              strcat((char *)baselink, "_");
            }
          }

         /*
	  * See if we have an existing <A NAME=...> for this heading...
	  */

          existing = NULL;

          if (t->parent != NULL && t->parent->markup == HD_ELEMENT_A)
	    existing = htmlGetVariable(t->parent, (uchar *)"NAME");

	  if (existing == NULL &&
              t->child != NULL && t->child->markup == HD_ELEMENT_A)
	    existing = htmlGetVariable(t->child, (uchar *)"NAME");

          if (existing != NULL &&
	      strlen((char *)existing) >= 124)	/* Max size of link name */
	    existing = NULL;

          if (existing != NULL)
	    sprintf((char *)link, "#%s", existing);
	  else
	    sprintf((char *)link, "#%s", baselink);

         /*
	  * Number the headings as needed...
	  */

          if (TocNumbers)
	  {
            strcat((char *)heading, " ");

            htmlInsertTree(t, HD_ELEMENT_NONE, heading);
	  }

         /*
	  * Add the heading to the table of contents...
	  */

          if (level < TocLevels)
          {
            if (level > last_level)
              heading_parents[level] = htmlAddTree(heading_parents[level - 1],
                                                   HD_ELEMENT_UL, NULL);

            if (level == 0)
            {
              if (last_level == 0)
              {
                htmlAddTree(heading_parents[level], HD_ELEMENT_BR, NULL);
                htmlAddTree(heading_parents[level], HD_ELEMENT_BR, NULL);
              }

              parent = htmlAddTree(heading_parents[level], HD_ELEMENT_B, NULL);
            }
            else
              parent = htmlAddTree(heading_parents[level], HD_ELEMENT_LI, NULL);

            if ((var = htmlGetVariable(t, (uchar *)"_HD_OMIT_TOC")) != NULL)
	      htmlSetVariable(parent, (uchar *)"_HD_OMIT_TOC", var);

            if (TocLinks)
            {
             /*
              * Add a link for the toc...
              */

              parent = htmlAddTree(parent, HD_ELEMENT_A, NULL);
              htmlSetVariable(parent, (uchar *)"HREF", link);

             /*
              * Insert a NAME marker if needed and reparent all the
	      * heading children.
              */

              if (existing == NULL)
	      {
	       /*
	        * Add NAME to existing A element, if present.
		*/

                if (t->parent != NULL && t->parent->markup == HD_ELEMENT_A)
	          htmlSetVariable(t->parent, (uchar *)"NAME", baselink);
		else if (t->child != NULL && t->child->markup == HD_ELEMENT_A)
	          htmlSetVariable(t->child, (uchar *)"NAME", baselink);
		else
		{
        	  target = htmlNewTree(t, HD_ELEMENT_A, NULL);

        	  htmlSetVariable(target, (uchar *)"NAME", baselink);
        	  for (temp = t->child; temp != NULL; temp = temp->next)
                    temp->parent = target;

        	  target->child = t->child;
        	  t->child      = target;
	        }
	      }
            }

            add_heading(parent, t->child);
          }

          last_level = level;
          break;

      default :
          if (t->child != NULL)
            parse_tree(t->child);
          break;
    }

    t = t->next;
  }
}


/*
 * End of "$Id: toc.cxx,v 1.19 2004/03/31 09:51:27 mike Exp $".
 */

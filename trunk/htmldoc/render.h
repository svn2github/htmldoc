//
// "$Id: render.h,v 1.17 2002/09/25 01:16:38 mike Exp $"
//
//   Render class definitions for HTMLDOC.
//
//   Copyright 1997-2002 by Easy Software Products.
//
//   These coded instructions, statements, and computer programs are the
//   property of Easy Software Products and are protected by Federal
//   copyright law.  Distribution and use rights are outlined in the file
//   "COPYING.txt" which should have been included with this file.  If this
//   file is missing or damaged please contact Easy Software Products
//   at:
//
//       Attn: ESP Licensing Information
//       Easy Software Products
//       44141 Airport View Drive, Suite 204
//       Hollywood, Maryland 20636-3111 USA
//
//       Voice: (301) 373-9600
//       EMail: info@easysw.com
//         WWW: http://www.easysw.com
//

//* @package HTMLDOC
#ifndef _HTMLDOC_RENDER_H_
#  define _HTMLDOC_RENDER_H_

//
// Include necessary headers.
//

#  include "style.h"
#  include "image.h"
#  include "tree.h"


//
// Render node types...
//

enum hdRenderType
{
  HD_RENDERTYPE_TEXT = 1,
  HD_RENDERTYPE_IMAGE = 2,
  HD_RENDERTYPE_BOX = 4,
  HD_RENDERTYPE_BACKGROUND = 6,
  HD_RENDERTYPE_LINK = 8,
  HD_RENDERTYPE_FORM = 16
};


//
// Structures...
//

/**
 * The hdRenderText structure 
 */
struct hdRenderText
{
  //* Font for text
  hdStyleFont	*font;
  //* Size of text in points
  float		font_size;
  //* Inter-character spacing
  float		char_spacing;
  //* Color of text
  float		rgb[3];
  //* Did we allocate the string?
  int		alloc_string;
  //* String pointer
  char		*string;
};


/**
 * The hdRenderURL structure describes a single link on a page.
 */
struct hdRenderURL
{
  //* Did we allocate the URL?
  int		alloc_url;
  //* Link URL
  char		*url;
};


/**
 * The hdRenderNode structure describes rendering primitives used when
 * producing PostScript and PDF output.
 */
struct hdRenderNode
{
  //* Next rendering node
  hdRenderNode	*next;
  //* Type of node
  hdRenderType	type;
  //* X position in points
  float		x;
  //* Y position in points
  float		y;
  //* Width in points
  float		width;
  //* Height in points
  float		height;

  union
  {
    //* Text data
    hdRenderText text;

    //* Image pointer
    hdImage	*image;

    //* Box color
    float	box[3];

    //* Background data
    hdStyle	*background;

    //* Link data
    hdRenderURL	link;
  }	data;

 /**
  * The constructor creates a new render node with the specified data.
  *
  * @param t The type of render primitive, HD_RENDERTYPE_TEXT,
  * HD_RENDERTYPE_IMAGE, HD_RENDERTYPE_BOX, HD_RENDERTYPE_BACKGROUND,
  * HD_RENDERTYPE_LINK, or HD_RENDERTYPE_FORM.
  * @param xx The X position in points.
  * @param yy The Y position in points.
  * @param w The width in points.
  * @param h The height in points.
  * @param d The data associated with the node, if any.
  * @param alloc_d Whether the data was allocated (1) or not (0).
  */
  hdRenderNode(hdRenderType t, float xx, float yy, float w, float h,
               const void *d = (const void *)0, int alloc_d = 0);

 /**
  * The destructor frees all memory associated with the node, including
  * the data if it was allocated.
  */
  ~hdRenderNode();
};


/**
 * The hdRenderPage structure holds all of the rendering data for a
 * page.
 */
struct hdRenderPage
{
  //* First node on page
  hdRenderNode	*first;
  //* Last node on page
  hdRenderNode	*last;

  //* Bitwise OR of all nodes
  int		types;
  //* Width of page in points
  int		width;
  //* Length of page in points
  int		length;
  //* Left margin in points
  int		left;
  //* Right margin in points
  int		right;
  //* Top margin in points
  int		top;
  //* Bottom margin in points
  int		bottom;
  //* Duplex this page?
  int		duplex;
  //* Landscape orientation?
  int		landscape;
  //* Title tree
  char		*title;
  //* Chapter tree
  char		*chapter;
  //* Heading tree
  char		*heading;
  //* Headers
  char		*header[3];
  //* Footers
  char		*footer[3];
  //* Media color
  char		media_color[64];
  //* Media type
  char		media_type[64];
  //* Media position
  int		media_position;
  //* Page object
  int		page_object;
  //* Annotation object
  int		annot_object;
  //* Page number for TOC
  char		page_text[64];
  //* Background color
  float		background_color[3];
  //* Background image
  hdImage	*background_image;
  //* Background start position
  float		background_position[2];
  //* Background repeat mode
  hdBackgroundRepeat background_repeat;
};


/**
 * The hdRenderLink structure holds the positions and pages of
 * named links.
 */
struct hdRenderLink
{
  //* Page number
  short		page;
  //* Top position
  short		top;
  //* Reference name
  char		*name;
};


/**
 * The hdRenderChapter structure holds the start and end pages for
 * a chapter.
 */
struct hdRenderChapter
{
  //* First page in chapter
  int		first;
  //* Last page in chapter
  int		last;
};


/**
 * The hdRenderHeading structure holds the page and position of each
 * heading in a document.
 */

struct hdRenderHeading
{
  //* Page number
  int		page;
  //* Top position
  int		top;
  //* HTML tree node with heading
  hdTree	*node;
};


//
// Main rendering classes...
//

class hdRender
{
  public:

  hdStyleSheet	*css;			// Stylesheet data
  hdStyleMedia	media;			// Media attributes
  float		background_color[3];	// Current background color
  hdImage	*background_image;	// Current background image
  float		background_position[2];	// Current background start position
  hdBackgroundRepeat background_repeat;	// Current background repeat mode

  char		doc_date[64];		// Document creation date
  time_t	doc_time;		// Document creation time
  char		*doc_title;		// Document title

  int		title_page;		// Non-zero when processing title page
  int		current_chapter,	// Current chapter
		num_chapters,		// Number of chapters
		alloc_chapters;		// Allocated chapters
  hdRenderChapter *chapters;		// Chapters

  char		*current_heading;	// Current heading
  int		num_headings,		// Number of headings
		alloc_headings;		// Allocated headings
  hdRenderHeading *headings;		// Headings

  int		num_imgmaps,		// Number of image maps
		alloc_imgmaps;		// Allocated image maps
  hdTree	**imgmaps;		// Image maps

  int		num_links,		// Number of links
		alloc_links;		// Allocated links
  hdRenderLink	*links;			// Links

  int		num_pages,		// Number of pages
		alloc_pages;		// Allocated pages
  hdRenderPage	*pages;			// Pages

  hdStyleFont	*render_font;		// Current font
  float		render_size,		// Current font size
		render_rgb[3],		// Current drawing color
		render_x,		// Current X position
		render_y,		// Current Y position
		render_startx,		// Current text X margin
		render_spacing;		// Current text spacing

  hdRender(hdStyleSheet *s);
  virtual	~hdRender();

  virtual int	write_chapter(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords) = 0;
  virtual int	write_document(hdFile *out,
		               const char *author, const char *creator,
		               const char *copyright, const char *keywords) = 0;
  virtual int	write_page(hdFile *out, int page) = 0;
  virtual int	write_prolog(hdFile *out, int num_pages,
		             const char *author, const char *creator,
		             const char *copyright, const char *keywords) = 0;
  virtual int	write_trailer(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords) = 0;

  void		finish_document(const char *author, const char *creator,
		                const char *copyright, const char *keywords);

  void		parse_block(hdTree *t, hdMargin *m, float *x, float *y,
		            int *page);
  int		parse_comment(hdTree *t, hdMargin *m, float *x, float *y,
		              int *page);
  void		parse_contents(hdTree *t, hdMargin *m, int *page,
		               const char *label);
  void		parse_doc(hdTree *t, hdMargin *m, float *x, float *y,
		          int *page);
  void		parse_image(hdTree *t, hdMargin *m, float *x, float *y,
		            int *page);
  void		parse_index(hdTree *t, hdMargin *m, int *page,
		            const char *label);
  void		parse_line(hdTree *line, hdMargin *m, float *x, float *y,
		           int *page, int lastline);
  void		parse_list(hdTree *t, hdMargin *m,  float *x, float *y,
		           int *page);
  void		parse_table(hdTree *t, hdMargin *m, float *x, float *y,
		            int *page);

  void		prepare_page(int page);
  void		prepare_heading(int page, int print_page, char **format,
			        int y, char *page_text, int page_len);

  void		add_chapter();

  void		add_heading(hdTree *node, int page, int top);

  void		add_imgmap(hdTree *t);
  hdTree	*find_imgmap(const char *name);

  void		add_link(const char *name, int page, int top);
  hdRenderLink	*find_link(const char *name);
  static int	compare_links(hdRenderLink *n1, hdRenderLink *n2);

  void		check_pages(int page);

  hdRenderNode	*add_render(int page, hdRenderType type, float x, float y,
		            float width, float height, const void *data,
			    int alloc_data = 0, int insert = 0);

  void		get_color(const char *c, float *rgb, int defblack = 0);
  float		get_cell_size(hdTree *t, float left, float right,
		              float *minwidth, float *prefwidth,
			      float *minheight);
  float		get_table_size(hdTree *t, float left, float right,
		               float *minwidth, float *prefwidth,
			       float *minheight);
  float		get_width(char *s, int typeface, int style, int size);
  void		update_size(hdTree *t);
  hdFile	*open_file(void);
  void		set_color(hdFile *out, float *rgb);
  void		set_font(hdFile *out, int typeface, int style, float size);
  void		set_pos(hdFile *out, float x, float y);
  void		write_prolog(hdFile *out, int pages, char *author,
		             char *creator, char *copyright, char *keywords);
};


class hdPSRender : public hdRender
{
  public:

  hdPSRender();
  virtual	~hdPSRender();

  virtual int	write_chapter(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords);
  virtual int	write_document(hdFile *out,
		               const char *author, const char *creator,
		               const char *copyright, const char *keywords);
  virtual int	write_page(hdFile *out, int page);
  virtual int	write_prolog(hdFile *out, int num_pages,
		             const char *author, const char *creator,
		             const char *copyright, const char *keywords);
  virtual int	write_trailer(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords);
};


class hdPDFRender : public hdRender
{
  public:

  char		stdout_filename[256];
  int		num_objects,
		alloc_objects,
		*objects,
		root_object,
		info_object,
		outline_object,
		pages_object,
		names_object,
		encrypt_object,
		font_objects[16];

  char		encrypt_key[16];
  int		encrypt_len;
  hdByte	file_id[16];

  hdPDFRender();
  virtual	~hdPDFRender();

  virtual int	write_chapter(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords);
  virtual int	write_document(hdFile *out,
		               const char *author, const char *creator,
		               const char *copyright, const char *keywords);
  virtual int	write_page(hdFile *out, int page);
  virtual int	write_prolog(hdFile *out, int num_pages,
		             const char *author, const char *creator,
		             const char *copyright, const char *keywords);
  virtual int	write_trailer(hdFile *out,
		              const char *author, const char *creator,
		              const char *copyright, const char *keywords);

  void	pspdf_prepare_page(int page);
  void	pspdf_prepare_heading(int page, int print_page, char **format,
		                      int y, char *page_text, int page_len);
  void	ps_write_document(char *author, char *creator,
		                  char *copyright, char *keywords);
  void	ps_write_page(hdFile *out, int page);
  void	ps_write_background(hdFile *out);
  void	pdf_write_document(char *author, char *creator,
		                   char *copyright, char *keywords,
				   hdTree *toc);
  void	pdf_write_page(hdFile *out, int page);
  void	pdf_write_resources(hdFile *out, int page);
  void	pdf_write_contents(hdFile *out, hdTree *toc, int parent,
		                   int prev, int next, int *heading);
  void	pdf_write_links(hdFile *out);
  void	pdf_write_names(hdFile *out);
  int	pdf_count_headings(hdTree *toc);

  int	pdf_start_object(hdFile *out, int array = 0);
  void	pdf_start_stream(hdFile *out);
  void	pdf_end_object(hdFile *out);

  void	encrypt_init(void);
  void	flate_open_stream(hdFile *out);
  void	flate_close_stream(hdFile *out);
  void	flate_puts(const char *s, hdFile *out);
  void	flate_printf(hdFile *out, const char *format, ...);
  void	flate_write(hdFile *out, char *inbuf, int length, int flush=0);	

  void	parse_contents(hdTree *t, float left, float width, float bottom,
		       float length, float *y, int *page, int *heading,
		       hdTree *chap);
  void	parse_doc(hdTree *t, float *left, float *right, float *bottom,
		  float *top, float *x, float *y, int *page,
		  hdTree *cpara, int *needspace);
  void	parse_heading(hdTree *t, float left, float width, float bottom,
		      float length, float *x, float *y, int *page,
		      int needspace);
  void	parse_paragraph(hdTree *t, float left, float width, float bottom,
		        float length, float *x, float *y, int *page,
			int needspace);
  void	parse_pre(hdTree *t, float left, float width, float bottom,
		  float length, float *x, float *y, int *page,
		  int needspace);
  void	parse_table(hdTree *t, float left, float width, float bottom,
		    float length, float *x, float *y, int *page,
		    int needspace);
  void	parse_list(hdTree *t, float *left, float *width, float *bottom,
		   float *length, float *x, float *y, int *page,
		   int needspace);
  void	init_list(hdTree *t);
  void	parse_comment(hdTree *t, float *left, float *width, float *bottom,
		              float *length, float *x, float *y, int *page,
			      hdTree *para, int needspace);

  hdTree	*real_prev(hdTree *t);
  hdTree	*real_next(hdTree *t);

  void	check_pages(int page);

  void	add_link(char *name, int page, int top);
  hdRenderLink	*find_link(char *name);
  int	compare_links(hdRenderLink *n1, hdRenderLink *n2);

  void	find_background(hdTree *t);
  void	write_background(int page, hdFile *out);

  hdRenderNode	*new_render(int page, int type, float x, float y,
		            float width, float height, void *data,
			    int insert = 0);
  void	copy_tree(hdTree *parent, hdTree *t);
  float	get_cell_size(hdTree *t, float left, float right,
		              float *minwidth, float *prefwidth,
			      float *minheight);
  float	get_table_size(hdTree *t, float left, float right,
		               float *minwidth, float *prefwidth,
			       float *minheight);
  hdTree	*flatten_tree(hdTree *t);
  float	get_width(char *s, int typeface, int style, int size);
  void	update_image_size(hdTree *t);
  char	*get_title(hdTree *doc);
  hdFile	*open_file(void);
  void	set_color(hdFile *out, float *rgb);
  void	set_font(hdFile *out, int typeface, int style, float size);
  void	set_pos(hdFile *out, float x, float y);
  void	write_prolog(hdFile *out, int pages, char *author,
		             char *creator, char *copyright, char *keywords);
  void	ps_hex(hdFile *out, char *data, int length);
  void	ps_ascii85(hdFile *out, char *data, int length);
  void	jpg_init(j_compress_ptr cinfo);
  boolean	jpg_empty(j_compress_ptr cinfo);
  void	jpg_term(j_compress_ptr cinfo);
  void	jpg_setup(hdFile *out, hdImage *img, j_compress_ptr cinfo);
  int	compare_rgb(unsigned *rgb1, unsigned *rgb2);
  void	write_image(hdFile *out, hdRenderNode *r, int write_obj = 0);
  void	write_imagemask(hdFile *out, hdRenderNode *r);
  void	write_string(hdFile *out, char *s, int compress);
  void	write_text(hdFile *out, hdRenderNode *r);
  void	write_trailer(hdFile *out, int pages);
  int	write_truetype(hdFile *out, hdStyleFont *f);
};


#endif // !_HTMLDOC_RENDER_H_

//
// End of "$Id: render.h,v 1.17 2002/09/25 01:16:38 mike Exp $".
//

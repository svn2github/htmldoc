/*
 * "$Id: gui.h,v 1.14.2.4 2001/02/23 13:47:14 mike Exp $"
 *
 *   GUI definitions for HTMLDOC, an HTML document processing program.
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

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Window.H>

#include <gui/CheckButton.h>
#include <gui/FileChooser.h>
#include <gui/HelpDialog.h>
#include <gui/Progress.h>


/*
 * Class definition for HTMLDOC dialog...
 */

class GUI
{
  private:

  Fl_Window	*window;
  Fl_Group	*controls;
  Fl_Tabs	*tabs;

  Fl_Group	*inputTab;
  CheckButton	*typeBook,
		*typeWebPage;
  FileBrowser	*inputFiles;
  Fl_Button	*addFile,
		*addURL,
		*editFile,
		*deleteFile,
		*moveUpFile,
		*moveDownFile;
  Fl_Input	*logoImage;
  Fl_Button	*logoBrowse;
  Fl_Input	*titleImage;
  Fl_Button	*titleBrowse;

  Fl_Group	*outputTab;
  CheckButton	*outputFile,
		*outputDirectory;
  Fl_Input	*outputPath;
  Fl_Button	*outputBrowse;
  CheckButton	*typeHTML,
		*typePS,
		*typePDF;
  CheckButton	*grayscale,
		*titlePage,
		*jpegCompress;
  Fl_Group	*compGroup;
  Fl_Slider	*compression;
  Fl_Group	*jpegGroup;
  Fl_Value_Slider *jpegQuality;

  Fl_Group	*pageTab;
  Fl_Input	*pageSize;
  Fl_Menu_Button *pageSizeMenu;
  CheckButton	*pageDuplex,
		*landscape;
  Fl_Input	*pageTop,
		*pageLeft,
		*pageRight,
		*pageBottom;
  Fl_Choice	*pageHeaderLeft,
      		*pageHeaderCenter,
      		*pageHeaderRight,
      		*pageFooterLeft,
      		*pageFooterCenter,
      		*pageFooterRight;

  Fl_Group	*tocTab;
  Fl_Choice	*tocLevels;
  CheckButton	*numberedToc;
  Fl_Group	*tocHeader;
  Fl_Choice	*tocHeaderLeft,
      		*tocHeaderCenter,
      		*tocHeaderRight;
  Fl_Group	*tocFooter;
  Fl_Choice	*tocFooterLeft,
      		*tocFooterCenter,
      		*tocFooterRight;
  Fl_Input	*tocTitle;

  Fl_Group	*colorsTab;
  Fl_Input	*bodyColor;
  Fl_Button	*bodyLookup;
  Fl_Input	*bodyImage;
  Fl_Button	*bodyBrowse;
  Fl_Input	*textColor;
  Fl_Button	*textLookup;
  Fl_Input	*linkColor;
  Fl_Button	*linkLookup;
  Fl_Choice	*linkStyle;

  Fl_Group	*fontsTab;
  Fl_Choice	*headingFont,
		*bodyFont,
		*headFootFont;
  Fl_Counter	*fontBaseSize,
		*fontSpacing,
		*headFootSize;
  Fl_Choice	*charset;

  Fl_Group	*psTab;
  Fl_Group	*psLevel;
  Fl_Button	*ps1,
		*ps2,
		*ps3;
  CheckButton	*psCommands;

  Fl_Group	*pdfTab;
  Fl_Group	*pdfVersion;
  Fl_Button	*pdf11,
		*pdf12,
		*pdf13;
  Fl_Choice	*pageMode,
		*pageLayout,
		*firstPage,
		*pageEffect;
  Fl_Value_Slider *pageDuration,
		*effectDuration;
  CheckButton	*links;
  CheckButton	*truetype;

  Fl_Group	*securityTab;
  Fl_Group	*encryption;
  CheckButton	*encryptionYes,
		*encryptionNo;
  Fl_Group	*permissions;
  CheckButton	*permPrint,
		*permModify,
		*permCopy,
		*permAnnotate;
  Fl_Secret_Input *ownerPassword,
		*userPassword;

  Fl_Group	*optionsTab;
  Fl_Input	*htmlEditor;
  Fl_Button	*htmlBrowse;
  Fl_Value_Slider *browserWidth;
  Fl_Input	*path;
  Fl_Input	*proxy;
  Fl_Button	*saveOptions;

  Fl_Button	*bookSave,
		*bookSaveAs,
		*bookGenerate;

  Progress	*progressBar;

  FileChooser	*fc;
  FileIcon	*icon;
  HelpDialog	*help;

  char		book_filename[1024];
  int		book_changed;

  void		title(const char *filename = NULL, int changed = 0);

  static void	changeCB(Fl_Widget *w, GUI *gui);

  static void	docTypeCB(Fl_Widget *w, GUI *gui);
  static void	inputFilesCB(Fl_Widget *w, GUI *gui);
  static void	addFileCB(Fl_Widget *w, GUI *gui);
  static void	addURLCB(Fl_Widget *w, GUI *gui);
  static void	editFilesCB(Fl_Widget *w, GUI *gui);
  static void	deleteFilesCB(Fl_Widget *w, GUI *gui);
  static void	moveUpFilesCB(Fl_Widget *w, GUI *gui);
  static void	moveDownFilesCB(Fl_Widget *w, GUI *gui);
  static void	logoImageCB(Fl_Widget *w, GUI *gui);
  static void	titleImageCB(Fl_Widget *w, GUI *gui);

  static void	outputTypeCB(Fl_Widget *w, GUI *gui);
  static void	outputPathCB(Fl_Widget *w, GUI *gui);
  static void	outputFormatCB(Fl_Widget *w, GUI *gui);
  static void	jpegCB(Fl_Widget *w, GUI *gui);

  static void	sizeCB(Fl_Widget *w, GUI *gui);

  static void	tocCB(Fl_Widget *w, GUI *gui);

  static void	bodyColorCB(Fl_Widget *w, GUI *gui);
  static void	bodyImageCB(Fl_Widget *w, GUI *gui);
  static void	textColorCB(Fl_Widget *w, GUI *gui);
  static void	linkColorCB(Fl_Widget *w, GUI *gui);

  static void	psCB(Fl_Widget *w, GUI *gui);

  static void	pdfCB(Fl_Widget *w, GUI *gui);
  static void	effectCB(Fl_Widget *w, GUI *gui);

  static void	encryptionCB(Fl_Widget *w, GUI *gui);

  static void	htmlEditorCB(Fl_Widget *w, GUI *gui);
  static void	saveOptionsCB(Fl_Widget *w, GUI *gui);

  static void	helpCB(Fl_Widget *w, GUI *gui);
  static void	newBookCB(Fl_Widget *w, GUI *gui);
  static void	openBookCB(Fl_Widget *w, GUI *gui);
  static void	saveBookCB(Fl_Widget *w, GUI *gui);
  static void	saveAsBookCB(Fl_Widget *w, GUI *gui);
  static void	generateBookCB(Fl_Widget *w, GUI *gui);
  static void	closeBookCB(Fl_Widget *w, GUI *gui);

  public:

  static const char	*help_dir;

  GUI(const char *filename = NULL);
  ~GUI(void);

  int	checkSave();
  void	hide() { window->hide(); help->hide(); fc->hide(); };
  int	loadBook(const char *bookfile);
  int	newBook();
  void	parseOptions(const char *line);
  void	progress(int percent, char *text = NULL);
  int	saveBook(const char *bookfile);
  void	show();
  int	visible() { return (window->visible()); }
};


/*
 * End of "$Id: gui.h,v 1.14.2.4 2001/02/23 13:47:14 mike Exp $".
 */

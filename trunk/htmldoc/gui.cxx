//
// "$Id: gui.cxx,v 1.5 1999/11/09 22:16:41 mike Exp $"
//
//   GUI routines for HTMLDOC, an HTML document processing program.
//
//   Copyright 1997-1999 by Easy Software Products.
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
// Contents:
//
//   GUI()             - Build the HTMLDOC GUI and load the indicated book as
//                       necessary.
//   ~GUI()            - Destroy the HTMLDOC GUI.
//   GUI::doGUI()      - Display the window and loop for events.
//   GUI::progress()   - Update the progress bar on the GUI.
//   GUI::title()      - Set the title bar of the window.
//   GUI::newBook()    - Clear out the current GUI settings for a new book.
//   GUI::loadBook()   - Load a book file from disk.
//   GUI::saveBook()   - Save a book to disk.
//   GUI::checkSave()  - Check to see if a save is needed.
//   docTypeCB()       - Handle input on the document type buttons.
//   inputFilesCB()    - Handle selections in the input files browser.
//   addFileCB()       - Add a file to the input files list.
//   editFilesCB()     - Edit one or more files in the input files list.
//   deleteFileCB()    - Delete one or more files from the input files list.
//   moveUpFileCB()    - Move one or more files up in the input files list.
//   moveDownFileCB()  - Move one or more files down in the input files list.
//   logoImageCB()     - Change the logo image file.
//   outputTypeCB()    - Set the output file type.
//   outputPathCB()    - Set the output path.
//   outputFormatCB()  - Set the output format.
//   changeCB()        - Mark the current book as changed.
//   jpegCB()          - Handle JPEG changes.
//   htmlEditorCB()    - Change the HTML editor.
//   bodyColorCB()     - Set the body color.
//   bodyImageCB()     - Set the body image.
//   newBookCB()       - Create a new book.
//   openBookCB()      - Open an existing book.
//   saveBookCB()      - Save the current book to disk.
//   saveAsBookCB()    - Save the current book to disk to a new file.
//   generateBookCB()  - Generate the current book.
//   closeBookCB()     - Close the current book.
//

#include "htmldoc.h"

#ifdef HAVE_LIBFLTK

//
// Include necessary headers.
//

#  include <ctype.h>
#  include <FL/fl_ask.H>
#  include <FL/Fl_Color_Chooser.H>
#  include <FL/fl_draw.H>
#  include <FL/x.H>

#  ifdef WIN32
#    include <direct.h>
#    include <io.h>
#    include "icons.h"
#  else
#    include <unistd.h>
#    include "htmldoc.xbm"
#  endif // WIN32


const char	*GUI::help_dir = DOCUMENTATION;


//
// 'GUI()' - Build the HTMLDOC GUI and load the indicated book as necessary.
//

GUI::GUI(const char *filename)		// Book file to load initially
{
  Fl_Tabs		*tabs;		// Tabs
  Fl_Group		*group;		// Group
  Fl_Button		*button;	// Push button
  Fl_Box		*label;		// Label box
  static char		*htmldoc[1] = { "htmldoc" };	// argv[] array
  static Fl_Menu	tocMenu[] =	// Menu items for TOC chooser
			{
			  {"None", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"1 level", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"2 levels", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"3 levels", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"4 Levels", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {0}
			};
  static Fl_Menu	formatMenu[] =	// Menu items for header/footer choosers
			{
			  {"Blank", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"Title", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"Chapter", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"Heading", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"Logo", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"1,2,3,...", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"i,ii,iii,...", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"I,II,III,...", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {0}
			};
  static Fl_Menu	typefaceMenu[] = // Menu items for typeface choosers
			{
			  {"Courier", 0,  0, 0, 0, 0, FL_COURIER, 14, 0},
			  {"Times", 0,  0, 0, 0, 0, FL_TIMES, 14, 0},
			  {"Helvetica", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {0}
			};
  static Fl_Menu	fontMenu[] =	// Menu items for font choosers
			{
			  {"Courier", 0,  0, 0, 0, 0, FL_COURIER, 14, 0},
			  {"Courier-Bold", 0,  0, 0, 0, 0, FL_COURIER_BOLD, 14, 0},
			  {"Courier-Oblique", 0,  0, 0, 0, 0, FL_COURIER_ITALIC, 14, 0},
			  {"Courier-BoldOblique", 0,  0, 0, 0, 0, FL_COURIER_BOLD_ITALIC, 14, 0},
			  {"Times-Roman", 0,  0, 0, 0, 0, FL_TIMES, 14, 0},
			  {"Times-Bold", 0,  0, 0, 0, 0, FL_TIMES_BOLD, 14, 0},
			  {"Times-Italic", 0,  0, 0, 0, 0, FL_TIMES_ITALIC, 14, 0},
			  {"Times-BoldItalic", 0,  0, 0, 0, 0, FL_TIMES_BOLD_ITALIC, 14, 0},
			  {"Helvetica", 0,  0, 0, 0, 0, FL_HELVETICA, 14, 0},
			  {"Helvetica-Bold", 0,  0, 0, 0, 0, FL_HELVETICA_BOLD, 14, 0},
			  {"Helvetica-Oblique", 0,  0, 0, 0, 0, FL_HELVETICA_ITALIC, 14, 0},
			  {"Helvetica-BoldOblique", 0,  0, 0, 0, 0, FL_HELVETICA_BOLD_ITALIC, 14, 0},
			  {0}
			};


  //
  // Create a dialog window...
  //

  window = new Fl_Window(470, 390, "HTMLDOC " SVERSION);
  window->callback((Fl_Callback *)closeBookCB, this);

  controls = new Fl_Group(0, 0, 470, 360);
  tabs     = new Fl_Tabs(10, 10, 450, 260);

  //
  // Input tab...
  //

  inputTab = new Fl_Group(10, 35, 450, 220, "Input");

  group = new Fl_Group(140, 45, 150, 20, "Document Type: ");
  group->align(FL_ALIGN_LEFT);
    typeBook = new CheckButton(140, 45, 60, 20, "Book");
    typeBook->type(FL_RADIO_BUTTON);
    typeBook->setonly();
    typeBook->callback((Fl_Callback *)docTypeCB, this);

    typeWebPage = new CheckButton(200, 45, 90, 20, "Web Page");
    typeWebPage->type(FL_RADIO_BUTTON);
    typeWebPage->callback((Fl_Callback *)docTypeCB, this);
  group->end();

  group = new Fl_Group(140, 70, 215, 20, "Input Files: ");
  group->align(FL_ALIGN_LEFT);
  group->end();

  inputFiles = new FileBrowser(140, 70, 215, 125);
  inputFiles->color((Fl_Color)196);
  inputFiles->type(FL_MULTI_BROWSER);
  inputFiles->callback((Fl_Callback *)inputFilesCB, this);
  inputFiles->when(FL_WHEN_RELEASE | FL_WHEN_NOT_CHANGED);

  addFile = new Fl_Button(355, 70, 95, 25, "Add Files...");
  addFile->callback((Fl_Callback *)addFileCB, this);

  editFile = new Fl_Button(355, 95, 95, 25, "Edit Files...");
  editFile->deactivate();
  editFile->callback((Fl_Callback *)editFilesCB, this);

  deleteFile = new Fl_Button(355, 120, 95, 25, "Delete Files");
  deleteFile->deactivate();
  deleteFile->callback((Fl_Callback *)deleteFilesCB, this);

  moveUpFile = new Fl_Button(355, 145, 95, 25, "Move Up");
  moveUpFile->deactivate();
  moveUpFile->callback((Fl_Callback *)moveUpFilesCB, this);

  moveDownFile = new Fl_Button(355, 170, 95, 25, "Move Down");
  moveDownFile->deactivate();
  moveDownFile->callback((Fl_Callback *)moveDownFilesCB, this);

  logoImage = new Fl_Input(140, 205, 230, 25, "Logo Image: ");
  logoImage->color((Fl_Color)196);
  logoImage->when(FL_WHEN_CHANGED);
  logoImage->callback((Fl_Callback *)logoImageCB, this);

  logoBrowse = new Fl_Button(370, 205, 80, 25, "Browse...");
  logoBrowse->callback((Fl_Callback *)logoImageCB, this);

  titleImage = new Fl_Input(140, 235, 230, 25, "Title Image: ");
  titleImage->color((Fl_Color)196);
  titleImage->when(FL_WHEN_CHANGED);
  titleImage->callback((Fl_Callback *)titleImageCB, this);

  titleBrowse = new Fl_Button(370, 235, 80, 25, "Browse...");
  titleBrowse->callback((Fl_Callback *)titleImageCB, this);

  inputTab->end();
  inputTab->resizable(inputFiles);

  //
  // Output tab...
  //

  outputTab = new Fl_Group(10, 35, 450, 220, "Output");
  outputTab->hide();

  group = new Fl_Group(140, 45, 265, 20, "Output To: ");
  group->align(FL_ALIGN_LEFT);
    outputFile = new CheckButton(140, 45, 50, 20, "File");
    outputFile->type(FL_RADIO_BUTTON);
    outputFile->setonly();
    outputFile->callback((Fl_Callback *)outputTypeCB, this);

    outputDirectory = new CheckButton(190, 45, 105, 20, "Directory");
    outputDirectory->type(FL_RADIO_BUTTON);
    outputDirectory->callback((Fl_Callback *)outputTypeCB, this);
  group->end();

  outputPath = new Fl_Input(140, 70, 230, 25, "Output Path: ");
  outputPath->color((Fl_Color)196);
  outputPath->when(FL_WHEN_CHANGED);
  outputPath->callback((Fl_Callback *)outputPathCB, this);

  outputBrowse = new Fl_Button(370, 70, 80, 25, "Browse...");
  outputBrowse->callback((Fl_Callback *)outputPathCB, this);

  group = new Fl_Group(140, 100, 255, 20, "Output Format: ");
  group->align(FL_ALIGN_LEFT);
    typeHTML = new CheckButton(140, 100, 65, 20, "HTML");
    typeHTML->type(FL_RADIO_BUTTON);
    typeHTML->setonly();
    typeHTML->callback((Fl_Callback *)outputFormatCB, this);

    typePS = new CheckButton(205, 100, 45, 20, "PS");
    typePS->type(FL_RADIO_BUTTON);
    typePS->callback((Fl_Callback *)outputFormatCB, this);

    typePDF = new CheckButton(250, 100, 55, 20, "PDF");
    typePDF->type(FL_RADIO_BUTTON);
    typePDF->callback((Fl_Callback *)outputFormatCB, this);
  group->end();

  group = new Fl_Group(140, 125, 265, 20, "Output Options: ");
  group->align(FL_ALIGN_LEFT);
  group->end();

  grayscale = new CheckButton(140, 125, 90, 20, "Grayscale");
  grayscale->callback((Fl_Callback *)changeCB, this);

  titlePage = new CheckButton(230, 125, 90, 20, "Title Page");
  titlePage->callback((Fl_Callback *)changeCB, this);

  jpegCompress = new CheckButton(320, 125, 140, 20, "JPEG Big Images");
  jpegCompress->callback((Fl_Callback *)jpegCB, this);

  compression = new Fl_Slider(140, 150, 310, 25, "Compression: ");
  compression->align(FL_ALIGN_LEFT);
  compression->type(FL_HOR_NICE_SLIDER);
  compression->minimum(0.0);
  compression->maximum(9.0);
  compression->value(1.0);
  compression->step(1.0);
  compression->callback((Fl_Callback *)changeCB, this);

  label = new Fl_Box(140, 175, 30, 10, "None");
  label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  label->labelsize(10);

  label = new Fl_Box(170, 175, 30, 10, "Fast");
  label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  label->labelsize(10);

  label = new Fl_Box(420, 175, 30, 10, "Best");
  label->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  label->labelsize(10);

  jpegQuality = new Fl_Value_Slider(140, 190, 310, 25, "JPEG Quality: ");
  jpegQuality->align(FL_ALIGN_LEFT);
  jpegQuality->type(FL_HOR_NICE_SLIDER);
  jpegQuality->minimum(50.0);
  jpegQuality->maximum(100.0);
  jpegQuality->value(90.0);
  jpegQuality->step(1.0);
  jpegQuality->callback((Fl_Callback *)changeCB, this);

  label = new Fl_Box(175, 215, 40, 10, "Good");
  label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  label->labelsize(10);

  label = new Fl_Box(410, 215, 40, 10, "Best");
  label->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
  label->labelsize(10);

  outputTab->end();

  //
  // Page tab...
  //

  pageTab = new Fl_Group(10, 35, 450, 220, "Page");
  pageTab->hide();

  pageSize = new Fl_Input(140, 45, 120, 25, "Page Size: ");
  pageSize->when(FL_WHEN_CHANGED);
  pageSize->callback((Fl_Callback *)changeCB, this);

  pageDuplex = new CheckButton(265, 48, 130, 20, "Double-sided");
  pageDuplex->callback((Fl_Callback *)changeCB, this);

  pageTop = new Fl_Input(225, 75, 60, 25, "Top");
  pageTop->when(FL_WHEN_CHANGED);
  pageTop->callback((Fl_Callback *)changeCB, this);

  pageLeft = new Fl_Input(190, 105, 60, 25, "Left");
  pageLeft->when(FL_WHEN_CHANGED);
  pageLeft->callback((Fl_Callback *)changeCB, this);

  pageRight = new Fl_Input(255, 105, 60, 25, "Right");
  pageRight->when(FL_WHEN_CHANGED);
  pageRight->align(FL_ALIGN_RIGHT);
  pageRight->callback((Fl_Callback *)changeCB, this);

  pageBottom = new Fl_Input(225, 135, 60, 25, "Bottom");
  pageBottom->when(FL_WHEN_CHANGED);
  pageBottom->callback((Fl_Callback *)changeCB, this);

  pageHeaderLeft = new Fl_Choice(140, 165, 100, 25, "Header: ");
  pageHeaderLeft->menu(formatMenu);
  pageHeaderLeft->callback((Fl_Callback *)changeCB, this);

  pageHeaderCenter = new Fl_Choice(245, 165, 100, 25);
  pageHeaderCenter->menu(formatMenu);
  pageHeaderCenter->callback((Fl_Callback *)changeCB, this);

  pageHeaderRight = new Fl_Choice(350, 165, 100, 25);
  pageHeaderRight->menu(formatMenu);
  pageHeaderRight->callback((Fl_Callback *)changeCB, this);

  pageFooterLeft = new Fl_Choice(140, 195, 100, 25, "Footer: ");
  pageFooterLeft->menu(formatMenu);
  pageFooterLeft->callback((Fl_Callback *)changeCB, this);

  pageFooterCenter = new Fl_Choice(245, 195, 100, 25);
  pageFooterCenter->menu(formatMenu);
  pageFooterCenter->callback((Fl_Callback *)changeCB, this);

  pageFooterRight = new Fl_Choice(350, 195, 100, 25);
  pageFooterRight->menu(formatMenu);
  pageFooterRight->callback((Fl_Callback *)changeCB, this);

  pageTab->end();

  //
  // TOC tab...
  //

  tocTab = new Fl_Group(10, 35, 450, 220, "TOC");
  tocTab->hide();

  tocLevels = new Fl_Choice(140, 45, 100, 25, "Table of Contents: ");
  tocLevels->menu(tocMenu);
  tocLevels->callback((Fl_Callback *)tocCB, this);

  numberedToc = new CheckButton(245, 47, 160, 20, "Numbered Headings");
  numberedToc->callback((Fl_Callback *)changeCB, this);

  tocHeader = new Fl_Group(140, 75, 310, 25, "Header: ");
  tocHeader->align(FL_ALIGN_LEFT);

    tocHeaderLeft = new Fl_Choice(140, 75, 100, 25);
    tocHeaderLeft->menu(formatMenu);
    tocHeaderLeft->callback((Fl_Callback *)changeCB, this);

    tocHeaderCenter = new Fl_Choice(245, 75, 100, 25);
    tocHeaderCenter->menu(formatMenu);
    tocHeaderCenter->callback((Fl_Callback *)changeCB, this);

    tocHeaderRight = new Fl_Choice(350, 75, 100, 25);
    tocHeaderRight->menu(formatMenu);
    tocHeaderRight->callback((Fl_Callback *)changeCB, this);

  tocHeader->end();

  tocFooter = new Fl_Group(140, 105, 310, 25, "Footer: ");
  tocFooter->align(FL_ALIGN_LEFT);

    tocFooterLeft = new Fl_Choice(140, 105, 100, 25, "Footer: ");
    tocFooterLeft->menu(formatMenu);
    tocFooterLeft->callback((Fl_Callback *)changeCB, this);

    tocFooterCenter = new Fl_Choice(245, 105, 100, 25);
    tocFooterCenter->menu(formatMenu);
    tocFooterCenter->callback((Fl_Callback *)changeCB, this);

    tocFooterRight = new Fl_Choice(350, 105, 100, 25);
    tocFooterRight->menu(formatMenu);
    tocFooterRight->callback((Fl_Callback *)changeCB, this);

  tocFooter->end();

  tocTitle = new Fl_Input(140, 135, 310, 25, "Title: ");
  tocTitle->when(FL_WHEN_CHANGED);
  tocTitle->callback((Fl_Callback *)changeCB, this);

  tocTab->end();

  //
  // Colors tab...
  //

  colorsTab = new Fl_Group(10, 35, 395, 220, "Colors");
  colorsTab->hide();

  bodyColor = new Fl_Input(140, 45, 100, 25, "Body Color: ");
  bodyColor->when(FL_WHEN_CHANGED);
  bodyColor->callback((Fl_Callback *)bodyColorCB, this);

  bodyLookup = new Fl_Button(240, 45, 80, 25, "Lookup...");
  bodyLookup->callback((Fl_Callback *)bodyColorCB, this);

  bodyImage = new Fl_Input(140, 75, 230, 25, "Body Image: ");
  bodyImage->color((Fl_Color)196);
  bodyImage->when(FL_WHEN_CHANGED);
  bodyImage->callback((Fl_Callback *)bodyImageCB, this);

  bodyBrowse = new Fl_Button(370, 75, 80, 25, "Browse...");
  bodyBrowse->callback((Fl_Callback *)bodyImageCB, this);

  textColor = new Fl_Input(140, 105, 100, 25, "Text Color: ");
  textColor->when(FL_WHEN_CHANGED);
  textColor->callback((Fl_Callback *)textColorCB, this);

  textLookup = new Fl_Button(240, 105, 80, 25, "Lookup...");
  textLookup->callback((Fl_Callback *)textColorCB, this);

  colorsTab->end();

  //
  // Fonts tab...
  //

  fontsTab = new Fl_Group(10, 35, 395, 220, "Fonts");
  fontsTab->hide();

  fontBaseSize = new Fl_Counter(200, 45, 150, 25, "Base Font Size: ");
  fontBaseSize->callback((Fl_Callback *)changeCB, this);
  fontBaseSize->minimum(4.0);
  fontBaseSize->maximum(24.0);
  fontBaseSize->step(0.1);
  fontBaseSize->value(11.0);
  fontBaseSize->align(FL_ALIGN_LEFT);

  fontSpacing = new Fl_Counter(200, 75, 150, 25, "Line Spacing: ");
  fontSpacing->callback((Fl_Callback *)changeCB, this);
  fontSpacing->minimum(1.0);
  fontSpacing->maximum(3.0);
  fontSpacing->step(0.1);
  fontSpacing->value(1.2);
  fontSpacing->align(FL_ALIGN_LEFT);

  bodyFont = new Fl_Choice(200, 105, 100, 25, "Body Typeface: ");
  bodyFont->menu(typefaceMenu);
  bodyFont->callback((Fl_Callback *)changeCB, this);
  bodyFont->value(TYPE_TIMES);

  headingFont = new Fl_Choice(200, 135, 100, 25, "Heading Typeface: ");
  headingFont->menu(typefaceMenu);
  headingFont->callback((Fl_Callback *)changeCB, this);
  headingFont->value(TYPE_HELVETICA);

  headFootSize = new Fl_Counter(200, 165, 150, 25, "Header/Footer Size: ");
  headFootSize->callback((Fl_Callback *)changeCB, this);
  headFootSize->minimum(4.0);
  headFootSize->maximum(24.0);
  headFootSize->step(0.1);
  headFootSize->value(11.0);
  headFootSize->align(FL_ALIGN_LEFT);

  headFootFont = new Fl_Choice(200, 195, 220, 25, "Header/Footer Font: ");
  headFootFont->menu(fontMenu);
  headFootFont->callback((Fl_Callback *)changeCB, this);
  headFootFont->value(TYPE_HELVETICA);

  fontsTab->end();

  //
  // Options tab...
  //

  optionsTab = new Fl_Group(10, 35, 450, 220, "Options");
  optionsTab->hide();

  htmlEditor = new Fl_Input(140, 45, 215, 25, "HTML Editor: ");
  htmlEditor->color((Fl_Color)196);
  htmlEditor->value(HTMLEditor);
  htmlEditor->callback((Fl_Callback *)htmlEditorCB, this);

  htmlBrowse = new Fl_Button(355, 45, 95, 25, "Browse...");
  htmlBrowse->callback((Fl_Callback *)htmlEditorCB, this);

  psLevel = new Fl_Group(140, 75, 310, 20, "PostScript: ");
  psLevel->align(FL_ALIGN_LEFT);

    ps1 = new CheckButton(140, 75, 70, 20, "Level 1");
    ps1->type(FL_RADIO_BUTTON);
    ps1->callback((Fl_Callback *)psCB, this);

    ps2 = new CheckButton(210, 75, 70, 20, "Level 2");
    ps2->type(FL_RADIO_BUTTON);
    ps2->callback((Fl_Callback *)psCB, this);

    ps3 = new CheckButton(280, 75, 70, 20, "Level 3");
    ps3->type(FL_RADIO_BUTTON);
    ps3->callback((Fl_Callback *)psCB, this);

  psLevel->end();

  psCommands = new CheckButton(140, 100, 310, 20, "Send Printer Commands");
  psCommands->callback((Fl_Callback *)changeCB, this);

  pdfVersion = new Fl_Group(140, 125, 310, 40, "PDF Version: \n ");
  pdfVersion->align(FL_ALIGN_LEFT);

    pdf11 = new CheckButton(140, 125, 125, 20, "1.1 (Acrobat 2.x)");
    pdf11->type(FL_RADIO_BUTTON);
    pdf11->callback((Fl_Callback *)pdfCB, this);

    pdf12 = new CheckButton(270, 125, 125, 20, "1.2 (Acrobat 3.0)");
    pdf12->type(FL_RADIO_BUTTON);
    pdf12->callback((Fl_Callback *)pdfCB, this);

    pdf13 = new CheckButton(140, 145, 125, 20, "1.3 (Acrobat 4.0)");
    pdf13->type(FL_RADIO_BUTTON);
    pdf13->callback((Fl_Callback *)pdfCB, this);

  pdfVersion->end();

  optionsTab->end();

  tabs->end();

  //
  // Button bar...
  //

  button = new Fl_Button(10, 330, 50, 25, "Help");
  button->shortcut(FL_F + 1);
  button->callback((Fl_Callback *)helpCB, this);

  button = new Fl_Button(65, 330, 45, 25, "New");
  button->shortcut(FL_CTRL | 'n');
  button->callback((Fl_Callback *)newBookCB, this);

  button = new Fl_Button(115, 330, 60, 25, "Open...");
  button->shortcut(FL_CTRL | 'o');
  button->callback((Fl_Callback *)openBookCB, this);

  bookSave = new Fl_Button(180, 330, 50, 25, "Save");
  bookSave->shortcut(FL_CTRL | 's');
  bookSave->callback((Fl_Callback *)saveBookCB, this);

  bookSaveAs = new Fl_Button(235, 330, 80, 25, "Save As...");
  bookSaveAs->shortcut(FL_CTRL | FL_SHIFT | 's');
  bookSaveAs->callback((Fl_Callback *)saveAsBookCB, this);

  bookGenerate = new Fl_Button(320, 330, 80, 25, "Generate");
  bookGenerate->shortcut(FL_CTRL | 'g');
  bookGenerate->callback((Fl_Callback *)generateBookCB, this);

  button = new Fl_Button(405, 330, 55, 25, "Close");
  button->shortcut(FL_CTRL | 'q');
  button->callback((Fl_Callback *)closeBookCB, this);

  controls->end();

  //
  // Copyright notice...
  //

  label = new Fl_Box(10, 275, 450, 50,
          "HTMLDOC " SVERSION " Copyright 1997-1999 by Easy Software Products "
	  "(http://www.easysw.com). This program is free software; you can "
	  "redistribute it and/or modify it under the terms of the GNU General "
	  "Public License as published by the Free Software Foundation. This "
	  "software is based in part on the work of the Independent JPEG Group."
	  );
  label->labelsize(10);
  label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);

  //
  // Progress bar...
  //

  progressText = new Fl_Group(10, 360, 250, 20, "HTMLDOC " SVERSION " Ready.");
  progressText->box(FL_DOWN_BOX);
  progressText->labelsize(12);
  progressText->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  progressText->end();

  progressBar = new Fl_Slider(260, 360, 200, 20);
  progressBar->type(3);
  progressBar->color(15);
  progressBar->color2(10);
  progressBar->maximum(100);
  progressBar->value(0);

  window->end();

  // Set the class name to "htmldoc".
  window->xclass("htmldoc");

#  ifdef WIN32
  // Load the HTMLDOC icon image...
  window->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON)));
#  else // X11
  // Open the X display and load the HTMLDOC icon image...
  fl_open_display();
  window->icon((char *)XCreateBitmapFromData(fl_display,
               DefaultRootWindow(fl_display), htmldoc_bits,
	       htmldoc_width, htmldoc_height));
#  endif // WIN32

  window->resizable(tabs);
  window->size_range(470, 390);
  window->show(1, htmldoc);

  fc = new FileChooser(".", "*", FileChooser::SINGLE, "Title");
  fc->color((Fl_Color)196);

  help = new HelpDialog();

  if (!FileIcon::first())
    FileIcon::load_system_icons();

  icon = FileIcon::find("file.html", FileIcon::PLAIN);

  while (window->damage())
    Fl::check();

  //
  // Load the given book or create a new one...
  //

  book_changed = 0;

  if (filename == NULL)
    newBookCB(NULL, this);
  else
    loadBook(filename);
}


//
// '~GUI()' - Destroy the HTMLDOC GUI.
//

GUI::~GUI(void)
{
  delete window;
  delete fc;
  delete help;
}


//
// 'GUI::show()' - Display the window.
//

void
GUI::show(void)
{
  static char	*htmldoc[1] = { "htmldoc" };	// argv[] array


  window->show(1, htmldoc);
}


//
// 'GUI::progress()' - Update the progress bar on the GUI.
//

void
GUI::progress(int  percent,	// I - Percent complete
              char *text)	// I - Text prompt
{
  if (text != NULL)
  {
    progressText->label(text);
    progressText->redraw();
  }
  else if (percent == 0)
  {
    progressText->label("HTMLDOC " SVERSION " Ready.");
    progressText->redraw();
  }

  progressBar->value(percent);

  Fl::check();
}


//
// 'GUI::title()' - Set the title bar of the window.
//

void
GUI::title(const char *filename,// Name of file being edited
           int        changed)	// Whether or not the file is modified
{
  static char	title[1024];	// Title string


  book_changed = changed;

  if (filename == NULL || filename[0] == '\0')
  {
    book_filename[0] = '\0';
    strcpy(title, "NewBook");
  }
  else
  {
    strcpy(book_filename, filename);
    strcpy(title, file_basename(filename));
  }

  if (changed)
    strcat(title, "(modified) - ");
  else
    strcat(title, " - ");

  strcat(title, "HTMLDOC " SVERSION);

  window->label(title);
  if (window->visible())
    Fl::check();
}


//
// 'GUI::newBook()' - Clear out the current GUI settings for a new book.
//

int			// O - 1 on success, 0 on failure
GUI::newBook(void)
{
  if (!checkSave())
    return (0);

  typeBook->setonly();
  docTypeCB(typeBook, this);

  inputFiles->clear();
  inputFilesCB(inputFiles, this);

  logoImage->value("");
  titleImage->value("");

  outputFile->setonly();
  outputTypeCB(outputFile, this);

  outputPath->value("");

  typeHTML->setonly();
  outputFormatCB(typeHTML, this);

  grayscale->clear();
  titlePage->set();
  jpegCompress->clear();

  bodyColor->value("");
  bodyImage->value("");
  textColor->value("");

  pageSize->value("Universal");
  pageLeft->value("1.0in");
  pageRight->value("0.5in");
  pageTop->value("0.5in");
  pageBottom->value("0.5in");
  pageDuplex->clear();

  pageHeaderLeft->value(0);	/* Blank */
  pageHeaderCenter->value(1);	/* Title */
  pageHeaderRight->value(0);	/* Blank */

  pageFooterLeft->value(3);	/* Heading */
  pageFooterCenter->value(0);	/* Blank */
  pageFooterRight->value(5);	/* 1,2,3,... */

  tocLevels->value(3);
  numberedToc->clear();

  tocHeaderLeft->value(0);	/* Blank */
  tocHeaderCenter->value(1);	/* Title */
  tocHeaderRight->value(0);	/* Blank */

  tocFooterLeft->value(3);	/* Heading */
  tocFooterCenter->value(0);	/* Blank */
  tocFooterRight->value(6);	/* i,ii,iii,... */

  tocTitle->value("Table of Contents");

  compression->value(0.0);
  compression->deactivate();

  jpegQuality->value(90.0);
  jpegQuality->deactivate();

  pdfVersion->deactivate();
  pdf12->setonly();

  psLevel->deactivate();
  ps2->setonly();

  psCommands->deactivate();
  psCommands->value(0);

  title(NULL, 0);

  return (1);
}


//
// 'GUI::loadBook()' - Load a book file from disk.
//

int					// O - 1 = success, 0 = fail
GUI::loadBook(const char *filename)	// I - Name of book file
{
  int		i,
		count;
  FILE		*fp;
  char		line[10240],
		*lineptr,
		temp[1024],
		temp2[1024],
		*tempptr;
  static char	formats[256],
		first_time = 1;
  static char	*types[] =		// Typeface names...
		{ "Courier", "Times", "Helvetica" };
  static char	*fonts[] =		// Font names...
		{
		  "Courier", "Courier-Bold", "Courier-Oblique",
		  "Courier-BoldOblique", "Times-Roman", "Times-Bold",
		  "Times-Italic", "Times-BoldItalic",
		  "Helvetica", "Helvetica-Bold",
		  "Helvetica-Oblique", "Helvetica-BoldOblique"
		};


  //
  // Initialize the format character lookup table as needed...
  //

  if (first_time)
  {
    first_time = 0;

    memset(formats, 0, sizeof(formats));
    formats['t'] = 1;
    formats['c'] = 2;
    formats['h'] = 3;
    formats['l'] = 4;
    formats['1'] = 5;
    formats['i'] = 6;
    formats['I'] = 7;
  }

  //
  // If the filename contains a path, chdir to it first...
  //

  if ((tempptr = file_directory(filename)) != NULL)
  {
   /*
    * Filename contains a complete path - get the directory portion and do
    * a chdir()...
    */

    chdir(tempptr);
    fc->directory(".");

    filename = file_basename(filename);
  }

  //
  // Open the file...
  //

  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fl_alert("Unable to open \"%s\"!", filename);
    return (0);
  }

  fgets(line, sizeof(line), fp);  /* Get header... */
  if (strncmp(line, "#HTMLDOC", 8) != 0)
  {
    fclose(fp);
    fl_alert("Bad or missing #HTMLDOC header:\n%-80.80s", line);
    return (0);
  }

  if (!newBook())
  {
    fclose(fp);
    return (0);
  }

  fgets(line, sizeof(line), fp);  /* Get input file count... */
  count = atoi(line);

  for (i = 0; i < count; i ++)
  {
    fgets(line, sizeof(line), fp);  /* Get input file... */
    line[strlen(line) - 1] = '\0';  /* Drop trailing newline */

    inputFiles->add(line, icon);
  }

  inputFiles->topline(1);

  fgets(line, sizeof(line), fp);  /* Get options... */
  line[strlen(line) - 1] = '\0';  /* Drop trailing newline */

  for (lineptr = line; *lineptr != '\0';)
  {
    while (*lineptr == ' ')
      lineptr ++;

    for (tempptr = temp; *lineptr != '\0' && *lineptr != ' ';)
      *tempptr++ = *lineptr++;
    *tempptr = '\0';

    if (strcmp(temp, "--duplex") == 0)
    {
      pageDuplex->set();
      continue;
    }
    else if (strncmp(temp, "--jpeg", 6) == 0)
    {
      jpegCompress->set();
      jpegQuality->activate();
      if (strlen(temp) > 7)
        jpegQuality->value(atof(temp + 7));
      else
        jpegQuality->value(90.0);
      continue;
    }
    else if (strcmp(temp, "--grayscale") == 0)
    {
      grayscale->set();
      continue;
    }
    else if (strcmp(temp, "--pscommands") == 0)
    {
      psCommands->set();
      continue;
    }
    else if (strncmp(temp, "--compression", 13) == 0)
    {
      if (strlen(temp) > 14)
        compression->value(atof(temp + 14));
      else
        compression->value(1.0);
      continue;
    }
    else if (strcmp(temp, "--no-compression") == 0)
    {
      compression->value(0.0);
      continue;
    }
    else if (strcmp(temp, "--numbered") == 0)
    {
      numberedToc->set();
      continue;
    }
    else if (strcmp(temp, "--no-toc") == 0)
    {
      tocLevels->value(0);
      continue;
    }
    else if (strcmp(temp, "--no-title") == 0)
    {
      titlePage->clear();
      continue;
    }
    else if (strcmp(temp, "--webpage") == 0)
    {
      typeWebPage->setonly();
      docTypeCB(typeWebPage, this);
      continue;
    }
    else if (temp[0] != '-')
    {
      inputFiles->add(temp, icon);
      continue;
    }

    while (*lineptr == ' ')
      lineptr ++;

    for (tempptr = temp2; *lineptr != '\0' && *lineptr != ' ';)
      *tempptr++ = *lineptr++;
    *tempptr = '\0';

    if (strcmp(temp, "-t") == 0)
    {
      if (strcmp(temp2, "html") == 0)
      {
        typeHTML->setonly();
	outputFormatCB(typeHTML, this);
      }
      else if (strcmp(temp2, "ps1") == 0)
      {
        typePS->setonly();
	ps1->setonly();
	outputFormatCB(typePS, this);
	psCB(ps1, this);
      }
      else if (strcmp(temp2, "ps") == 0 ||
               strcmp(temp2, "ps2") == 0)
      {
        typePS->setonly();
	ps2->setonly();
	outputFormatCB(typePS, this);
	psCB(ps2, this);
      }
      else if (strcmp(temp2, "ps3") == 0)
      {
        typePS->setonly();
	ps3->setonly();
	outputFormatCB(typePS, this);
	psCB(ps3, this);
      }
      else if (strcmp(temp2, "pdf11") == 0)
      {
        typePDF->setonly();
	pdf11->setonly();
	outputFormatCB(typePDF, this);
	pdfCB(pdf11, this);
      }
      else if (strcmp(temp2, "pdf") == 0 ||
               strcmp(temp2, "pdf12") == 0)
      {
        typePDF->setonly();
	pdf12->setonly();
	outputFormatCB(typePDF, this);
	pdfCB(pdf12, this);
      }
      else if (strcmp(temp2, "pdf13") == 0)
      {
        typePDF->setonly();
	pdf13->setonly();
	outputFormatCB(typePDF, this);
	pdfCB(pdf13, this);
      }
    }
    else if (strcmp(temp, "--logo") == 0)
      logoImage->value(temp2);
    else if (strcmp(temp, "--title") == 0)
      titleImage->value(temp2);
    else if (strcmp(temp, "-f") == 0)
    {
      outputPath->value(temp2);
      outputFile->setonly();
      outputTypeCB(outputFile, this);
    }
    else if (strcmp(temp, "-d") == 0)
    {
      outputPath->value(temp2);
      outputDirectory->setonly();
      outputTypeCB(outputDirectory, this);
    }
    else if (strcmp(temp, "--size") == 0)
      pageSize->value(temp2);
    else if (strcmp(temp, "--left") == 0)
      pageLeft->value(temp2);
    else if (strcmp(temp, "--right") == 0)
      pageRight->value(temp2);
    else if (strcmp(temp, "--top") == 0)
      pageTop->value(temp2);
    else if (strcmp(temp, "--bottom") == 0)
      pageBottom->value(temp2);
    else if (strcmp(temp, "--header") == 0)
    {
      pageHeaderLeft->value(formats[temp2[0]]);
      pageHeaderCenter->value(formats[temp2[1]]);
      pageHeaderRight->value(formats[temp2[2]]);
    }
    else if (strcmp(temp, "--footer") == 0)
    {
      pageFooterLeft->value(formats[temp2[0]]);
      pageFooterCenter->value(formats[temp2[1]]);
      pageFooterRight->value(formats[temp2[2]]);
    }
    else if (strcmp(temp, "--bodycolor") == 0)
      bodyColor->value(temp2);
    else if (strcmp(temp, "--bodyimage") == 0)
      bodyImage->value(temp2);
    else if (strcmp(temp, "--textcolor") == 0)
      textColor->value(temp2);
    else if (strcmp(temp, "--toclevels") == 0)
      tocLevels->value(atoi(temp2));
    else if (strcmp(temp, "--tocheader") == 0)
    {
      tocHeaderLeft->value(formats[temp2[0]]);
      tocHeaderCenter->value(formats[temp2[1]]);
      tocHeaderRight->value(formats[temp2[2]]);
    }
    else if (strcmp(temp, "--tocfooter") == 0)
    {
      tocFooterLeft->value(formats[temp2[0]]);
      tocFooterCenter->value(formats[temp2[1]]);
      tocFooterRight->value(formats[temp2[2]]);
    }
    else if (strcmp(temp, "--fontsize") == 0)
      fontBaseSize->value(atof(temp2));
    else if (strcmp(temp, "--fontspacing") == 0)
      fontSpacing->value(atof(temp2));
    else if (strcmp(temp, "--headingfont") == 0)
    {
      for (i = 0; i < 3; i ++)
        if (strcasecmp(types[i], temp2) == 0)
	{
	  headingFont->value(i);
	  break;
	}
    }
    else if (strcmp(temp, "--bodyfont") == 0)
    {
      for (i = 0; i < 3; i ++)
        if (strcasecmp(types[i], temp2) == 0)
	{
	  bodyFont->value(i);
	  break;
	}
    }
    else if (strcmp(temp, "--headfootsize") == 0)
      headFootSize->value(atof(temp2));
    else if (strcmp(temp, "--headfootfont") == 0)
    {
      for (i = 0; i < 3; i ++)
        if (strcasecmp(fonts[i], temp2) == 0)
	{
	  headFootFont->value(i);
	  break;
	}
    }
  }

  fclose(fp);

  title(filename, 0);

  return (1);
}


//
// 'GUI::saveBook()' - Save a book to disk.
//

int					// O - 1 = success, 0 = fail
GUI::saveBook(const char *filename)	// I - Name of book file
{
  int		i,			// Looping var
		count;			// Number of files
  FILE		*fp;			// Book file pointer
  static char	*formats = ".tchl1iI";	// Format characters
  static char	*types[] =		// Typeface names...
		{ "Courier", "Times", "Helvetica" };
  static char	*fonts[] =		// Font names...
		{
		  "Courier", "Courier-Bold", "Courier-Oblique",
		  "Courier-BoldOblique", "Times-Roman", "Times-Bold",
		  "Times-Italic", "Times-BoldItalic",
		  "Helvetica", "Helvetica-Bold",
		  "Helvetica-Oblique", "Helvetica-BoldOblique"
		};


  fp = fopen(filename, "w");
  if (fp == NULL)
  {
    fl_alert("Unable to create \"%s\"!", filename);
    return (0);
  }

  fputs("#HTMLDOC " SVERSION "\n", fp);

  count = inputFiles->size();
  fprintf(fp, "%d\n", count);

  for (i = 1; i <= count; i ++)
    fprintf(fp, "%s\n", inputFiles->text(i));

  if (typeHTML->value())
    fputs("-t html", fp);
  else if (typePS->value())
  {
    if (ps1->value())
      fputs("-t ps1", fp);
    else if (ps2->value())
      fputs("-t ps2", fp);
    else if (ps3->value())
      fputs("-t ps3", fp);
  }
  else if (pdf11->value())
    fputs("-t pdf11", fp);
  else if (pdf12->value())
    fputs("-t pdf12", fp);
  else
    fputs("-t pdf13", fp);

  if (outputFile->value())
    fprintf(fp, " -f %s", outputPath->value());
  else
    fprintf(fp, " -d %s", outputPath->value());

  if (typeWebPage->value())
    fputs(" --webpage", fp);
  else
  {
    if (titlePage->value() == 0)
      fputs(" --no-title", fp);

    if (tocLevels->value() == 0)
      fputs(" --no-toc", fp);
    else
      fprintf(fp, " --toclevels %d", tocLevels->value());

    if (numberedToc->value())
      fputs(" --numbered", fp);
  }

  if (logoImage->size() > 0)
    fprintf(fp, " --logo %s", logoImage->value());

  if (titleImage->size() > 0)
    fprintf(fp, " --title %s", titleImage->value());

  if (bodyColor->size() > 0)
    fprintf(fp, " --bodycolor %s", bodyColor->value());

  if (bodyImage->size() > 0)
    fprintf(fp, " --bodyimage %s", bodyImage->value());

  if (!typeHTML->value())
  {
    if (pageSize->size() > 0)
      fprintf(fp, " --size %s", pageSize->value());

    if (pageLeft->size() > 0)
      fprintf(fp, " --left %s", pageLeft->value());

    if (pageRight->size() > 0)
      fprintf(fp, " --right %s", pageRight->value());

    if (pageTop->size() > 0)
      fprintf(fp, " --top %s", pageTop->value());

    if (pageBottom->size() > 0)
      fprintf(fp, " --bottom %s", pageBottom->value());

    fprintf(fp, " --header %c%c%c",
            formats[pageHeaderLeft->value()],
	    formats[pageHeaderCenter->value()],
	    formats[pageHeaderRight->value()]);

    fprintf(fp, " --footer %c%c%c",
            formats[pageFooterLeft->value()],
	    formats[pageFooterCenter->value()],
	    formats[pageFooterRight->value()]);

    fprintf(fp, " --tocheader %c%c%c",
            formats[tocHeaderLeft->value()],
	    formats[tocHeaderCenter->value()],
	    formats[tocHeaderRight->value()]);

    fprintf(fp, " --tocfooter %c%c%c",
            formats[tocFooterLeft->value()],
	    formats[tocFooterCenter->value()],
	    formats[tocFooterRight->value()]);

    if (pageDuplex->value())
      fputs(" --duplex", fp);

    if (grayscale->value())
      fputs(" --grayscale", fp);

    if (psCommands->value())
      fputs(" --pscommands", fp);

    if (compression->value() == 0.0f)
      fputs(" --no-compression", fp);
    else
      fprintf(fp, " --compression=%.0f", compression->value());

    if (jpegCompress->value())
      fprintf(fp, " --jpeg=%.0f", jpegQuality->value());

    fprintf(fp, " --fontsize %.1f", fontBaseSize->value());
    fprintf(fp, " --fontspacing %.1f", fontSpacing->value());
    fprintf(fp, " --headingfont %s", types[headingFont->value()]);
    fprintf(fp, " --bodyfont %s", types[bodyFont->value()]);
    fprintf(fp, " --headfootsize %.1f", headFootSize->value());
    fprintf(fp, " --headfootfont %s", fonts[headFootFont->value()]);
  }

  fputs("\n", fp);
  fclose(fp);

  title(filename, 0);

  return (1);
}


//
// 'GUI::checkSave()' - Check to see if a save is needed.
//

int			// O - 1 if no save is needed, 0 if save is needed
GUI::checkSave(void)
{
  if (book_changed)
  {
    switch (fl_choice("The current book has been changed.\n"
                      "Do you wish to save it first?",
		      "Cancel", "Save", "Discard"))
    {
      case 0 : /* Cancel */
          return (0);

      case 1 : /* Save */          
	  if (book_filename[0] != '\0')
            return (saveBook(book_filename));
	  else
          {
	    saveAsBookCB(NULL, this);
	    return (!book_changed);
	  }

      case 2 : /* Discard */
          return (1);
    }
  }

  return (1);
}


//
// 'GUI::changeCB()' - Mark the current book as changed.
//

void
GUI::changeCB(Fl_Widget *w,	// I - Widget
              GUI       *gui)	// I - GUI
{
  REF(w);

  gui->title(gui->book_filename, 1);
}


//
// 'GUI::docTypeCB()' - Handle input on the document type buttons.
//

void
GUI::docTypeCB(Fl_Widget *w,		// I - Toggle button widget
               GUI       *gui)	// I - GUI
{
  REF(w);

  gui->title(gui->book_filename, 1);

  if (w == gui->typeBook)
  {
    gui->typeHTML->activate();

    gui->titlePage->value(0);

    gui->tocLevels->value(3);
    gui->tocLevels->activate();
    gui->numberedToc->activate();

    gui->tocHeaderLeft->activate();
    gui->tocHeaderCenter->activate();
    gui->tocHeaderRight->activate();

    gui->tocFooterLeft->activate();
    gui->tocFooterCenter->activate();
    gui->tocFooterRight->activate();
  }
  else
  {
    if (gui->typeHTML->value())
      gui->typePS->set();

    gui->typeHTML->deactivate();

    gui->titlePage->value(0);

    gui->tocLevels->value(0);
    gui->tocLevels->deactivate();

    gui->numberedToc->value(0);
    gui->numberedToc->deactivate();

    gui->tocHeaderLeft->deactivate();
    gui->tocHeaderCenter->deactivate();
    gui->tocHeaderRight->deactivate();

    gui->tocFooterLeft->deactivate();
    gui->tocFooterCenter->deactivate();
    gui->tocFooterRight->deactivate();
  }
}


//
// 'GUI::inputFilesCB()' - Handle selections in the input files browser.
//

void
GUI::inputFilesCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  int	i,			// Looping var
	num_items;		// Number of items in the file list


  REF(w);

  num_items = gui->inputFiles->size();

  for (i = 1; i <= num_items; i ++)
    if (gui->inputFiles->selected(i))
      break;

  if (i <= num_items)
  {
    gui->editFile->activate();
    gui->deleteFile->activate();
  }
  else
  {
    gui->editFile->deactivate();
    gui->deleteFile->deactivate();
  }

  if (gui->inputFiles->selected(1) || num_items == 0)
    gui->moveUpFile->deactivate();
  else
    gui->moveUpFile->activate();

  if (gui->inputFiles->selected(num_items) || num_items == 0)
    gui->moveDownFile->deactivate();
  else
    gui->moveDownFile->activate();

  if (Fl::event_clicks() && i <= num_items)
    editFilesCB(gui->editFile, gui);
}


//
// 'GUI::addFileCB()' - Add a file to the input files list.
//

void
GUI::addFileCB(Fl_Widget *w,	// I - Widget
               GUI       *gui)	// I - GUI
{
  int	i;			// Looping var


  REF(w);

  gui->fc->filter("*.htm*");
  gui->fc->type(FileChooser::MULTI);
  gui->fc->label("Add HTML Files?");
  gui->fc->show();
  while (gui->fc->visible())
    Fl::wait();

  if (gui->fc->count())
  {    
    for (i = 1; i <= gui->fc->count(); i ++)
      gui->inputFiles->add(gui->fc->value(i), gui->icon);

    gui->title(gui->book_filename, 1);
  }
}


//
// 'GUI::editFilesCB()' - Edit one or more files in the input files list.
//

void
GUI::editFilesCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  int			i,		// Looping var
			num_items;	// Number of items in the file list
  char			command[1024];	// Editor command
#ifdef WIN32
  STARTUPINFO		suInfo;		// Process startup information
  PROCESS_INFORMATION	prInfo;		// Process information
#endif // WIN32


  REF(w);

  num_items = gui->inputFiles->size();

  for (i = 1; i <= num_items; i ++)
    if (gui->inputFiles->selected(i))
    {
      sprintf(command, gui->htmlEditor->value(), gui->inputFiles->text(i));

#ifdef WIN32
      memset(&suInfo, 0, sizeof(suInfo));
      suInfo.cb = sizeof(suInfo);

      if (!CreateProcess(NULL, command, NULL, NULL, FALSE,
                         NORMAL_PRIORITY_CLASS, NULL, NULL, &suInfo, &prInfo))
        fl_alert("Unable to start editor!");
#else
      strcat(command, "&");
      if (system(command))
        fl_alert("Unable to start editor!");
#endif // !WIN32
    }
}


//
// 'GUI::deleteFileCB()' - Delete one or more files from the input files list.
//

void
GUI::deleteFilesCB(Fl_Widget *w,	// I - Widget
                   GUI       *gui)	// I - GUI
{
  int	i,			// Looping var
	num_items;		// Number of items in the file list


  REF(w);

  num_items = gui->inputFiles->size();

  for (i = num_items; i > 0; i --)
    if (gui->inputFiles->selected(i))
    {
      gui->inputFiles->select(i, 0);
      gui->inputFiles->remove(i);
      gui->title(gui->book_filename, 1);
    }
}


//
// 'GUI::moveUpFileCB()' - Move one or more files up in the input files list.
//

void
GUI::moveUpFilesCB(Fl_Widget *w,	// I - Widget
                   GUI       *gui)	// I - GUI
{
  int	i,			// Looping var
	num_items;		// Number of items in the file list
  char	*file;			// File to move up


  REF(w);

  num_items = gui->inputFiles->size();

  for (i = 1; i <= num_items; i ++)
    if (gui->inputFiles->selected(i))
    {
      file = (char *)gui->inputFiles->text(i);
      gui->inputFiles->insert(i - 1, file);
      gui->inputFiles->select(i - 1);
      gui->inputFiles->remove(i + 1);
      gui->inputFiles->select(i, 0);
      gui->title(gui->book_filename, 1);
    }

  if (gui->inputFiles->selected(1))
    gui->moveUpFile->deactivate();

  if (!gui->inputFiles->selected(num_items))
    gui->moveDownFile->activate();
}


//
// 'GUI::moveDownFileCB()' - Move one or more files down in the input files list.
//

void
GUI::moveDownFilesCB(Fl_Widget *w,	// I - Widget
                     GUI       *gui)	// I - GUI
{
  int	i,			// Looping var
	num_items;		// Number of items in the file list
  char	*file;			// File to move down


  REF(w);

  num_items = gui->inputFiles->size();

  for (i = num_items; i > 0; i --)
    if (gui->inputFiles->selected(i))
    {
      file = (char *)gui->inputFiles->text(i);
      gui->inputFiles->insert(i + 2, file);
      gui->inputFiles->select(i + 2);
      gui->inputFiles->remove(i);
      gui->inputFiles->select(i, 0);
      gui->title(gui->book_filename, 1);
    }

  if (!gui->inputFiles->selected(1))
    gui->moveUpFile->activate();

  if (gui->inputFiles->selected(num_items))
    gui->moveDownFile->deactivate();
}


//
// 'GUI::logoImageCB()' - Change the logo image file.
//

void
GUI::logoImageCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  if (w == gui->logoBrowse)
  {
    gui->fc->filter("*.{gif|jpg|png}");
    gui->fc->label("Logo Image?");
    gui->fc->type(FileChooser::SINGLE);
    gui->fc->show();
    while (gui->fc->visible())
      Fl::wait();

    if (gui->fc->count())
    {
      gui->logoImage->value(gui->fc->value());
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::titleImageCB()' - Change the title image file.
//

void
GUI::titleImageCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  if (w == gui->titleBrowse)
  {
    gui->fc->filter("*.{gif|jpg|png}");
    gui->fc->label("Title Image?");
    gui->fc->type(FileChooser::SINGLE);
    gui->fc->show();
    while (gui->fc->visible())
      Fl::wait();

    if (gui->fc->count())
    {
      gui->titleImage->value(gui->fc->value());
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::outputTypeCB()' - Set the output file type.
//

void
GUI::outputTypeCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  if (w == gui->outputFile)
  {
    gui->outputBrowse->activate();
    gui->typePDF->activate();
  }
  else if (gui->typePDF->value())
    gui->outputFile->setonly();
  else
  {
    gui->outputBrowse->deactivate();
    gui->typePDF->deactivate();
  }

  gui->title(gui->book_filename, 1);
}


//
// 'GUI::outputPathCB()' - Set the output path.
//

void
GUI::outputPathCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  if (w == gui->outputBrowse)
  {
    gui->fc->label("Output Path?");
    gui->fc->type(FileChooser::SINGLE);

    if (gui->typeHTML->value())
      gui->fc->filter("*.htm*");
    else if (gui->typePDF->value())
      gui->fc->filter("*.pdf");
    else
      gui->fc->filter("*.ps");

    gui->fc->show();
    while (gui->fc->visible())
      Fl::wait();

    if (gui->fc->count())
    {
      gui->outputPath->value(gui->fc->value());
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::outputFormatCB()' - Set the output format.
//

void
GUI::outputFormatCB(Fl_Widget *w,	// I - Widget
                    GUI       *gui)	// I - GUI
{
  gui->title(gui->book_filename, 1);

  if (w == gui->typePDF)
  {
    gui->pdfVersion->activate();
    gui->outputDirectory->deactivate();
  }
  else
  {
    gui->pdfVersion->deactivate();
    gui->outputDirectory->activate();
  }

  if (w == gui->typeHTML)
  {
    gui->jpegCompress->value(0);
    gui->jpegCompress->deactivate();
    gui->jpegQuality->deactivate();

    gui->grayscale->value(0);
    gui->grayscale->deactivate();

    gui->pageTab->deactivate();

    gui->tocHeaderLeft->deactivate();
    gui->tocHeaderCenter->deactivate();
    gui->tocHeaderRight->deactivate();

    gui->tocFooterLeft->deactivate();
    gui->tocFooterCenter->deactivate();
    gui->tocFooterRight->deactivate();

    gui->fontsTab->deactivate();
  }
  else
  {
    gui->grayscale->activate();

    gui->pageTab->activate();

    gui->tocHeaderLeft->activate();
    gui->tocHeaderCenter->activate();
    gui->tocHeaderRight->activate();

    gui->tocFooterLeft->activate();
    gui->tocFooterCenter->activate();
    gui->tocFooterRight->activate();

    gui->fontsTab->activate();

    if (w == gui->typePDF || !gui->ps1->value())
      gui->jpegCompress->activate();
    else
      gui->jpegCompress->deactivate();
  }

  if (w == gui->typePS)
  {
    gui->psLevel->activate();

    if (gui->ps1->value())
      gui->psCommands->deactivate();
    else
      gui->psCommands->activate();
  }
  else
  {
    gui->psLevel->deactivate();
    gui->psCommands->deactivate();
  }
}


//
// 'GUI::jpegCB()' - Handle JPEG changes.
//

void
GUI::jpegCB(Fl_Widget *w,	// I - Widget
            GUI       *gui)	// I - GUI
{
  REF(w);

  gui->title(gui->book_filename, 1);

  if (gui->jpegCompress->value())
    gui->jpegQuality->activate();
  else
    gui->jpegQuality->deactivate();
}


//
// 'GUI::tocCB()' - Handle Table-of-Contents changes.
//

void
GUI::tocCB(Fl_Widget *w,	// I - Widget
              GUI       *gui)	// I - GUI
{
  REF(w);

  gui->title(gui->book_filename, 1);

  if (gui->tocLevels->value())
  {
    gui->numberedToc->activate();
    gui->tocHeader->activate();
    gui->tocFooter->activate();
    gui->tocTitle->activate();
  }
  else
  {
    gui->numberedToc->deactivate();
    gui->tocHeader->deactivate();
    gui->tocFooter->deactivate();
    gui->tocTitle->deactivate();
  }
}


//
// 'GUI::pdfCB()' - Handle PDF version changes.
//

void
GUI::pdfCB(Fl_Widget *w,	// I - Widget
           GUI       *gui)	// I - GUI
{
  REF(w);


  if (gui->pdf11->value())
    gui->compression->deactivate();
  else
    gui->compression->activate();

  gui->title(gui->book_filename, 1);
}


//
// 'GUI::psCB()' - Handle PS language level changes.
//

void
GUI::psCB(Fl_Widget *w,		// I - Widget
          GUI       *gui)	// I - GUI
{
  REF(w);


  if (gui->ps1->value())
  {
    gui->jpegCompress->deactivate();
    gui->psCommands->deactivate();
    gui->psCommands->value(0);
  }
  else
  {
    gui->jpegCompress->activate();
    gui->psCommands->activate();
  }

  gui->title(gui->book_filename, 1);
}


//
// 'GUI::htmlEditorCB()' - Change the HTML editor.
//

void
GUI::htmlEditorCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  const char	*filename;	// New HTML editor file
  char		command[1024];	// Command string


  if (w == gui->htmlBrowse)
  {
#  if defined(WIN32) || defined(__EMX__)
    gui->fc->filter("*.exe");
#  else
    gui->fc->filter("*");
#  endif // WIN32 || __EMX__
    gui->fc->label("HTML Editor?");
    gui->fc->type(FileChooser::SINGLE);
    gui->fc->show();
    while (gui->fc->visible())
      Fl::wait();

    if (gui->fc->count())
    {
      filename = gui->fc->value();

      if (strstr(filename, "netscape") != NULL ||
          strstr(filename, "NETSCAPE") != NULL)
#if defined(WIN32) || defined(__EMX__)
        sprintf(command, "%s -edit %%s", filename);
#else
        sprintf(command, "%s -remote \'editFile(%%s)\'", filename);
#endif // WIN32 || __EMX__
      else
        sprintf(command, "%s %%s", filename);

      gui->htmlEditor->value(command);
    }
  }

  strcpy(HTMLEditor, gui->htmlEditor->value());
}


//
// 'GUI::bodyColorCB()' - Set the body color.
//

void
GUI::bodyColorCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  uchar	r, g, b;		// Color values
  int	color;			// Color from bar color
  char	newcolor[255];		// New color string


  if (w == gui->bodyLookup)
  {
    if (sscanf(gui->bodyColor->value(), "#%x", &color) == 1)
    {
      r = color >> 16;
      g = (color >> 8) & 255;
      b = color & 255;
    }
    else
    {
      r = 191;
      g = 191;
      b = 191;
    }

    if (fl_color_chooser("Body Color?", r, g, b))
    {
      sprintf(newcolor, "#%02x%02x%02x", r, g, b);
      gui->bodyColor->value(newcolor);
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::bodyImageCB()' - Set the body image.
//

void
GUI::bodyImageCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  if (w == gui->bodyBrowse)
  {
    gui->fc->filter("*.{gif|jpg|png}");
    gui->fc->label("Body Image?");
    gui->fc->type(FileChooser::SINGLE);
    gui->fc->show();
    while (gui->fc->visible())
      Fl::wait();

    if (gui->fc->count())
    {
      gui->bodyImage->value(gui->fc->value());
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::textColorCB()' - Set the text color.
//

void
GUI::textColorCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  uchar	r, g, b;		// Color values
  int	color;			// Color from bar color
  char	newcolor[255];		// New color string


  if (w == gui->textLookup)
  {
    if (sscanf(gui->textColor->value(), "#%x", &color) == 1)
    {
      r = color >> 16;
      g = (color >> 8) & 255;
      b = color & 255;
    }
    else
    {
      r = 0;
      g = 0;
      b = 0;
    }

    if (fl_color_chooser("Text Color?", r, g, b))
    {
      sprintf(newcolor, "#%02x%02x%02x", r, g, b);
      gui->textColor->value(newcolor);
      gui->title(gui->book_filename, 1);
    }
  }
  else
    gui->title(gui->book_filename, 1);
}


//
// 'GUI::helpCB()' - Show on-line help...
//

void
GUI::helpCB(Fl_Widget *w,	// I - Widget
            GUI       *gui)	// I - GUI
{
  char	link[1024];	// filename#link


  REF(w);

  sprintf(link, "%s/htmldoc.html#4", help_dir);
  gui->help->load(link);
  gui->help->show();
}


//
// 'GUI::newBookCB()' - Create a new book.
//

void
GUI::newBookCB(Fl_Widget *w,	// I - Widget
               GUI       *gui)	// I - GUI
{
  REF(w);

  gui->newBook();
}


//
// 'GUI::openBookCB()' - Open an existing book.
//

void
GUI::openBookCB(Fl_Widget *w,	// I - Widget
                GUI       *gui)	// I - GUI
{
  REF(w);

  gui->fc->filter("*.book");
  gui->fc->label("Book File?");
  gui->fc->type(FileChooser::SINGLE);
  gui->fc->show();
  while (gui->fc->visible())
    Fl::wait();

  if (gui->fc->count())
    gui->loadBook(gui->fc->value());
}


//
// 'GUI::saveBookCB()' - Save the current book to disk.
//

void
GUI::saveBookCB(Fl_Widget *w,	// I - Widget
                GUI       *gui)	// I - GUI
{
  if (gui->book_filename[0] == '\0')
    saveAsBookCB(w, gui);
  else
    gui->saveBook(gui->book_filename);
}


//
// 'GUI::saveAsBookCB()' - Save the current book to disk to a new file.
//

void
GUI::saveAsBookCB(Fl_Widget *w,		// I - Widget
                  GUI       *gui)	// I - GUI
{
  const char	*filename;	// Book filename
  char		*newfile;	// New filename
  char		*dir;		// Book directory


  REF(w);

  gui->fc->filter("*.book");
  gui->fc->label("Book File?");
  gui->fc->type(FileChooser::CREATE);
  gui->fc->show();
  while (gui->fc->visible())
    Fl::wait();

  if (gui->fc->count())
  {
    filename = gui->fc->value();

    if (access(filename, 0) == 0)
      if (!fl_ask("File already exists!  OK to overwrite?"))
	return;

    dir = file_directory(filename);

    for (int i = 1; i <= gui->inputFiles->size(); i ++)
    {
      newfile = file_localize((char *)gui->inputFiles->text(i), dir);
      gui->inputFiles->text(i, newfile);
    }

    newfile = file_localize((char *)gui->logoImage->value(), dir);
    gui->logoImage->value(newfile);

    newfile = file_localize((char *)gui->titleImage->value(), dir);
    gui->titleImage->value(newfile);

    newfile = file_localize((char *)gui->bodyImage->value(), dir);
    gui->bodyImage->value(newfile);

    newfile = file_localize((char *)gui->outputPath->value(), dir);
    gui->outputPath->value(newfile);

    chdir(dir);

    gui->saveBook(filename);
  }
}


//
// 'GUI::generateBookCB()' - Generate the current book.
//

void
GUI::generateBookCB(Fl_Widget *w,	// I - Widget
                    GUI       *gui)	// I - GUI
{
  int		i,		/* Looping var */
	        count;		/* Number of files */
  char	  	temp[1024];	/* Temporary string */
  FILE		*docfile;	/* Document file */
  tree_t	*document,	/* Master HTML document */
		*file,		/* HTML document file */
		*toc;		/* Table of contents */
  char		*filename,	/* HTML filename */
		base[1024],	/* Base directory of HTML file */
		bookbase[1024];	/* Base directory of book file */
  static char	*formats = ".tchl1iI";	// Format characters


  REF(w);

 /*
  * Disable the GUI while we generate...
  */

  gui->controls->deactivate();

 /*
  * Set global vars used for converting the HTML files to XYZ format...
  */

  strcpy(bookbase, file_directory(gui->book_filename));

  Verbosity = 1;

  strcpy(LogoImage, gui->logoImage->value());
  strcpy(TitleImage, gui->titleImage->value());
  strcpy(OutputPath, gui->outputPath->value());

  OutputFiles = gui->outputDirectory->value();

  set_page_size((char *)gui->pageSize->value());

  PageLeft     = get_measurement((char *)gui->pageLeft->value());
  PageRight    = get_measurement((char *)gui->pageRight->value());
  PageTop      = get_measurement((char *)gui->pageTop->value());
  PageBottom   = get_measurement((char *)gui->pageBottom->value());

  PageDuplex   = gui->pageDuplex->value();
  Compression  = (int)gui->compression->value();
  OutputColor  = !gui->grayscale->value();
  TocNumbers   = gui->numberedToc->value();
  TocLevels    = gui->tocLevels->value();
  TitlePage    = gui->titlePage->value();

  if (gui->jpegCompress->value())
    OutputJPEG = (int)gui->jpegQuality->value();
  else
    OutputJPEG = 0;

  TocHeader[0] = formats[gui->tocHeaderLeft->value()];
  TocHeader[1] = formats[gui->tocHeaderCenter->value()];
  TocHeader[2] = formats[gui->tocHeaderRight->value()];

  TocFooter[0] = formats[gui->tocFooterLeft->value()];
  TocFooter[1] = formats[gui->tocFooterCenter->value()];
  TocFooter[2] = formats[gui->tocFooterRight->value()];

  Header[0]    = formats[gui->pageHeaderLeft->value()];
  Header[1]    = formats[gui->pageHeaderCenter->value()];
  Header[2]    = formats[gui->pageHeaderRight->value()];

  Footer[0]    = formats[gui->pageFooterLeft->value()];
  Footer[1]    = formats[gui->pageFooterCenter->value()];
  Footer[2]    = formats[gui->pageFooterRight->value()];

  _htmlBodyFont    = (typeface_t)gui->bodyFont->value();
  _htmlHeadingFont = (typeface_t)gui->headingFont->value();
  htmlSetBaseSize(gui->fontBaseSize->value(), gui->fontSpacing->value());

  HeadFootType  = (typeface_t)(gui->headFootFont->value() / 4);
  HeadFootStyle = (style_t)(gui->headFootFont->value() & 3);
  HeadFootSize  = gui->headFootSize->value();

  if (gui->pdf11->value())
    PDFVersion = 1.1;
  else if (gui->pdf12->value())
    PDFVersion = 1.2;
  else
    PDFVersion = 1.3;

  if (gui->typePDF->value())
    PSLevel = 0;
  else if (gui->ps1->value())
    PSLevel = 1;
  else if (gui->ps2->value())
    PSLevel = 2;
  else
    PSLevel = 3;

  PSCommands = gui->psCommands->value();

  strcpy(BodyColor, gui->bodyColor->value());
  strcpy(BodyImage, gui->bodyImage->value());

  htmlSetTextColor((uchar *)gui->textColor->value());

 /*
  * Load the input files...
  */

  count    = gui->inputFiles->size();
  document = NULL;

  for (i = 1; i <= count; i ++)
  {
    filename = (char *)gui->inputFiles->text(i);

    if ((docfile = fopen(filename, "rb")) != NULL)
    {
     /*
      * Read from a file...
      */

      sprintf(temp, "Loading \"%s\"...", filename);
      gui->progress(100 * i / count, temp);

      strcpy(base, file_directory(filename));

      file = htmlAddTree(NULL, MARKUP_FILE, NULL);
      htmlSetVariable(file, (uchar *)"FILENAME", (uchar *)filename);

      htmlReadFile(file, docfile, base);

      fclose(docfile);

      if (file->child != NULL)
      {
        if (document == NULL)
          document = file;
        else
        {
          while (document->next != NULL)
            document = document->next;

          document->next = file;
          file->prev     = document;
        }
      }
      else
        htmlDeleteTree(file);
    }
    else
      fl_alert("Unable to open \"%s\" for reading!",
               gui->inputFiles->text(i));
  }

 /*
  * We *must* have a document to process...
  */

  if (document == NULL)
  {
    gui->controls->activate();
    gui->progress(0);
    fl_alert("No HTML files to format, cannot generate!");
    return;
  }

 /*
  * Find the first one in the list...
  */

  while (document->prev != NULL)
    document = document->prev;

 /*
  * Build a table of contents for the documents...
  */

  if (TocLevels > 0)
    toc = toc_build(document);
  else
    toc = NULL;

 /*
  * Figure out the printable area of the output page...
  */

  PagePrintWidth  = PageWidth - PageLeft - PageRight;
  PagePrintLength = PageLength - PageTop - PageBottom;

 /*
  * Generate the output file(s).
  */

  if (gui->typeHTML->value())
    html_export(document, toc);
  else
    pspdf_export(document, toc);

  htmlDeleteTree(document);
  htmlDeleteTree(toc);

  gui->controls->activate();
  gui->progress(0);

  fl_message("Document Generated!");
}


//
// 'GUI::closeBookCB()' - Close the current book.
//

void
GUI::closeBookCB(Fl_Widget *w,		// I - Widget
                 GUI       *gui)	// I - GUI
{
  REF(w);

  if (gui->checkSave())
    gui->hide();
}


//
// 'file_localize()' - Localize a filename for the new working directory.
//

char *					// O - New filename
GUI::file_localize(char *filename,	// I - Filename
                   char *newcwd)	// I - New directory
{
  char		*newslash, *slash;	// Directory separators
  char		cwd[1024];		// Current directory
  char		temp[1024];		// Temporary pathname
  static char	newfilename[1024];	// New filename


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
    sprintf(temp, "%s/%s", cwd, filename);
  else
    strcpy(temp, filename);

//  printf("temp = \'%s\'\n", temp);
//  printf("newcwd = \'%s\'\n", newcwd);

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

  slash ++;

//  printf("slash = \'%s\'\n", slash);

  if (*newslash != '\0')
    while (*newslash != '/' && *newslash != '\\' && newslash > newcwd)
      newslash --;

//  printf("newslash = \'%s\'\n", newslash);

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


#endif // HAVE_LIBFLTK

//
// End of "$Id: gui.cxx,v 1.5 1999/11/09 22:16:41 mike Exp $".
//

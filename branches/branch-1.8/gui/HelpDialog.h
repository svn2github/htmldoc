// generated by Fast Light User Interface Designer (fluid) version 1.0008

#ifndef HelpDialog_h
#define HelpDialog_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <string.h>
#include "HelpView.h"
#include <FL/Fl_Button.H>

class HelpDialog {
  int index_;
  int max_;
  int line_[100];
  char file_[100][256];
public:
  HelpDialog();
private:
  Fl_Window *window_;
  HelpView *view_;
  inline void cb_view__i(HelpView*, void*);
  static void cb_view_(HelpView*, void*);
  inline void cb_Close_i(Fl_Button*, void*);
  static void cb_Close(Fl_Button*, void*);
  Fl_Button *back_;
  inline void cb_back__i(Fl_Button*, void*);
  static void cb_back_(Fl_Button*, void*);
  Fl_Button *forward_;
  inline void cb_forward__i(Fl_Button*, void*);
  static void cb_forward_(Fl_Button*, void*);
  Fl_Button *smaller_;
  inline void cb_smaller__i(Fl_Button*, void*);
  static void cb_smaller_(Fl_Button*, void*);
  Fl_Button *larger_;
  inline void cb_larger__i(Fl_Button*, void*);
  static void cb_larger_(Fl_Button*, void*);
public:
  int h();
  void hide();
  void load(const char *f);
  void position(int xx, int yy);
  void resize(int xx, int yy, int ww, int hh);
  void show();
  void textsize(uchar s);
  uchar textsize();
  void topline(const char *n);
  void topline(int n);
  int visible();
  int w();
  int x();
  int y();
};
#endif

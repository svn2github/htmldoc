// generated by Fast Light User Interface Designer (fluid) version 1.0100

#include "gui.h"
#include "HelpDialog.h"

inline void HelpDialog::cb_view__i(Fl_Help_View*, void*) {
  if (view_->changed())
{
  index_ ++;

  if (index_ >= 100)
  {
    memcpy(line_, line_ + 10, sizeof(line_[0]) * 90);
    memcpy(file_, file_ + 10, sizeof(file_[0]) * 90);
    index_ -= 10;
  }

  max_ = index_;

  strcpy(file_[index_], view_->filename());
  line_[index_] = view_->topline();

  if (index_ > 0)
    back_->activate();
  else
    back_->deactivate();

  forward_->deactivate();

  window_->label(view_->title());
}
else if (view_->filename())
{
  strncpy(file_[index_], view_->filename(), 255);
  file_[index_][255] = '\0';
  line_[index_] = view_->topline();
};
}
void HelpDialog::cb_view_(Fl_Help_View* o, void* v) {
  ((HelpDialog*)(o->parent()->user_data()))->cb_view__i(o,v);
}

inline void HelpDialog::cb_back__i(Fl_Button*, void*) {
  if (index_ > 0)
  index_ --;

if (index_ == 0)
  back_->deactivate();

forward_->activate();

if (strcmp(view_->filename(), file_[index_]) != 0)
  view_->load(file_[index_]);

view_->topline(line_[index_]);
}
void HelpDialog::cb_back_(Fl_Button* o, void* v) {
  ((HelpDialog*)(o->parent()->parent()->user_data()))->cb_back__i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *image_back[] = {
"32 32 33 1",
" \tc None",
".\tc #064606",
"+\tc #057005",
"@\tc #C7CBC3",
"#\tc #029202",
"$\tc #B0BAAE",
"%\tc #03BE03",
"&\tc #02E402",
"*\tc #A3A99F",
"=\tc #383736",
"-\tc #969C96",
";\tc #727A72",
">\tc #02D302",
",\tc #6A746A",
"\'\tc #166214",
")\tc #3EEC3E",
"!\tc #464745",
"~\tc #08B307",
"{\tc #33C332",
"]\tc #3ADA36",
"^\tc #545753",
"/\tc #3AEA3A",
"(\tc #085708",
"_\tc #027602",
":\tc #02CA02",
"<\tc #02A102",
"[\tc #D4E1D3",
"}\tc #02EC02",
"|\tc #36D636",
"1\tc #026702",
"2\tc #636561",
"3\tc #048104",
"4\tc #1E861E",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                         -*~    ",
"                       *${~<2   ",
"                     *${~~~1!   ",
"                   *${%%%%%+^   ",
"                 $${%%%%%%%+^   ",
"               $@|:::::::::_^   ",
"             @@]>>>>>>>>>>>_2   ",
"           @@]>>>>>>>>>>>>>_^   ",
"         @[/&&&&&&&&&&&&&&&32   ",
"       [[)&&&&&&&&&&&&&&&&&32   ",
"     [[)}}}}}}}}}}}}}}}}}}}#2   ",
"    @)}}}}}}}}}}}}}}}}}}}}}32   ",
"    -{<<%%%%%%%%%%%%%%%%%%%+^   ",
"      ;4#~:::::::::::::::::_^   ",
"        ,4#<%%%%%%%%%%%%%%%+^   ",
"          ,4#<%%%%%%%%%%%%%+^   ",
"            2+_#<<<<<<<<<<<(!   ",
"              ^\'13#########(=   ",
"                ^\'1_3333333.=   ",
"                  ^\'1+33333.=   ",
"                    ^((+333.=   ",
"                      ^((1_.=   ",
"                        !((.=   ",
"                          !.=   ",
"                                ",
"                                "
};
static Fl_Pixmap pixmap_back(image_back);

static const char *image_dback[] = {
"32 32 33 1",
" \tc None",
".\tc #242424",
"+\tc #A6A6A6",
"@\tc #424242",
"#\tc #5A5A5A",
"$\tc #767676",
"%\tc #727272",
"&\tc #B6B6B6",
"*\tc #959595",
"=\tc #666666",
"-\tc #2A2A2A",
";\tc #4E4E4E",
">\tc #D4D4D4",
",\tc #8B8B8B",
"\'\tc #2E2E2E",
")\tc #323232",
"!\tc #5E5E5E",
"~\tc #363636",
"{\tc #CBCBCB",
"]\tc #464646",
"^\tc #868686",
"/\tc #3A3A3A",
"(\tc #6A6A6A",
"_\tc #626262",
":\tc #565656",
"<\tc #AFAFAF",
"[\tc #BDBDBD",
"}\tc #525252",
"|\tc #C3C3C3",
"1\tc #DBDBDB",
"2\tc #3E3E3E",
"3\tc #7E7E7E",
"4\tc #4A4A4A",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                         *+(    ",
"                       +<$#;!   ",
"                     +<$###~;   ",
"                   +&$!!!!!~}   ",
"                 <[3_______~}   ",
"               [|^=========/#   ",
"             |{,(((((((((((2#   ",
"           [{,(((((((((((((2#   ",
"         {>,%%%%%%%%%%%%%%%@_   ",
"       >1*%%%%%%%%%%%%%%%%%@_   ",
"     11*$$$$$$$$$$$$$$$$$$$]=   ",
"    |*$$$$$$$$$$$$$$$$$$$$$@=   ",
"    *%::!__________________/:   ",
"      $}4#=================/#   ",
"        %4]}!!!!!!!!!!!!!!!~}   ",
"          %4]:!!!!!!!!!!!!!~}   ",
"            _@/];;;;;;;;;;;\']   ",
"              #2~2]44444444-2   ",
"                }/)/@@@@@@@./   ",
"                  }/)/@@@@@./   ",
"                    }~\'/222.~   ",
"                      ;~-~/.)   ",
"                        ;)-.~   ",
"                          4\'2   ",
"                                ",
"                                "
};
static Fl_Pixmap pixmap_dback(image_dback);

inline void HelpDialog::cb_forward__i(Fl_Button*, void*) {
  if (index_ < max_)
  index_ ++;

if (index_ >= max_)
  forward_->deactivate();

back_->activate();

if (strcmp(view_->filename(), file_[index_]) != 0)
  view_->load(file_[index_]);

view_->topline(line_[index_]);
}
void HelpDialog::cb_forward_(Fl_Button* o, void* v) {
  ((HelpDialog*)(o->parent()->parent()->user_data()))->cb_forward__i(o,v);
}

static const char *image_forward[] = {
"32 32 33 1",
" \tc None",
".\tc #073D07",
"+\tc #056305",
"@\tc #028602",
"#\tc #B8B8B6",
"$\tc #0AB409",
"%\tc #02E402",
"&\tc #9FA19E",
"*\tc #8FC68E",
"=\tc #787877",
"-\tc #84B481",
";\tc #719F70",
">\tc #6C6A6A",
",\tc #02D302",
"\'\tc #424241",
")\tc #648C64",
"!\tc #626260",
"~\tc #095709",
"{\tc #026A02",
"]\tc #02BE02",
"^\tc #2C3E2C",
"/\tc #029202",
"(\tc #02CA02",
"_\tc #2AE729",
":\tc #D3D2D2",
"<\tc #027E02",
"[\tc #054F05",
"}\tc #26D326",
"|\tc #22BE22",
"1\tc #04A004",
"2\tc #02EC02",
"3\tc #263326",
"4\tc #425542",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"   &$)                          ",
"   &$$$;                        ",
"   &$$$$|;                      ",
"   &]]]]]]|;                    ",
"   #]]]]]]]]|;                  ",
"   #((((((((((}-                ",
"   #,,,,,,,,,,,,}-              ",
"   #,,,,,,,,,,,,,,}-            ",
"   :%%%%%%%%%%%%%%%%_*          ",
"   :%%%%%%%%%%%%%%%%%%_*        ",
"   :22222222222222222222_*      ",
"   :2222222222222222222%2%|-    ",
"   #]]]]]]]]]]]]]]]]]]]1@++)    ",
"   #(((((((((((((((((1@{~4>     ",
"   &]]]]]]]]]]]]]]]1<+~4!       ",
"   #]]]]]]]]]]]]]1<+~4!         ",
"   &11111111111@{[.^4           ",
"   =/////////<+[.^\'             ",
"   =@@@@@@@{~[.^\'               ",
"   =@@@@@{~..^\'                 ",
"   =@<<{~..^\'                   ",
"   ><+[..3\'                     ",
"   ={..3\'                       ",
"    ~3\'                         ",
"                                ",
"                                "
};
static Fl_Pixmap pixmap_forward(image_forward);

static const char *image_dforward[] = {
"32 32 33 1",
" \tc None",
".\tc #1E1E1E",
"+\tc #767676",
"@\tc #373737",
"#\tc #424242",
"$\tc #BABABA",
"%\tc #4A4A4A",
"&\tc #A6A6A6",
"*\tc #535353",
"=\tc #6A6A6A",
"-\tc #9B9B9B",
";\tc #8C8C8C",
">\tc #5A5A5A",
",\tc #2E2E2E",
"\'\tc #666666",
")\tc #727272",
"!\tc #868686",
"~\tc #BEBEBE",
"{\tc #626262",
"]\tc #828282",
"^\tc #2A2A2A",
"/\tc #CCCCCC",
"(\tc #3E3E3E",
"_\tc #7E7E7E",
":\tc #ACACAC",
"<\tc #232323",
"[\tc #4E4E4E",
"}\tc #5E5E5E",
"|\tc #A2A2A2",
"1\tc #D4D4D4",
"2\tc #6E6E6E",
"3\tc #323232",
"4\tc #B2B2B2",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"                                ",
"   -\'+                          ",
"   |>>=]                        ",
"   |>>>>=!                      ",
"   &}}}}}}2;                    ",
"   :{{{{{}{{);                  ",
"   $\'\'\'\'\'\'\'\'\'\'+-                ",
"   ~============_-              ",
"   $==============_-            ",
"   /)))))))))))))2))!&          ",
"   /))))))))))))))))))!:        ",
"   1++++++++++++++++++++;:      ",
"   1+++++++++++++++++++)+22-    ",
"   4{{{{{{{{{{{{{{{{{{{*#3@)    ",
"   $\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'*#@@[=     ",
"   &}}}}}}}}}}}}}}}[(3,#{       ",
"   :{}}}}}}}}}}}}*(33#{         ",
"   ;*[[[[[[[[[[#@^^(*           ",
"   ]%%%%%%%%%(3<<@%             ",
"   +#######@^<<3#               ",
"   +#####@^<<3#                 ",
"   )#((@^<<3#                   ",
"   2@3<..,(                     ",
"   +@..,(                       ",
"    @,(                         ",
"                                ",
"                                "
};
static Fl_Pixmap pixmap_dforward(image_dforward);

inline void HelpDialog::cb_smaller__i(Fl_Button*, void*) {
  if (view_->textsize() > 8)
  view_->textsize(view_->textsize() - 2);

if (view_->textsize() <= 8)
  smaller_->deactivate();
larger_->activate();
}
void HelpDialog::cb_smaller_(Fl_Button* o, void* v) {
  ((HelpDialog*)(o->parent()->parent()->user_data()))->cb_smaller__i(o,v);
}

inline void HelpDialog::cb_larger__i(Fl_Button*, void*) {
  if (view_->textsize() < 18)
  view_->textsize(view_->textsize() + 2);

if (view_->textsize() >= 18)
  larger_->deactivate();
smaller_->activate();
}
void HelpDialog::cb_larger_(Fl_Button* o, void* v) {
  ((HelpDialog*)(o->parent()->parent()->user_data()))->cb_larger__i(o,v);
}

inline void HelpDialog::cb_Close_i(Fl_Menu_*, void*) {
  window_->hide();
}
void HelpDialog::cb_Close(Fl_Menu_* o, void* v) {
  ((HelpDialog*)(o->parent()->user_data()))->cb_Close_i(o,v);
}

Fl_Menu_Item HelpDialog::menu_[] = {
 {_("&File"), 0,  0, 0, 64, 0, 0, 14, 0},
 {_("&Close"), 0x40077,  (Fl_Callback*)HelpDialog::cb_Close, 0, 0, 0, 0, 14, 0},
 {0},
 {0}
};

HelpDialog::HelpDialog() {
  Fl_Window* w;
  { Fl_Window* o = window_ = new Fl_Window(535, 385, _("Help Dialog"));
    w = o;
    o->user_data((void*)(this));
    { Fl_Help_View* o = view_ = new Fl_Help_View(0, 80, 535, 305);
      o->box(FL_DOWN_BOX);
      o->callback((Fl_Callback*)cb_view_);
      Fl_Group::current()->resizable(o);
    }
    { Fl_Pack* o = new Fl_Pack(0, 25, 535, 55);
      { Fl_Button* o = back_ = new Fl_Button(0, 25, 60, 55, _("Back"));
        o->box(FL_FLAT_BOX);
        o->down_box(FL_DOWN_BOX);
        o->image(pixmap_back);
        o->deimage(pixmap_dback);
        o->callback((Fl_Callback*)cb_back_);
        o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
      }
      { Fl_Button* o = forward_ = new Fl_Button(60, 25, 60, 55, _("Forward"));
        o->box(FL_FLAT_BOX);
        o->down_box(FL_DOWN_BOX);
        o->image(pixmap_forward);
        o->deimage(pixmap_dforward);
        o->callback((Fl_Callback*)cb_forward_);
        o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
      }
      { Fl_Button* o = smaller_ = new Fl_Button(120, 25, 55, 55, _("F"));
        o->box(FL_FLAT_BOX);
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)cb_smaller_);
      }
      { Fl_Button* o = larger_ = new Fl_Button(175, 25, 55, 55, _("F"));
        o->box(FL_FLAT_BOX);
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->labelsize(40);
        o->callback((Fl_Callback*)cb_larger_);
      }
      o->type(Fl_Pack::HORIZONTAL);
      o->end();
    }
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 535, 25);
      o->menu(menu_);
    }
    o->end();
  }
  back_->deactivate();
forward_->deactivate();

index_ = -1;
max_  = 0;
}

int HelpDialog::h() {
  return (window_->h());
}

void HelpDialog::hide() {
  window_->hide();
}

void HelpDialog::load(const char *f) {
  view_->set_changed();
view_->load(f);
window_->label(view_->title());
}

void HelpDialog::position(int xx, int yy) {
  window_->position(xx, yy);
}

void HelpDialog::resize(int xx, int yy, int ww, int hh) {
  window_->resize(xx, yy, ww, hh);
}

void HelpDialog::show() {
  char *name = (char *)"HelpDialog";

window_->show(1, &name);
}

void HelpDialog::textsize(uchar s) {
  view_->textsize(s);

if (s <= 8)
  smaller_->deactivate();
else
  smaller_->activate();

if (s >= 18)
  larger_->deactivate();
else
  larger_->activate();
}

uchar HelpDialog::textsize() {
  return (view_->textsize());
}

void HelpDialog::topline(const char *n) {
  view_->topline(n);
}

void HelpDialog::topline(int n) {
  view_->topline(n);
}

int HelpDialog::visible() {
  return (window_->visible());
}

int HelpDialog::w() {
  return (window_->w());
}

int HelpDialog::x() {
  return (window_->x());
}

int HelpDialog::y() {
  return (window_->y());
}

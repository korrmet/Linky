#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include "editor.hpp"
#include "app.hpp"

editor::workspace::workspace(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h) {}

int editor::workspace::handle(int event) { return Fl_Widget::handle(event); }

void editor::workspace::draw()
{ fl_push_clip(x(), y(), w(), h());
  fl_pop_clip(); }

editor::window::window()
: Fl_Window(640, 480, "Linky v0"),
  menu_bar(0, 0, 640, 20),
  side_screen(540, 20, 100, 460),
  wsp(0, 20, 540, 460)
{ bus + IC(handler, this);
  size_range(640, 480);

  add(menu_bar);
  add(side_screen);
  add(wsp);

  resizable(&wsp);
  
  menu_bar.box(FL_BORDER_BOX);
  menu_bar.color(Fl_Color(0xFFFFFF00));

  menu_bar.add("File/Open", 0, menu_cb, (void*)"open");
  menu_bar.add("File/Save", 0, menu_cb, (void*)"save");
  menu_bar.add("File/Save As", 0, menu_cb, (void*)"save as");
  menu_bar.add("Help", 0, menu_cb, (void*)"help");

#ifdef DEBUG
  menu_bar.add("Debug/Print circuit", 0, menu_cb, (void*)"print circuit");
  menu_bar.add("Debug/Print params", 0, menu_cb, (void*)"print params");
  menu_bar.add("Debug/Print context", 0, menu_cb, (void*)"print context");
#endif

  side_screen.box(FL_BORDER_BOX);
  side_screen.color(Fl_Color(0xFFFFFF00));

  show(); }

void editor::window::menu_cb(Fl_Widget* w, void* arg) {}

void editor::window::handler(void* ctx, IM mess) {}

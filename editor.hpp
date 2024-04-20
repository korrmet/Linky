#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "app.hpp"
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>

namespace editor {

class workspace : public Fl_Widget
{ public:
  workspace(int x, int y, int w, int h);
  virtual int handle(int event) override;
  virtual void draw() override; };

class window : public Fl_Window
{ public:
  window();

  private:
  static void control_cb(Fl_Widget* w, void* arg);
  static void handler(void* ctx, IM mess);

  Fl_Menu_Bar menu_bar;
  Fl_Group side_screen;
  workspace wsp; };

}

#endif // EDITOR_HPP

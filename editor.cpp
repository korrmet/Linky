#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Button.H>
#include "editor.hpp"
#include "app.hpp"

editor::workspace::workspace(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h) {}

int editor::workspace::handle(int event)
{ if (event == FL_FOCUS) { return 1; }
  if (event == FL_KEYDOWN) { return 1; }
  if (event == FL_ENTER || event == FL_LEAVE) { return 1; }
  if (event == FL_DRAG) { return 1; }
  if (event == FL_MOVE) { return 1; }
  if (event == FL_PUSH) { return 1; }
  if (event == FL_RELEASE) { return 1; }

  // zoom action, moving the view area
  if (event == FL_MOUSEWHEEL)
  { if (!context(root / "draw pos" / "x"))
    { context[root / "draw pos" / "x"] = 0; }
    if (!context(root / "draw pos" / "y"))
    { context[root / "draw pos" / "y"] = 0; }
    if (!context(root / "grid size")) { context[root / "grid size"] = 20; }

    if (Fl::event_state() & FL_CTRL) // zoom
    { context[root / "grid size"] += Fl::event_dy() * 5;
      if ((int)context[root / "grid size"] > 100)
      { context[root / "grid size"] = 100; }
      else if ((int)context[root / "grid size"] < 5)
      { context[root / "grid size"] = 5; } }

    else if (Fl::event_state() & FL_SHIFT) // move horizontally
    { context[root / "draw pos" / "x"] += Fl::event_dy(); }

    else // move vertically
    { context[root / "draw pos" / "y"] += Fl::event_dy(); }

    redraw(); return 1; }

  return Fl_Widget::handle(event); }

void editor::workspace::draw()
{ fl_push_clip(x(), y(), w(), h());

  // background
  fl_color(0xFFFFFF00);
  fl_rectf(x(), y(), w(), h());

  // grid
  fl_color(0xCCCCFF00);
  if (!context(root / "grid size")) { context[root / "grid size"] = 20; }
  for (unsigned int i = 0; i < w(); i += (int)context[root / "grid size"])
  { fl_line(x() + i, y(), x() + i, y() + h()); }
  for (unsigned int i = 0; i < h(); i += (int)context[root / "grid size"])
  { fl_line(x(), y() + i, x() + w(), y() + i); }

  fl_pop_clip(); }

editor::window::window()
: Fl_Window(640, 480, "Linky v0"),
  menu_bar(0, 0, 640, 20),
  side_screen(440, 20, 200, 460),
  wsp(0, 20, 440, 460)
{ bus + IC(handler, this);
  size_range(640, 480);

  add(menu_bar);
  add(side_screen);
  add(wsp);

  resizable(&wsp);
  
  menu_bar.box(FL_BORDER_BOX);
  menu_bar.color(Fl_Color(0xFFFFFF00));
  menu_bar.color2(Fl_Color(0x0000B400));

  menu_bar.add("File/Open", 0, control_cb, (void*)"open");
  menu_bar.add("File/Save", 0, control_cb, (void*)"save");
  menu_bar.add("File/Save As", 0, control_cb, (void*)"save as");
  menu_bar.add("Help", 0, control_cb, (void*)"help");

#ifdef DEBUG
  menu_bar.add("Debug/Print circuit", 0, control_cb, (void*)"print circuit");
  menu_bar.add("Debug/Print params", 0, control_cb, (void*)"print params");
  menu_bar.add("Debug/Print context", 0, control_cb, (void*)"print context");
#endif

  side_screen.box(FL_BORDER_BOX);
  side_screen.color(Fl_Color(0xFFFFFF00));

  Fl_Button* btn = nullptr;
  btn = new Fl_Button(side_screen.x() + 5,
                      side_screen.y() + 5,
                      side_screen.w() - 10,
                      20,
                      "Wire");
  btn->box(FL_BORDER_BOX);
  btn->labelsize(12);
  btn->clear_visible_focus();
  btn->color(Fl_Color(0xFFFFFF00));
  btn->color2(Fl_Color(0x0000B400));
  btn->callback(control_cb, (void*)"add wire");
  side_screen.add(btn);

  // dummy group to correct resizing
  Fl_Group* dummy = new Fl_Group(side_screen.x(),
                                 side_screen.y() + side_screen.h(),
                                 side_screen.w(),
                                 0);
  dummy->box(FL_BORDER_BOX);
  side_screen.add(dummy);
  side_screen.resizable(dummy);

  show(); }

void editor::window::control_cb(Fl_Widget* w, void* arg)
{ std::string cmd((const char*)arg);

  if (cmd == "print circuit")
  { PRINT("Circuit:\n%s\n", circuit.serialize().c_str()); }

  else if (cmd == "print params")
  { PRINT("Params:\n%s\n", params.serialize().c_str()); }

  else if (cmd == "print context")
  { PRINT("Context:\n%s\n", context.serialize().c_str()); }

  else if (cmd == "open")
  { char* path = fl_file_chooser("Open file", "*.linky", nullptr, true);
    if (path) { bus(IM("file open") << IV("path", path)); } }

  else if (cmd == "save")
  { if (!params(root / "circruit file path"))
    { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
      if (path) { bus(IM("file save") << IV("path", path)); } }
    else
    { bus(IM("file save")
          << IV("path", (std::string)params[root / "circuit file path"])); } }

  else if (cmd == "save as")
  { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
    if (path) { bus(IM("file save") << IV("path", path)); } }

  else if (cmd == "help")
  { fl_message("Linky\nv0\nsimple digital circuit simulator"); }

  // TODO:
  // Wire is just set of lines. Line can be described as two points. Every
  // point can have absolute coordinates and also it can points to another
  // object. In this case, coordinates of object used as coordinates of point.
  // Point can reference to other points. If one point referenced to multiple
  // lines, filled circle should be drawn, because of it's an intersection.
  // 
  // General rules:
  // 1) When user clicks 'Wire' button, editor switches to the wire drawing
  //    mode
  // 2) In wire drawing mode user clicks on the workspace. Every click produces
  //    the point.
  // 3) User can click on empty space of the diagram or on the other objects:
  //    - unit inputs
  //    - line points
  // 4) When user clicks very first time, line would not produces, but
  //    all of the click afters produces the line
  // 5) Editor utilized point referencing feature to produce networks. Networks
  //    always contain at least one input and exactly one output.
  // 6) Editing is over, when:
  //    - user pushes [Esc] key
  //    - current point referencing to the input
  //    - current point referencing to the another line's point
  else if (cmd == "add wire") { PRINT("not implemented yet...\n"); } }

void editor::window::handler(void* ctx, IM mess) {}

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

  if (event == FL_KEYDOWN)
  { if (Fl::event_key() == 'w') { bus(IM("add wire")); }
    else if (Fl::event_key() == FL_Escape) { bus(IM("end input")); }
    return 1; }

  if (event == FL_ENTER || event == FL_LEAVE) { return 1; }
  if (event == FL_DRAG) { return 1; }
  if (event == FL_MOVE) { return 1; }
  
  if (event == FL_PUSH)
  { if (context[root/"input mode"] == "wire input")
    { // TODO: make global converter of the current coordinates
      int s = context.ls(root/"current line").size();
      int x = Fl::event_x();
      int y = Fl::event_y();
      int x0 = context[root/"draw pos"/"x"];
      int y0 = context[root/"draw pos"/"y"];
      int gs = context[root/"grid size"];
      int x_real = (x0 * gs + x) / gs;
      int y_real = (y0 * gs + y) / gs;
      context[root/"current line"/s/"x"] = x_real;
      context[root/"current line"/s/"y"] = y_real;

      if (s == 1)
      { circuit[root/"lines"];
        int idx = circuit.ls(root/"lines").size();
        circuit[root/"lines"/idx];
        context.cp(root/"current line", root/"lines"/idx, &circuit);
        context.mv(root/"current line"/1, root/"current line"/0);
        redraw(); } }
    return 1; }
  
  if (event == FL_RELEASE) { return 1; }

  // zoom action, moving the view area
  if (event == FL_MOUSEWHEEL)
  { if (Fl::event_state() & FL_CTRL) // zoom
    { context[root/"grid size"] += Fl::event_dy() * 5;
      if ((int)context[root/"grid size"] > 100)
      { context[root/"grid size"] = 100; }
      else if ((int)context[root/"grid size"] < 5)
      { context[root/"grid size"] = 5; } }

    else if (Fl::event_state() & FL_SHIFT) // move horizontally
    { context[root/"draw pos"/"x"] += Fl::event_dy(); }

    else // move vertically
    { context[root/"draw pos"/"y"] += Fl::event_dy(); }

    redraw(); return 1; }

  return Fl_Widget::handle(event); }

void editor::workspace::draw()
{ fl_push_clip(x(), y(), w(), h());

  // background
  fl_color(0xFFFFFF00);
  fl_rectf(x(), y(), w(), h());

  // grid
  fl_color(0xCCCCFF00);
  if (!context(root/"grid size")) { context[root/"grid size"] = 20; }
  for (unsigned int i = 0; i < w(); i += (int)context[root/"grid size"])
  { fl_line(x() + i, y(), x() + i, y() + h()); }
  for (unsigned int i = 0; i < h(); i += (int)context[root/"grid size"])
  { fl_line(x(), y() + i, x() + w(), y() + i); }

  // lines
  fl_color(0x00000000);
  for (std::string line : circuit.ls(root/"lines"))
  { int x0 = circuit[root/"lines"/line/0/"x"];
    x0 -= (int)context[root/"draw pos"/"x"];
    x0 *= (int)context[root/"grid size"];

    int y0 = circuit[root/"lines"/line/0/"y"];
    y0 -= (int)context[root/"draw pos"/"y"];
    y0 *= (int)context[root/"grid size"];
    
    int x1 = circuit[root/"lines"/line/1/"x"];
    x1 -= (int)context[root/"draw pos"/"x"];
    x1 *= (int)context[root/"grid size"];
    
    int y1 = circuit[root/"lines"/line/1/"y"];
    y1 -= (int)context[root/"draw pos"/"y"];
    y1 *= (int)context[root/"grid size"];
    
    fl_line(x() + x0, y() + y0, x() + x1, y() + y1); }

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

  context[root/"draw pos"/"x"] = 0;
  context[root/"draw pos"/"y"] = 0;
  context[root/"grid size"] = 20;

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
  { if (!params(root/"circruit file path"))
    { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
      if (path) { bus(IM("file save") << IV("path", path)); } }
    else
    { bus(IM("file save")
          << IV("path", (std::string)params[root/"circuit file path"])); } }

  else if (cmd == "save as")
  { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
    if (path) { bus(IM("file save") << IV("path", path)); } }

  else if (cmd == "help")
  { fl_message("Linky\nv0\nsimple digital circuit simulator"); }

  else if (cmd == "add wire") { bus(IM("add wire")); }
}

// TODO: highlight with the green (0x00AC0000) currently selected buttons
//       for the current mode

void editor::window::handler(void* ctx, IM mess)
{ if (mess == "add wire")
  { context.del(root/"current line");
    context[root/"input mode"] = "wire input"; }

  else if (mess == "end input")
  { // finalize previous operations if needed
    if (context[root/"input mode"] == "wire input")
    { context.del(root/"current line"); }

    context[root/"input mode"] = "normal"; } }

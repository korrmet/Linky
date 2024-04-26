#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Button.H>
#include <string>
#include "editor.hpp"
#include "app.hpp"

#define GREEN 0x00AC0000
#define BLUE  0x0000B400
#define LBLUE 0xCCCCFF00
#define BLACK 0x00000000
#define WHITE 0xFFFFFF00

#define CSIZE 5

static editor::window* single_window = nullptr;
static Fl_Button* wire_button = nullptr;
static Fl_Button* input_button = nullptr;
static Fl_Button* output_button = nullptr;

// TODO: add simple units
//       - const
//       - delay
//       - sum / prod (threestate)

// TODO: add wires to network conversion

editor::workspace::workspace(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h)
{ 
}

static int x_real(int x_screen)
{ int x0 = context[root/"draw pos"/"x"];
  int gs = context[root/"grid size"];
  return  (x0 * gs + x_screen) / gs; }

static int y_real(int y_screen)
{ int y0 = context[root/"draw pos"/"y"];
  int gs = context[root/"grid size"];
  return (y0 * gs + y_screen) / gs; }

static int x_screen(int x_real)
{ x_real -= (int)context[root/"draw pos"/"x"];
  x_real *= (int)context[root/"grid size"];
  return x_real; }

static int y_screen(int y_real)
{ y_real -= (int)context[root/"draw pos"/"y"];
  y_real *= (int)context[root/"grid size"];
  return y_real; }

int editor::workspace::handle(int event)
{ if (event == FL_FOCUS) { return 1; }

  if (event == FL_KEYDOWN)
  { if (Fl::event_key() == 'w') { bus(IM("add wire")); }
    else if (Fl::event_key() == 'i') { bus(IM("add input")); }
    else if (Fl::event_key() == 'o') { bus(IM("add output")); }
    else if (Fl::event_key() == FL_Escape) { bus(IM("end input")); }
    else if (Fl::event_key() == FL_Delete) { bus(IM("delete")); }
    else if (Fl::event_key() == FL_BackSpace) { bus(IM("delete")); }
    // That's pretty convenient, especially if you're aware about VIM
    else if (Fl::event_key() == 'x') { bus(IM("delete")); }
    // Keyboard cursor control
    else if (Fl::event_key() == ' ') { bus(IM("space down")); }
    else if (Fl::event_key() == FL_Up) { bus(IM("up")); }
    else if (Fl::event_key() == FL_Down) { bus(IM("down")); }
    else if (Fl::event_key() == FL_Left) { bus(IM("left")); }
    else if (Fl::event_key() == FL_Right) { bus(IM("right")); }
    else if (Fl::event_key() == FL_Control_L) { bus(IM("control down")); }
    else if (Fl::event_key() == FL_Shift_L) { bus(IM("shift down")); }
    return 1; }


  if (event == FL_KEYUP)
  { if (Fl::event_key() == ' ') { bus(IM("space up")); }
    else if (Fl::event_key() == FL_Control_L) { bus(IM("control up")); }
    else if (Fl::event_key() == FL_Shift_L) { bus(IM("shift up")); }
    return 1; }

  if (event == FL_ENTER) { single_window->cursor(FL_CURSOR_NONE); return 1; }
  if (event == FL_LEAVE) { single_window->cursor(FL_CURSOR_DEFAULT); return 1; }

  if (event == FL_DRAG)
  { if (context[root/"edit mode"] == "normal" &&
        !!context(root/"highlight"))
    { if (context[root/"highlight"/"type"] == "wire point")
      { std::string wire = context[root/"highlight"/"wire"];
        std::string point = context[root/"highlight"/"point"];
        circuit[root/"wires"/wire/point/"x"] = x_real(Fl::event_x() - x());
        circuit[root/"wires"/wire/point/"y"] = y_real(Fl::event_y() - y());
        redraw(); } 

      else if (context[root/"highlight"/"type"] == "input")
      { std::string input = context[root/"highlight"/"input"];
        circuit[root/"inputs"/input/"x"] = x_real(Fl::event_x() - x());
        circuit[root/"inputs"/input/"y"] = y_real(Fl::event_y() - y()); }

      else if (context[root/"highlight"/"type"] == "output")
      { std::string output = context[root/"highlight"/"output"];
        circuit[root/"outputs"/output/"x"] = x_real(Fl::event_x() - x());
        circuit[root/"outputs"/output/"y"] = y_real(Fl::event_y() - y()); } }
    return 1; }

  if (event == FL_MOVE)
  { int cx = context[root/"cursor pos"/"x"];
    int cy = context[root/"cursor pos"/"y"];
    int rx = x_real(Fl::event_x() - x());
    int ry = y_real(Fl::event_y() - y());
    
    if (cx != rx || cy != ry)
    { context[root/"cursor pos"/"x"] = rx;
      context[root/"cursor pos"/"y"] = ry;
      bus(IM("cursor update")); }

    return 1; }
  
  if (event == FL_PUSH)
  { if (context[root/"edit mode"] == "place wire")
    { bus(IM("place wire press")); }
    else if (context[root/"edit mode"] == "place input")
    { bus(IM("place input press")); }
    else if (context[root/"edit mode"] == "place output")
    { bus(IM("place output press")); }

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
  fl_line_style(FL_SOLID, 1);

  // background
  fl_color(WHITE);
  fl_rectf(x(), y(), w(), h());

  // grid
  fl_color(LBLUE);
  if (!context(root/"grid size")) { context[root/"grid size"] = 20; }
  for (unsigned int i = 0; i < w(); i += (int)context[root/"grid size"])
  { fl_line(x() + i, y(), x() + i, y() + h()); }
  for (unsigned int i = 0; i < h(); i += (int)context[root/"grid size"])
  { fl_line(x(), y() + i, x() + w(), y() + i); }

  // inputs
  fl_color(BLACK);
  for (std::string input : circuit.ls(root/"inputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[root/"inputs"/input/"x"];
    _y[0] = circuit[root/"inputs"/input/"y"];

    _x[1] = _x[0] + 4; _y[1] = _y[0];
    _x[2] = _x[0] + 5; _y[2] = _y[0] + 1;
    _x[3] = _x[0] + 4; _y[3] = _y[0] + 2;
    _x[4] = _x[0];     _y[4] = _y[0] + 2;
    _x[5] = _x[0];     _y[5] = _y[0];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); } }

  // outputs
  for (std::string output : circuit.ls(root/"outputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[root/"outputs"/output/"x"];
    _y[0] = circuit[root/"outputs"/output/"y"];

    _x[1] = _x[0] + 5; _y[1] = _y[0];
    _x[2] = _x[0] + 5; _y[2] = _y[0] + 2;
    _x[3] = _x[0];     _y[3] = _y[0] + 2;
    _x[4] = _x[0] + 1; _y[4] = _y[0] + 1;
    _x[5] = _x[0];     _y[5] = _y[0];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); } }

  // wires 
  fl_color(BLACK);
  for (std::string wire : circuit.ls(root/"wires"))
  { fl_line(x() + x_screen((int)circuit[root/"wires"/wire/0/"x"]),
            y() + y_screen((int)circuit[root/"wires"/wire/0/"y"]),
            x() + x_screen((int)circuit[root/"wires"/wire/1/"x"]),
            y() + y_screen((int)circuit[root/"wires"/wire/1/"y"])); }

  // joints
  fl_color(BLACK);
  struct point_data { int x; int y; int count; };
  std::list<point_data> points;
  for (std::string wire : circuit.ls(root/"wires"))
  { for (int i = 0; i < 2; i++)
    { int _x = circuit[root/"wires"/wire/i/"x"];
      int _y = circuit[root/"wires"/wire/i/"y"];
      point_data* point = nullptr;

      for (point_data& p : points)
      { if (p.x == _x && p.y == _y) { point = &p; break; } }

      if (point) { point->count++; }
      else { point_data pd = { .x = _x, .y = _y, .count = 0 };
             points.push_back(pd); } } }

  for (point_data& point : points)
  { if (point.count > 1)
    { fl_pie(x() + x_screen(point.x) - CSIZE,
             y() + y_screen(point.y) - CSIZE,
             CSIZE*2, CSIZE*2, 0.0, 360.0); } }

  // cursor
  fl_line_style(FL_SOLID, 3); fl_color(GREEN);
  fl_line(x() + x_screen((int)context[root/"cursor pos"/"x"]) - CSIZE,
          y() + y_screen((int)context[root/"cursor pos"/"y"]),
          x() + x_screen((int)context[root/"cursor pos"/"x"]) + CSIZE,
          y() + y_screen((int)context[root/"cursor pos"/"y"]));
  fl_line(x() + x_screen((int)context[root/"cursor pos"/"x"]),
          y() + y_screen((int)context[root/"cursor pos"/"y"]) - CSIZE,
          x() + x_screen((int)context[root/"cursor pos"/"x"]),
          y() + y_screen((int)context[root/"cursor pos"/"y"]) + CSIZE);


  // highlight
  if (!!context(root/"highlight"))
  { fl_line_style(FL_SOLID, 1); fl_color(BLUE);

    if (context[root/"highlight"/"type"] == "wire point")
    { std::string wire = context[root/"highlight"/"wire"];
      std::string point = context[root/"highlight"/"point"];
      int sx = x_screen((int)circuit[root/"wires"/wire/point/"x"]);
      int sy = y_screen((int)circuit[root/"wires"/wire/point/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[root/"highlight"/"type"] == "input")
    { std::string input = context[root/"highlight"/"input"];
      int sx = x_screen((int)circuit[root/"inputs"/input/"x"]);
      int sy = y_screen((int)circuit[root/"inputs"/input/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[root/"highlight"/"type"] == "output")
    { std::string output = context[root/"highlight"/"output"];
      int sx = x_screen((int)circuit[root/"outputs"/output/"x"]);
      int sy = y_screen((int)circuit[root/"outputs"/output/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); } }

  fl_line_style(0); fl_color(0); fl_pop_clip(); }

editor::window::window()
: Fl_Window(640, 480, "Linky v0"),
  menu_bar(0, 0, 640, 20),
  side_screen(440, 20, 200, 460),
  wsp(0, 20, 440, 460)
{ single_window = this;
  
  bus + IC(handler, this);
  size_range(640, 480);

  add(menu_bar);
  add(side_screen);
  add(wsp);

  resizable(&wsp);
  
  menu_bar.box(FL_BORDER_BOX);
  menu_bar.color(Fl_Color(WHITE));
  menu_bar.color2(Fl_Color(BLUE));

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
  side_screen.color(Fl_Color(WHITE));

  wire_button = new Fl_Button(side_screen.x() + 5,
                              side_screen.y() + 5,
                              side_screen.w() - 10,
                              20,
                              "Wire [W]");
  wire_button->box(FL_BORDER_BOX);
  wire_button->labelsize(12);
  wire_button->clear_visible_focus();
  wire_button->color(Fl_Color(WHITE));
  wire_button->color2(Fl_Color(BLUE));
  wire_button->callback(control_cb, (void*)"add wire");
  side_screen.add(wire_button);

  input_button = new Fl_Button(wire_button->x(),
                               wire_button->y() + wire_button->h() + 5,
                               (wire_button->w() - 5) / 2,
                               wire_button->h(),
                               "Input [I]");
  input_button->box(FL_BORDER_BOX);
  input_button->labelsize(12);
  input_button->clear_visible_focus();
  input_button->color(Fl_Color(WHITE));
  input_button->color2(Fl_Color(BLUE));
  input_button->callback(control_cb, (void*)"add input");
  side_screen.add(input_button);

  output_button = new Fl_Button(input_button->x() + input_button->w() + 5,
                                input_button->y(),
                                input_button->w(),
                                input_button->h(),
                                "Output [O]");
  output_button->box(FL_BORDER_BOX);
  output_button->labelsize(12);
  output_button->clear_visible_focus();
  output_button->color(Fl_Color(WHITE));
  output_button->color2(Fl_Color(BLUE));
  output_button->callback(control_cb, (void*)"add output");
  side_screen.add(output_button);

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
  context[root/"cursor pos"/"x"] = 0;
  context[root/"cursor pos"/"y"] = 0;
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

  else { bus(IM(cmd)); } }

void editor::window::handler(void* ctx, IM mess)
{ editor::window* that = (editor::window*)ctx;

  if (mess == "add wire")
  { bus(IM("end input"));
    wire_button->color(GREEN); wire_button->redraw();
    context.del(root/"current wire");
    context[root/"edit mode"] = "place wire"; }

  else if (mess == "add input")
  { bus(IM("end input"));
    input_button->color(GREEN); input_button->redraw();
    context[root/"edit mode"] = "place input"; }

  else if (mess == "add output")
  { bus(IM("end input"));
    output_button->color(GREEN); output_button->redraw();
    context[root/"edit mode"] = "place output"; }

  else if (mess == "end input")
  { wire_button->color(WHITE); wire_button->redraw();
    input_button->color(WHITE); input_button->redraw();
    output_button->color(WHITE); output_button->redraw();

    // finalize previous operations if needed
    if (context[root/"edit mode"] == "place wire")
    { context.del(root/"current wire"); }

    context[root/"edit mode"] = "normal"; }

  else if (mess == "delete")
  { if (!context(root/"highlight")) { return; }

    if (context[root/"highlight"/"type"] == "wire point")
    { std::string wire = context[root/"highlight"/"wire"];
      circuit.del(root/"wires"/wire);
      context.del(root/"highlight"); that->redraw(); }

    else if (context[root/"highlight"/"type"] == "input")
    { std::string input = context[root/"highlight"/"input"];
      circuit.del(root/"inputs"/input);
      context.del(root/"highlight"); that->redraw(); }

    else if (context[root/"highlight"/"type"] == "output")
    { std::string output = context[root/"highlight"/"output"];
      circuit.del(root/"outputs"/output);
      context.del(root/"highlight"); that->redraw(); } }

  else if (mess == "space down")
  { context[root/"space"] = (int)1;
    if (context[root/"edit mode"] == "place wire")
    { bus(IM("place wire press")); }
    else if (context[root/"edit mode"] == "place input")
    { bus(IM("place input press")); }
    else if (context[root/"edit mode"] == "place output")
    { bus(IM("place output press")); } }

  else if (mess == "space up")     { context[root/"space"] = (int)0; }
  else if (mess == "control down") { context[root/"control"] = (int)1; }
  else if (mess == "control up")   { context[root/"control"] = (int)0; }
  else if (mess == "shift down")   { context[root/"shift"] = (int)1; }
  else if (mess == "shift up")     { context[root/"shift"] = (int)0; }

  else if (mess == "up")
  { if ((int)context(root/"shift"))
    { context[root/"draw pos"/"y"] -= 1; that->redraw(); }
    else if ((int)context(root/"control"))
    { context[root/"grid size"] += 5;
      if ((int)context[root/"grid size"] > 100)
      { context[root/"grid size"] = 100; }
      that->redraw(); }
    else if ((int)context(root/"space"))
    { if (context[root/"edit mode"] == "normal" &&
          !!context(root/"highlight"))
      { if (context[root/"highlight"/"type"] == "wire point")
        { std::string wire = context[root/"highlight"/"wire"];
          std::string point = context[root/"highlight"/"point"];
          circuit[root/"wires"/wire/point/"y"] -= 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "input")
        { std::string input = context[root/"highlight"/"input"];
          circuit[root/"inputs"/input/"y"] -= 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "output")
        { std::string output = context[root/"highlight"/"output"];
          circuit[root/"outputs"/output/"y"] -= 1; that->redraw(); } } }
    else { context[root/"cursor pos"/"y"] -= 1; bus(IM("cursor update")); } }

  else if (mess == "down")
  { if ((int)context(root/"shift"))
    { context[root/"draw pos"/"y"] += 1; that->redraw(); }
    else if ((int)context(root/"control"))
    { context[root/"grid size"] -= 5;
      if ((int)context[root/"grid size"] < 5)
      { context[root/"grid size"] = 5; }
      that->redraw(); }
    else if ((int)context(root/"space"))
    { if (context[root/"edit mode"] == "normal" &&
          !!context(root/"highlight"))
      { if (context[root/"highlight"/"type"] == "wire point")
        { std::string wire = context[root/"highlight"/"wire"];
          std::string point = context[root/"highlight"/"point"];
          circuit[root/"wires"/wire/point/"y"] += 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "input")
        { std::string input = context[root/"highlight"/"input"];
          circuit[root/"inputs"/input/"y"] += 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "output")
        { std::string output = context[root/"highlight"/"output"];
          circuit[root/"outputs"/output/"y"] += 1; that->redraw(); } } }
    else { context[root/"cursor pos"/"y"] += 1; bus(IM("cursor update")); } }

  else if (mess == "left")
  { if ((int)context(root/"shift"))
    { context[root/"draw pos"/"x"] -= 1; that->redraw(); }
    else if ((int)context(root/"space"))
    { if (context[root/"edit mode"] == "normal" &&
          !!context(root/"highlight"))
      { if (context[root/"highlight"/"type"] == "wire point")
        { std::string wire = context[root/"highlight"/"wire"];
          std::string point = context[root/"highlight"/"point"];
          circuit[root/"wires"/wire/point/"x"] -= 1; that->redraw(); } 
      
        if (context[root/"highlight"/"type"] == "input")
        { std::string input = context[root/"highlight"/"input"];
          circuit[root/"inputs"/input/"x"] -= 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "output")
        { std::string output = context[root/"highlight"/"output"];
          circuit[root/"outputs"/output/"x"] -= 1; that->redraw(); } } }
    else { context[root/"cursor pos"/"x"] -= 1; bus(IM("cursor update")); } }

  else if (mess == "right")
  { if ((int)context(root/"shift"))
    { context[root/"draw pos"/"x"] += 1; that->redraw(); }
    else if ((int)context(root/"space"))
    { if (context[root/"edit mode"] == "normal" &&
          !!context(root/"highlight"))
      { if (context[root/"highlight"/"type"] == "wire point")
        { std::string wire = context[root/"highlight"/"wire"];
          std::string point = context[root/"highlight"/"point"];
          circuit[root/"wires"/wire/point/"x"] += 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "input")
        { std::string input = context[root/"highlight"/"input"];
          circuit[root/"inputs"/input/"x"] += 1; that->redraw(); }
      
        if (context[root/"highlight"/"type"] == "output")
        { std::string output = context[root/"highlight"/"output"];
          circuit[root/"outputs"/output/"x"] += 1; that->redraw(); } } }
    else { context[root/"cursor pos"/"x"] += 1; bus(IM("cursor update")); } }

  else if (mess == "cursor update")
  { bool highlight = false;
    int cx = context[root/"cursor pos"/"x"];
    int cy = context[root/"cursor pos"/"y"];

    if (!highlight)
    { for (std::string wire : circuit.ls(root/"wires"))
      { if (cx == (int)circuit[root/"wires"/wire/0/"x"] &&
            cy == (int)circuit[root/"wires"/wire/0/"y"])
        { context[root/"highlight"/"type"] = "wire point";
          context[root/"highlight"/"wire"] = wire;
          context[root/"highlight"/"point"] = "0";
          highlight = true; break; }

        else if (cx == (int)circuit[root/"wires"/wire/1/"x"] &&
                 cy == (int)circuit[root/"wires"/wire/1/"y"])
        { context[root/"highlight"/"type"] = "wire point";
          context[root/"highlight"/"wire"] = wire;
          context[root/"highlight"/"point"] = "1";
          highlight = true; break; } } }

    if (!highlight)
    { for (std::string input : circuit.ls(root/"inputs"))
      { if (cx == (int)circuit[root/"inputs"/input/"x"] &&
            cy == (int)circuit[root/"inputs"/input/"y"])
        { context[root/"highlight"/"type"] = "input";
          context[root/"highlight"/"input"] = input;
          highlight = true; break; } } }

    if (!highlight)
    { for (std::string output : circuit.ls(root/"outputs"))
      { if (cx == (int)circuit[root/"outputs"/output/"x"] &&
            cy == (int)circuit[root/"outputs"/output/"y"])
        { context[root/"highlight"/"type"] = "output";
          context[root/"highlight"/"output"] = output;
          highlight = true; break; } } }

    if (!highlight) { context.del(root/"highlight"); } that->redraw(); }

  else if (mess == "place wire press")
  { int s = context.ls(root/"current wire").size();
    context[root/"current wire"/s/"x"] = context[root/"cursor pos"/"x"];
    context[root/"current wire"/s/"y"] = context[root/"cursor pos"/"y"];

    if (s == 1)
    { circuit[root/"wires"];
      std::list<std::string> wires = circuit.ls(root/"wires");
      int idx = wires.size() ? std::stoi(wires.back()) + 1 : 0;
      circuit[root/"wires"/idx];
      context.cp(root/"current wire", root/"wires"/idx, &circuit);
      context.mv(root/"current wire"/1, root/"current wire"/0);
      that->redraw(); } }

  else if (mess == "place input press")
  { circuit[root/"inputs"];
    std::list<std::string> inputs = circuit.ls(root/"inputs");
    int idx = inputs.size() ? std::stoi(inputs.back()) + 1 : 0;
    circuit[root/"inputs"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"inputs"/idx/"y"] = context[root/"cursor pos"/"y"];
    that->redraw(); }

  else if (mess == "place output press")
  { circuit[root/"outputs"];
    std::list<std::string> outputs = circuit.ls(root/"outputs");
    int idx = outputs.size() ? std::stoi(outputs.back()) + 1 : 0;
    circuit[root/"outputs"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"outputs"/idx/"y"] = context[root/"cursor pos"/"y"];
    that->redraw(); }
}

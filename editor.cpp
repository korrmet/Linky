#include <FL/Enumerations.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Box.H>
#include <string>
#include "editor.hpp"
#include "simulator.hpp"
#include "app.hpp"

#define GREEN 0x00AC0000
#define BLUE  0x0000B400
#define LBLUE 0xEEEEFF00
#define RED   0xFF000000
#define BLACK 0x00000000
#define WHITE 0xFFFFFF00

#define CSIZE 5

// ---> edit input window
class inputEditWindow : public Fl_Window
{ public:
  inputEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Name of the input"),
    name_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    input_name(name)
  { save_btn.callback(save_cb, (void*)this);
    name_input.value(((std::string)
                      circuit[ROOT/"inputs"/name/"name"]).c_str());
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { inputEditWindow* that = (inputEditWindow*)arg;
    circuit[ROOT/"inputs"/that->input_name/"name"] = that->name_input.value();
    bus(IM("screen update")); that->hide(); }
  
  Fl_Input    name_input;
  Fl_Box      label;
  Fl_Button   save_btn;
  std::string input_name; };
// <---

// ---> edit output window
class outputEditWindow : public Fl_Window
{ public:
  outputEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Name of the output"),
    name_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    output_name(name)
  { save_btn.callback(save_cb, (void*)this);
    name_input.value(((std::string)
                      circuit[ROOT/"outputs"/name/"name"]).c_str());
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { outputEditWindow* that = (outputEditWindow*)arg;
    circuit[ROOT/"outputs"/that->output_name/"name"] = that->name_input.value();
    bus(IM("screen update")); that->hide(); }
  
  Fl_Input    name_input;
  Fl_Box      label;
  Fl_Button   save_btn;
  std::string output_name; };
// <---

// ---> edit constant window
class constEditWindow : public Fl_Window
{ public:
  constEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Value of the constant"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.value((float)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { constEditWindow* that = (constEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (float)that->value_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> edit delay window
class delayEditWindow : public Fl_Window
{ public:
  delayEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Value of the delay"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.step(1);
    value_input.value((int)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { delayEditWindow* that = (delayEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (int)that->value_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> edit function window
class functionEditWindow : public Fl_Window
{ public:
  functionEditWindow(std::string name)
  : Fl_Window(300, 165),
    function_name_label(5, 5, 290, 20, "Name of the function"),
    function_name_input(5, 25, 290, 20),
    numerator_label(5, 50, 290, 20, "Numerator coeffs separated by \";\""),
    numerator_input(5, 70, 290, 20),
    denominator_label(5, 95, 290, 20, "Denominator coeffs separated by \";\""),
    denominator_input(5, 115, 290, 20),
    save_btn((300 / 2) - (60 / 2), 140, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    function_name_input.value(((std::string)
                               circuit[ROOT/"units"/name/"function name"]
                              ).c_str());
    numerator_input.value(((std::string)
                           circuit[ROOT/"units"/name/"numerator poly"]
                          ).c_str());
    denominator_input.value(((std::string)
                             circuit[ROOT/"units"/name/"denominator poly"]
                            ).c_str());
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { functionEditWindow* that = (functionEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"function name"]
      = that->function_name_input.value();
    circuit[ROOT/"units"/that->unit_name/"numerator poly"]
      = that->numerator_input.value();
    circuit[ROOT/"units"/that->unit_name/"denominator poly"]
      = that->denominator_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Box      function_name_label;
  Fl_Input    function_name_input;
  Fl_Box      numerator_label;
  Fl_Input    numerator_input;
  Fl_Box      denominator_label;
  Fl_Input    denominator_input;
  Fl_Button   save_btn;
  std::string unit_name; };
// <---

// ---> edit loopback window
class loopbackEditWindow : public Fl_Window
{ public:
  loopbackEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Value of the loopback delay"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.step(1);
    value_input.value((int)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { loopbackEditWindow* that = (loopbackEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (int)that->value_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> coeff edit window
class coeffEditWindow : public Fl_Window
{ public:
  coeffEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Value of the coefficient"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.value((float)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { coeffEditWindow* that = (coeffEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (float)that->value_input.value();
    bus(IM("screen update")); that->hide(); }
  
  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> limit max edit window
class limitMaxEditWindow : public Fl_Window
{ public:
  limitMaxEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Maximum value"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.value((float)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { limitMaxEditWindow* that = (limitMaxEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (float)that->value_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> limit min edit window
class limitMinEditWindow : public Fl_Window
{ public:
  limitMinEditWindow(std::string name)
  : Fl_Window(300, 80),
    label(5, 5, 290, 20, "Minimum value"),
    value_input(5, 25, 290, 20),
    save_btn((300 / 2) - (60 / 2), 50, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    value_input.value((float)circuit[ROOT/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { limitMinEditWindow* that = (limitMinEditWindow*)arg;
    circuit[ROOT/"units"/that->unit_name/"value"]
      = (float)that->value_input.value();
    bus(IM("screen update")); that->hide(); }

  Fl_Value_Input value_input;
  Fl_Box         label;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

// ---> GUI objects
static editor::window* single_window   = nullptr;
static Fl_Button* simulate_button      = nullptr;
static Fl_Button* generate_button      = nullptr;
static Fl_Button* check_circuit_button = nullptr;
static Fl_Button* clear_errors_button  = nullptr;
static Fl_Button* edit_button          = nullptr;
static Fl_Button* rotate_button        = nullptr;
static Fl_Button* external_button      = nullptr;
static Fl_Button* wire_button          = nullptr;
static Fl_Button* input_button         = nullptr;
static Fl_Button* output_button        = nullptr;
static Fl_Button* const_button         = nullptr;
static Fl_Button* delay_button         = nullptr;
static Fl_Button* sum_button           = nullptr;
static Fl_Button* diff_button          = nullptr;
static Fl_Button* prod_button          = nullptr;
static Fl_Button* div_button           = nullptr;
static Fl_Button* function_button      = nullptr;
static Fl_Button* loopback_button      = nullptr;
static Fl_Button* coeff_button         = nullptr;
static Fl_Button* abs_button           = nullptr;
static Fl_Button* limit_max_button     = nullptr;
static Fl_Button* limit_min_button     = nullptr;
static Fl_Button* more_button          = nullptr;
static Fl_Button* less_button          = nullptr;
static Fl_Button* eq_button            = nullptr;
static Fl_Button* not_button           = nullptr;
// <---

editor::workspace::workspace(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h)
{ }

static int x_real(int x_screen)
{ int x0 = context[ROOT/"draw pos"/"x"];
  int gs = context[ROOT/"grid size"];
  return  (x0 * gs + x_screen) / gs; }

static int y_real(int y_screen)
{ int y0 = context[ROOT/"draw pos"/"y"];
  int gs = context[ROOT/"grid size"];
  return (y0 * gs + y_screen) / gs; }

static int x_screen(int x_real)
{ x_real -= (int)context[ROOT/"draw pos"/"x"];
  x_real *= (int)context[ROOT/"grid size"];
  return x_real; }

static int y_screen(int y_real)
{ y_real -= (int)context[ROOT/"draw pos"/"y"];
  y_real *= (int)context[ROOT/"grid size"];
  return y_real; }

// ---> event handler
int editor::workspace::handle(int event)
{ if (event == FL_FOCUS) { return 1; }

  if (event == FL_KEYDOWN)
  { if      (Fl::event_key() == 'e')
    { if (context[ROOT/"shift"] == 0) { bus(IM("edit items")); }
      if (context[ROOT/"shift"] == 1) { bus(IM("add external")); } }
    else if (Fl::event_key() == 'r') { bus(IM("rotate mode")); }
    else if (Fl::event_key() == 'w') { bus(IM("add wire")); }
    else if (Fl::event_key() == 'i') { bus(IM("add input")); }
    else if (Fl::event_key() == 'o') { bus(IM("add output")); }
    else if (Fl::event_key() == 'c') { bus(IM("add const")); }
    else if (Fl::event_key() == 'z') { bus(IM("add delay")); }
    else if (Fl::event_key() == 's')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add diff")); }
      if (context[ROOT/"shift"] == 0) { bus(IM("add sum"));  } }
    else if (Fl::event_key() == 'p')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add div")); }
      if (context[ROOT/"shift"] == 0) { bus(IM("add prod")); } }
    else if (Fl::event_key() == 'f') { bus(IM("add function")); }
    else if (Fl::event_key() == 'l') { bus(IM("add loopback")); }
    else if (Fl::event_key() == 'k') { bus(IM("add coefficient")); }
    else if (Fl::event_key() == 'a') { bus(IM("add abs")); }
    else if (Fl::event_key() == 'm')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add min limit")); }
      if (context[ROOT/"shift"] == 0) { bus(IM("add max limit")); } }
    else if (Fl::event_key() == '.')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add more unit")); } }
    else if (Fl::event_key() == ',')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add less unit")); } }
    else if (Fl::event_key() == '=') { bus(IM("add eq unit")); }
    else if (Fl::event_key() == '1')
    { if (context[ROOT/"shift"] == 1) { bus(IM("add not unit")); } }
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
  { if      (Fl::event_key() == ' ') { bus(IM("space up")); }
    else if (Fl::event_key() == FL_Control_L) { bus(IM("control up")); }
    else if (Fl::event_key() == FL_Shift_L) { bus(IM("shift up")); }
    return 1; }

  if (event == FL_ENTER) { single_window->cursor(FL_CURSOR_NONE); return 1; }
  if (event == FL_LEAVE) { single_window->cursor(FL_CURSOR_DEFAULT); return 1; }

  if (event == FL_DRAG)
  { if (context[ROOT/"edit mode"] == "normal" &&
        !!context(ROOT/"highlight"))
    { if (context[ROOT/"highlight"/"type"] == "wire point")
      { std::string wire = context[ROOT/"highlight"/"wire"];
        std::string point = context[ROOT/"highlight"/"point"];
        circuit[ROOT/"wires"/wire/point/"x"] = x_real(Fl::event_x() - x());
        circuit[ROOT/"wires"/wire/point/"y"] = y_real(Fl::event_y() - y());
        redraw(); } 

      else if (context[ROOT/"highlight"/"type"] == "input")
      { std::string input = context[ROOT/"highlight"/"input"];
        circuit[ROOT/"inputs"/input/"x"] = x_real(Fl::event_x() - x());
        circuit[ROOT/"inputs"/input/"y"] = y_real(Fl::event_y() - y()); }

      else if (context[ROOT/"highlight"/"type"] == "output")
      { std::string output = context[ROOT/"highlight"/"output"];
        circuit[ROOT/"outputs"/output/"x"] = x_real(Fl::event_x() - x());
        circuit[ROOT/"outputs"/output/"y"] = y_real(Fl::event_y() - y()); } }
    return 1; }

  if (event == FL_MOVE)
  { int cx = context[ROOT/"cursor pos"/"x"];
    int cy = context[ROOT/"cursor pos"/"y"];
    int rx = x_real(Fl::event_x() - x());
    int ry = y_real(Fl::event_y() - y());
    
    if (cx != rx || cy != ry)
    { context[ROOT/"cursor pos"/"x"] = rx;
      context[ROOT/"cursor pos"/"y"] = ry;
      bus(IM("cursor update")); }

    return 1; }
  
  if (event == FL_PUSH) { bus(IM("click")); return 1; }
  
  if (event == FL_RELEASE) { return 1; }

  // zoom action, moving the view area
  if (event == FL_MOUSEWHEEL)
  { if (Fl::event_state() & FL_CTRL) // zoom
    { context[ROOT/"grid size"] += Fl::event_dy() * 5;
      if ((int)context[ROOT/"grid size"] > 100)
      { context[ROOT/"grid size"] = 100; }
      else if ((int)context[ROOT/"grid size"] < 5)
      { context[ROOT/"grid size"] = 5; } }

    else if (Fl::event_state() & FL_SHIFT) // move horizontally
    { context[ROOT/"draw pos"/"x"] += Fl::event_dy(); }

    else // move vertically
    { context[ROOT/"draw pos"/"y"] += Fl::event_dy(); }

    redraw(); return 1; }

  return Fl_Widget::handle(event); }
// <---

// ---> draw
void editor::workspace::draw()
{ fl_push_clip(x(), y(), w(), h());
  fl_line_style(FL_SOLID, 1);

  // ---> background
  fl_color(WHITE);
  fl_rectf(x(), y(), w(), h());
  // <---

  // ---> grid
  fl_color(LBLUE); fl_line_style(FL_SOLID, 1);
  if (!context(ROOT/"grid size")) { context[ROOT/"grid size"] = 20; }
  for (unsigned int i = 0; i < w(); i += (int)context[ROOT/"grid size"])
  { fl_line(x() + i, y(), x() + i, y() + h()); }
  for (unsigned int i = 0; i < h(); i += (int)context[ROOT/"grid size"])
  { fl_line(x(), y() + i, x() + w(), y() + i); }
  // <---

  // ---> inputs
  fl_color(BLACK); fl_line_style(FL_SOLID, 1);
  for (std::string input : circuit.ls(ROOT/"inputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[ROOT/"inputs"/input/"x"];
    _y[0] = circuit[ROOT/"inputs"/input/"y"];

    fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
    int pixel_w = 
      fl_width(((std::string)circuit[ROOT/"inputs"/input/"name"]).c_str());
    int name_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

    _x[1] = _x[0] + name_w;     _y[1] = _y[0];
    _x[2] = _x[0] + name_w + 1; _y[2] = _y[0] + 1;
    _x[3] = _x[0] + name_w;     _y[3] = _y[0] + 2;
    _x[4] = _x[0];              _y[4] = _y[0] + 2;
    _x[5] = _x[0];              _y[5] = _y[0];

    circuit[ROOT/"inputs"/input/"point"/"x"] = _x[2];
    circuit[ROOT/"inputs"/input/"point"/"y"] = _y[2];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

    fl_draw(((std::string)circuit[ROOT/"inputs"/input/"name"]).c_str(),
            x() + x_screen(_x[0]), y() + y_screen(_y[0]),
            pixel_w, 2 * (int)context[ROOT/"grid size"],
            FL_ALIGN_LEFT); }
  // <---

  // ---> outputs
  for (std::string output : circuit.ls(ROOT/"outputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[ROOT/"outputs"/output/"x"];
    _y[0] = circuit[ROOT/"outputs"/output/"y"];

    fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
    int pixel_w =
      fl_width(((std::string)circuit[ROOT/"outputs"/output/"name"]).c_str());
    int name_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

    _x[1] = _x[0] + 1 + name_w; _y[1] = _y[0];
    _x[2] = _x[0] + 1 + name_w; _y[2] = _y[0] + 2;
    _x[3] = _x[0];              _y[3] = _y[0] + 2;
    _x[4] = _x[0] + 1;          _y[4] = _y[0] + 1;
    _x[5] = _x[0];              _y[5] = _y[0];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

    circuit[ROOT/"outputs"/output/"point"/"x"] = _x[4];
    circuit[ROOT/"outputs"/output/"point"/"y"] = _y[4];
              
    fl_draw(((std::string)circuit[ROOT/"outputs"/output/"name"]).c_str(),
            x() + x_screen(_x[0] + 1), y() + y_screen(_y[0]),
            pixel_w, 2 * (int)context[ROOT/"grid size"],
            FL_ALIGN_LEFT); }
  // <---

  // ---> units
  for (std::string unit : circuit.ls(ROOT/"units"))
  { 
    // ---> constant
    if      (circuit[ROOT/"units"/unit/"type"] == "constant")
    { fl_color(BLACK);
      float val = circuit[ROOT/"units"/unit/"value"];
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int pixel_w = fl_width(std::to_string(val).c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = circuit[ROOT/"units"/unit/"x"];
      _y[0] = circuit[ROOT/"units"/unit/"y"];
      _x[1] = _x[0] + label_w; _y[1] = _y[0];
      _x[2] = _x[0] + label_w; _y[2] = _y[0] + 2;
      _x[3] = _x[0];           _y[3] = _y[0] + 2;
      _x[4] = _x[0];           _y[4] = _y[0];

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x[0] + label_w;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y[0] + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x[0];
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y[0] + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(std::to_string(val).c_str(),
              x() + x_screen(_x[0]), y() + y_screen(_y[0]),
              pixel_w, 2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_LEFT); }
    // <---
    
    // ---> delay
    else if (circuit[ROOT/"units"/unit/"type"] == "delay")
    { fl_color(BLACK);
      int val = circuit[ROOT/"units"/unit/"value"];
      std::string label = std::string("z-").append(std::to_string(val));
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;
      
      int _x[7] = { 0 }; int _y[7] = { 0 };
      _x[0] = circuit[ROOT/"units"/unit/"x"];
      _y[0] = circuit[ROOT/"units"/unit/"y"];
      _x[1] = _x[0] + label_w + 2; _y[1] = _y[0];
      _x[2] = _x[0] + label_w + 3; _y[2] = _y[0] + 1;
      _x[3] = _x[0] + label_w + 2; _y[3] = _y[0] + 2;
      _x[4] = _x[0];               _y[4] = _y[0] + 2;
      _x[5] = _x[0] + 1;           _y[5] = _y[0] + 1;
      _x[6] = _x[0];               _y[6] = _y[0];

      circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x[0] + 1;
      circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y[0] + 1;

      circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x[0] + label_w + 3;
      circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y[0] + 1;

      for (unsigned int i = 0; i < 6; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
              x() + x_screen(_x[0] + 2), y() + y_screen(_y[0]),
              pixel_w, 2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_LEFT); }
    // <---
    
    // ---> sum
    else if (circuit[ROOT/"units"/unit/"type"] == "sum")
    { fl_color(BLACK);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"]);
      fl_line(x() + x_screen(_x + 1),
              y() + y_screen(_y    ) + (int)context[ROOT/"grid size"] / 2,
              x() + x_screen(_x + 1),
              y() + y_screen(_y + 1) + (int)context[ROOT/"grid size"] / 2);
      fl_line(x() + x_screen(_x    ) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1),
              x() + x_screen(_x + 1) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1)); }
    // <---
    
    // ---> difference
    else if (circuit[ROOT/"units"/unit/"type"] == "difference")
    { fl_color(BLACK);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"]);
      fl_line(x() + x_screen(_x    ) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1),
              x() + x_screen(_x + 1) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1)); }
    // <---
    
    // ---> product
    else if (circuit[ROOT/"units"/unit/"type"] == "product")
    { fl_color(BLACK);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"]);
      fl_line(x() + x_screen(_x + 0) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 0) + (int)context[ROOT/"grid size"] / 2,
              x() + x_screen(_x + 1) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1) + (int)context[ROOT/"grid size"] / 2);
      fl_line(x() + x_screen(_x + 1) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 0) + (int)context[ROOT/"grid size"] / 2,
              x() + x_screen(_x + 0) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1) + (int)context[ROOT/"grid size"] / 2); }
    // <---
    
    // ---> division
    else if (circuit[ROOT/"units"/unit/"type"] == "division")
    { fl_color(BLACK);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"]);
      fl_line(x() + x_screen(_x + 1) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 0) + (int)context[ROOT/"grid size"] / 2,
              x() + x_screen(_x + 0) + (int)context[ROOT/"grid size"] / 2,
              y() + y_screen(_y + 1) + (int)context[ROOT/"grid size"] / 2); }
    // <---
    
    // ---> function
    else if (circuit[ROOT/"units"/unit/"type"] == "function")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x0 = circuit[ROOT/"units"/unit/"x"];
      int _y0 = circuit[ROOT/"units"/unit/"y"];
      std::string label
        = (std::string)circuit[ROOT/"units"/unit/"function name"];
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;
        
        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[ROOT/"grid size"],
             FL_ALIGN_LEFT); }
    // <---
  
    // ---> loopback
    else if (circuit[ROOT/"units"/unit/"type"] == "loopback")
    { fl_color(BLACK);
      int val = circuit[ROOT/"units"/unit/"value"];
      std::string label = std::string("z-").append(std::to_string(val));
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[7] = { 0 }; int _y[7] = { 0 };
      _x[0] = circuit[ROOT/"units"/unit/"x"];
      _y[0] = circuit[ROOT/"units"/unit/"y"];
      _x[1] = _x[0] + label_w + 2; _y[1] = _y[0];
      _x[2] = _x[0] + label_w + 1; _y[2] = _y[0] + 1;
      _x[3] = _x[0] + label_w + 2; _y[3] = _y[0] + 2;
      _x[4] = _x[0];               _y[4] = _y[0] + 2;
      _x[5] = _x[0] - 1;           _y[5] = _y[0] + 1;
      _x[6] = _x[0];               _y[6] = _y[0]; 

      circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x[0] + label_w + 1;
      circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y[0] + 1;

      circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x[0] - 1;
      circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y[0] + 1;

      for (unsigned int i = 0; i < 6; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
              x() + x_screen(_x[0] + 1), y() + y_screen(_y[0]),
              pixel_w, 2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_LEFT); }
    // <---
    
    // ---> coeff
    else if (circuit[ROOT/"units"/unit/"type"] == "coeff")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x0 = circuit[ROOT/"units"/unit/"x"];
      int _y0 = circuit[ROOT/"units"/unit/"y"];

      float val = circuit[ROOT/"units"/unit/"value"];
      std::string label;
      label.append("x").append(std::to_string(val).c_str());
      
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[ROOT/"grid size"],
             FL_ALIGN_LEFT); }
    // <---
    
    // ---> abs
    else if (circuit[ROOT/"units"/unit/"type"] == "abs")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x0 = circuit[ROOT/"units"/unit/"x"];
      int _y0 = circuit[ROOT/"units"/unit/"y"];
      std::string label("abs");
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[ROOT/"grid size"],
             FL_ALIGN_LEFT); }
    // <---
    
    // ---> limit max
    else if (circuit[ROOT/"units"/unit/"type"] == "limit max")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x0 = circuit[ROOT/"units"/unit/"x"];
      int _y0 = circuit[ROOT/"units"/unit/"y"];

      float val = circuit[ROOT/"units"/unit/"value"];
      std::string label;
      label.append("max:").append(std::to_string(val).c_str());

      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;
        
        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[ROOT/"grid size"],
             FL_ALIGN_LEFT); }
    // <---
    
    // ---> limit min
    else if (circuit[ROOT/"units"/unit/"type"] == "limit min")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x0 = circuit[ROOT/"units"/unit/"x"];
      int _y0 = circuit[ROOT/"units"/unit/"y"];

      float val = circuit[ROOT/"units"/unit/"value"];
      std::string label;
      label.append("min:").append(std::to_string(val).c_str());

      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[ROOT/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x0 + label_w + 1;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y0 + 1;
        
        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x0 - 1;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y0 + 1; }

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[ROOT/"grid size"],
             FL_ALIGN_LEFT); }
    // <---

    // ---> more
    else if (circuit[ROOT/"units"/unit/"type"] == "more")
    { fl_color(BLACK);
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
             2 * (int)context[ROOT/"grid size"],
             2 * (int)context[ROOT/"grid size"]);
      fl_draw(">", x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_CENTER); }
    // <---
    
    // ---> less
    else if (circuit[ROOT/"units"/unit/"type"] == "less")
    { fl_color(BLACK);
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
             2 * (int)context[ROOT/"grid size"],
             2 * (int)context[ROOT/"grid size"]);
      fl_draw("<", x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_CENTER); }
    // <---
    
    // ---> eq
    else if (circuit[ROOT/"units"/unit/"type"] == "eq")
    { fl_color(BLACK);
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;
      
      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 4;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y + 2;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 2)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;
        
        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 3)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"inputs"/1/"x"] = _x + 1;
        circuit[ROOT/"units"/unit/"inputs"/1/"y"] = _y;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
             2 * (int)context[ROOT/"grid size"],
             2 * (int)context[ROOT/"grid size"]);
      fl_draw("=", x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_CENTER); }
    // <---
    
    // ---> not
    else if (circuit[ROOT/"units"/unit/"type"] == "not")
    { fl_color(BLACK);
      fl_font(FL_COURIER, 2 * (int)context[ROOT/"grid size"]);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];

      int rotate_pos = 0;

      if (!!circuit(ROOT/"units"/unit/"rotate pos"))
      { rotate_pos = (int)circuit[ROOT/"units"/unit/"rotate pos"];
        rotate_pos %= 2;
        circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos; }

      if (rotate_pos == 0)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }
      else if (rotate_pos == 1)
      { circuit[ROOT/"units"/unit/"inputs"/0/"x"] = _x + 2;
        circuit[ROOT/"units"/unit/"inputs"/0/"y"] = _y + 1;

        circuit[ROOT/"units"/unit/"outputs"/0/"x"] = _x;
        circuit[ROOT/"units"/unit/"outputs"/0/"y"] = _y + 1; }

      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
             2 * (int)context[ROOT/"grid size"],
             2 * (int)context[ROOT/"grid size"]);
      fl_draw("!", x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[ROOT/"grid size"],
              2 * (int)context[ROOT/"grid size"],
              FL_ALIGN_CENTER); }
    // <---
  }
  // <---

  // ---> wires 
  fl_color(BLACK);
  for (std::string wire : circuit.ls(ROOT/"wires"))
  { fl_line(x() + x_screen((int)circuit[ROOT/"wires"/wire/0/"x"]),
            y() + y_screen((int)circuit[ROOT/"wires"/wire/0/"y"]),
            x() + x_screen((int)circuit[ROOT/"wires"/wire/1/"x"]),
            y() + y_screen((int)circuit[ROOT/"wires"/wire/1/"y"])); }
  // <---

  // ---> joints
  fl_line_style(FL_SOLID, 1); fl_color(BLACK);
  struct point_data { int x; int y; int count; };
  std::list<point_data> points;
  for (std::string wire : circuit.ls(ROOT/"wires"))
  { for (int i = 0; i < 2; i++)
    { int _x = circuit[ROOT/"wires"/wire/i/"x"];
      int _y = circuit[ROOT/"wires"/wire/i/"y"];
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
  // <---

  // ---> highlight
  if (!!context(ROOT/"highlight"))
  { fl_line_style(FL_SOLID, 1); fl_color(BLUE);

    if (context[ROOT/"highlight"/"type"] == "wire point")
    { std::string wire = context[ROOT/"highlight"/"wire"];
      std::string point = context[ROOT/"highlight"/"point"];
      int sx = x_screen((int)circuit[ROOT/"wires"/wire/point/"x"]);
      int sy = y_screen((int)circuit[ROOT/"wires"/wire/point/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[ROOT/"highlight"/"type"] == "input")
    { std::string input = context[ROOT/"highlight"/"input"];
      int sx = x_screen((int)circuit[ROOT/"inputs"/input/"x"]);
      int sy = y_screen((int)circuit[ROOT/"inputs"/input/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[ROOT/"highlight"/"type"] == "output")
    { std::string output = context[ROOT/"highlight"/"output"];
      int sx = x_screen((int)circuit[ROOT/"outputs"/output/"x"]);
      int sy = y_screen((int)circuit[ROOT/"outputs"/output/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[ROOT/"highlight"/"type"] == "unit")
    { std::string unit = context[ROOT/"highlight"/"unit"];
      int sx = x_screen((int)circuit[ROOT/"units"/unit/"x"]);
      int sy = y_screen((int)circuit[ROOT/"units"/unit/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); } }

  // units inputs and outputs (only when editing or highlighting wires)
  // inputs are green circles, outputs are blue circuits
  if (context[ROOT/"edit mode"]        == "place wire" ||
      context(ROOT/"highlight"/"type") == "wire point")
  { // ---> wire markers
    fl_line_style(FL_SOLID, 1); fl_color(BLUE);
    for (std::string wire : circuit.ls(ROOT/"wires"))
    { int _x0 = circuit[ROOT/"wires"/wire/0/"x"];
      int _y0 = circuit[ROOT/"wires"/wire/0/"y"];
      fl_rect(x() + x_screen(_x0) - CSIZE, y() + y_screen(_y0) - CSIZE,
              CSIZE*2, CSIZE*2);
      int _x1 = circuit[ROOT/"wires"/wire/1/"x"];
      int _y1 = circuit[ROOT/"wires"/wire/1/"y"];
      fl_rect(x() + x_screen(_x1) - CSIZE, y() + y_screen(_y1) - CSIZE,
              CSIZE*2, CSIZE*2); }
    // <---
    
    // ---> units inputs and outputs
    for (std::string unit : circuit.ls(ROOT/"units"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      for (std::string input : circuit.ls(ROOT/"units"/unit/"inputs"))
      { int _x = circuit[ROOT/"units"/unit/"inputs"/input/"x"];
        int _y = circuit[ROOT/"units"/unit/"inputs"/input/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); }

      fl_line_style(FL_SOLID, 1); fl_color(BLUE);
      for (std::string output : circuit.ls(ROOT/"units"/unit/"outputs"))
      { int _x = circuit[ROOT/"units"/unit/"outputs"/output/"x"];
        int _y = circuit[ROOT/"units"/unit/"outputs"/output/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); } }
    // <---

    // ---> circuit inputs
    // input can contain only output (respectively to scheme point of view)
    fl_line_style(FL_SOLID, 1); fl_color(BLUE);
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { if (!circuit(ROOT/"inputs"/input/"point")) { continue; }
      int _x = circuit[ROOT/"inputs"/input/"point"/"x"];
      int _y = circuit[ROOT/"inputs"/input/"point"/"y"];
      fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
             CSIZE*2, CSIZE*2, 0.0, 360.0); }
    // <---

    // ---> circuit outputs
    // output can contain only input (respectively to scheme point of view)
    fl_line_style(FL_SOLID, 1); fl_color(GREEN);
    for (std::string output : circuit.ls(ROOT/"outputs"))
    { if (!circuit(ROOT/"outputs"/output/"point")) { continue; }
      int _x = circuit[ROOT/"outputs"/output/"point"/"x"];
      int _y = circuit[ROOT/"outputs"/output/"point"/"y"];
      fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
             CSIZE*2, CSIZE*2, 0.0, 360.0); }
    // <---
  }

  // ---> rotation highlight
  if (context[ROOT/"edit mode"] == "rotate")
  { for (std::string unit : circuit.ls(ROOT/"units"))
    { // ---> inputs
      fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      for (std::string input : circuit.ls(ROOT/"units"/unit/"inputs"))
      { int _x = circuit[ROOT/"units"/unit/"inputs"/input/"x"];
        int _y = circuit[ROOT/"units"/unit/"inputs"/input/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); }
      // <---
      
      // ---> outputs
      fl_line_style(FL_SOLID, 1); fl_color(BLUE);
      for (std::string output : circuit.ls(ROOT/"units"/unit/"outputs"))
      { int _x = circuit[ROOT/"units"/unit/"outputs"/output/"x"];
        int _y = circuit[ROOT/"units"/unit/"outputs"/output/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); }
      // <---
    }
  }
  // <---

  // editable items
  if (context[ROOT/"edit mode"] == "edit properties")
  { for (std::string unit : circuit.ls(ROOT/"units"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[ROOT/"units"/unit/"x"];
      int _y = circuit[ROOT/"units"/unit/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); }

    for (std::string input : circuit.ls(ROOT/"inputs"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[ROOT/"inputs"/input/"x"];
      int _y = circuit[ROOT/"inputs"/input/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); }

    for (std::string output : circuit.ls(ROOT/"outputs"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[ROOT/"outputs"/output/"x"];
      int _y = circuit[ROOT/"outputs"/output/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); } }
  // <---
  
  // ---> errors
  fl_line_style(FL_SOLID, 3); fl_color(RED);
  for (std::string net : context.ls(ROOT/"network errors"))
  { for (std::string wire : context.ls(ROOT/"network"/net/"wires"))
    { fl_line(x() + x_screen((int)circuit[ROOT/"wires"/wire/0/"x"]),
              y() + y_screen((int)circuit[ROOT/"wires"/wire/0/"y"]),
              x() + x_screen((int)circuit[ROOT/"wires"/wire/1/"x"]),
              y() + y_screen((int)circuit[ROOT/"wires"/wire/1/"y"])); } }

  for (std::string error : context.ls(ROOT/"sequence errors"))
  { std::string type = context[ROOT/"sequence errors"/error/"type"];
    std::string id = context[ROOT/"sequence errors"/error/"id"];

    if (type == "input")
    { int sx = x_screen((int)circuit[ROOT/"inputs"/id/"x"]);
      int sy = y_screen((int)circuit[ROOT/"inputs"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); }
    else if (type == "output")
    { int sx = x_screen((int)circuit[ROOT/"outputs"/id/"x"]);
      int sy = y_screen((int)circuit[ROOT/"outputs"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); }
    else if (type == "unit")
    { int sx = x_screen((int)circuit[ROOT/"units"/id/"x"]);
      int sy = y_screen((int)circuit[ROOT/"units"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); } }
  // <---

  // ---> cursor
  fl_line_style(FL_SOLID, 3); fl_color(GREEN);
  fl_line(x() + x_screen((int)context[ROOT/"cursor pos"/"x"]) - CSIZE,
          y() + y_screen((int)context[ROOT/"cursor pos"/"y"]),
          x() + x_screen((int)context[ROOT/"cursor pos"/"x"]) + CSIZE,
          y() + y_screen((int)context[ROOT/"cursor pos"/"y"]));
  fl_line(x() + x_screen((int)context[ROOT/"cursor pos"/"x"]),
          y() + y_screen((int)context[ROOT/"cursor pos"/"y"]) - CSIZE,
          x() + x_screen((int)context[ROOT/"cursor pos"/"x"]),
          y() + y_screen((int)context[ROOT/"cursor pos"/"y"]) + CSIZE);
  // <---

  fl_line_style(FL_SOLID, 1); fl_color(0); fl_pop_clip(); }
// <---

// ---> constructor
editor::window::window()
: Fl_Window(640, 480),
  menu_bar(0, 0, 640, 20),
  side_screen(440, 20, 200, 460),
  wsp(0, 20, 440, 460)
{ single_window = this;
  
  bus + IC(handler, this);
  size_range(640, 480);
  std::string title = "Linky";
  title.append(" ").append(VERSION);
  label(title.c_str());

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
  menu_bar.add("About", 0, control_cb, (void*)"about");

#ifdef DEBUG
  menu_bar.add("Debug/Print circuit", 0, control_cb, (void*)"print circuit");
  menu_bar.add("Debug/Print params", 0, control_cb, (void*)"print params");
  menu_bar.add("Debug/Print context", 0, control_cb, (void*)"print context");
#endif

  side_screen.box(FL_BORDER_BOX);
  side_screen.color(Fl_Color(WHITE));

  // ---> simulate
  simulate_button = new Fl_Button(side_screen.x() + 5,
                                  side_screen.y() + 5,
                                  (side_screen.w() - 10 - 5) / 2,
                                  20, "Simulate");
  simulate_button->box(FL_BORDER_BOX);
  simulate_button->labelsize(12);
  simulate_button->clear_visible_focus();
  simulate_button->color(Fl_Color(WHITE));
  simulate_button->color2(Fl_Color(BLUE));
  simulate_button->callback(control_cb, (void*)"simulate");
  side_screen.add(simulate_button);
  // <---

  // ---> generate
  generate_button = new Fl_Button(simulate_button->x()
                                    + simulate_button->w() + 5,
                                  simulate_button->y(),
                                  simulate_button->w(),
                                  20, "Generate Code");
  generate_button->box(FL_BORDER_BOX);
  generate_button->labelsize(12);
  generate_button->clear_visible_focus();
  generate_button->color(Fl_Color(WHITE));
  generate_button->color2(Fl_Color(BLUE));
  generate_button->callback(control_cb, (void*)"generate code");
  side_screen.add(generate_button);
  // <---

  // ---> check circuit
  check_circuit_button = new Fl_Button(simulate_button->x(),
                                       simulate_button->y()
                                         + simulate_button->h() + 5,
                                       simulate_button->w(), 
                                       20, "Check circuit");
  check_circuit_button->box(FL_BORDER_BOX);
  check_circuit_button->labelsize(12);
  check_circuit_button->clear_visible_focus();
  check_circuit_button->color(Fl_Color(WHITE));
  check_circuit_button->color2(Fl_Color(BLUE));
  check_circuit_button->callback(control_cb, (void*)"check circuit errors");
  side_screen.add(check_circuit_button);
  // <---
  
  // ---> clear errors
  clear_errors_button = new Fl_Button(check_circuit_button->x()
                                        + check_circuit_button->w() + 5,
                                      check_circuit_button->y(),
                                      check_circuit_button->w(),
                                      20, "Clear errors");
  clear_errors_button->box(FL_BORDER_BOX);
  clear_errors_button->labelsize(12);
  clear_errors_button->clear_visible_focus();
  clear_errors_button->color(Fl_Color(WHITE));
  clear_errors_button->color2(Fl_Color(BLUE));
  clear_errors_button->callback(control_cb, (void*)"clear errors");
  side_screen.add(clear_errors_button);
  // <---
  
  // ---> edit
  edit_button = new Fl_Button(check_circuit_button->x(),
                              check_circuit_button->y()
                                + check_circuit_button->h() + 15,
                              check_circuit_button->w(),
                              20, "Edit [E]");
  edit_button->box(FL_BORDER_BOX);
  edit_button->labelsize(12);
  edit_button->clear_visible_focus();
  edit_button->color(Fl_Color(WHITE));
  edit_button->color2(Fl_Color(BLUE));
  edit_button->callback(control_cb, (void*)"edit items");
  side_screen.add(edit_button);
  // <---
  
  // ---> rotate
  rotate_button = new Fl_Button(edit_button->x() + edit_button->w() + 5,
                                edit_button->y(),
                                edit_button->w(),
                                20,  "Rotate [R]");
  rotate_button->box(FL_BORDER_BOX);
  rotate_button->labelsize(12);
  rotate_button->clear_visible_focus();
  rotate_button->color(Fl_Color(WHITE));
  rotate_button->color2(Fl_Color(BLUE));
  rotate_button->callback(control_cb, (void*)"rotate mode");
  side_screen.add(rotate_button);
  // <---

  // ---> external
  external_button = new Fl_Button(edit_button->x(),
                                  edit_button->y() + edit_button->h() + 5,
                                  edit_button->w(),
                                  20, "External[^E]");
  external_button->box(FL_BORDER_BOX);
  external_button->labelsize(12);
  external_button->clear_visible_focus();
  external_button->color(Fl_Color(WHITE));
  external_button->color2(Fl_Color(BLUE));
  external_button->callback(control_cb, (void*)"add external");
  side_screen.add(external_button);
  // <---
  
  // ---> wire
  wire_button = new Fl_Button(external_button->x() + external_button->w() + 5,
                              external_button->y(),
                              external_button->w(),
                              20, "Wire [W]");
  wire_button->box(FL_BORDER_BOX);
  wire_button->labelsize(12);
  wire_button->clear_visible_focus();
  wire_button->color(Fl_Color(WHITE));
  wire_button->color2(Fl_Color(BLUE));
  wire_button->callback(control_cb, (void*)"add wire");
  side_screen.add(wire_button);
  // <---
  
  // ---> input
  input_button = new Fl_Button(external_button->x(),
                               external_button->y() + external_button->h() + 5,
                               external_button->w(),
                               20, "Input [I]");
  input_button->box(FL_BORDER_BOX);
  input_button->labelsize(12);
  input_button->clear_visible_focus();
  input_button->color(Fl_Color(WHITE));
  input_button->color2(Fl_Color(BLUE));
  input_button->callback(control_cb, (void*)"add input");
  side_screen.add(input_button);
  // <---
 
  // ---> output
  output_button = new Fl_Button(input_button->x() + input_button->w() + 5,
                                input_button->y(),
                                input_button->w(),
                                20, "Output [O]");
  output_button->box(FL_BORDER_BOX);
  output_button->labelsize(12);
  output_button->clear_visible_focus();
  output_button->color(Fl_Color(WHITE));
  output_button->color2(Fl_Color(BLUE));
  output_button->callback(control_cb, (void*)"add output");
  side_screen.add(output_button);
  // <---
  
  // ---> const
  const_button = new Fl_Button(input_button->x(),
                               input_button->y() + input_button->h() + 5,
                               input_button->w(),
                               20, "Const [C]");
  const_button->box(FL_BORDER_BOX);
  const_button->labelsize(12);
  const_button->clear_visible_focus();
  const_button->color(Fl_Color(WHITE));
  const_button->color2(Fl_Color(BLUE));
  const_button->callback(control_cb, (void*)"add const");
  side_screen.add(const_button);
  // <---
  
  // ---> delay
  delay_button = new Fl_Button(output_button->x(),
                               output_button->y() + output_button->h() + 5,
                               output_button->w(),
                               20, "z-1 [Z]");
  delay_button->box(FL_BORDER_BOX);
  delay_button->labelsize(12);
  delay_button->clear_visible_focus();
  delay_button->color(Fl_Color(WHITE));
  delay_button->color2(Fl_Color(BLUE));
  delay_button->callback(control_cb, (void*)"add delay");
  side_screen.add(delay_button);
  // <---
  
  // ---> sum
  sum_button = new Fl_Button(const_button->x(), 
                             const_button->y() + const_button->h() + 5,
                             const_button->w(),
                             20, "Sum [S]");
  sum_button->box(FL_BORDER_BOX);
  sum_button->labelsize(12);
  sum_button->clear_visible_focus();
  sum_button->color(Fl_Color(WHITE));
  sum_button->color2(Fl_Color(BLUE));
  sum_button->callback(control_cb, (void*)"add sum");
  side_screen.add(sum_button);
  // <---
  
  // ---> diff
  diff_button = new Fl_Button(delay_button->x(),
                              delay_button->y() + delay_button->h() + 5,
                              delay_button->w(),
                              20, "Diff [^S]");
  diff_button->box(FL_BORDER_BOX);
  diff_button->labelsize(12);
  diff_button->clear_visible_focus();
  diff_button->color(Fl_Color(WHITE));
  diff_button->color2(Fl_Color(BLUE));
  diff_button->callback(control_cb, (void*)"add diff");
  side_screen.add(diff_button);
  // <---
  
  // ---> prod
  prod_button = new Fl_Button(sum_button->x(),
                              sum_button->y() + sum_button->h() + 5,
                              sum_button->w(),
                              20, "Prod [P]");
  prod_button->box(FL_BORDER_BOX);
  prod_button->labelsize(12);
  prod_button->clear_visible_focus();
  prod_button->color(Fl_Color(WHITE));
  prod_button->color2(Fl_Color(BLUE));
  prod_button->callback(control_cb, (void*)"add prod");
  side_screen.add(prod_button);
  // <---
  
  // ---> div
  div_button = new Fl_Button(diff_button->x(),
                             diff_button->y() + diff_button->h() + 5,
                             diff_button->w(),
                             20, "Div [^P]");
  div_button->box(FL_BORDER_BOX);
  div_button->labelsize(12);
  div_button->clear_visible_focus();
  div_button->color(Fl_Color(WHITE));
  div_button->color2(Fl_Color(BLUE));
  div_button->callback(control_cb, (void*)"add div");
  side_screen.add(div_button);
  // <---
  
  // ---> function
  function_button = new Fl_Button(prod_button->x(),
                                  prod_button->y() + prod_button->h() + 5,
                                  prod_button->w(),
                                  20, "Function [F]");
  function_button->box(FL_BORDER_BOX);
  function_button->labelsize(12);
  function_button->clear_visible_focus();
  function_button->color(Fl_Color(WHITE));
  function_button->color2(Fl_Color(BLUE));
  function_button->callback(control_cb, (void*)"add function");
  side_screen.add(function_button);
  // <---
  
  // ---> abs
  abs_button = new Fl_Button(div_button->x(),
                             div_button->y()
                               + div_button->h() + 5,
                             div_button->w(),
                             20, "Abs value [A]");
  abs_button->box(FL_BORDER_BOX);
  abs_button->labelsize(12);
  abs_button->clear_visible_focus();
  abs_button->color(Fl_Color(WHITE));
  abs_button->color2(Fl_Color(BLUE));
  abs_button->callback(control_cb, (void*)"add abs");
  side_screen.add(abs_button);
  // <---
  
  // ---> loopback
  loopback_button = new Fl_Button(function_button->x(),
                                  function_button->y()
                                    + function_button->h() + 5,
                                  function_button->w(),
                                  20, "Loopback [L]");
  loopback_button->box(FL_BORDER_BOX);
  loopback_button->labelsize(12);
  loopback_button->clear_visible_focus();
  loopback_button->color(Fl_Color(WHITE));
  loopback_button->color2(Fl_Color(BLUE));
  loopback_button->callback(control_cb, (void*)"add loopback");
  side_screen.add(loopback_button);
  // <---
  
  // ---> coeff
  coeff_button = new Fl_Button(abs_button->x(),
                               abs_button->y() + abs_button->h() + 5,
                               abs_button->w(),
                               20, "Coefficient [K]");
  coeff_button->box(FL_BORDER_BOX);
  coeff_button->labelsize(12);
  coeff_button->clear_visible_focus();
  coeff_button->color(Fl_Color(WHITE));
  coeff_button->color2(Fl_Color(BLUE));
  coeff_button->callback(control_cb, (void*)"add coefficient");
  side_screen.add(coeff_button);
  // <---
  
  // ---> limit max
  limit_max_button = new Fl_Button(loopback_button->x(),
                                   loopback_button->y() + loopback_button->h() + 5,
                                   loopback_button->w(),
                                   20, "Limit Max [M]");
  limit_max_button->box(FL_BORDER_BOX);
  limit_max_button->labelsize(12);
  limit_max_button->clear_visible_focus();
  limit_max_button->color(Fl_Color(WHITE));
  limit_max_button->color2(Fl_Color(BLUE));
  limit_max_button->callback(control_cb, (void*)"add max limit");
  side_screen.add(limit_max_button);
  // <---
  
  // ---> limit min
  limit_min_button = new Fl_Button(limit_max_button->x()
                                     + limit_max_button->w() + 5,
                                   limit_max_button->y(),
                                   limit_max_button->w(),
                                   20, "Limit Min [^M]");
  limit_min_button->box(FL_BORDER_BOX);
  limit_min_button->labelsize(12);
  limit_min_button->clear_visible_focus();
  limit_min_button->color(Fl_Color(WHITE));
  limit_min_button->color2(Fl_Color(BLUE));
  limit_min_button->callback(control_cb, (void*)"add min limit");
  side_screen.add(limit_min_button);
  // <---
  
  // ---> more
  more_button = new Fl_Button(limit_max_button->x(),
                              limit_max_button->y() + limit_max_button->h() + 5,
                              limit_max_button->w(),
                              20, "More [>]");
  more_button->box(FL_BORDER_BOX);
  more_button->labelsize(12);
  more_button->clear_visible_focus();
  more_button->color(Fl_Color(WHITE));
  more_button->color2(Fl_Color(BLUE));
  more_button->callback(control_cb, (void*)"add more unit");
  side_screen.add(more_button);
  // <---

  // ---> less
  less_button = new Fl_Button(limit_min_button->x(),
                              limit_min_button->y() + limit_min_button->h() + 5,
                              limit_min_button->w(),
                              20, "Less [<]");
  less_button->box(FL_BORDER_BOX);
  less_button->labelsize(12);
  less_button->clear_visible_focus();
  less_button->color(Fl_Color(WHITE));
  less_button->color2(Fl_Color(BLUE));
  less_button->callback(control_cb, (void*)"add less unit");
  side_screen.add(less_button);
  // <---

  // ---> eq
  eq_button = new Fl_Button(more_button->x(),
                            more_button->y() + more_button->h() + 5,
                            more_button->w(),
                            20, "Equal [=]");
  eq_button->box(FL_BORDER_BOX);
  eq_button->labelsize(12);
  eq_button->clear_visible_focus();
  eq_button->color(Fl_Color(WHITE));
  eq_button->color2(Fl_Color(BLUE));
  eq_button->callback(control_cb, (void*)"add eq unit");
  side_screen.add(eq_button);
  // <---

  // ---> not
  not_button = new Fl_Button(less_button->x(),
                             less_button->y() + less_button->h() + 5,
                             less_button->w(),
                             20, "Not [!]");
  not_button->box(FL_BORDER_BOX);
  not_button->labelsize(12);
  not_button->clear_visible_focus();
  not_button->color(Fl_Color(WHITE));
  not_button->color2(Fl_Color(BLUE));
  not_button->callback(control_cb, (void*)"add not unit");
  side_screen.add(not_button);
  // <---

  // dummy group to correct resizing
  Fl_Group* dummy = new Fl_Group(side_screen.x(),
                                 side_screen.y() + side_screen.h(),
                                 side_screen.w(),
                                 0);
  dummy->box(FL_BORDER_BOX);
  side_screen.add(dummy);
  side_screen.resizable(dummy);

  context[ROOT/"draw pos"/"x"] = 0;
  context[ROOT/"draw pos"/"y"] = 0;
  context[ROOT/"cursor pos"/"x"] = 0;
  context[ROOT/"cursor pos"/"y"] = 0;
  context[ROOT/"grid size"] = 10;
  context[ROOT/"shift"] = 0;
  context[ROOT/"space"] = 0;
  context[ROOT/"control"] = 0;

  show(); }
// <---

// ---> control_cb
void editor::window::control_cb(Fl_Widget* w, void* arg)
{ std::string cmd((const char*)arg);

  if (cmd == "print circuit")
  { PRINT("Circuit:\n%s\n", circuit.serialize().c_str()); }

  else if (cmd == "print params")
  { PRINT("Params:\n%s\n", params.serialize().c_str()); }

  else if (cmd == "print context")
  { PRINT("Context:\n%s\n", context.serialize().c_str()); }

  else if (cmd == "generate code")
  { bus(IM("check circuit errors"));
    bool fail = false;
    if (context.ls(ROOT/"network errors").size() ||
        context.ls(ROOT/"sequence errors").size()) { fail = true; }
    if (!fail)
    { bus(IM("solve"));
      bus(IM("generate code"));
      bus(IM("screen update")); } }

  else if (cmd == "check circuit errors") { bus(IM("check circuit errors"));
                                            bus(IM("screen update")); }

  else if (cmd == "open")
  { char* path = fl_file_chooser("Open file", "*.linky", nullptr, true);
    if (path) { bus(IM("file open") << iv("path", path)); }
    bus(IM("screen update")); }

  else if (cmd == "save")
  { if (!params(ROOT/"circruit file path"))
    { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
      if (path) { bus(IM("file save") << iv("path", path)); } }
    else
    { bus(IM("file save")
          << iv("path", (std::string)context[ROOT/"circuit file path"])); } }

  else if (cmd == "save as")
  { char* path = fl_file_chooser("Save File", "*.linky", nullptr, true);
    if (path) { bus(IM("file save") << iv("path", path)); } }

  else if (cmd == "about")
  { std::string mess;
    mess.append("Linky ")
        .append(VERSION)
        .append("\nSimple digital circuits simulator.");
    fl_message("%s", mess.c_str()); }

  else if (cmd == "simulate")
  { bus(IM("check circuit errors"));

    bool fail = false;
    if (context.ls(ROOT/"network errors").size() ||
        context.ls(ROOT/"sequence errors").size()) { fail = true; }
    if (fail) { return; }

    bus(IM("solve"));
    bus(IM("screen update"));

    // preparing simulation parameters
    simulator::params sp;
    
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { sp.inputs.push_back((std::string)
                          circuit[ROOT/"inputs"/input/"name"]); }
    
    for (std::string output : circuit.ls(ROOT/"outputs"))
    { sp.outputs.push_back((std::string)
                           circuit[ROOT/"outputs"/output/"name"]); }
    
    sp.context_size = (int)context[ROOT/"solution"/"context num"];
    simulator::window sim(sp);
    while (sim.shown()) { Fl::wait(); } }

  else { bus(IM(cmd)); } }
// <---

// ---> handler
void editor::window::handler(void* ctx, IM mess)
{ editor::window* that = (editor::window*)ctx;

  if (mess == "add wire")
  { bus(IM("end input"));
    wire_button->color(GREEN); wire_button->redraw();
    context.del(ROOT/"current wire");
    context[ROOT/"edit mode"] = "place wire";
    that->redraw(); }

  else if (mess == "add input")
  { bus(IM("end input"));
    input_button->color(GREEN); input_button->redraw();
    context[ROOT/"edit mode"] = "place input";
    that->redraw(); }

  else if (mess == "add output")
  { bus(IM("end input"));
    output_button->color(GREEN); output_button->redraw();
    context[ROOT/"edit mode"] = "place output";
    that->redraw(); }

  else if (mess == "add const")
  { bus(IM("end input"));
    const_button->color(GREEN); const_button->redraw();
    context[ROOT/"edit mode"] = "place const";
    that->redraw(); }

  else if (mess == "add delay")
  { bus(IM("end input"));
    delay_button->color(GREEN); delay_button->redraw();
    context[ROOT/"edit mode"] = "place delay";
    that->redraw(); }

  else if (mess == "add sum")
  { bus(IM("end input"));
    sum_button->color(GREEN); sum_button->redraw();
    context[ROOT/"edit mode"] = "place sum";
    that->redraw(); }

  else if (mess == "add diff")
  { bus(IM("end input"));
    diff_button->color(GREEN); diff_button->redraw();
    context[ROOT/"edit mode"] = "place diff";
    that->redraw(); }

  else if (mess == "add prod")
  { bus(IM("end input"));
    prod_button->color(GREEN); prod_button->redraw();
    context[ROOT/"edit mode"] = "place prod";
    that->redraw(); }

  else if (mess == "add div")
  { bus(IM("end input"));
    div_button->color(GREEN); div_button->redraw();
    context[ROOT/"edit mode"] = "place div";
    that->redraw(); }

  else if (mess == "add function")
  { bus(IM("end input"));
    function_button->color(GREEN); function_button->redraw();
    context[ROOT/"edit mode"] = "place function";
    that->redraw(); }

  else if (mess == "add loopback")
  { bus(IM("end input"));
    loopback_button->color(GREEN); loopback_button->redraw();
    context[ROOT/"edit mode"] = "place loopback";
    that->redraw(); }

  else if (mess == "add coefficient")
  { bus(IM("end input"));
    coeff_button->color(GREEN); coeff_button->redraw();
    context[ROOT/"edit mode"] = "place coefficient";
    that->redraw(); }

  else if (mess == "add abs")
  { bus(IM("end input"));
    abs_button->color(GREEN); abs_button->redraw();
    context[ROOT/"edit mode"] = "place abs";
    that->redraw(); }

  else if (mess == "add max limit")
  { bus(IM("end input"));
    limit_max_button->color(GREEN); limit_max_button->redraw();
    context[ROOT/"edit mode"] = "place max limit";
    that->redraw(); }

  else if (mess == "add min limit")
  { bus(IM("end input"));
    limit_min_button->color(GREEN); limit_min_button->redraw();
    context[ROOT/"edit mode"] = "place min limit";
    that->redraw(); }

  else if (mess == "add more unit")
  { bus(IM("end input"));
    more_button->color(GREEN); more_button->redraw();
    context[ROOT/"edit mode"] = "place more unit";
    that->redraw(); }

  else if (mess == "add less unit")
  { bus(IM("end input"));
    less_button->color(GREEN); less_button->redraw();
    context[ROOT/"edit mode"] = "place less unit";
    that->redraw(); }

  else if (mess == "add eq unit")
  { bus(IM("end input"));
    eq_button->color(GREEN); eq_button->redraw();
    context[ROOT/"edit mode"] = "place eq unit";
    that->redraw(); }

  else if (mess == "add not unit")
  { bus(IM("end input"));
    not_button->color(GREEN); not_button->redraw();
    context[ROOT/"edit mode"] = "place not unit";
    that->redraw(); }

  else if (mess == "edit items")
  { bus(IM("end input"));
    edit_button->color(GREEN); edit_button->redraw();
    context[ROOT/"edit mode"] = "edit properties";
    that->redraw(); }

  else if (mess == "rotate mode")
  { bus(IM("end input"));
    rotate_button->color(GREEN); rotate_button->redraw();
    context[ROOT/"edit mode"] = "rotate";
    that->redraw(); }

  else if (mess == "add external")
  { bus(IM("end input"));
    external_button->color(GREEN); external_button->redraw();
    context[ROOT/"edit mode"] = "place external";
    that->redraw(); }

  else if (mess == "end input")
  { edit_button->color(WHITE); edit_button->redraw();
    rotate_button->color(WHITE); rotate_button->redraw();
    external_button->color(WHITE); external_button->redraw();
    wire_button->color(WHITE); wire_button->redraw();
    input_button->color(WHITE); input_button->redraw();
    output_button->color(WHITE); output_button->redraw();
    const_button->color(WHITE); const_button->redraw();
    delay_button->color(WHITE); delay_button->redraw();
    sum_button->color(WHITE); sum_button->redraw();
    diff_button->color(WHITE); diff_button->redraw();
    prod_button->color(WHITE); prod_button->redraw();
    div_button->color(WHITE); div_button->redraw();
    function_button->color(WHITE); function_button->redraw();
    loopback_button->color(WHITE); loopback_button->redraw();
    coeff_button->color(WHITE); coeff_button->redraw();
    abs_button->color(WHITE); abs_button->redraw();
    limit_max_button->color(WHITE); limit_max_button->redraw();
    limit_min_button->color(WHITE); limit_min_button->redraw();
    more_button->color(WHITE); more_button->redraw();
    less_button->color(WHITE); less_button->redraw();
    eq_button->color(WHITE); eq_button->redraw();
    not_button->color(WHITE); not_button->redraw();

    // finalize previous operations if needed
    if (context[ROOT/"edit mode"] == "place wire")
    { context.del(ROOT/"current wire"); }

    context[ROOT/"edit mode"] = "normal";
    that->redraw(); }

  else if (mess == "delete")
  { bus(IM("clear errors"));
    if (!context(ROOT/"highlight")) { return; }

    if (context[ROOT/"highlight"/"type"] == "wire point")
    { std::string wire = context[ROOT/"highlight"/"wire"];
      circuit.del(ROOT/"wires"/wire);
      context.del(ROOT/"highlight"); that->redraw(); }

    else if (context[ROOT/"highlight"/"type"] == "input")
    { std::string input = context[ROOT/"highlight"/"input"];
      circuit.del(ROOT/"inputs"/input);
      context.del(ROOT/"highlight"); that->redraw(); }

    else if (context[ROOT/"highlight"/"type"] == "output")
    { std::string output = context[ROOT/"highlight"/"output"];
      circuit.del(ROOT/"outputs"/output);
      context.del(ROOT/"highlight"); that->redraw(); }

    else if (context[ROOT/"highlight"/"type"] == "unit")
    { std::string unit = context[ROOT/"highlight"/"unit"];
      circuit.del(ROOT/"units"/unit);
      context.del(ROOT/"highlight"); that->redraw(); }
  
    bus(IM("cursor update")); }

  else if (mess == "rotate press")
  { if (!context(ROOT/"highlight")) { return; }
    else if (!(context[ROOT/"highlight"/"type"] == "unit")) { return; }

    std::string unit = context[ROOT/"highlight"/"unit"];

    int rotate_pos = 0;
    if (!!circuit(ROOT/"units"/unit/"rotate pos"))
    { rotate_pos = circuit[ROOT/"units"/unit/"rotate pos"]; }
    rotate_pos++;
    circuit[ROOT/"units"/unit/"rotate pos"] = rotate_pos;
    that->redraw(); }

  else if (mess == "edit")
  { if (!context(ROOT/"highlight")) { return; }

    else if (context[ROOT/"highlight"/"type"] == "input")
    { inputEditWindow w((std::string)context[ROOT/"highlight"/"input"]);
      while (w.shown()) { Fl::wait(); } }

    else if (context[ROOT/"highlight"/"type"] == "output")
    { outputEditWindow w((std::string)context[ROOT/"highlight"/"output"]);
      while (w.shown()) { Fl::wait(); } }

    else if (context[ROOT/"highlight"/"type"] == "unit")
    { std::string unit = context[ROOT/"highlight"/"unit"];
      if (circuit[ROOT/"units"/unit/"type"] == "constant")
      { constEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "delay")
      { delayEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "function")
      { functionEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "loopback")
      { loopbackEditWindow w(unit); while(w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "coeff")
      { coeffEditWindow w(unit); while(w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "limit max")
      { limitMaxEditWindow w(unit); while(w.shown()) { Fl::wait(); } }

      else if (circuit[ROOT/"units"/unit/"type"] == "limit min")
      { limitMinEditWindow w(unit); while(w.shown()) { Fl::wait(); } }
      
      } }

  else if (mess == "space down") { context[ROOT/"space"] = (int)1;
                                   bus(IM("click")); }

  else if (mess == "clear errors") { context.del(ROOT/"network");
                                     context.del(ROOT/"network errors");
                                     context.del(ROOT/"sequence errors");
                                     bus(IM("screen update")); }

  else if (mess == "click")
  { bus(IM("clear errors"));

    if (context[ROOT/"edit mode"] == "place wire")
    { bus(IM("place wire press")); }
    else if (context[ROOT/"edit mode"] == "rotate")
    { bus(IM("rotate press")); }
    else if (context[ROOT/"edit mode"] == "place input")
    { bus(IM("place input press")); }
    else if (context[ROOT/"edit mode"] == "place output")
    { bus(IM("place output press")); }
    else if (context[ROOT/"edit mode"] == "place const")
    { bus(IM("place const press")); }
    else if (context[ROOT/"edit mode"] == "place delay")
    { bus(IM("place delay press")); }
    else if (context[ROOT/"edit mode"] == "place sum")
    { bus(IM("place sum press")); }
    else if (context[ROOT/"edit mode"] == "place diff")
    { bus(IM("place diff press")); }
    else if (context[ROOT/"edit mode"] == "place prod")
    { bus(IM("place prod press")); }
    else if (context[ROOT/"edit mode"] == "place div")
    { bus(IM("place div press")); }
    else if (context[ROOT/"edit mode"] == "place function")
    { bus(IM("place function press")); }
    else if (context[ROOT/"edit mode"] == "place loopback")
    { bus(IM("place loopback press")); }
    else if (context[ROOT/"edit mode"] == "place coefficient")
    { bus(IM("place coefficient press")); }
    else if (context[ROOT/"edit mode"] == "place abs")
    { bus(IM("place abs press")); }
    else if (context[ROOT/"edit mode"] == "place max limit")
    { bus(IM("place max limit press")); }
    else if (context[ROOT/"edit mode"] == "place min limit")
    { bus(IM("place min limit press")); }
    else if (context[ROOT/"edit mode"] == "place more unit")
    { bus(IM("place more unit press")); }
    else if (context[ROOT/"edit mode"] == "place less unit")
    { bus(IM("place less unit press")); }
    else if (context[ROOT/"edit mode"] == "place eq unit")
    { bus(IM("place eq unit press")); }
    else if (context[ROOT/"edit mode"] == "place not unit")
    { bus(IM("place not unit press")); }
    else if (context[ROOT/"edit mode"] == "edit properties")
    { bus(IM("edit")); }
    
    bus(IM("cursor update")); }

  else if (mess == "space up")     { context[ROOT/"space"] = (int)0; }
  else if (mess == "control down") { context[ROOT/"control"] = (int)1; }
  else if (mess == "control up")   { context[ROOT/"control"] = (int)0; }
  else if (mess == "shift down")   { context[ROOT/"shift"] = (int)1; }
  else if (mess == "shift up")     { context[ROOT/"shift"] = (int)0; }

  else if (mess == "up")
  { if ((int)context(ROOT/"shift"))
    { context[ROOT/"draw pos"/"y"] -= 1; that->redraw(); }
    else if ((int)context(ROOT/"control"))
    { context[ROOT/"grid size"] += 5;
      if ((int)context[ROOT/"grid size"] > 100)
      { context[ROOT/"grid size"] = 100; }
      that->redraw(); }
    else if ((int)context(ROOT/"space"))
    { if (context[ROOT/"edit mode"] == "normal" &&
          !!context(ROOT/"highlight"))
      { if (context[ROOT/"highlight"/"type"] == "wire point")
        { std::string wire = context[ROOT/"highlight"/"wire"];
          std::string point = context[ROOT/"highlight"/"point"];
          circuit[ROOT/"wires"/wire/point/"y"] -= 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "input")
        { std::string input = context[ROOT/"highlight"/"input"];
          circuit[ROOT/"inputs"/input/"y"] -= 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "output")
        { std::string output = context[ROOT/"highlight"/"output"];
          circuit[ROOT/"outputs"/output/"y"] -= 1; that->redraw(); }
          
        if (context[ROOT/"highlight"/"type"] == "unit")
        { std::string unit = context[ROOT/"highlight"/"unit"];
          circuit[ROOT/"units"/unit/"y"] -= 1; that->redraw(); } } }
    else { context[ROOT/"cursor pos"/"y"] -= 1; bus(IM("cursor update")); } }

  else if (mess == "down")
  { if ((int)context(ROOT/"shift"))
    { context[ROOT/"draw pos"/"y"] += 1; that->redraw(); }
    else if ((int)context(ROOT/"control"))
    { context[ROOT/"grid size"] -= 5;
      if ((int)context[ROOT/"grid size"] < 5)
      { context[ROOT/"grid size"] = 5; }
      that->redraw(); }
    else if ((int)context(ROOT/"space"))
    { if (context[ROOT/"edit mode"] == "normal" &&
          !!context(ROOT/"highlight"))
      { if (context[ROOT/"highlight"/"type"] == "wire point")
        { std::string wire = context[ROOT/"highlight"/"wire"];
          std::string point = context[ROOT/"highlight"/"point"];
          circuit[ROOT/"wires"/wire/point/"y"] += 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "input")
        { std::string input = context[ROOT/"highlight"/"input"];
          circuit[ROOT/"inputs"/input/"y"] += 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "output")
        { std::string output = context[ROOT/"highlight"/"output"];
          circuit[ROOT/"outputs"/output/"y"] += 1; that->redraw(); }

        if (context[ROOT/"highlight"/"type"] == "unit")
        { std::string unit = context[ROOT/"highlight"/"unit"];
          circuit[ROOT/"units"/unit/"y"] += 1; that->redraw(); } } }
    else { context[ROOT/"cursor pos"/"y"] += 1; bus(IM("cursor update")); } }

  else if (mess == "left")
  { if ((int)context(ROOT/"shift"))
    { context[ROOT/"draw pos"/"x"] -= 1; that->redraw(); }
    else if ((int)context(ROOT/"space"))
    { if (context[ROOT/"edit mode"] == "normal" &&
          !!context(ROOT/"highlight"))
      { if (context[ROOT/"highlight"/"type"] == "wire point")
        { std::string wire = context[ROOT/"highlight"/"wire"];
          std::string point = context[ROOT/"highlight"/"point"];
          circuit[ROOT/"wires"/wire/point/"x"] -= 1; that->redraw(); } 
      
        if (context[ROOT/"highlight"/"type"] == "input")
        { std::string input = context[ROOT/"highlight"/"input"];
          circuit[ROOT/"inputs"/input/"x"] -= 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "output")
        { std::string output = context[ROOT/"highlight"/"output"];
          circuit[ROOT/"outputs"/output/"x"] -= 1; that->redraw(); }
          
        if (context[ROOT/"highlight"/"type"] == "unit")
        { std::string unit = context[ROOT/"highlight"/"unit"];
          circuit[ROOT/"units"/unit/"x"] -= 1; that->redraw(); } } }
    else { context[ROOT/"cursor pos"/"x"] -= 1; bus(IM("cursor update")); } }

  else if (mess == "right")
  { if ((int)context(ROOT/"shift"))
    { context[ROOT/"draw pos"/"x"] += 1; that->redraw(); }
    else if ((int)context(ROOT/"space"))
    { if (context[ROOT/"edit mode"] == "normal" &&
          !!context(ROOT/"highlight"))
      { if (context[ROOT/"highlight"/"type"] == "wire point")
        { std::string wire = context[ROOT/"highlight"/"wire"];
          std::string point = context[ROOT/"highlight"/"point"];
          circuit[ROOT/"wires"/wire/point/"x"] += 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "input")
        { std::string input = context[ROOT/"highlight"/"input"];
          circuit[ROOT/"inputs"/input/"x"] += 1; that->redraw(); }
      
        if (context[ROOT/"highlight"/"type"] == "output")
        { std::string output = context[ROOT/"highlight"/"output"];
          circuit[ROOT/"outputs"/output/"x"] += 1; that->redraw(); }

        if (context[ROOT/"highlight"/"type"] == "unit")
        { std::string unit = context[ROOT/"highlight"/"unit"];
          circuit[ROOT/"units"/unit/"x"] += 1; that->redraw(); } } }
    else { context[ROOT/"cursor pos"/"x"] += 1; bus(IM("cursor update")); } }

  else if (mess == "screen update") { that->redraw(); }

  else if (mess == "cursor update")
  { bool highlight = false;
    int cx = context[ROOT/"cursor pos"/"x"];
    int cy = context[ROOT/"cursor pos"/"y"];

    if (!highlight)
    { for (std::string wire : circuit.ls(ROOT/"wires"))
      { if (cx == (int)circuit[ROOT/"wires"/wire/0/"x"] &&
            cy == (int)circuit[ROOT/"wires"/wire/0/"y"])
        { context[ROOT/"highlight"/"type"] = "wire point";
          context[ROOT/"highlight"/"wire"] = wire;
          context[ROOT/"highlight"/"point"] = "0";
          highlight = true; break; }

        else if (cx == (int)circuit[ROOT/"wires"/wire/1/"x"] &&
                 cy == (int)circuit[ROOT/"wires"/wire/1/"y"])
        { context[ROOT/"highlight"/"type"] = "wire point";
          context[ROOT/"highlight"/"wire"] = wire;
          context[ROOT/"highlight"/"point"] = "1";
          highlight = true; break; } } }

    if (!highlight)
    { for (std::string input : circuit.ls(ROOT/"inputs"))
      { if (cx == (int)circuit[ROOT/"inputs"/input/"x"] &&
            cy == (int)circuit[ROOT/"inputs"/input/"y"])
        { context[ROOT/"highlight"/"type"] = "input";
          context[ROOT/"highlight"/"input"] = input;
          highlight = true; break; } } }

    if (!highlight)
    { for (std::string output : circuit.ls(ROOT/"outputs"))
      { if (cx == (int)circuit[ROOT/"outputs"/output/"x"] &&
            cy == (int)circuit[ROOT/"outputs"/output/"y"])
        { context[ROOT/"highlight"/"type"] = "output";
          context[ROOT/"highlight"/"output"] = output;
          highlight = true; break; } } }

    if (!highlight)
    { for (std::string unit : circuit.ls(ROOT/"units"))
      { if (cx == (int)circuit[ROOT/"units"/unit/"x"] &&
            cy == (int)circuit[ROOT/"units"/unit/"y"])
        { context[ROOT/"highlight"/"type"] = "unit";
          context[ROOT/"highlight"/"unit"] = unit;
          highlight = true; break; } } }

    if (!highlight) { context.del(ROOT/"highlight"); } that->redraw(); }

  else if (mess == "place wire press")
  { int s = context.ls(ROOT/"current wire").size();
    context[ROOT/"current wire"/s/"x"] = context[ROOT/"cursor pos"/"x"];
    context[ROOT/"current wire"/s/"y"] = context[ROOT/"cursor pos"/"y"];

    if (s == 1)
    { circuit[ROOT/"wires"];
      std::list<std::string> wires = circuit.ls(ROOT/"wires");
      int idx = wires.size() ? std::stoi(wires.back()) + 1 : 0;
      circuit[ROOT/"wires"/idx];
      context.cp(ROOT/"current wire", ROOT/"wires"/idx, &circuit);
      context.mv(ROOT/"current wire"/1, ROOT/"current wire"/0);
      that->redraw(); } }

  else if (mess == "place input press")
  { circuit[ROOT/"inputs"];
    std::list<std::string> inputs = circuit.ls(ROOT/"inputs");
    int idx = inputs.size() ? std::stoi(inputs.back()) + 1 : 0;
    circuit[ROOT/"inputs"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"inputs"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"inputs"/idx/"name"] =
      std::string("I_").append(std::to_string(idx));
    that->redraw(); }

  else if (mess == "place output press")
  { circuit[ROOT/"outputs"];
    std::list<std::string> outputs = circuit.ls(ROOT/"outputs");
    int idx = outputs.size() ? std::stoi(outputs.back()) + 1 : 0;
    circuit[ROOT/"outputs"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"outputs"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"outputs"/idx/"name"] =
      std::string("O_").append(std::to_string(idx));
    that->redraw(); }

  else if (mess == "place const press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "constant";
    circuit[ROOT/"units"/idx/"value"] = 1.0f;
    that->redraw(); }
    
  else if (mess == "place delay press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "delay";
    circuit[ROOT/"units"/idx/"value"] = 1;
    that->redraw(); }
    
  else if (mess == "place sum press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "sum";
    that->redraw(); }

  else if (mess == "place diff press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "difference";
    that->redraw(); }
  
  else if (mess == "place prod press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "product";
    that->redraw(); }

  else if (mess == "place div press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "division";
    that->redraw(); }
    
  else if (mess == "place function press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "function";
    circuit[ROOT/"units"/idx/"function name"] = "";
    circuit[ROOT/"units"/idx/"numerator poly"] = "1;";
    circuit[ROOT/"units"/idx/"denominator poly"] = "1;";
    that->redraw(); }

  else if (mess == "place loopback press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "loopback";
    circuit[ROOT/"units"/idx/"value"] = 1;
    that->redraw(); }

  else if (mess == "place coefficient press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "coeff";
    circuit[ROOT/"units"/idx/"value"] = 1;
    that->redraw(); }

  else if (mess == "place abs press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "abs";
    that->redraw(); }

  else if (mess == "place max limit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "limit max";
    circuit[ROOT/"units"/idx/"value"] = 1.0f;
    that->redraw(); }

  else if (mess == "place min limit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "limit min";
    circuit[ROOT/"units"/idx/"value"] = 1.0f;
    that->redraw(); }

  else if (mess == "place more unit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "more";
    that->redraw(); }

  else if (mess == "place less unit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "less";
    that->redraw(); }
  
  else if (mess == "place eq unit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "eq";
    that->redraw(); }
  
  else if (mess == "place not unit press")
  { circuit[ROOT/"units"];
    std::list<std::string> units = circuit.ls(ROOT/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[ROOT/"units"/idx/"x"] = context[ROOT/"cursor pos"/"x"];
    circuit[ROOT/"units"/idx/"y"] = context[ROOT/"cursor pos"/"y"];
    circuit[ROOT/"units"/idx/"type"] = "not";
    that->redraw(); }
}
// <---

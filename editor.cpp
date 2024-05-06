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
                      circuit[root/"inputs"/name/"name"]).c_str());
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { inputEditWindow* that = (inputEditWindow*)arg;
    circuit[root/"inputs"/that->input_name/"name"] = that->name_input.value();
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
                      circuit[root/"outputs"/name/"name"]).c_str());
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { outputEditWindow* that = (outputEditWindow*)arg;
    circuit[root/"outputs"/that->output_name/"name"] = that->name_input.value();
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
    value_input.value((float)circuit[root/"units"/name/"value"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { constEditWindow* that = (constEditWindow*)arg;
    circuit[root/"units"/that->unit_name/"value"]
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
    value_input.value((int)circuit[root/"units"/name/"value"]);
    set_modal(); show(); }
  
  static void save_cb(Fl_Widget* w, void* arg)
  { delayEditWindow* that = (delayEditWindow*)arg;
    circuit[root/"units"/that->unit_name/"value"]
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
                               circuit[root/"units"/name/"function name"]
                              ).c_str());
    numerator_input.value(((std::string)
                           circuit[root/"units"/name/"numerator poly"]
                          ).c_str());
    denominator_input.value(((std::string)
                             circuit[root/"units"/name/"denominator poly"]
                            ).c_str());
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { functionEditWindow* that = (functionEditWindow*)arg;
    circuit[root/"units"/that->unit_name/"function name"]
      = that->function_name_input.value();
    circuit[root/"units"/that->unit_name/"numerator poly"]
      = that->numerator_input.value();
    circuit[root/"units"/that->unit_name/"denominator poly"]
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

// ---> edit code window
class codeEditWindow : public Fl_Window
{ public:
  codeEditWindow(std::string name)
  : Fl_Window(300, 255),
    file_name_label(5, 5, 290, 20, "File name (in current circuit directory)"),
    file_name_input(5, 25, 290, 20),
    function_name_label(5, 50, 290, 20, "Name of the function"),
    function_name_input(5, 70, 290, 20),
    inputs_list_label(5, 95, 290, 20, "List of the input names separated by \";\""),
    inputs_list_input(5, 115, 290, 20),
    outputs_list_label(5, 140, 290, 20, "List of the output names separated by \";\""),
    outputs_list_input(5, 160, 290, 20),
    context_size_label(5, 185, 290, 20, "Size of the context of function"),
    context_size_input(5, 205, 290, 20),
    save_btn((300 / 2) - (60 / 2), 230, 60, 20, "Save"),
    unit_name(name)
  { save_btn.callback(save_cb, (void*)this);
    file_name_input.value(((std::string)
                           circuit[root/"units"/name/"source file"]
                          ).c_str());
    function_name_input.value(((std::string)
                               circuit[root/"units"/name/"function name"]
                              ).c_str());
    inputs_list_input.value(((std::string)
                             circuit[root/"units"/name/"inputs names"]
                            ).c_str());
    outputs_list_input.value(((std::string)
                              circuit[root/"units"/name/"outputs names"]
                             ).c_str());
    context_size_input.step(1);
    context_size_input.value((int)circuit[root/"units"/name/"context size"]);
    set_modal(); show(); }

  static void save_cb(Fl_Widget* w, void* arg)
  { codeEditWindow* that = (codeEditWindow*)arg;
    circuit[root/"units"/that->unit_name/"source file"]
      = that->file_name_input.value();
    circuit[root/"units"/that->unit_name/"function name"]
      = that->function_name_input.value();
    circuit[root/"units"/that->unit_name/"inputs names"]
      = that->inputs_list_input.value();
    circuit[root/"units"/that->unit_name/"outputs names"]
      = that->outputs_list_input.value();
    circuit[root/"units"/that->unit_name/"context size"]
      = (int)that->context_size_input.value();
    bus(IM("screen update")); that->hide(); }
  
  Fl_Box         file_name_label;
  Fl_Input       file_name_input;
  Fl_Box         function_name_label;
  Fl_Input       function_name_input;
  Fl_Box         inputs_list_label;
  Fl_Input       inputs_list_input;
  Fl_Box         outputs_list_label;
  Fl_Input       outputs_list_input;
  Fl_Box         context_size_label;
  Fl_Value_Input context_size_input;
  Fl_Button      save_btn;
  std::string    unit_name; };
// <---

static editor::window* single_window = nullptr;
static Fl_Button* generate_button = nullptr;
static Fl_Button* check_circuit_button = nullptr;
static Fl_Button* clear_errors_button = nullptr;
static Fl_Button* edit_button = nullptr;
static Fl_Button* wire_button = nullptr;
static Fl_Button* input_button = nullptr;
static Fl_Button* output_button = nullptr;
static Fl_Button* const_button = nullptr;
static Fl_Button* delay_button = nullptr;
static Fl_Button* sum_button = nullptr;
static Fl_Button* prod_button = nullptr;
static Fl_Button* function_button = nullptr;
static Fl_Button* code_button = nullptr;

// TODO: add wires points highlight when wire highlighted or wire editing mode
// TODO: add wires to network conversion

editor::workspace::workspace(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h)
{ }

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
  { if      (Fl::event_key() == 'e') { bus(IM("edit items")); }
    else if (Fl::event_key() == 'w') { bus(IM("add wire")); }
    else if (Fl::event_key() == 'i') { bus(IM("add input")); }
    else if (Fl::event_key() == 'o') { bus(IM("add output")); }
    else if (Fl::event_key() == 'c') { bus(IM("add const")); }
    else if (Fl::event_key() == 'z') { bus(IM("add delay")); }
    else if (Fl::event_key() == 's') { bus(IM("add sum")); }
    else if (Fl::event_key() == 'p') { bus(IM("add prod")); }
    else if (Fl::event_key() == 'f') { bus(IM("add function")); }
    else if (Fl::event_key() == 'b') { bus(IM("add code block")); }
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
  
  if (event == FL_PUSH) { bus(IM("click")); return 1; }
  
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

  // ---> background
  fl_color(WHITE);
  fl_rectf(x(), y(), w(), h());
  // <---

  // ---> grid
  fl_color(LBLUE);
  if (!context(root/"grid size")) { context[root/"grid size"] = 20; }
  for (unsigned int i = 0; i < w(); i += (int)context[root/"grid size"])
  { fl_line(x() + i, y(), x() + i, y() + h()); }
  for (unsigned int i = 0; i < h(); i += (int)context[root/"grid size"])
  { fl_line(x(), y() + i, x() + w(), y() + i); }
  // <---

  // ---> inputs
  fl_color(BLACK);
  for (std::string input : circuit.ls(root/"inputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[root/"inputs"/input/"x"];
    _y[0] = circuit[root/"inputs"/input/"y"];

    fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
    int pixel_w = 
      fl_width(((std::string)circuit[root/"inputs"/input/"name"]).c_str());
    int name_w = pixel_w / (int)context[root/"grid size"] + 1;

    _x[1] = _x[0] + name_w;     _y[1] = _y[0];
    _x[2] = _x[0] + name_w + 1; _y[2] = _y[0] + 1;
    _x[3] = _x[0] + name_w;     _y[3] = _y[0] + 2;
    _x[4] = _x[0];              _y[4] = _y[0] + 2;
    _x[5] = _x[0];              _y[5] = _y[0];

    circuit[root/"inputs"/input/"point"/"x"] = _x[2];
    circuit[root/"inputs"/input/"point"/"y"] = _y[2];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

    fl_draw(((std::string)circuit[root/"inputs"/input/"name"]).c_str(),
            x() + x_screen(_x[0]), y() + y_screen(_y[0]),
            pixel_w, 2 * (int)context[root/"grid size"],
            FL_ALIGN_LEFT); }
  // <---

  // ---> outputs
  for (std::string output : circuit.ls(root/"outputs"))
  { int _x[6] = { 0 }; int _y[6] = { 0 };

    _x[0] = circuit[root/"outputs"/output/"x"];
    _y[0] = circuit[root/"outputs"/output/"y"];

    fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
    int pixel_w =
      fl_width(((std::string)circuit[root/"outputs"/output/"name"]).c_str());
    int name_w = pixel_w / (int)context[root/"grid size"] + 1;

    _x[1] = _x[0] + 1 + name_w; _y[1] = _y[0];
    _x[2] = _x[0] + 1 + name_w; _y[2] = _y[0] + 2;
    _x[3] = _x[0];              _y[3] = _y[0] + 2;
    _x[4] = _x[0] + 1;          _y[4] = _y[0] + 1;
    _x[5] = _x[0];              _y[5] = _y[0];

    for (unsigned int i = 0; i < 5; i++)
    { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
              x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

    circuit[root/"outputs"/output/"point"/"x"] = _x[4];
    circuit[root/"outputs"/output/"point"/"y"] = _y[4];
              
    fl_draw(((std::string)circuit[root/"outputs"/output/"name"]).c_str(),
            x() + x_screen(_x[0] + 1), y() + y_screen(_y[0]),
            pixel_w, 2 * (int)context[root/"grid size"],
            FL_ALIGN_LEFT); }
  // <---

  // ---> units
  for (std::string unit : circuit.ls(root/"units"))
  { if      (circuit[root/"units"/unit/"type"] == "constant")
    { fl_color(BLACK);
      float val = circuit[root/"units"/unit/"value"];
      fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
      int pixel_w = fl_width(std::to_string(val).c_str());
      int label_w = pixel_w / (int)context[root/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = circuit[root/"units"/unit/"x"];
      _y[0] = circuit[root/"units"/unit/"y"];
      _x[1] = _x[0] + label_w; _y[1] = _y[0];
      _x[2] = _x[0] + label_w; _y[2] = _y[0] + 2;
      _x[3] = _x[0];           _y[3] = _y[0] + 2;
      _x[4] = _x[0];           _y[4] = _y[0];

      circuit[root/"units"/unit/"outputs"/0/"x"] = _x[0] + label_w;
      circuit[root/"units"/unit/"outputs"/0/"y"] = _y[0] + 1;

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(std::to_string(val).c_str(),
              x() + x_screen(_x[0]), y() + y_screen(_y[0]),
              pixel_w, 2 * (int)context[root/"grid size"],
              FL_ALIGN_LEFT); }
    
    else if (circuit[root/"units"/unit/"type"] == "delay")
    { fl_color(BLACK);
      int val = circuit[root/"units"/unit/"value"];
      std::string label = std::string("z-").append(std::to_string(val));
      fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[root/"grid size"] + 1;
      
      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = circuit[root/"units"/unit/"x"];
      _y[0] = circuit[root/"units"/unit/"y"];
      _x[1] = _x[0] + label_w + 2; _y[1] = _y[0];
      _x[2] = _x[0] + label_w;     _y[2] = _y[0] + 2;
      _x[3] = _x[0] - 2;           _y[3] = _y[0] + 2;
      _x[4] = _x[0];               _y[4] = _y[0];

      circuit[root/"units"/unit/"inputs"/0/"x"] = _x[0] - 1;
      circuit[root/"units"/unit/"inputs"/0/"y"] = _y[0] + 1;

      circuit[root/"units"/unit/"outputs"/0/"x"] = _x[0] + label_w + 1;
      circuit[root/"units"/unit/"outputs"/0/"y"] = _y[0] + 1;

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
              x() + x_screen(_x[0]), y() + y_screen(_y[0]),
              pixel_w, 2 * (int)context[root/"grid size"],
              FL_ALIGN_LEFT); }
    
    else if (circuit[root/"units"/unit/"type"] == "sum")
    { fl_color(BLACK);
      int _x = circuit[root/"units"/unit/"x"];
      int _y = circuit[root/"units"/unit/"y"];

      circuit[root/"units"/unit/"inputs"/0/"x"] = _x;
      circuit[root/"units"/unit/"inputs"/0/"y"] = _y + 1;

      circuit[root/"units"/unit/"inputs"/1/"x"] = _x + 1;
      circuit[root/"units"/unit/"inputs"/1/"y"] = _y + 2;

      circuit[root/"units"/unit/"outputs"/0/"x"] = _x + 2;
      circuit[root/"units"/unit/"outputs"/0/"y"] = _y + 1;
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[root/"grid size"],
              2 * (int)context[root/"grid size"]);
      fl_line(x() + x_screen(_x + 1),
              y() + y_screen(_y    ) + (int)context[root/"grid size"] / 2,
              x() + x_screen(_x + 1),
              y() + y_screen(_y + 1) + (int)context[root/"grid size"] / 2);
      fl_line(x() + x_screen(_x    ) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 1),
              x() + x_screen(_x + 1) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 1)); }
    
    else if (circuit[root/"units"/unit/"type"] == "product")
    { fl_color(BLACK);
      int _x = circuit[root/"units"/unit/"x"];
      int _y = circuit[root/"units"/unit/"y"];
      
      circuit[root/"units"/unit/"inputs"/0/"x"] = _x;
      circuit[root/"units"/unit/"inputs"/0/"y"] = _y + 1;

      circuit[root/"units"/unit/"inputs"/1/"x"] = _x + 1;
      circuit[root/"units"/unit/"inputs"/1/"y"] = _y + 2;

      circuit[root/"units"/unit/"outputs"/0/"x"] = _x + 2;
      circuit[root/"units"/unit/"outputs"/0/"y"] = _y + 1;
      
      fl_rect(x() + x_screen(_x), y() + y_screen(_y),
              2 * (int)context[root/"grid size"],
              2 * (int)context[root/"grid size"]);
      fl_line(x() + x_screen(_x + 0) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 0) + (int)context[root/"grid size"] / 2,
              x() + x_screen(_x + 1) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 1) + (int)context[root/"grid size"] / 2);
      fl_line(x() + x_screen(_x + 1) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 0) + (int)context[root/"grid size"] / 2,
              x() + x_screen(_x + 0) + (int)context[root/"grid size"] / 2,
              y() + y_screen(_y + 1) + (int)context[root/"grid size"] / 2); }
    
    else if (circuit[root/"units"/unit/"type"] == "function")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
      int _x0 = circuit[root/"units"/unit/"x"];
      int _y0 = circuit[root/"units"/unit/"y"];
      std::string label
        = (std::string)circuit[root/"units"/unit/"function name"];
      int pixel_w = fl_width(label.c_str());
      int label_w = pixel_w / (int)context[root/"grid size"] + 1;

      int _x[5] = { 0 }; int _y[5] = { 0 };
      _x[0] = _x0;               _y[0] = _y0;
      _x[1] = _x0 + label_w + 2; _y[1] = _y0;
      _x[2] = _x0 + label_w;     _y[2] = _y0 + 2;
      _x[3] = _x0 - 2;           _y[3] = _y0 + 2;
      _x[4] = _x0;               _y[4] = _y0;

      circuit[root/"units"/unit/"inputs"/0/"x"] = _x0 - 1;
      circuit[root/"units"/unit/"inputs"/0/"y"] = _y0 + 1;

      circuit[root/"units"/unit/"outputs"/0/"x"] = _x0 + label_w + 1;
      circuit[root/"units"/unit/"outputs"/0/"y"] = _y0 + 1;

      for (unsigned int i = 0; i < 4; i++)
      { fl_line(x() + x_screen(_x[i    ]), y() + y_screen(_y[i    ]),
                x() + x_screen(_x[i + 1]), y() + y_screen(_y[i + 1])); }

      fl_draw(label.c_str(),
             x() + x_screen(_x0), y() + y_screen(_y0),
             pixel_w, 2 * (int)context[root/"grid size"],
             FL_ALIGN_LEFT); }
    
    else if (circuit[root/"units"/unit/"type"] == "code block")
    { fl_color(BLACK); fl_font(FL_COURIER, 2 * (int)context[root/"grid size"]);
      int _x0 = circuit[root/"units"/unit/"x"];
      int _y0 = circuit[root/"units"/unit/"y"];
      std::string label = (std::string)circuit[root/"units"/unit/"source file"];
      label.append(":");
      label.append((std::string)circuit[root/"units"/unit/"function name"]);
      int pixel_w = fl_width(label.c_str());

      std::string inputs
        = (std::string)circuit[root/"units"/unit/"inputs names"];
      std::list<std::string> ilist;
      std::string current_i;
      for (char c : inputs)
      { if (c == ';') { ilist.push_back(current_i); current_i.clear();
                        continue; }
        current_i.push_back(c); }
      for (std::string input : ilist)
      { int pw = fl_width(input.c_str());
        if (pw > pixel_w) { pixel_w = pw; } }

      std::string outputs
        = (std::string)circuit[root/"units"/unit/"outputs names"];
      std::list<std::string> olist;
      std::string current_o;
      for (char c : outputs)
      { if (c == ';') { olist.push_back(current_o); current_o.clear();
                        continue; }
        current_o.push_back(c); }
      for (std::string output : olist)
      { int pw = fl_width(output.c_str());
        if (pw > pixel_w) { pixel_w = pw; } }

      int label_w = pixel_w / (int)context[root/"grid size"] + 1;
      fl_rect(x() + x_screen(_x0), y() + y_screen(_y0),
              label_w * (int)context[root/"grid size"],
              2 * (int)context[root/"grid size"]);
      fl_draw(label.c_str(),
              x() + x_screen(_x0), y() + y_screen(_y0),
              pixel_w, 2 * (int)context[root/"grid size"],
              FL_ALIGN_LEFT);

      fl_rect(x() + x_screen(_x0), y() + y_screen(_y0 + 2),
              label_w * (int)context[root/"grid size"],
              ilist.size() * 2 * (int)context[root/"grid size"]);
      int icount = 0;
      for (std::string input : ilist)
      { fl_draw(input.c_str(),
                x() + x_screen(_x0), y() + y_screen(_y0 + 2 + 2 * icount),
                pixel_w, 2 * (int)context[root/"grid size"],
                FL_ALIGN_LEFT);
        circuit[root/"units"/unit/"inputs"/icount/"x"] = _x0;
        circuit[root/"units"/unit/"inputs"/icount/"y"]
          = _y0 + 2 + 2 * icount + 1;
        icount++; }

      fl_rect(x() + x_screen(_x0), y() + y_screen(_y0 + 2 + 2 * ilist.size()),
              label_w * (int)context[root/"grid size"],
              olist.size() * 2 * (int)context[root/"grid size"]);
      int ocount = 0;
      for (std::string output : olist)
      { fl_draw(output.c_str(),
                x() + x_screen(_x0),
                y() + y_screen(_y0 + 2 + 2 * ilist.size() + 2 * ocount),
                pixel_w, 2 * (int)context[root/"grid size"],
                FL_ALIGN_LEFT);
        circuit[root/"units"/unit/"outputs"/ocount/"x"] = _x0 + label_w;
        circuit[root/"units"/unit/"outputs"/ocount/"y"]
          = _y0 + 2 + 2 * (int)ilist.size() + 2 * ocount + 1;
        ocount++; }
    } }
  // <---

  // ---> wires 
  fl_color(BLACK);
  for (std::string wire : circuit.ls(root/"wires"))
  { fl_line(x() + x_screen((int)circuit[root/"wires"/wire/0/"x"]),
            y() + y_screen((int)circuit[root/"wires"/wire/0/"y"]),
            x() + x_screen((int)circuit[root/"wires"/wire/1/"x"]),
            y() + y_screen((int)circuit[root/"wires"/wire/1/"y"])); }
  // <---

  // ---> joints
  fl_line_style(FL_SOLID, 1); fl_color(BLACK);
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
  // <---

  // ---> highlight
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
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE * 2, CSIZE * 2); }

    else if (context[root/"highlight"/"type"] == "unit")
    { std::string unit = context[root/"highlight"/"unit"];
      int sx = x_screen((int)circuit[root/"units"/unit/"x"]);
      int sy = y_screen((int)circuit[root/"units"/unit/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); } }

  // units inputs and outputs (only when editing or highlighting wires)
  // inputs are green circles, outputs are blue circuits
  if (context[root/"edit mode"]        == "place wire" ||
      context(root/"highlight"/"type") == "wire point")
  { // ---> wire markers
    fl_line_style(FL_SOLID, 1); fl_color(BLUE);
    for (std::string wire : circuit.ls(root/"wires"))
    { int _x0 = circuit[root/"wires"/wire/0/"x"];
      int _y0 = circuit[root/"wires"/wire/0/"y"];
      fl_rect(x() + x_screen(_x0) - CSIZE, y() + y_screen(_y0) - CSIZE,
              CSIZE*2, CSIZE*2);
      int _x1 = circuit[root/"wires"/wire/1/"x"];
      int _y1 = circuit[root/"wires"/wire/1/"y"];
      fl_rect(x() + x_screen(_x1) - CSIZE, y() + y_screen(_y1) - CSIZE,
              CSIZE*2, CSIZE*2); }
    // <---
    
    // ---> units inputs and outputs
    for (std::string unit : circuit.ls(root/"units"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      for (std::string input : circuit.ls(root/"units"/unit/"inputs"))
      { int _x = circuit[root/"units"/unit/"inputs"/input/"x"];
        int _y = circuit[root/"units"/unit/"inputs"/input/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); }

      fl_line_style(FL_SOLID, 1); fl_color(BLUE);
      for (std::string output : circuit.ls(root/"units"/unit/"outputs"))
      { int _x = circuit[root/"units"/unit/"outputs"/output/"x"];
        int _y = circuit[root/"units"/unit/"outputs"/output/"y"];
        fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
               CSIZE*2, CSIZE*2, 0.0, 360.0); } }
    // <---

    // ---> circuit inputs
    // input can contain only output (respectively to scheme point of view)
    fl_line_style(FL_SOLID, 1); fl_color(BLUE);
    for (std::string input : circuit.ls(root/"inputs"))
    { if (!circuit(root/"inputs"/input/"point")) { continue; }
      int _x = circuit[root/"inputs"/input/"point"/"x"];
      int _y = circuit[root/"inputs"/input/"point"/"y"];
      fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
             CSIZE*2, CSIZE*2, 0.0, 360.0); }
    // <---

    // ---> circuit outputs
    // output can contain only input (respectively to scheme point of view)
    fl_line_style(FL_SOLID, 1); fl_color(GREEN);
    for (std::string output : circuit.ls(root/"outputs"))
    { if (!circuit(root/"outputs"/output/"point")) { continue; }
      int _x = circuit[root/"outputs"/output/"point"/"x"];
      int _y = circuit[root/"outputs"/output/"point"/"y"];
      fl_pie(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
             CSIZE*2, CSIZE*2, 0.0, 360.0); }
    // <---
    }

  // editable items
  if (context[root/"edit mode"] == "edit properties")
  { for (std::string unit : circuit.ls(root/"units"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[root/"units"/unit/"x"];
      int _y = circuit[root/"units"/unit/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); }

    for (std::string input : circuit.ls(root/"inputs"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[root/"inputs"/input/"x"];
      int _y = circuit[root/"inputs"/input/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); }

    for (std::string output : circuit.ls(root/"outputs"))
    { fl_line_style(FL_SOLID, 1); fl_color(GREEN);
      int _x = circuit[root/"outputs"/output/"x"];
      int _y = circuit[root/"outputs"/output/"y"];
      fl_rect(x() + x_screen(_x) - CSIZE, y() + y_screen(_y) - CSIZE,
              CSIZE*2, CSIZE*2); } }
  // <---
  
  // ---> errors
  fl_line_style(FL_SOLID, 3); fl_color(RED);
  for (std::string net : context.ls(root/"network errors"))
  { for (std::string wire : context.ls(root/"network"/net/"wires"))
    { fl_line(x() + x_screen((int)circuit[root/"wires"/wire/0/"x"]),
              y() + y_screen((int)circuit[root/"wires"/wire/0/"y"]),
              x() + x_screen((int)circuit[root/"wires"/wire/1/"x"]),
              y() + y_screen((int)circuit[root/"wires"/wire/1/"y"])); } }

  for (std::string error : context.ls(root/"sequence errors"))
  { std::string type = context[root/"sequence errors"/error/"type"];
    std::string id = context[root/"sequence errors"/error/"id"];

    if (type == "input")
    { int sx = x_screen((int)circuit[root/"inputs"/id/"x"]);
      int sy = y_screen((int)circuit[root/"inputs"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); }
    else if (type == "output")
    { int sx = x_screen((int)circuit[root/"outputs"/id/"x"]);
      int sy = y_screen((int)circuit[root/"outputs"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); }
    else if (type == "unit")
    { int sx = x_screen((int)circuit[root/"units"/id/"x"]);
      int sy = y_screen((int)circuit[root/"units"/id/"y"]);
      fl_rectf(x() + sx - CSIZE, y() + sy - CSIZE, CSIZE*2, CSIZE*2); } }
  // <---

  // ---> cursor
  fl_line_style(FL_SOLID, 3); fl_color(GREEN);
  fl_line(x() + x_screen((int)context[root/"cursor pos"/"x"]) - CSIZE,
          y() + y_screen((int)context[root/"cursor pos"/"y"]),
          x() + x_screen((int)context[root/"cursor pos"/"x"]) + CSIZE,
          y() + y_screen((int)context[root/"cursor pos"/"y"]));
  fl_line(x() + x_screen((int)context[root/"cursor pos"/"x"]),
          y() + y_screen((int)context[root/"cursor pos"/"y"]) - CSIZE,
          x() + x_screen((int)context[root/"cursor pos"/"x"]),
          y() + y_screen((int)context[root/"cursor pos"/"y"]) + CSIZE);
  // <---

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

  generate_button = new Fl_Button(side_screen.x() + 5,
                                  side_screen.y() + 5,
                                  side_screen.w() - 10,
                                  20, "Generate Code");
  generate_button->box(FL_BORDER_BOX);
  generate_button->labelsize(12);
  generate_button->clear_visible_focus();
  generate_button->color(Fl_Color(WHITE));
  generate_button->color2(Fl_Color(BLUE));
  generate_button->callback(control_cb, (void*)"generate code");
  side_screen.add(generate_button);

  check_circuit_button = new Fl_Button(generate_button->x(),
                                       generate_button->y()
                                         + generate_button->h() + 5,
                                       generate_button->w(), 
                                       20, "Check circuit");
  check_circuit_button->box(FL_BORDER_BOX);
  check_circuit_button->labelsize(12);
  check_circuit_button->clear_visible_focus();
  check_circuit_button->color(Fl_Color(WHITE));
  check_circuit_button->color2(Fl_Color(BLUE));
  check_circuit_button->callback(control_cb, (void*)"check circuit errors");
  side_screen.add(check_circuit_button);

  clear_errors_button = new Fl_Button(check_circuit_button->x(),
                                      check_circuit_button->y()
                                        + check_circuit_button->h() + 5,
                                      check_circuit_button->w(),
                                      20, "Clear errors");
  clear_errors_button->box(FL_BORDER_BOX);
  clear_errors_button->labelsize(12);
  clear_errors_button->clear_visible_focus();
  clear_errors_button->color(Fl_Color(WHITE));
  clear_errors_button->color2(Fl_Color(BLUE));
  clear_errors_button->callback(control_cb, (void*)"clear errors");
  side_screen.add(clear_errors_button);

  edit_button = new Fl_Button(clear_errors_button->x(),
                              clear_errors_button->y()
                                + clear_errors_button->h() + 10,
                              (clear_errors_button->w() - 5) / 2,
                              20, "Edit [E]");
  edit_button->box(FL_BORDER_BOX);
  edit_button->labelsize(12);
  edit_button->clear_visible_focus();
  edit_button->color(Fl_Color(WHITE));
  edit_button->color2(Fl_Color(BLUE));
  edit_button->callback(control_cb, (void*)"edit items");
  side_screen.add(edit_button);

  wire_button = new Fl_Button(edit_button->x() + edit_button->w() + 5,
                              edit_button->y(),
                              edit_button->w(),
                              20, "Wire [W]");
  wire_button->box(FL_BORDER_BOX);
  wire_button->labelsize(12);
  wire_button->clear_visible_focus();
  wire_button->color(Fl_Color(WHITE));
  wire_button->color2(Fl_Color(BLUE));
  wire_button->callback(control_cb, (void*)"add wire");
  side_screen.add(wire_button);

  input_button = new Fl_Button(edit_button->x(),
                               edit_button->y() + edit_button->h() + 5,
                               edit_button->w(),
                               20, "Input [I]");
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
                                20, "Output [O]");
  output_button->box(FL_BORDER_BOX);
  output_button->labelsize(12);
  output_button->clear_visible_focus();
  output_button->color(Fl_Color(WHITE));
  output_button->color2(Fl_Color(BLUE));
  output_button->callback(control_cb, (void*)"add output");
  side_screen.add(output_button);

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

  prod_button = new Fl_Button(delay_button->x(),
                              delay_button->y() + delay_button->h() + 5,
                              delay_button->w(),
                              20, "Prod [P]");
  prod_button->box(FL_BORDER_BOX);
  prod_button->labelsize(12);
  prod_button->clear_visible_focus();
  prod_button->color(Fl_Color(WHITE));
  prod_button->color2(Fl_Color(BLUE));
  prod_button->callback(control_cb, (void*)"add prod");
  side_screen.add(prod_button);

  function_button = new Fl_Button(sum_button->x(),
                                  sum_button->y() + sum_button->h() + 5,
                                  sum_button->w(),
                                  20, "Function [F]");
  function_button->box(FL_BORDER_BOX);
  function_button->labelsize(12);
  function_button->clear_visible_focus();
  function_button->color(Fl_Color(WHITE));
  function_button->color2(Fl_Color(BLUE));
  function_button->callback(control_cb, (void*)"add function");
  side_screen.add(function_button);

  code_button = new Fl_Button(prod_button->x(),
                              prod_button->y() + prod_button->h() + 5,
                              prod_button->w(),
                              20, "Code Block [B]");
  code_button->box(FL_BORDER_BOX);
  code_button->labelsize(12);
  code_button->clear_visible_focus();
  code_button->color(Fl_Color(WHITE));
  code_button->color2(Fl_Color(BLUE));
  code_button->callback(control_cb, (void*)"add code block");
  side_screen.add(code_button);

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
  context[root/"grid size"] = 10;

  show(); }

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
    if (context.ls(root/"network errors").size() ||
        context.ls(root/"sequence errors").size()) { fail = true; }
    if (!fail)
    { bus(IM("generate code"));
      bus(IM("screen update")); } }

  else if (cmd == "check circuit errors") { bus(IM("check circuit errors"));
                                            bus(IM("screen update")); }

  else if (cmd == "open")
  { char* path = fl_file_chooser("Open file", "*.linky", nullptr, true);
    if (path) { bus(IM("file open") << IV("path", path)); }
    bus(IM("screen update")); }

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
    context[root/"edit mode"] = "place wire";
    that->redraw(); }

  else if (mess == "add input")
  { bus(IM("end input"));
    input_button->color(GREEN); input_button->redraw();
    context[root/"edit mode"] = "place input";
    that->redraw(); }

  else if (mess == "add output")
  { bus(IM("end input"));
    output_button->color(GREEN); output_button->redraw();
    context[root/"edit mode"] = "place output";
    that->redraw(); }

  else if (mess == "add const")
  { bus(IM("end input"));
    const_button->color(GREEN); const_button->redraw();
    context[root/"edit mode"] = "place const";
    that->redraw(); }

  else if (mess == "add delay")
  { bus(IM("end input"));
    delay_button->color(GREEN); delay_button->redraw();
    context[root/"edit mode"] = "place delay";
    that->redraw(); }

  else if (mess == "add sum")
  { bus(IM("end input"));
    sum_button->color(GREEN); sum_button->redraw();
    context[root/"edit mode"] = "place sum";
    that->redraw(); }

  else if (mess == "add prod")
  { bus(IM("end input"));
    prod_button->color(GREEN); prod_button->redraw();
    context[root/"edit mode"] = "place prod";
    that->redraw(); }

  else if (mess == "add function")
  { bus(IM("end input"));
    function_button->color(GREEN); function_button->redraw();
    context[root/"edit mode"] = "place function";
    that->redraw(); }

  else if (mess == "add code block")
  { bus(IM("end input"));
    code_button->color(GREEN); code_button->redraw();
    context[root/"edit mode"] = "place code block";
    that->redraw(); }

  else if (mess == "edit items")
  { bus(IM("end input"));
    edit_button->color(GREEN); edit_button->redraw();
    context[root/"edit mode"] = "edit properties";
    that->redraw(); }

  else if (mess == "end input")
  { edit_button->color(WHITE); edit_button->redraw();
    wire_button->color(WHITE); wire_button->redraw();
    input_button->color(WHITE); input_button->redraw();
    output_button->color(WHITE); output_button->redraw();
    const_button->color(WHITE); const_button->redraw();
    delay_button->color(WHITE); delay_button->redraw();
    sum_button->color(WHITE); sum_button->redraw();
    prod_button->color(WHITE); prod_button->redraw();
    function_button->color(WHITE); function_button->redraw();
    code_button->color(WHITE); code_button->redraw();

    // finalize previous operations if needed
    if (context[root/"edit mode"] == "place wire")
    { context.del(root/"current wire"); }

    context[root/"edit mode"] = "normal";
    that->redraw(); }

  else if (mess == "delete")
  { bus(IM("clear errors"));
    if (!context(root/"highlight")) { return; }

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
      context.del(root/"highlight"); that->redraw(); }

    else if (context[root/"highlight"/"type"] == "unit")
    { std::string unit = context[root/"highlight"/"unit"];
      circuit.del(root/"units"/unit);
      context.del(root/"highlight"); that->redraw(); }
  
    bus(IM("cursor update")); }

  else if (mess == "edit")
  { if (!context(root/"highlight")) { return; }

    else if (context[root/"highlight"/"type"] == "input")
    { inputEditWindow w((std::string)context[root/"highlight"/"input"]);
      while (w.shown()) { Fl::wait(); } }

    else if (context[root/"highlight"/"type"] == "output")
    { outputEditWindow w((std::string)context[root/"highlight"/"output"]);
      while (w.shown()) { Fl::wait(); } }

    else if (context[root/"highlight"/"type"] == "unit")
    { std::string unit = context[root/"highlight"/"unit"];
      if (circuit[root/"units"/unit/"type"] == "constant")
      { constEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[root/"units"/unit/"type"] == "delay")
      { delayEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[root/"units"/unit/"type"] == "function")
      { functionEditWindow w(unit); while (w.shown()) { Fl::wait(); } }

      else if (circuit[root/"units"/unit/"type"] == "code block")
      { codeEditWindow w(unit); while (w.shown()) { Fl::wait(); } }
      
      } }

  else if (mess == "space down") { context[root/"space"] = (int)1;
                                   bus(IM("click")); }

  else if (mess == "clear errors") { context.del(root/"network");
                                     context.del(root/"network errors");
                                     context.del(root/"sequence errors");
                                     bus(IM("screen update")); }

  else if (mess == "click")
  { bus(IM("clear errors"));

    if (context[root/"edit mode"] == "place wire")
    { bus(IM("place wire press")); }
    else if (context[root/"edit mode"] == "place input")
    { bus(IM("place input press")); }
    else if (context[root/"edit mode"] == "place output")
    { bus(IM("place output press")); }
    else if (context[root/"edit mode"] == "place const")
    { bus(IM("place const press")); }
    else if (context[root/"edit mode"] == "place delay")
    { bus(IM("place delay press")); }
    else if (context[root/"edit mode"] == "place sum")
    { bus(IM("place sum press")); }
    else if (context[root/"edit mode"] == "place prod")
    { bus(IM("place prod press")); }
    else if (context[root/"edit mode"] == "place function")
    { bus(IM("place function press")); }
    else if (context[root/"edit mode"] == "place code block")
    { bus(IM("place code block press")); }
    else if (context[root/"edit mode"] == "edit properties")
    { bus(IM("edit")); }
    
    bus(IM("cursor update")); }

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
          circuit[root/"outputs"/output/"y"] -= 1; that->redraw(); }
          
        if (context[root/"highlight"/"type"] == "unit")
        { std::string unit = context[root/"highlight"/"unit"];
          circuit[root/"units"/unit/"y"] -= 1; that->redraw(); } } }
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
          circuit[root/"outputs"/output/"y"] += 1; that->redraw(); }

        if (context[root/"highlight"/"type"] == "unit")
        { std::string unit = context[root/"highlight"/"unit"];
          circuit[root/"units"/unit/"y"] += 1; that->redraw(); } } }
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
          circuit[root/"outputs"/output/"x"] -= 1; that->redraw(); }
          
        if (context[root/"highlight"/"type"] == "unit")
        { std::string unit = context[root/"highlight"/"unit"];
          circuit[root/"units"/unit/"x"] -= 1; that->redraw(); } } }
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
          circuit[root/"outputs"/output/"x"] += 1; that->redraw(); }

        if (context[root/"highlight"/"type"] == "unit")
        { std::string unit = context[root/"highlight"/"unit"];
          circuit[root/"units"/unit/"x"] += 1; that->redraw(); } } }
    else { context[root/"cursor pos"/"x"] += 1; bus(IM("cursor update")); } }

  else if (mess == "screen update") { that->redraw(); }

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

    if (!highlight)
    { for (std::string unit : circuit.ls(root/"units"))
      { if (cx == (int)circuit[root/"units"/unit/"x"] &&
            cy == (int)circuit[root/"units"/unit/"y"])
        { context[root/"highlight"/"type"] = "unit";
          context[root/"highlight"/"unit"] = unit;
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
    circuit[root/"inputs"/idx/"name"] =
      std::string("I_").append(std::to_string(idx));
    that->redraw(); }

  else if (mess == "place output press")
  { circuit[root/"outputs"];
    std::list<std::string> outputs = circuit.ls(root/"outputs");
    int idx = outputs.size() ? std::stoi(outputs.back()) + 1 : 0;
    circuit[root/"outputs"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"outputs"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"outputs"/idx/"name"] =
      std::string("O_").append(std::to_string(idx));
    that->redraw(); }

  else if (mess == "place const press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "constant";
    circuit[root/"units"/idx/"value"] = 1.0f;
    that->redraw(); }
    
  else if (mess == "place delay press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "delay";
    circuit[root/"units"/idx/"value"] = 1;
    that->redraw(); }
    
  else if (mess == "place sum press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "sum";
    that->redraw(); }
  
  else if (mess == "place prod press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "product";
    that->redraw(); }
    
  else if (mess == "place function press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "function";
    circuit[root/"units"/idx/"function name"] = "";
    circuit[root/"units"/idx/"numerator poly"] = "1;";
    circuit[root/"units"/idx/"denominator poly"] = "1;";
    that->redraw(); }
    
  else if (mess == "place code block press")
  { circuit[root/"units"];
    std::list<std::string> units = circuit.ls(root/"units");
    int idx = units.size() ? std::stoi(units.back()) + 1 : 0;
    circuit[root/"units"/idx/"x"] = context[root/"cursor pos"/"x"];
    circuit[root/"units"/idx/"y"] = context[root/"cursor pos"/"y"];
    circuit[root/"units"/idx/"type"] = "code block";
    circuit[root/"units"/idx/"source file"] = "";
    circuit[root/"units"/idx/"function name"] = "";
    circuit[root/"units"/idx/"inputs names"] = "";
    circuit[root/"units"/idx/"outputs names"] = "";
    circuit[root/"units"/idx/"context size"] = 0; }
}  

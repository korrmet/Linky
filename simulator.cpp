#include "simulator.hpp"
#include "app.hpp"
#include <cmath>

#define GREEN 0x00AC0000
#define BLUE  0x0000B400
#define LBLUE 0xEEEEFF00
#define RED   0xFF000000
#define BLACK 0x00000000
#define WHITE 0xFFFFFF00

#define CSIZE 5
#define PI 3.141592654

// ---> string printf
#include <cstdio>
#include <string>
#include <cstdarg>
inline void print(std::string& s, const char* fmt, ...)
{ std::string tmp;
  va_list args, args2;
  va_start(args, fmt);
  va_copy(args2, args);
  tmp.resize(vsnprintf(nullptr, 0, fmt, args2) + 1);
  va_end(args2);
  vsprintf(tmp.data(), fmt, args);
  va_end(args);
  tmp.pop_back();
  s.append(tmp); }
// <---

class simulation_database
{ public:
  struct io_data { std::string name; float* values; };

  simulation_database(unsigned int size) : _size(size) { }

  ~simulation_database()
  { for (io_data& item : inputs ) { delete[] item.values; }
    for (io_data& item : outputs) { delete[] item.values; } }

  bool has_i(std::string name)
  { for (io_data& item : inputs) { if (item.name == name) { return true; } }
    return false; }

  io_data& i(std::string name)
  { for (io_data& item : inputs) { if (item.name == name) { return item; } }
    io_data new_item = { .name = name, .values = new float[_size] };
    for (unsigned int i = 0; i < _size; i++) { new_item.values[i] = 0; }
    inputs.push_back(new_item);
    return inputs.back(); }

  bool has_o(std::string name)
  { for (io_data& item : outputs) { if (item.name == name) { return true; } }
    return false; }

  io_data& o(std::string name)
  { for (io_data& item : outputs) { if (item.name ==name) { return item; } }
    io_data new_item = { .name = name, .values = new float[_size] };
    for (unsigned int i = 0; i < _size; i++) { new_item.values[i] = 0; }
    outputs.push_back(new_item);
    return outputs.back(); }

  unsigned int size() { return _size; }

  private:
  unsigned int _size;
  std::list <io_data> inputs, outputs; };

// ---> signals generators
// ---> sine
float
sine(float delay, float period, float v_min, float v_max, float x)
{ if (x < delay) { return v_min; }
  float amplitude = v_max - v_min;
  float k = 2 * PI / period;
  return v_min + (amplitude / 2) * (1 + std::sin(k * (x - delay))); }
// <---

// ---> single step
float
step(float delay, float v_min, float v_max, float x)
{ if (x < delay) { return v_min; } return v_max; }
// <---

// ---> dirac delta function
float
delta(float delay, float width, float v_min, float v_max, float x)
{ if (x >= delay && x < delay + width) { return v_max; } return v_min; }
// <---

// ---> meander
float
meander(float delay, float period, float ratio, float v_min, float v_max,
        float x)
{ if (x < delay) { return v_min; }
  int period_num = (x - delay) / period;
  float x_rel = x - delay - period_num * period;
  if (x_rel < period * ratio) { return v_max; }
  return v_min; }
// <---

// ---> sawtooth "/|"
float
sawtooth(float delay, float period, float v_min, float v_max, float x)
{ if (x < delay) { return v_min; }
  int period_num = (x - delay) / period;
  float x_rel = x - delay - period_num * period;
  float amplitude = v_max - v_min;
  float y = v_min + amplitude * (x_rel / period);
  return y; }
// <---

// ---> reverse sawtooth "|\"
float
rsawtooth(float delay, float period, float v_min, float v_max, float x)
{ if (x < delay) { return v_min; }
  int period_num = (x - delay) / period;
  float x_rel = x - delay - period_num * period;
  float amplitude = v_max - v_min;
  float y = v_max - amplitude * (x_rel / period);
  return y; }
// <---
// <---

// ---> chart constructor
simulator::chart::chart(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h), x_min(0), x_max(0), y_min(), y_max()
{ }
// <---

// ---> chart clear
void simulator::chart::clear()
{ x_min = 0; x_max = 0; y_min = 0; y_max = 0;
  lines.clear(); x_markers.clear(); y_markers.clear(); }
// <---

// ---> chart draw
void simulator::chart::draw()
{ fl_push_clip(x(), y(), w(), h()); fl_line_style(FL_SOLID, 1);
  fl_color(BLACK); fl_rectf(x(), y(), w(), h());

  fl_line_style(FL_SOLID, 1);
  for (line l : lines)
  { if (l.points.size() == 0) { continue; } // empty line, skip it
    fl_color(Fl_Color(l.color));
    for (unsigned int i = 0; i < l.points.size() - 1; i++)
    { fl_line(x_screen(l.points[i    ].x),
              y_screen(l.points[i    ].y),
              x_screen(l.points[i + 1].x),
              y_screen(l.points[i    ].y));
      fl_line(x_screen(l.points[i + 1].x),
              y_screen(l.points[i    ].y),
              x_screen(l.points[i + 1].x),
              y_screen(l.points[i + 1].y));
    } }

  fl_color(WHITE);
  for (float m : x_markers)
  { fl_line(x() + x_screen(m), y(), x() + x_screen(m), y() + h()); }

  for (float m : y_markers)
  { fl_line(x(), y() + y_screen(m), x() + w(), y() + y_screen(m)); }

  fl_line_style(0); fl_color(0); fl_pop_clip(); }
// <---

// ---> chart screen conversions
//      vr_start * vs_end - vs_start * vr_end + vr * (vs_start - vs_end)
// vs = ----------------------------------------------------------------
//      vr_start - vr_end

int simulator::chart::x_screen(float x_real)
{ float xs_start = x(); float xs_end = x() + w();
  float xr_start = x_min; float xr_end = x_max;
  float xs = 0;
  xs += xr_start * xs_end;
  xs += -xs_start * xr_end;
  xs += x_real * (xs_start - xs_end);
  xs /= xr_start - xr_end;
  return (int)xs; }

int simulator::chart::y_screen(float y_real)
{ float ys_start = y() + h(); float ys_end = y();
  float yr_start = y_min; float yr_end = y_max;
  float ys = 0;
  ys += yr_start * ys_end;
  ys += -ys_start * yr_end;
  ys += y_real * (ys_start - ys_end);
  ys /= yr_start - yr_end;
  return (int)ys; }
// <---

// ---> simulator window constructor
simulator::window::window(struct simulator::params sim_params)
: Fl_Window(640, 480, "Simulator"),
  ch(0, 0, 640, 320),
  params(5, 325, 630, 125),
  run_btn(575, 455, 60, 20, "Run"),
  sim_params(sim_params),
  x_max(0), x_min(0), y_max(0), y_min(0), endpoint(0), Ts(0)
{ size_range(640, 480); params.buffer(&buf); set_modal(); show();
  run_btn.callback(run_btn_cb, this); }
// <---

// ---> simulator window resize
void simulator::window::resize(int x, int y, int w, int h)
{ ch.resize(0, 0, w, h - 160);
  params.resize(5, h - 155, w - 10, 125);
  run_btn.resize(w - 65, h - 25, 60, 20); }
// <---

// ---> simulator signal constructor
simulator::window::signal::signal()
: delay(0), v_min(0), v_max(0), period(0), width(0), ratio(0) {}
// <---

// ---> simulator signal clear
void simulator::window::signal::clear()
{ input_name.clear(); type.clear();
  delay = 0; v_min = 0; v_max = 0; period = 0; width = 0; ratio = 0; }
// <---

// ---> simulator line constructor
simulator::window::line::line() : rgb_color(0) {}
// <---

// ---> simulator line clear
void simulator::window::line::clear()
{ name.clear(); type.clear(); rgb_color = 0; }
// <---

// ---> run button callback
void simulator::window::run_btn_cb(Fl_Widget* w, void* arg)
{ window* that = (window*)arg;

  if (!context(ROOT/"solution")) { return; }

  that->lines.clear(); that->ch.lines.clear();
  that->parse(that->buf.text());

  // ---> preparing simulation database
  simulation_database sim_base((that->endpoint / that->Ts) + 1);
  for (std::string item : that->sim_params.inputs ) { sim_base.i(item); }
  for (std::string item : that->sim_params.outputs) { sim_base.o(item); }
  // <---

  // ---> fill database with the input signals
  for (signal& s : that->signals)
  { if (!sim_base.has_i(s.input_name)) { continue; }
    
    for (unsigned int i = 0; i * that->Ts < that->endpoint; i++)
    { float x = that->Ts * i;
      float y = 0;
      if (s.type == "sine")
      { y = sine(s.delay, s.period, s.v_min, s.v_max, x); }
      else if (s.type == "step")
      { y = step(s.delay, s.v_min, s.v_max, x); }
      else if (s.type == "delta")
      { y = delta(s.delay, s.width, s.v_min, s.v_max, x); }
      else if (s.type == "meander")
      { y = meander(s.delay, s.period, s.ratio, s.v_min, s.v_max, x); }
      else if (s.type == "sawtooth")
      { y = sawtooth(s.delay, s.period, s.v_min, s.v_max, x); }
      else if (s.type == "rsawtooth")
      { y = rsawtooth(s.delay, s.period, s.v_min, s.v_max, x); }

      sim_base.i(s.input_name).values[i] = y; } }
  // <---

  float inputs[(int)context[ROOT/"solution"/"inputs num"]];
  float outputs[(int)context[ROOT/"solution"/"outputs num"]];
  float ctx[(int)context[ROOT/"solution"/"context num"]];

  for (unsigned int i = 0;
       i < (int)context[ROOT/"solution"/"inputs num"];
       i++)
  { inputs[i] = 0; }

  for (unsigned int i = 0;
       i < (int)context[ROOT/"solution"/"outputs num"];
       i++)
  { outputs[i] = 0; }

  for (unsigned int i = 0;
       i < (int)context[ROOT/"solution"/"context num"];
       i++)
  { ctx[i] = 0; }

  for (unsigned int i = 0; i < sim_base.size(); i++)
  { unsigned int ctr = 0;
    for (std::string input : that->sim_params.inputs)
    { inputs[ctr] = sim_base.i(input).values[i]; ctr++; }

    for (std::string step : context.ls(ROOT/"solution"/"sequence"))
    { std::string cmd = context[ROOT/"solution"/"sequence"/step/"cmd"];

      if (cmd == "net = input")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int input = context[ROOT/"solution"/"sequence"/step/"input"];
        ctx[net] = inputs[input]; }
      
      else if (cmd == "output = net")
      { int output = context[ROOT/"solution"/"sequence"/step/"output"];
        int net = context[ROOT/"solution"/"sequence"/step/"net"];
        outputs[output] = ctx[net]; }
      
      else if (cmd == "net = value")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        float value = context[ROOT/"solution"/"sequence"/step/"value"];
        context[net] = value; }
      
      else if (cmd == "net = net1 + net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net] = ctx[net1] + ctx[net2]; }
      
      else if (cmd == "net = net1 - net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net] = ctx[net1] - ctx[net2]; }
      
      else if (cmd == "net = net1 * net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net] = ctx[net1] * ctx[net2]; }
      
      else if (cmd == "net = net1 / net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net] = ctx[net1] / ctx[net2]; }
      
      else if (cmd == "net = unit[num]")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        ctx[net] = ctx[unit_num]; }
      
      else if (cmd == "unit[num1] = unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        ctx[unit_num1] = ctx[unit_num2]; }
      
      else if (cmd == "unit[num] = net")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        int net = context[ROOT/"solution"/"sequence"/step/"net"];
        ctx[unit_num] = ctx[net]; }
      
      else if (cmd == "unit[num] = val")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[unit_num] = val; }
      
      else if (cmd == "unit[num1] += val * unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[unit_num1] += val * ctx[unit_num2]; }
      
      else if (cmd == "unit[num1] -= val * unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[unit_num1] -= val * ctx[unit_num2]; }
      
      else if (cmd == "unit[num] /= val")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[unit_num] /= val; }
      
      else if (cmd == "net1 = net2")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net1] = ctx[net2]; }
      
      else if (cmd == "net1 = val * net2")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[net1] = val * ctx[net2]; }
      
      else if (cmd == "net1 = net2 * (net2 > 0)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net1] = ctx[net2] * (ctx[net2] > 0); }
      
      else if (cmd == "net1 -= net2 * (net2 < 0)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net1] -= ctx[net2] * (ctx[net2] < 0); }
      
      else if (cmd == "net1 = val * (net2 >= val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[net1] = val * (ctx[net2] >= val); }
      
      else if (cmd == "net1 += net2 * (net2 < val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[net1] += ctx[net2] * (ctx[net2] < val); }
      
      else if (cmd == "net1 = val * (net2 <= val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[net1] = val * (ctx[net2] <= val); }
      
      else if (cmd == "net1 += net2 * (net2 > val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        ctx[net1] += ctx[net2] * (ctx[net2] > val); }
    
      else if (cmd == "net1 = net2 > net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        ctx[net1] = ctx[net2] > ctx[net3]; }

      else if (cmd == "net1 = net2 < net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        ctx[net1] = ctx[net2] < ctx[net3]; }
      
      else if (cmd == "net1 = net2 == net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        ctx[net1] = ctx[net2] == ctx[net3]; }

      else if (cmd == "net1 = net2 <= 0")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        ctx[net1] = ctx[net2] <= 0; } }

    ctr = 0;
    for (std::string output : that->sim_params.outputs)
    { sim_base.o(output).values[i] = outputs[ctr]; ctr++; } }

  // ---> push the simulation data to the chart
  for (line& l : that->lines)
  { chart::line chl;
    chl.color = l.rgb_color;
    if (l.type == "input" && sim_base.has_i(l.name))
    { for (unsigned int i = 0; i < sim_base.size(); i++)
      { chart::point p = { .x = i * that->Ts,
                           .y = sim_base.i(l.name).values[i] };
        chl.points.push_back(p); } }
    else if (l.type == "output" && sim_base.has_o(l.name))
    { for (unsigned int i = 0; i < sim_base.size(); i++)
      { chart::point p = { .x = i * that->Ts,
                           .y = sim_base.o(l.name).values[i] };
        chl.points.push_back(p); } }

    that->ch.lines.push_back(chl); }
  // <---
  
  // ---> preparing chart to display
  that->ch.x_min = that->x_min;
  that->ch.x_max = that->x_max;
  that->ch.y_min = that->y_min;
  that->ch.y_max = that->y_max;

  that->ch.x_markers.clear();
  that->ch.y_markers.clear();

  for (float m : that->x_markers) { that->ch.x_markers.push_back(m); }
  for (float m : that->y_markers) { that->ch.y_markers.push_back(m); }
  // <---

  that->ch.redraw(); }
// <---

// ---> simulation parameters parser
void simulator::window::parse(std::string str)
{ signals.clear(); lines.clear(); x_markers.clear(); y_markers.clear();
  x_max = 0; x_min = 0; y_max = 0; y_min = 0; endpoint = 0; Ts = 0;

  std::string cwd, com, par; signal csg; line cln;

  for (char c : str)
  { if (c == '\n')
    { if (com == "signal") { signals.push_back(csg); csg.clear(); }
      else if (com == "line") { lines.push_back(cln); cln.clear(); }
      com.clear(); par.clear(); cwd.clear(); }

    else if (c == ':') { com = cwd; cwd.clear(); }

    else if (c == ';')
    { if (com == "parameters")
      { if      (par == "x_max"   ) { x_max = std::stof(cwd); }
        else if (par == "x_min"   ) { x_min = std::stof(cwd); }
        else if (par == "y_max"   ) { y_max = std::stof(cwd); }
        else if (par == "y_min"   ) { y_min = std::stof(cwd); }
        else if (par == "endpoint") { endpoint = std::stof(cwd); }
        else if (par == "Ts"      ) { Ts = std::stof(cwd); } }

      else if (com == "x_markers")
      { float marker = std::stof(cwd); x_markers.push_back(marker); }

      else if (com == "y_markers")
      { float marker = std::stof(cwd); y_markers.push_back(marker); }

      else if (com == "signal")
      { if      (par == "name"  ) { csg.input_name = cwd; }
        else if (par == "type"  ) { csg.type = cwd; }
        else if (par == "delay" ) { csg.delay = std::stof(cwd); }
        else if (par == "v_min" ) { csg.v_min = std::stof(cwd); }
        else if (par == "v_max" ) { csg.v_max = std::stof(cwd); }
        else if (par == "period") { csg.period = std::stof(cwd); }
        else if (par == "width" ) { csg.width = std::stof(cwd); }
        else if (par == "ratio" ) { csg.ratio = std::stof(cwd); } }

      else if (com == "line")
      { if      (par == "name" ) { cln.name = cwd; }
        else if (par == "type" ) { cln.type = cwd; }
        else if (par == "color") { cln.rgb_color = std::stoi(cwd, 0, 16);
                                   cln.rgb_color <<= 8;
                                   cln.rgb_color &= 0xFFFFFF00; } }

      cwd.clear(); par.clear(); }

    else if (c == '=') { par = cwd; cwd.clear(); }

    else { cwd.push_back(c); } } }
// <---

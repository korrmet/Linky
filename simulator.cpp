#include "simulator.hpp"

#define GREEN 0x00AC0000
#define BLUE  0x0000B400
#define LBLUE 0xEEEEFF00
#define RED   0xFF000000
#define BLACK 0x00000000
#define WHITE 0xFFFFFF00

#define CSIZE 5

class simulation_database
{ public:
  struct io_data { std::string name; float* values; };

  simulation_database(unsigned int size) : _size(size) { }

  ~simulation_database()
  { for (io_data& item : inputs ) { delete[] item.values; }
    for (io_data& item : outputs) { delete[] item.values; }
  }

  bool has_i(std::string name)
  { for (io_data& item : inputs) { if (item.name == name) { return true; } }
    return false; }

  io_data& i(std::string name)
  { for (io_data& item : inputs) { if (item.name == name) { return item; } }
    io_data new_item = { .name = name, .values = new float[_size] };
    inputs.push_back(new_item);
    return inputs.back(); }

  bool has_o(std::string name)
  { for (io_data& item : outputs) { if (item.name == name) { return true; } }
    return false; }

  io_data& o(std::string name)
  { for (io_data& item : outputs) { if (item.name ==name) { return item; } }
    io_data new_item = { .name = name, .values = new float[_size] };
    outputs.push_back(new_item);
    return outputs.back(); }

  unsigned int size() { return _size; }

  private:
  unsigned int _size;
  std::list <io_data> inputs, outputs; };

// ---> signals generators
float sine(float delay, float amp, float period, float x) { return 0; }
float step(float delay, float y0, float y1, float x) { return 0; }
float delta(float delay, float y0, float y1, float x) { return 0; }
float meander(float delay, float amp, float period, float ratio, float x) { return 0; }
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
  { fl_color(Fl_Color(l.color));
    for (unsigned int i = 0; i < l.points.size() - 1; i++)
    { fl_line(x_screen(l.points[i    ].x),
              y_screen(l.points[i    ].y),
              x_screen(l.points[i + 1].x),
              y_screen(l.points[i + 1].y)); } }

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
{ size_range(640, 480); params.buffer(&buf);
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
: delay(0), amplitude(0), period(0), width(0), ratio(0) {}
// <---

// ---> simulator signal clear
void simulator::window::signal::clear()
{ input_name.clear(); type.clear();
  delay = 0; amplitude = 0; period = 0; width = 0; ratio = 0; }
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
  that->x_max = 0; that->x_min = 0;
  that->y_max = 0; that->y_min = 0;
  that->Ts = 0;
  that->parse(that->buf.text());
  
  that->ch.x_min = that->x_min;
  that->ch.x_max = that->x_max;
  that->ch.y_min = that->y_min;
  that->ch.y_max = that->y_max;

  that->ch.x_markers.clear();
  that->ch.y_markers.clear();

  for (float m : that->x_markers) { that->ch.x_markers.push_back(m); }
  for (float m : that->y_markers) { that->ch.y_markers.push_back(m); }

  chart::line l;
  chart::point p;

  l.color = 0xFF000000;
  p.x = 0 * that->Ts; p.y = 1; l.points.push_back(p);
  p.x = 1 * that->Ts; p.y = 1; l.points.push_back(p);
  p.x = 2 * that->Ts; p.y = 1; l.points.push_back(p);
  p.x = 3 * that->Ts; p.y = 1; l.points.push_back(p);
  that->ch.lines.push_back(l); l.points.clear();

  l.color = 0x00FF0000;
  p.x = 0 * that->Ts; p.y = 1; l.points.push_back(p);
  p.x = 1 * that->Ts; p.y = 2; l.points.push_back(p);
  p.x = 2 * that->Ts; p.y = 4; l.points.push_back(p);
  p.x = 3 * that->Ts; p.y = 8; l.points.push_back(p);
  that->ch.lines.push_back(l); l.points.clear();

  l.color = 0x0000FF00;
  p.x = 0 * that->Ts; p.y = 1;  l.points.push_back(p);
  p.x = 1 * that->Ts; p.y = 3;  l.points.push_back(p);
  p.x = 2 * that->Ts; p.y = 9;  l.points.push_back(p);
  p.x = 3 * that->Ts; p.y = 21; l.points.push_back(p);
  that->ch.lines.push_back(l); l.points.clear();

  // ---> preparing simulation database
  simulation_database sim_base((that->endpoint / that->Ts) + 1);
  for (std::string item : that->sim_params.inputs ) { sim_base.i(item); }
  for (std::string item : that->sim_params.outputs) { sim_base.o(item); }
  // <---

  // ---> fill database with the input signals
  for (signal& s : that->signals)
  { if (!sim_base.has_i(s.input_name)) { continue; }
  }
  // <---
  
  // ---> generating simulation harness source file
  // <---

  // ---> compiling the sources to the shared library for simulation core
  // <---

  // ---> loading compiled library
  // <---

  // ---> run the simulation using signals values
  // <---

  // ---> push the simulation data to the chart
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
      { if      (par == "name"     ) { csg.input_name = cwd; }
        else if (par == "type"     ) { csg.type = cwd; }
        else if (par == "delay"    ) { csg.delay = std::stof(cwd); }
        else if (par == "amplitude") { csg.amplitude = std::stof(cwd); }
        else if (par == "period"   ) { csg.period = std::stof(cwd); }
        else if (par == "width"    ) { csg.width = std::stof(cwd); }
        else if (par == "ratio"    ) { csg.ratio = std::stof(cwd); } }

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

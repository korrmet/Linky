#include "simulator.hpp"

#define GREEN 0x00AC0000
#define BLUE  0x0000B400
#define LBLUE 0xEEEEFF00
#define RED   0xFF000000
#define BLACK 0x00000000
#define WHITE 0xFFFFFF00

#define CSIZE 5

simulator::chart::chart(int x, int y, int w, int h)
: Fl_Widget(x, y, w, h)
{
}

void simulator::chart::draw()
{ fl_push_clip(x(), y(), w(), h()); fl_line_style(FL_SOLID, 1);
  fl_color(BLACK); fl_rectf(x(), y(), w(), h());

  fl_line_style(0); fl_color(0); fl_pop_clip(); }

simulator::window::window()
: Fl_Window(640, 480, "Simulator"),
  ch(0, 0, 640, 320),
  params(5, 325, 630, 125),
  run_btn(575, 455, 60, 20, "Run"),
  start(0), stop(0), Ts(0)
{ size_range(640, 480); params.buffer(&buf); }

void simulator::window::resize(int x, int y, int w, int h)
{ ch.resize(0, 0, w, h - 160);
  params.resize(5, h - 155, w - 10, 125);
  run_btn.resize(w - 65, h - 25, 60, 20); }

simulator::window::signal::signal()
: delay(0), amplitude(0), period(0), width(0), ratio(0) {}

void simulator::window::signal::clear()
{ input_name.clear(); type.clear();
  delay = 0; amplitude = 0; period = 0; width = 0; ratio = 0; }

simulator::window::line::line() : rgb_color(0) {}

void simulator::window::line::clear()
{ name.clear(); type.clear(); rgb_color = 0; }

void simulator::window::parse(std::string str)
{ signals.clear(); lines.clear(); markers.clear();
  start = 0; stop = 0; Ts = 0;

  std::string cwd, com, par; signal csg; line cln;

  for (char c : str)
  { if (c == '\n')
    { if (com == "signal") { signals.push_back(csg); csg.clear(); }
      else if (com == "line") { lines.push_back(cln); cln.clear(); }
      com.clear(); par.clear(); cwd.clear(); }

    else if (c == ':') { com = cwd; cwd.clear(); }

    else if (c == ';')
    { if (com == "parameters")
      { if (par == "start") { start = std::stof(cwd); }
        else if (par == "stop") { stop = std::stof(cwd); }
        else if (par == "Ts") { Ts = std::stof(cwd); } }

      else if (com == "markers")
      { float marker = std::stof(cwd); markers.push_back(marker); }

      else if (com == "signal")
      { if (par == "name") { csg.input_name = cwd; }
        else if (par == "type") { csg.type = cwd; }
        else if (par == "delay") { csg.delay = std::stof(cwd); }
        else if (par == "amplitude") { csg.amplitude = std::stof(cwd); }
        else if (par == "period") { csg.period = std::stof(cwd); }
        else if (par == "width") { csg.width = std::stof(cwd); }
        else if (par == "ratio") { csg.ratio = std::stof(cwd); } }

      else if (com == "line")
      { if (par == "name") { cln.name = cwd; }
        else if (par == "type") { cln.type = cwd; }
        else if (par == "color") { cln.rgb_color = std::stoi(cwd, 0, 16); } }

      cwd.clear(); par.clear(); }

    else if (c == '=') { par = cwd; cwd.clear(); }

    else { cwd.push_back(c); } } }

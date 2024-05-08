#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Button.H>
#include "app.hpp"

namespace simulator {

class chart : public Fl_Widget
{ public:
  chart(int x, int y, int w, int h);
  virtual void draw() override; };

class window : public Fl_Window
{ public:
  window();

  virtual void resize(int x, int y, int w, int h);
  chart ch; Fl_Text_Editor params; Fl_Text_Buffer buf; Fl_Button run_btn;

  private:
  struct signal
  { signal();
    void clear();
    std::string input_name;
    std::string type;
    float delay;
    float amplitude;
    float period;
    float width;
    float ratio; };

  struct line
  { line();
    void clear();
    std::string name;
    std::string type;
    unsigned int rgb_color; };

  std::list <signal> signals;
  std::list <line> lines;
  std::list <float> markers;
  float start;
  float stop;
  float Ts;

  void parse(std::string str); };
}

#endif // SIMULATOR_HPP

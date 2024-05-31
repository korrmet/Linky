#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Button.H>
#include <string>
#include <vector>
#include <list>

namespace simulator {

// ---> simulation input parameters
struct params
{ std::string circuit_name; // equal to the function name and file name
  
  // size of this list is equal <name>_inputs_size macro definition
  // position of the input inside this list is equal to the macro definitions
  std::list <std::string> inputs; 

  // size of this list is equal <name>_outputs_size macro definition
  // position of the output inside this list is equal to the macro definitions
  std::list <std::string> outputs;

  unsigned int context_size; };
// <---

// ---> simulation output chart
class chart : public Fl_Widget
{ public:
  chart(int x, int y, int w, int h);
  virtual void draw() override;
  void clear();
  
  struct point { float x; float y; };
  struct line { unsigned int color; std::vector <point> points; };
  std::list <line> lines;
  std::list <float> x_markers, y_markers;
  float x_min, x_max, y_min, y_max;

  private:
  int x_screen(float x_real);
  int y_screen(float y_real); };
// <---

// ---> simulator window
class window : public Fl_Window
{ public:
  window(struct params sim_params);

  virtual void resize(int x, int y, int w, int h);
  chart ch; Fl_Text_Editor params; Fl_Text_Buffer buf; Fl_Button run_btn;

  private:
  struct params sim_params;

  struct signal
  { signal();
    void clear();
    std::string input_name;
    std::string type;
    float delay;
    float v_min;
    float v_max;
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
  std::list <float> x_markers;
  std::list <float> y_markers;
  float x_max, x_min, y_max, y_min; float endpoint;
  float Ts;
  
  static void run_btn_cb(Fl_Widget* w, void* arg);

  void parse(std::string str); };
// <---

}

#endif // SIMULATOR_HPP

#include "app.hpp"
#include "editor.hpp"
#include "generator.hpp"
#include "simulator.hpp"
#include "independency.hpp"
#include <FL/Fl.H>
#include <fstream>
#include <sstream>

independency::bus bus;
independency::storage circuit;
independency::storage context;
independency::storage params;

namespace app {

void handler(void* ctx, IM mess);

}

int main(int argc, char** argv)
{ bus + app::handler;
  bus + generator::handler;
  PRINT("Linky v0\n");
  
  if (argc > 1) { bus(IM("file open") << IV("path", argv[1])); }
  if (argc > 2 && std::string("--generate") == argv[2])
  { bus(IM("check circuit errors"));
    bus(IM("generate code"));
    return 0; }

  editor::window main_window;
  // ---> for debug purpose only
/*
parameters:x_min=0;x_max=10;y_min=-1;y_max=1;endpoint=10;Ts=0.01;
y_markers:0;
signal:name=I_0;type=sine;period=2;v_min=-1;v_max=1;
line:name=I_0;type=input;color=FFFFFF;
line:name=O_0;type=output;color=0000FF;
line:name=O_1;type=output;color=00FFFF;
line:name=O_2;type=output;color=00FF00;
line:name=O_3;type=output;color=FFFF00;
*/
  simulator::params sp;
  sp.circuit_name = "functions";
  sp.source_files.push_back("functions.c");
  sp.context_size = 11;
  sp.inputs.push_back("I_0");
  sp.outputs.push_back("O_0");
  sp.outputs.push_back("O_1");
  sp.outputs.push_back("O_2");
  sp.outputs.push_back("O_3");
  simulator::window sim(sp);
  sim.show();
  // <---
  Fl::run(); return 0; }

namespace app {

void handler(void* ctx, IM mess)
{ if (mess == "file open")
  { std::ifstream file(mess["path"]);
    if (!file.is_open())
    { PRINT("Unable to open %s, skipping ...\n",
            ((std::string)mess["path"]).c_str());
      return; }
    std::stringstream data; data << file.rdbuf(); file.close();
    circuit.parse(data.str());
    context[ROOT / "circuit file path"] = (std::string)mess["path"];
    bus(IM("circuit file parsed")); }

  else if (mess == "file save")
  { context[ROOT / "circuit file path"] = (std::string)mess["path"];
    std::ofstream file(mess["path"]);
    if (!file.is_open())
    { PRINT("Can't save %s\n", ((std::string)mess["path"]).c_str());
      return; }
    file << circuit.serialize(); file.close(); } }

}

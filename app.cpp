#include "app.hpp"
#include "editor.hpp"
#include "independency.hpp"
#include <FL/Fl.H>
#include <fstream>
#include <sstream>

independency::bus bus;
independency::storage circuit;
independency::storage context;
independency::storage params;

void app_handler(void* ctx, IM mess);

int main(int argc, char** argv)
{ bus + app_handler;
  PRINT("Linky v0\n");
  
  if (argc > 1) { bus(IM("file open") << IV("name", argv[1])); }
  params[root / "scale factor"] = 1;

  editor::window main_window;
  Fl::run(); return 0; }

void app_handler(void* ctx, IM mess)
{ if (mess == "file open")
  { std::ifstream file(mess["name"]);
    if (!file.is_open())
    { PRINT("Unable to open %s, skipping ...\n",
            ((std::string)mess["name"]).c_str());
      return; }
    std::stringstream data; data << file.rdbuf(); file.close();
    circuit.parse(data.str());
    params[root / "circuit file path"] = (std::string)mess["name"];
    bus(IM("circuit file parsed")); }

  else if (mess == "file save")
  { params[root / "circuit file path"] = (std::string)mess["path"];
    std::ofstream file(mess["path"]);
    if (!file.is_open())
    { PRINT("Can't save %s\n", ((std::string)mess["path"]).c_str());
      return; }
    file << circuit.serialize(); file.close(); } }

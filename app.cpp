#include "app.hpp"
#include "editor.hpp"
#include "generator.hpp"
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

static std::string home_path()
{
  std::string result;
#ifdef LINUX
  result.append(std::getenv("HOME"));
#endif

#ifdef WINDOWS
  result.append(std::getenv("HOMEDRIVE"))
        .append(std::getenv("HOMEPATH"));
#endif
  return result;
}

int main(int argc, char** argv)
{ bus + app::handler;
  bus + generator::handler;

  std::fstream settings_file(home_path().append("/.linkyrc"));
  if (!settings_file.good()) { params[ROOT/"compiler path"] = "gcc"; 
                               settings_file.close(); }
  else
  { std::stringstream data; data << settings_file.rdbuf();
    settings_file.close();
    params.parse(data.str()); }
  
  if (argc > 1) { bus(IM("file open") << IV("path", argv[1])); }
  if (argc > 2 && std::string("--generate") == argv[2])
  { bus(IM("check circuit errors"));
    bus(IM("generate code"));
    return 0; }

  editor::window main_window;
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
    file << circuit.serialize(); file.close(); }

  else if (mess == "settings file save")
  { std::ofstream file(home_path().append("/.linkyrc"));
    if (!file.is_open()) { PRINT("Can't save settings to file.\n"); return; }
    file << params.serialize(); file.close(); } }

}

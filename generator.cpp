#include "generator.hpp"

void generator::handler(void* ctx, IM mess)
{ // ---> generate code
  if (mess == "generate code")
  { bus(IM("check circuit errors"));
    if (circuit.ls(root/"network errors").size()) { return; }
    
    // ---> code generation routine
    // <---
  }
  // <---

  // ---> check circuit errors
  else if (mess == "check circuit errors")
  { context.del(root/"network");
    context.del(root/"network errors");
    bus(IM("make network"));
    bus(IM("scan network"));
    bus(IM("check network")); }
  // <---
  
  // ---> make network
  else if (mess == "make network")
  { std::list<std::string> wires = circuit.ls(root/"wires");
    while (wires.size())
    { std::list<std::string> net;

      net.push_back(wires.front()); wires.pop_front();
      bool added;
      do
      { added = false;
        for (std::string nw : net)
        { for (std::string w : wires)
          { if ( ( circuit[root/"wires"/w /0/"x"] ==
                   circuit[root/"wires"/nw/0/"x"] &&
                   circuit[root/"wires"/w /0/"y"] ==
                   circuit[root/"wires"/nw/0/"y"])
                 ||
                 ( circuit[root/"wires"/w /0/"x"] ==
                   circuit[root/"wires"/nw/1/"x"] &&
                   circuit[root/"wires"/w /0/"y"] ==
                   circuit[root/"wires"/nw/1/"y"])
                 ||
                 ( circuit[root/"wires"/w /1/"x"] ==
                   circuit[root/"wires"/nw/0/"x"] &&
                   circuit[root/"wires"/w /1/"y"] ==
                   circuit[root/"wires"/nw/0/"y"])
                 ||
                 ( circuit[root/"wires"/w /1/"x"] ==
                   circuit[root/"wires"/nw/1/"x"] &&
                   circuit[root/"wires"/w /1/"y"] ==
                   circuit[root/"wires"/nw/1/"y"]))
            { net.push_back(w); wires.remove(w); added = true; break; } }
          if (added) { break; } }
      } while (added);

      std::list<std::string> nets = context.ls(root/"network");
      int idx = nets.size() ? std::stoi(nets.back()) + 1 : 0;
      for (std::string nwire : net)
      { context[root/"network"/idx/"wires"/nwire]; } } }
  // <---

  // ---> scan network
  else if (mess == "scan network")
  { struct point { int x; int y; };
    for (std::string net : context.ls(root/"network"))
    { // ---> generating list of points
      std::list<point> points;
      for (std::string wire : context.ls(root/"network"/net/"wires"))
      { point p;
        p.x = (int)circuit[root/"wires"/wire/0/"x"];
        p.y = (int)circuit[root/"wires"/wire/0/"y"];
        points.push_back(p);
        p.x = (int)circuit[root/"wires"/wire/1/"x"];
        p.y = (int)circuit[root/"wires"/wire/1/"y"];
        points.push_back(p); }
      // <---

      // ---> scanning circuit inputs
      for (std::string input : circuit.ls(root/"inputs"))
      { for (point p : points)
        { if (p.x == (int)circuit[root/"inputs"/input/"point"/"x"] &&
              p.y == (int)circuit[root/"inputs"/input/"point"/"y"])
          { context[root/"network"/net/"inputs"/input]; break; } } }
      // <---

      // ---> scanning circuit outputs
      for (std::string output : circuit.ls(root/"outputs"))
      { for (point p : points)
        { if (p.x == (int)circuit[root/"outputs"/output/"point"/"x"] &&
              p.y == (int)circuit[root/"outputs"/output/"point"/"y"])
          { context[root/"network"/net/"outputs"/output]; break; } } }
      // <---

      // ---> scanning units
      for (std::string unit : circuit.ls(root/"units"))
      { for (std::string input : circuit.ls(root/"units"/unit/"inputs"))
        { for (point p : points)
          { if (p.x == (int)circuit[root/"units"/unit/"inputs"/input/"x"] &&
                p.y == (int)circuit[root/"units"/unit/"inputs"/input/"y"])
            { context[root/"network"/net/"units"/unit/"inputs"/input]; } } }

        for (std::string output : circuit.ls(root/"units"/unit/"outputs"))
        { for (point p : points)
          { if (p.x == (int)circuit[root/"units"/unit/"outputs"/output/"x"] &&
                p.y == (int)circuit[root/"units"/unit/"outputs"/output/"y"])
            { context[root/"network"/net/"units"/unit/"outputs"/output]; } } } }
      // <---
    } }
  // <---
  
  // ---> check network
  if (mess == "check network")
  { for (std::string net : context.ls(root/"network"))
    { int logical_out_count = 0;
      logical_out_count += context.ls(root/"network"/net/"inputs").size();
      for (std::string unit : context.ls(root/"network"/net/"units"))
      { logical_out_count
          += context.ls(root/"network"/net/"units"/unit/"outputs").size(); }
      if (logical_out_count != 1)
      { context[root/"network errors"/net]; } } }
  // <---
}

#include "generator.hpp"

// Calculations order
// Algorithm is:
// 1) All of the units at start have unresolved state, except of:
//    - circuit inputs
//    - constants
//    - delay lines
//    - functions with maximum order lower than z-1
// 2) Run through the all of the unit list check is it solved:
//    - circuit inputs are always solved
//    - delay lines are always solved (actually no, but they consume
//      initial zero signal so, it's okay)
//    - functionw with maximum numerator order equal or lower than z-1
//      (in other words k*z0 -> k == 0)
//    - all of the units that have solved all of their inputs
// 3) Mark solved units and increment the iteration counter
// 4) Add marked unit if appears to the list
// 5) If unit have output, run through the connected network and
//    mark the connected inputs of other units
// 6) Check the iteration counter, if no units marked this iteration,
//    stop algorithm
// 7) If all of the units are marked, output list contains valid
//    calculations order. Otherwise you have an error.

// Example usage:
// [in]-->(+)-----------*-->[k]-->[out]
//  |            |
//         +--[delay]<--+
//   circuit_calculator c;
//   c.u("in").o("val", true)
//    .u("sum").i("in 1").i("in 2").o("out")
//    .u("delay").i("in").o("out", true)
//    .u("k").i("in").o("out")
//    .u("out").i("val")
//    .connect("in", "val", "input")
//    .connect("sum", "in 1", "input")
//    .connect("sum", "out", "nogain")
//    .connect("k", "in", "nogain")
//    .connect("k", "out", "output")
//    .connect("out", "val", "output")
//    .connect("delay", "in", "nogain")
//    .connect("delay", "out", "loopback")
//    .connect("sum", "in 2", "loopback");
//
//  std::list<std::string> solving_order;
//  c.schedule(solving_order);
//
//  WARNING: did not check when one output connected to multiple output !!!
class circuit_calculator
{ public:

  circuit_calculator& u(std::string id)
  { unit u; u.id = id; units.push_back(u); return *this; }

  circuit_calculator& i(std::string id)
  { unit::pin p; p.id = id; units.back().i.push_back(p); return *this; }

  circuit_calculator& o(std::string id, bool active = false)
  { unit::pin p; p.id = id; p.active = active;
    units.back().o.push_back(p); return *this; }

  circuit_calculator& connect(std::string uid,
                              std::string pid,
                              std::string nid)
  { unit* u = nullptr;
    if (!u)
    { for (unit& i : units) { if (i.id == uid) { u = &i; break; } } }
    if (!u) { return *this; }

    unit::pin* p = nullptr;
    if (!p)
    { for (unit::pin& i : u->i) { if (i.id == pid) { p = &i; break; } } }
    if (!p)
    { for (unit::pin& i : u->o) { if (i.id == pid) { p = &i; break; } } }
    if (!p) { return *this; }

    p->nid = nid;

    return *this; }

  // TODO: add unresolved list to mark errant units
  bool schedule(std::list<std::string>& s)
  { unsigned int count = 0;

    do { count = 0;
      for (unit& u : units)
      { if (u.solved) { continue; }

        unsigned int icount = 0;
        for (unit::pin& p : u.i)
        { unit::pin source;
          if (!output(p.nid, source)) { break; }
          if (!source.active) { break; }
          p.active = true;
          icount++; }

        if (icount == u.i.size())
        { u.solved = true;
          for (unit::pin& p : u.o)
          { p.active = true; } }

      if (u.solved) { s.push_back(u.id); count++; } }
    } while (count);

    unsigned int ucount = 0;
    for (unit& u : units) { if (u.solved) { ucount++; } }

    if (ucount != units.size()) { return false; }

    return true; }

  struct unit
  { struct pin { pin() : active(false) {}
                 bool active; std::string id; std::string nid; };
    std::string id;
    bool solved;
    std::list<pin> i, o; };

  private:
  std::list<unit> units;
  bool inputs(std::string nid, std::list<unit::pin*> pins)
  { bool res = false;

    for (unit& u : units)
    { for (unit::pin& p : u.i)
      { if (p.nid == nid) { pins.push_back(&p); }
        res = true; } }

    return res; }

  bool output(std::string nid, unit::pin& pin)
  { for (unit& u : units)
    { for (unit::pin& p : u.o)
      { if (p.nid == nid) { pin = p; return true; } } }

    return false; }
};

void generator::handler(void* ctx, IM mess)
{ // ---> generate code
  if (mess == "generate code")
  { bus(IM("check circuit errors"));
    if (circuit.ls(root/"network errors").size()) { return; }
    
    // ---> code generation routine

    // prepare list of the units
    // TODO: commented lines below may be not needed, but don't sure now
    context.del(root/"calc list in");
    for (std::string input : circuit.ls(root/"inputs"))
    { int idx = context.ls(root/"calc list in").size();
      context[root/"calc list in"/idx/"type"] = "input";
      context[root/"calc list in"/idx/"id"] = input; }

    for (std::string output : circuit.ls(root/"outputs"))
    { int idx = context.ls(root/"calc list in").size();
      context[root/"calc list in"/idx/"type"] = "output";
      context[root/"calc list in"/idx/"id"] = output; }

    for (std::string unit : circuit.ls(root/"units"))
    { int idx = context.ls(root/"calc list in").size();
      context[root/"calc list in"/idx/"type"]
        = circuit[root/"units"/unit/"type"];
      context[root/"calc list in"/idx/"id"] = unit; }

    circuit_calculator c;
    for (std::string uid : context.ls(root/"calc list in"))
    { c.u(uid);
      std::string type = context[root/"calc list in"/uid/"type"];
      std::string id   = context[root/"calc list in"/uid/"id"];

      if (type == "input")         {         c.o("o0", true); }
      else if (type == "output")   { c.i("i0");               }
      else if (type == "delay")    { c.i("i0").o("o0", true); }
      else if (type == "constant") {         c.o("o0", true); }
      else if (type == "function") { c.i("i0").o("o0"      ); }
      else
      { for (std::string input : circuit.ls(root/"units"/id/"inputs"))
        { c.i(std::string("i").append(input)); }

        for (std::string output : circuit.ls(root/"units"/id/"outputs"))
        { c.o(std::string("o").append(output)); } } }

    // TODO: run through the network, find out all of UID's and push
    // it to the calculator using connect method
    for (std::string net : context.ls(root/"network"))
    {
    }
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
  // Every network should contain exactly one logical output and at least one
  // logical input. Circuit input are logical outputs because of in the
  // circuit's point of view it's a source of the signals, and circuit
  // outputs are logical inputs because it's a data consumer.
  if (mess == "check network")
  { for (std::string net : context.ls(root/"network"))
    { int logical_out_count = 0;
      logical_out_count += context.ls(root/"network"/net/"inputs").size();
      for (std::string unit : context.ls(root/"network"/net/"units"))
      { logical_out_count
          += context.ls(root/"network"/net/"units"/unit/"outputs").size(); }
      if (logical_out_count != 1)
      { context[root/"network errors"/net]; continue; }
    
      int logical_in_count = 0;
      logical_in_count += context.ls(root/"network"/net/"outputs").size(); 
      for (std::string unit : context.ls(root/"network"/net/"units"))
      { logical_in_count
          += context.ls(root/"network"/net/"units"/unit/"inputs").size(); }
      if (logical_out_count == 0)
      { context[root/"network errors"/net]; continue; } } }
  // <---
}

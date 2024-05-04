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

class circuit_calculator
{ public:

  /** \brief Adds unit to the calculator
   *  \param id Identifier of the unit, should be unique for each */
  circuit_calculator& u(std::string id)
  { unit u; u.id = id; units.push_back(u); return *this; }

  /** \brief Adds input to last added unit
   *  \param id Identifier of the input, should be unique pin name inside
   *            the unit */
  circuit_calculator& i(std::string id)
  { unit::pin p; p.id = id; units.back().i.push_back(p); return *this; }

  /** \brief Adds output to last added unit
   *  \param id     Identifier of the output, should be unique pin name
   *                inside the unit
   *  \param active This flag says that this output is alerady solved */
  circuit_calculator& o(std::string id, bool active = false)
  { unit::pin p; p.id = id; p.active = active;
    units.back().o.push_back(p); return *this; }

  /** \brief Connects unit pin to the net
   *  \param uid Unit identifier string
   *  \param pid Pin identifier string
   *  \param nid Net identifier string */
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

  /** \brief Make sequence of units calculation
   *  \param s Reference to the list of strings to store the sequence
   *  \retrun Result of scheduling
   *  \retval true Calculations was performed well, the list of strings
   *               contains right sequence of the units
   *  \retval false Something went wrong, some units can't be calculated
   *                because of the possible loops */
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
          p.active = true; icount++; }

        if (icount == u.i.size())
        { u.solved = true; for (unit::pin& p : u.o) { p.active = true; } }

        if (u.solved) { s.push_back(u.id); count++; } }
    } while (count);

    unsigned int ucount = 0;
    for (unit& u : units) { if (u.solved) { ucount++; } }
    if (ucount != units.size()) { return false; }
    return true; }

  struct unit
  { struct pin { pin() : active(false) {}
                 bool active; std::string id; std::string nid; };
    unit() : solved(false) {}
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
    if (context.ls(root/"network errors").size()) { return; }
    if (context.ls(root/"sequence errors").size()) { return; }
    
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
    bus(IM("check network"));
    bus(IM("make sequence"));
    bus(IM("check sequence")); }
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
      if (logical_in_count == 0)
      { context[root/"network errors"/net]; continue; } } }
  // <---

  // ---> make sequence
  else if (mess == "make sequence")
  { // ---> prepare for calculations
    context.del(root/"sequence");
    for (std::string unit : circuit.ls(root/"units"))
    { if (circuit[root/"units"/unit/"type"] == "function")
      { for (std::string k : circuit.ls(root/"units"/unit/"numerator poly"))
        { circuit.del(root/"units"/unit/"numerator poly"/k); }
        for (std::string k : circuit.ls(root/"units"/unit/"denominator poly"))
        { circuit.del(root/"units"/unit/"denominator poly"/k); } } }
    // <---

    // ---> expand functions polynoms
    for (std::string unit : circuit.ls(root/"units"))
    { if (circuit[root/"units"/unit/"type"] == "function")
      { std::list<float> polynom;
        int counter = 0;
        std::string current;
        std::string numerator = circuit[root/"units"/unit/"numerator poly"];
        std::string denominator = circuit[root/"units"/unit/"denominator poly"];

        for (char c : numerator)
        { if (c == ';') { polynom.push_back(std::stof(current));
                          current.clear(); continue; }
          current.push_back(c); }

        for (float v : polynom)
        { circuit[root/"units"/unit/"numerator poly"/counter] = v; counter++; }
        polynom.clear();
        current.clear();

        for (char c : denominator)
        { if (c == ';') { polynom.push_back(std::stof(current));
                          current.clear(); continue; }
          current.push_back(c); }

        for (float v : polynom)
        { circuit[root/"units"/unit/"denominator poly"/counter] = v;
          counter++; } } }
    // <---
    
    // ---> numerate all of the entities to correctly insert it to the
    //      calculator
    unsigned int counter = 0;
    for (std::string input : circuit.ls(root/"inputs"))
    { circuit[root/"inputs"/input] = std::to_string(counter); counter++;
      circuit[root/"inputs"/input/"vout"] = std::to_string(counter);
      counter++; }

    for (std::string output : circuit.ls(root/"outputs"))
    { circuit[root/"outputs"/output] = std::to_string(counter); counter++;
      circuit[root/"outputs"/output/"vin"] = std::to_string(counter);
      counter++; }

    for (std::string unit : circuit.ls(root/"units"))
    { circuit[root/"units"/unit] = std::to_string(counter); counter++;
      for (std::string input : circuit.ls(root/"units"/unit/"inputs"))
      { circuit[root/"units"/unit/"inputs"/input] = std::to_string(counter);
        counter++; }
      for (std::string output : circuit.ls(root/"units"/unit/"outputs"))
      { circuit[root/"units"/unit/"outputs"/output] = std::to_string(counter);
        counter++; } }

    for (std::string net : context.ls(root/"network"))
    { context[root/"network"/net] = std::to_string(counter); counter++; }
    // <---

    // ---> insert all of the units and nets to the calculator
    circuit_calculator c;
    for (std::string input : circuit.ls(root/"inputs"))
    { c.u((std::string)circuit[root/"inputs"/input])
       .o((std::string)circuit[root/"inputs"/input/"vout"], true); }

    for (std::string output : circuit.ls(root/"outputs"))
    { c.u((std::string)circuit[root/"outputs"/output])
       .i((std::string)circuit[root/"outputs"/output/"vin"]); }

    for (std::string unit : circuit.ls(root/"units"))
    { c.u((std::string)circuit[root/"units"/unit]);
      for (std::string input : circuit.ls(root/"units"/unit/"inputs"))
      { c.i((std::string)circuit[root/"units"/unit/"inputs"/input]); }
      for (std::string output : circuit.ls(root/"units"/unit/"outputs"))
      { bool active = false;
        if (circuit[root/"units"/unit/"type"] == "constant") { active = true; }
        if (circuit[root/"units"/unit/"type"] == "delay")    { active = true; }
        if (circuit[root/"units"/unit/"type"] == "function" &&
            !!circuit(root/"units"/unit/"numerator poly"/0) &&
            circuit[root/"units"/unit/"numerator poly"/0] == 0)
                                                             { active = true; }
        c.o((std::string)circuit[root/"units"/unit/"outputs"/output],
            active); } }

    for (std::string net : context.ls(root/"network"))
    { std::string nid = (std::string)context[root/"network"/net];
      for (std::string input : context.ls(root/"network"/net/"inputs"))
      { std::string uid = (std::string)circuit[root/"inputs"/input];
        std::string pid = (std::string)circuit[root/"inputs"/input/"vout"];
        c.connect(uid, pid, nid); }

      for (std::string output : context.ls(root/"network"/net/"outputs"))
      { std::string uid = (std::string)circuit[root/"outputs"/output];
        std::string pid = (std::string)circuit[root/"outputs"/output/"vin"];
        c.connect(uid, pid, nid); }

      for (std::string unit : context.ls(root/"network"/net/"units"))
      { std::string uid = (std::string)circuit[root/"units"/unit];

        for (std::string input :
             context.ls(root/"network"/net/"units"/unit/"inputs"))
        { std::string pid
            = (std::string)circuit[root/"units"/unit/"inputs"/input];
          c.connect(uid, pid, nid); }

        for (std::string output :
             context.ls(root/"network"/net/"units"/unit/"outputs"))
        { std::string pid
            = (std::string)circuit[root/"units"/unit/"outputs"/output];
          c.connect(uid, pid, nid); } } }
    // <---

    // ---> get the sequence
    std::list<std::string> sequence;
    c.schedule(sequence);

    for (std::string item : sequence)
    { context[root/"sequence"/item];
      bool found = false;
      for (std::string input : circuit.ls(root/"inputs"))
      { if (circuit[root/"inputs"/input] == item)
        { context[root/"sequence"/item/"type"] = "input";
          context[root/"sequence"/item/"id"] = input;
          found = true; break; } }
      if (found) { continue; }

      for (std::string output : circuit.ls(root/"outputs"))
      { if (circuit[root/"outputs"/output] == item)
        { context[root/"sequence"/item/"type"] = "output";
          context[root/"sequence"/item/"id"] = output;
          found = true; break; } }
      if (found) { continue; }

      for (std::string unit : circuit.ls(root/"units"))
      { if (circuit[root/"units"/unit] == item)
        { context[root/"sequence"/item/"type"] = "unit";
          context[root/"sequence"/item/"id"] = unit;
          found = true; break; } }
      if (found) { continue; } }
    // <---
  }
  // <---

  // ---> check sequence
  else if (mess == "check sequence")
  { context.del(root/"sequence errors");
    unsigned int counter = 0;
    for (std::string input : circuit.ls(root/"inputs"))
    { context[root/"sequence errors"/counter/"type"] = "input";
      context[root/"sequence errors"/counter/"id"] = input;
      counter++; }

    for (std::string output : circuit.ls(root/"outputs"))
    { context[root/"sequence errors"/counter/"type"] = "output";
      context[root/"sequence errors"/counter/"id"] = output;
      counter++; }

    for (std::string unit : circuit.ls(root/"units"))
    { context[root/"sequence errors"/counter/"type"] = "unit";
      context[root/"sequence errors"/counter/"id"] = unit;
      counter++; } }

  for (std::string item : context.ls(root/"sequence"))
  { for (std::string error : context.ls(root/"sequence errors"))
    { if (context[root/"sequence"/item/"type"] ==
          context[root/"sequence errors"/error/"type"]
          &&
          context[root/"sequence"/item/"id"] == 
          context[root/"sequence errors"/error/"id"])
      { context.del(root/"sequence errors"/error); break; } } }
  // <---
}

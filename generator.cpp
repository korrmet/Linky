#include "generator.hpp"
#include <fstream>

// ---> string printf
#include <cstdio>
#include <string>
#include <cstdarg>
inline void print(std::string& s, const char* fmt, ...)
{ std::string tmp;
  va_list args, args2;
  va_start(args, fmt);
  va_copy(args2, args);
  tmp.resize(vsnprintf(nullptr, 0, fmt, args2) + 1);
  va_end(args2);
  vsprintf(tmp.data(), fmt, args);
  va_end(args);
  tmp.pop_back();
  s.append(tmp); }
// <---

// ---> circuit calculator
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

  // ---> schedule
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
      { if (!u.o.size()) { continue; }
        if (u.solved) { continue; }
        unsigned int ocount = 0;
        for (unit::pin& p : u.o) { if (p.active) { ocount++; } }
        if (ocount == u.o.size())
        { u.solved = true; s.push_back(u.id); count++; } }

      if (count) { continue; }

      for (unit& u : units)
      { if (u.solved) { continue; }

        unsigned int icount = 0;
        for (unit::pin& p : u.i)
        { unit::pin source;
          if (p.nid == "") { break; }
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
  // <---

  struct unit
  { struct pin { pin() : active(false) {}
                 bool active; std::string id; std::string nid; };
    unit() : solved(false) {}
    std::string id;
    bool solved;
    std::list<pin> i, o; };

  private:
  std::list<unit> units;

  // ---> inputs
  bool inputs(std::string nid, std::list<unit::pin*> pins)
  { bool res = false;

    for (unit& u : units)
    { for (unit::pin& p : u.i)
      { if (p.nid == nid) { pins.push_back(&p); }
        res = true; } }

    return res; }
  // <---

  // ---> output
  // circuit should have only output
  bool output(std::string nid, unit::pin& pin)
  { for (unit& u : units)
    { for (unit::pin& p : u.o)
      { if (p.nid == nid) { pin = p; return true; } } }

    return false; }
  // <---
};
// <---

void generator::handler(void* ctx, IM mess)
{ // ---> generate code
  if (mess == "generate code")
  { std::string header; std::string source;
    std::string tmp = context[ROOT/"circuit file path"];
    std::string fname;
    for (char c : tmp) { if (c == '.') { break; } fname.push_back(c); }
    std::string header_name = fname; header_name.append(".h");
    std::string source_name = fname; source_name.append(".c");

    // ---> header
    print(header, "#ifndef %s_h\n", fname.c_str());
    print(header, "#define %s_h\n\n", fname.c_str());

    unsigned int counter = 0;
    print(header, "// inputs\n");
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { print(header, "#define %s_%s %d\n",
            fname.c_str(),
            ((std::string)circuit[ROOT/"inputs"/input/"name"]).c_str(),
            counter);
      counter++; }
    print(header, "#define %s_inputs_size %d\n", fname.c_str(), counter);

    counter = 0;
    print(header, "\n// outputs\n");
    for (std::string output : circuit.ls(ROOT/"outputs"))
    { print(header, "#define %s_%s %d\n",
            fname.c_str(),
            ((std::string)circuit[ROOT/"outputs"/output/"name"]).c_str(),
            counter);
      counter++; }
    print(header, "#define %s_outputs_size %d\n", fname.c_str(), counter);

    print(header, "\n// context\n");
    unsigned int context_size = 0;
    
    for (std::string unit : circuit.ls(ROOT/"units"))
    { std::string utype = circuit[ROOT/"units"/unit/"type"];
      if (utype == "delay")
      { print(header, "#define %s_unit_%s %d\n",
              fname.c_str(), unit.c_str(), context_size);
        int value = circuit[ROOT/"units"/unit/"value"];
        print(header, "#define %s_unit_%s_SIZE %d\n",
              fname.c_str(), unit.c_str(), value);
        context_size += value; }
      else if (utype == "function")
      { std::string num_poly = circuit[ROOT/"units"/unit/"numerator poly"];
        std::string den_poly = circuit[ROOT/"units"/unit/"denominator poly"];
        unsigned int num_count = 0;
        unsigned int den_count = 0;
        for (char c : num_poly) { if (c == ';') { num_count++; } }
        for (char c : den_poly) { if (c == ';') { den_count++; } }
        print(header,  "#define %s_unit_%s %d\n",
              fname.c_str(), unit.c_str(), context_size);
        context_size += num_count;
        context_size += den_count; }
      else if (utype == "code block")
      { unsigned int block_context_size
          = (int)circuit[ROOT/"units"/unit/"context size"];
        unsigned int inputs_size
          = circuit.ls(ROOT/"units"/unit/"inputs").size();
        unsigned int outputs_size
          = circuit.ls(ROOT/"units"/unit/"outputs").size();
        if (block_context_size)
        { print(header, "#define %s_unit_%s %d\n",
                fname.c_str(), unit.c_str(), context_size);
          context_size += inputs_size + outputs_size + block_context_size; }
        std::string function_name = circuit[ROOT/"units"/unit/"function name"];
        print(header, "extern void %s(float* inputs, "
                                "float* outputs, "
                                "float* context);\n",
              function_name.c_str()); } }

    print(header, "#define %s_context_size %d\n", fname.c_str(), context_size);

    print(header, "\nvoid %s(float* inputs,"
                  " float* outputs, float* context);\n", fname.c_str());

    print(header, "\n#endif\n");

    std::ofstream header_file(header_name);
    if (!header_file.is_open()) { PRINT("Can't write generated header!\n"); }
    else { header_file << header; header_file.close(); }
    // <---

    // ---> source
    print(source, "#include \"%s.h\"\n", fname.c_str());
    for (std::string unit : circuit.ls(ROOT/"units"))
    { if (circuit[ROOT/"units"/unit/"type"] == "code block")
      { std::string fname = circuit[ROOT/"units"/unit/"function name"];
        print(source,
              "extern void %s"
              "(float* inputs, float* outputs, float* context);\n",
              fname.c_str()); } }

    print(source, "\nvoid %s(float* inputs, "
                  " float* outputs, float* context) {\n", fname.c_str());
    
    for (std::string net : context.ls(ROOT/"network"))
    { print(source, "  float net_%s = 0.0f;\n", net.c_str()); }

    for (std::string step : context.ls(ROOT/"sequence"))
    { std::string type = context[ROOT/"sequence"/step/"type"];
      std::string id = context[ROOT/"sequence"/step/"id"];
      
      std::string unit_type;
      if (type == "input") { unit_type = "circuit input"; }
      else if (type == "output") { unit_type = "circuit output"; }
      else if (type == "unit")
      { unit_type = (std::string)circuit[ROOT/"units"/id/"type"]; }

      print(source, "\n  // %s, %s (%s)\n",
            type.c_str(), unit_type.c_str(), id.c_str());

      // ---> input
      if (type == "input")
      { std::string nid = circuit[ROOT/"inputs"/id/"net"];
        std::string iname = circuit[ROOT/"inputs"/id/"name"];
        print(source, "  net_%s = inputs[%s_%s];\n",
              nid.c_str(), fname.c_str(), iname.c_str()); }
      // <---

      // ---> output
      else if (type == "output")
      { std::string nid = circuit[ROOT/"outputs"/id/"net"];
        std::string oname = circuit[ROOT/"outputs"/id/"name"];
        print(source, "  outputs[%s_%s] = net_%s;\n",
              fname.c_str(), oname.c_str(), nid.c_str()); }
      // <---
      
      else if (type == "unit")
      { // ---> constant
        if (unit_type == "constant")
        { std::string nid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          float value = circuit[ROOT/"units"/id/"value"];
          print(source, "  net_%s = %f;\n", nid.c_str(), value); }
        // <---
        
        // ---> sum
        else if (unit_type == "sum")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source, "  net_%s = net_%s + net_%s;\n",
                onid.c_str(), i0nid.c_str(), i1nid.c_str()); }
        // <---

        // ---> product
        else if (unit_type == "product")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source, "  net_%s = net_%s * net_%s;\n",
                onid.c_str(), i0nid.c_str(), i1nid.c_str()); }
        // <---

        // ---> delay
        else if (unit_type == "delay")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int value = circuit[ROOT/"units"/id/"value"];

          // set output network
          print(source, "  net_%s = context[%s_unit_%s + %d];\n",
                onid.c_str(), fname.c_str(), id.c_str(), value - 1);

          // manage input
          for (unsigned int i = value - 1; i > 0; i--)
          { print(source, "  context[%s_unit_%s + %d] "
                          "= context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), i,
                  fname.c_str(), id.c_str(), i - 1); }
          print(source, "  context[%s_unit_%s] = net_%s;\n",
                fname.c_str(), id.c_str(), inid.c_str()); }
        // <---

        // ---> function
        //      sum[i = 0..n](k_xi * x[n - i]) - sum[j = 1..m](k_yj * y[m - j])
        //  y = ---------------------------------------------------------------
        //      k_y0
        //  WARNING: this code is definitely needs to be debugged!
        //           it's written just for the overview
        else if (unit_type == "function")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string num_poly = circuit[ROOT/"units"/id/"numerator poly"];
          std::string den_poly = circuit[ROOT/"units"/id/"denominator poly"];
          int num_count = 0;
          int den_count = 0;
          for (char c : num_poly) { if (c == ';') { num_count++; } }
          for (char c : den_poly) { if (c == ';') { den_count++; } }
          float num_coeffs[num_count];
          float den_coeffs[den_count];
          std::string coeff_str; unsigned int coeff_ctr = 0;
          for (char c : num_poly)
          { if (c == ';') { num_coeffs[coeff_ctr] = std::stof(coeff_str);
                            coeff_str.clear(); coeff_ctr++; continue; }
            coeff_str.push_back(c); }
          coeff_str.clear(); coeff_ctr = 0;
          for (char c : den_poly)
          { if (c == ';') { den_coeffs[coeff_ctr] = std::stof(coeff_str);
                            coeff_str.clear(); coeff_ctr++; continue; }
            coeff_str.push_back(c); }
          
          // manage numerator delays
          for (int i = 0; i < num_count - 1; i++)
          { print(source, "  context[%s_unit_%s + %d] "
                          "= context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), i,
                  fname.c_str(), id.c_str(), i + 1); }
          print(source, "  context[%s_unit_%s + %d] = net_%s;\n",
                fname.c_str(), id.c_str(), num_count - 1, inid.c_str());

          // manage denominator delays
          for (int i = 0; i < den_count - 1; i++)
          { print(source, "  context[%s_unit_%s + %d] "
                          "= context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), num_count + i,
                  fname.c_str(), id.c_str(), num_count + i + 1); }

          // calculate output
          print(source, "  context[%s_unit_%s + %d] = 0.0;\n",
                fname.c_str(), id.c_str(), num_count + den_count - 1);
          
          // numerator sum
          for (int i = 0; i < num_count; i++)
          { print(source, "  context[%s_unit_%s + %d] += "
                          "%f * context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), num_count + den_count - 1,
                  num_coeffs[i],
                  fname.c_str(), id.c_str(), num_count - 1 - i); }

          // denominator sum
          for (unsigned int i = 1; i < den_count; i++)
          { print(source, "  context[%s_unit_%s + %d] -= "
                          "%f * context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), num_count + den_count - 1, 
                  den_coeffs[i],
                  fname.c_str(), id.c_str(), num_count + den_count - 1 - i); }

          // finalizing last Y
          print(source, "  context[%s_unit_%s + %d] /= %f;\n",
                fname.c_str(), id.c_str(), num_count + den_count - 1,
                den_coeffs[0]);

          // setting output
          print(source, "  net_%s = context[%s_unit_%s + %d];\n",
                onid.c_str(), fname.c_str(), id.c_str(),
                num_count + den_count - 1);
        }
        // <---

        // ---> code block
        else if (unit_type == "code block")
        { unsigned int inputs_size
            = circuit.ls(ROOT/"units"/id/"inputs").size();
          unsigned int outputs_size
            = circuit.ls(ROOT/"units"/id/"outputs").size();
          unsigned int counter = 0;
          for (std::string input : circuit.ls(ROOT/"units"/id/"inputs"))
          { std::string inid = circuit[ROOT/"units"/id/"inputs"/input/"net"];
            print(source, "  context[%s_unit_%s + %d] = net_%s;\n",
                  fname.c_str(), id.c_str(), counter, inid.c_str());
            counter++; }
          std::string function_name = circuit[ROOT/"units"/id/"function name"];
          print(source, "  %s(&context[%s_unit_%s + %d], "
                             "&context[%s_unit_%s + %d], "
                             "&context[%s_unit_%s + %d]);\n",
                function_name.c_str(),
                fname.c_str(), id.c_str(), 0,
                fname.c_str(), id.c_str(), inputs_size,
                fname.c_str(), id.c_str(), inputs_size + outputs_size);
          counter = 0;
          for (std::string output : circuit.ls(ROOT/"units"/id/"outputs"))
          { std::string onid = circuit[ROOT/"units"/id/"outputs"/output/"net"];
            print(source, "  net_%s = context[%s_unit_%s + %d];\n",
                  onid.c_str(), fname.c_str(), id.c_str(),
                  inputs_size + counter);
            counter++; } }
        // <---
      }
    }
    print(source, "}\n");

    std::ofstream source_file(source_name);
    if (!source_file.is_open()) { PRINT("Can't write generated source!\n"); }
    else { source_file << source; source_file.close(); }
    // <---
  }
  // <---

  // ---> check circuit errors
  else if (mess == "check circuit errors")
  { context.del(ROOT/"network");
    context.del(ROOT/"network errors");
    bus(IM("make network"));
    bus(IM("scan network"));
    bus(IM("check network"));
    bus(IM("make sequence"));
    bus(IM("check sequence")); }
  // <---
  
  // ---> make network
  else if (mess == "make network")
  { std::list<std::string> wires = circuit.ls(ROOT/"wires");
    while (wires.size())
    { std::list<std::string> net;

      net.push_back(wires.front()); wires.pop_front();
      bool added;
      do
      { added = false;
        for (std::string nw : net)
        { for (std::string w : wires)
          { if ( ( circuit[ROOT/"wires"/w /0/"x"] ==
                   circuit[ROOT/"wires"/nw/0/"x"] &&
                   circuit[ROOT/"wires"/w /0/"y"] ==
                   circuit[ROOT/"wires"/nw/0/"y"])
                 ||
                 ( circuit[ROOT/"wires"/w /0/"x"] ==
                   circuit[ROOT/"wires"/nw/1/"x"] &&
                   circuit[ROOT/"wires"/w /0/"y"] ==
                   circuit[ROOT/"wires"/nw/1/"y"])
                 ||
                 ( circuit[ROOT/"wires"/w /1/"x"] ==
                   circuit[ROOT/"wires"/nw/0/"x"] &&
                   circuit[ROOT/"wires"/w /1/"y"] ==
                   circuit[ROOT/"wires"/nw/0/"y"])
                 ||
                 ( circuit[ROOT/"wires"/w /1/"x"] ==
                   circuit[ROOT/"wires"/nw/1/"x"] &&
                   circuit[ROOT/"wires"/w /1/"y"] ==
                   circuit[ROOT/"wires"/nw/1/"y"]))
            { net.push_back(w); wires.remove(w); added = true; break; } }
          if (added) { break; } }
      } while (added);

      std::list<std::string> nets = context.ls(ROOT/"network");
      int idx = nets.size() ? std::stoi(nets.back()) + 1 : 0;
      for (std::string nwire : net)
      { context[ROOT/"network"/idx/"wires"/nwire]; } } }
  // <---

  // ---> scan network
  else if (mess == "scan network")
  { struct point { int x; int y; };
    for (std::string net : context.ls(ROOT/"network"))
    { // ---> generating list of points
      std::list<point> points;
      for (std::string wire : context.ls(ROOT/"network"/net/"wires"))
      { point p;
        p.x = (int)circuit[ROOT/"wires"/wire/0/"x"];
        p.y = (int)circuit[ROOT/"wires"/wire/0/"y"];
        points.push_back(p);
        p.x = (int)circuit[ROOT/"wires"/wire/1/"x"];
        p.y = (int)circuit[ROOT/"wires"/wire/1/"y"];
        points.push_back(p); }
      // <---

      // ---> scanning circuit inputs
      for (std::string input : circuit.ls(ROOT/"inputs"))
      { for (point p : points)
        { if (p.x == (int)circuit[ROOT/"inputs"/input/"point"/"x"] &&
              p.y == (int)circuit[ROOT/"inputs"/input/"point"/"y"])
          { context[ROOT/"network"/net/"inputs"/input];
            circuit[ROOT/"inputs"/input/"net"] = net;
            break; } } }
      // <---

      // ---> scanning circuit outputs
      for (std::string output : circuit.ls(ROOT/"outputs"))
      { for (point p : points)
        { if (p.x == (int)circuit[ROOT/"outputs"/output/"point"/"x"] &&
              p.y == (int)circuit[ROOT/"outputs"/output/"point"/"y"])
          { context[ROOT/"network"/net/"outputs"/output];
            circuit[ROOT/"outputs"/output/"net"] = net;
            break; } } }
      // <---

      // ---> scanning units
      for (std::string unit : circuit.ls(ROOT/"units"))
      { for (std::string input : circuit.ls(ROOT/"units"/unit/"inputs"))
        { for (point p : points)
          { if (p.x == (int)circuit[ROOT/"units"/unit/"inputs"/input/"x"] &&
                p.y == (int)circuit[ROOT/"units"/unit/"inputs"/input/"y"])
            { context[ROOT/"network"/net/"units"/unit/"inputs"/input];
              circuit[ROOT/"units"/unit/"inputs"/input/"net"] = net; } } }

        for (std::string output : circuit.ls(ROOT/"units"/unit/"outputs"))
        { for (point p : points)
          { if (p.x == (int)circuit[ROOT/"units"/unit/"outputs"/output/"x"] &&
                p.y == (int)circuit[ROOT/"units"/unit/"outputs"/output/"y"])
            { context[ROOT/"network"/net/"units"/unit/"outputs"/output];
              circuit[ROOT/"units"/unit/"outputs"/output/"net"] = net; } } } }
      // <---
    } }
  // <---
  
  // ---> check network
  // Every network should contain exactly one logical output and at least one
  // logical input. Circuit input are logical outputs because of in the
  // circuit's point of view it's a source of the signals, and circuit
  // outputs are logical inputs because it's a data consumer.
  if (mess == "check network")
  { for (std::string net : context.ls(ROOT/"network"))
    { int logical_out_count = 0;
      logical_out_count += context.ls(ROOT/"network"/net/"inputs").size();
      for (std::string unit : context.ls(ROOT/"network"/net/"units"))
      { logical_out_count
          += context.ls(ROOT/"network"/net/"units"/unit/"outputs").size(); }
      if (logical_out_count != 1)
      { context[ROOT/"network errors"/net]; continue; }
    
      int logical_in_count = 0;
      logical_in_count += context.ls(ROOT/"network"/net/"outputs").size(); 
      for (std::string unit : context.ls(ROOT/"network"/net/"units"))
      { logical_in_count
          += context.ls(ROOT/"network"/net/"units"/unit/"inputs").size(); }
      if (logical_in_count == 0)
      { context[ROOT/"network errors"/net]; continue; } } }
  // <---

  // ---> make sequence
  else if (mess == "make sequence")
  { // ---> prepare for calculations
    context.del(ROOT/"sequence");
    for (std::string unit : circuit.ls(ROOT/"units"))
    { if (circuit[ROOT/"units"/unit/"type"] == "function")
      { for (std::string k : circuit.ls(ROOT/"units"/unit/"numerator poly"))
        { circuit.del(ROOT/"units"/unit/"numerator poly"/k); }
        for (std::string k : circuit.ls(ROOT/"units"/unit/"denominator poly"))
        { circuit.del(ROOT/"units"/unit/"denominator poly"/k); } } }
    // <---

    // ---> expand functions polynoms
    for (std::string unit : circuit.ls(ROOT/"units"))
    { if (circuit[ROOT/"units"/unit/"type"] == "function")
      { std::list<float> polynom;
        int counter = 0;
        std::string current;
        std::string numerator = circuit[ROOT/"units"/unit/"numerator poly"];
        std::string denominator = circuit[ROOT/"units"/unit/"denominator poly"];

        for (char c : numerator)
        { if (c == ';') { polynom.push_back(std::stof(current));
                          current.clear(); continue; }
          current.push_back(c); }

        for (float v : polynom)
        { circuit[ROOT/"units"/unit/"numerator poly"/counter] = v; counter++; }
        polynom.clear();
        current.clear();

        for (char c : denominator)
        { if (c == ';') { polynom.push_back(std::stof(current));
                          current.clear(); continue; }
          current.push_back(c); }

        for (float v : polynom)
        { circuit[ROOT/"units"/unit/"denominator poly"/counter] = v;
          counter++; } } }
    // <---
    
    // ---> numerate all of the entities to correctly insert it to the
    //      calculator
    unsigned int counter = 0;
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { circuit[ROOT/"inputs"/input] = std::to_string(counter); counter++;
      circuit[ROOT/"inputs"/input/"vout"] = std::to_string(counter);
      counter++; }

    for (std::string output : circuit.ls(ROOT/"outputs"))
    { circuit[ROOT/"outputs"/output] = std::to_string(counter); counter++;
      circuit[ROOT/"outputs"/output/"vin"] = std::to_string(counter);
      counter++; }

    for (std::string unit : circuit.ls(ROOT/"units"))
    { circuit[ROOT/"units"/unit] = std::to_string(counter); counter++;
      for (std::string input : circuit.ls(ROOT/"units"/unit/"inputs"))
      { circuit[ROOT/"units"/unit/"inputs"/input] = std::to_string(counter);
        counter++; }
      for (std::string output : circuit.ls(ROOT/"units"/unit/"outputs"))
      { circuit[ROOT/"units"/unit/"outputs"/output] = std::to_string(counter);
        counter++; } }

    for (std::string net : context.ls(ROOT/"network"))
    { context[ROOT/"network"/net] = std::to_string(counter); counter++; }
    // <---

    // ---> insert all of the units and nets to the calculator
    circuit_calculator c;
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { c.u((std::string)circuit[ROOT/"inputs"/input])
       .o((std::string)circuit[ROOT/"inputs"/input/"vout"], true); }

    for (std::string output : circuit.ls(ROOT/"outputs"))
    { c.u((std::string)circuit[ROOT/"outputs"/output])
       .i((std::string)circuit[ROOT/"outputs"/output/"vin"]); }

    for (std::string unit : circuit.ls(ROOT/"units"))
    { c.u((std::string)circuit[ROOT/"units"/unit]);
      for (std::string input : circuit.ls(ROOT/"units"/unit/"inputs"))
      { c.i((std::string)circuit[ROOT/"units"/unit/"inputs"/input]); }
      for (std::string output : circuit.ls(ROOT/"units"/unit/"outputs"))
      { bool active = false;
        if (circuit[ROOT/"units"/unit/"type"] == "constant") { active = true; }
        if (circuit[ROOT/"units"/unit/"type"] == "delay")    { active = true; }
        if (circuit[ROOT/"units"/unit/"type"] == "function" &&
            !!circuit(ROOT/"units"/unit/"numerator poly"/0) &&
            circuit[ROOT/"units"/unit/"numerator poly"/0] == 0)
                                                             { active = true; }
        c.o((std::string)circuit[ROOT/"units"/unit/"outputs"/output],
            active); } }

    for (std::string net : context.ls(ROOT/"network"))
    { std::string nid = (std::string)context[ROOT/"network"/net];
      for (std::string input : context.ls(ROOT/"network"/net/"inputs"))
      { std::string uid = (std::string)circuit[ROOT/"inputs"/input];
        std::string pid = (std::string)circuit[ROOT/"inputs"/input/"vout"];
        c.connect(uid, pid, nid); }

      for (std::string output : context.ls(ROOT/"network"/net/"outputs"))
      { std::string uid = (std::string)circuit[ROOT/"outputs"/output];
        std::string pid = (std::string)circuit[ROOT/"outputs"/output/"vin"];
        c.connect(uid, pid, nid); }

      for (std::string unit : context.ls(ROOT/"network"/net/"units"))
      { std::string uid = (std::string)circuit[ROOT/"units"/unit];

        for (std::string input :
             context.ls(ROOT/"network"/net/"units"/unit/"inputs"))
        { std::string pid
            = (std::string)circuit[ROOT/"units"/unit/"inputs"/input];
          c.connect(uid, pid, nid); }

        for (std::string output :
             context.ls(ROOT/"network"/net/"units"/unit/"outputs"))
        { std::string pid
            = (std::string)circuit[ROOT/"units"/unit/"outputs"/output];
          c.connect(uid, pid, nid); } } }
    // <---

    // ---> get the sequence
    std::list<std::string> sequence;
    c.schedule(sequence);

    for (std::string item : sequence)
    { context[ROOT/"sequence"/item];
      bool found = false;
      for (std::string input : circuit.ls(ROOT/"inputs"))
      { if (circuit[ROOT/"inputs"/input] == item)
        { context[ROOT/"sequence"/item/"type"] = "input";
          context[ROOT/"sequence"/item/"id"] = input;
          found = true; break; } }
      if (found) { continue; }

      for (std::string output : circuit.ls(ROOT/"outputs"))
      { if (circuit[ROOT/"outputs"/output] == item)
        { context[ROOT/"sequence"/item/"type"] = "output";
          context[ROOT/"sequence"/item/"id"] = output;
          found = true; break; } }
      if (found) { continue; }

      for (std::string unit : circuit.ls(ROOT/"units"))
      { if (circuit[ROOT/"units"/unit] == item)
        { context[ROOT/"sequence"/item/"type"] = "unit";
          context[ROOT/"sequence"/item/"id"] = unit;
          found = true; break; } }
      if (found) { continue; } }
    // <---
  }
  // <---

  // ---> check sequence
  else if (mess == "check sequence")
  { context.del(ROOT/"sequence errors");
    unsigned int counter = 0;
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { context[ROOT/"sequence errors"/counter/"type"] = "input";
      context[ROOT/"sequence errors"/counter/"id"] = input;
      counter++; }

    for (std::string output : circuit.ls(ROOT/"outputs"))
    { context[ROOT/"sequence errors"/counter/"type"] = "output";
      context[ROOT/"sequence errors"/counter/"id"] = output;
      counter++; }

    for (std::string unit : circuit.ls(ROOT/"units"))
    { context[ROOT/"sequence errors"/counter/"type"] = "unit";
      context[ROOT/"sequence errors"/counter/"id"] = unit;
      counter++; } }

  for (std::string item : context.ls(ROOT/"sequence"))
  { for (std::string error : context.ls(ROOT/"sequence errors"))
    { if (context[ROOT/"sequence"/item/"type"] ==
          context[ROOT/"sequence errors"/error/"type"]
          &&
          context[ROOT/"sequence"/item/"id"] == 
          context[ROOT/"sequence errors"/error/"id"])
      { context.del(ROOT/"sequence errors"/error); break; } } }
  // <---
}

#include "solver.hpp"

/** \brief Solves the network.
 *  \details Solution includes enumeration, sizes of inputs, outputs and
 *           context, and the sequence of actions, that may be used for
 *           simulation and code generation.
 *  \param c Storage with circuit to solve.
 *  \return Storage with solution. */
static independency::storage circuit_solver(independency::storage c)
{ independency::storage sol;

  unsigned int inputs_counter = 0;
  for (std::string i : c.ls(ROOT/"inputs"))
  { sol[ROOT/"inputs"/i] = (int)inputs_counter; inputs_counter++; }
  sol[ROOT/"inputs num"] = (int)inputs_counter;

  unsigned int outputs_counter = 0;
  for (std::string o : c.ls(ROOT/"outptus"))
  { sol[ROOT/"outputs"/o] = (int)outputs_counter; outputs_counter++; }
  sol[ROOT/"outputs num"] = (int)outputs_counter;

  unsigned int context_counter = 0;
  for (std::string unit : c.ls(ROOT/"units"))
  { std::string utype = c[ROOT/"units"/unit/"type"];

    if (utype == "delay")
    { sol[ROOT/"units"/unit] = (int)context_counter;
      context_counter += (int)c[ROOT/"units"/unit/"value"]; }

    else if (utype == "function")
    { context[ROOT/"solution"/"units"/unit] = (int)context_counter;
      std::string num_poly = circuit[ROOT/"units"/unit/"numerator poly"];
      std::string den_poly = circuit[ROOT/"units"/unit/"denominator poly"];
      unsigned int num_count = 0;
      unsigned int den_count = 0;
      for (char ch : num_poly) { if (ch == ';') { num_count++; } }
      for (char ch : den_poly) { if (ch == ';') { den_count++; } }
      context_counter += num_count;
      context_counter += den_count; }

    else if (utype == "loopback")
    { sol[ROOT/"units"/unit] = (int)context_counter;
      context_counter += (int)circuit[ROOT/"units"/unit/"value"] - 1; } }

  // make network
  std::list<std::string> wires = c.ls(ROOT/"wires");
  while (wires.size())
  { std::list<std::string> net;
    net.push_back(wires.front()); wires.pop_front();
    bool added;
    do
    { added = false;
      for (std::string nw : net)
      { for (std::string w : wires)
        { if ((c[ROOT/"wires"/w/0/"x"] == c[ROOT/"wires"/nw/0/"x"] &&
               c[ROOT/"wires"/w/0/"y"] == c[ROOT/"wires"/nw/0/"y"]) ||
              (c[ROOT/"wires"/w/0/"x"] == c[ROOT/"wires"/nw/1/"x"] &&
               c[ROOT/"wires"/w/0/"y"] == c[ROOT/"wires"/nw/1/"y"]) ||
              (c[ROOT/"wires"/w/1/"x"] == c[ROOT/"wires"/nw/0/"x"] &&
               c[ROOT/"wires"/w/1/"y"] == c[ROOT/"wires"/nw/0/"y"]) ||
              (c[ROOT/"wires"/w/1/"x"] == c[ROOT/"wires"/nw/1/"x"] &&
               c[ROOT/"wires"/w/1/"y"] == c[ROOT/"wires"/nw/1/"y"]))
        { net.push_back(w); wires.remove(w); added = true; break; } }
      if (added) { break; } }
    } while(added);

    std::list<std::string> nets = sol.ls(ROOT/"network");
    int idx = nets.size() ? std::stoi(nets.back()) + 1 : 0;
    for (std::string nwire : net) { sol[ROOT/"network"/idx/"wires"/nwire]; } }

  for (std::string net : sol.ls(ROOT/"network"))
  { sol[ROOT/"network"/net] = (int)context_counter; context_counter++; }

  sol[ROOT/"context num"] = (int)context_counter;

  // scan network
  // check network
  // make sequence
  // check sequence
  // solve

  return sol;
}

void solver::handler(void* ctx, IM mess)
{ // ---> solve
  if (mess == "solve")
  { context.del(ROOT/"solution");

    unsigned int inputs_counter = 0;
    for (std::string input : circuit.ls(ROOT/"inputs"))
    { context[ROOT/"solution"/"inputs"/input] = (int)inputs_counter;
      inputs_counter++; }
    context[ROOT/"solution"/"inputs num"] = (int)inputs_counter;

    unsigned int outputs_counter = 0;
    for (std::string output : circuit.ls(ROOT/"outputs"))
    { context[ROOT/"solution"/"outputs"/output] = (int)outputs_counter;
      outputs_counter++; }
    context[ROOT/"solution"/"outputs num"] = (int)outputs_counter;

    unsigned int context_counter = 0;
    for (std::string unit : circuit.ls(ROOT/"units"))
    { std::string utype = circuit[ROOT/"units"/unit/"type"];

      if (utype == "delay")
      { context[ROOT/"solution"/"units"/unit] = (int)context_counter;
        context_counter += (int)circuit[ROOT/"units"/unit/"value"]; }

      else if (utype == "function")
      { context[ROOT/"solution"/"units"/unit] = (int)context_counter;
        std::string num_poly = circuit[ROOT/"units"/unit/"numerator poly"];
        std::string den_poly = circuit[ROOT/"units"/unit/"denominator poly"];
        unsigned int num_count = 0;
        unsigned int den_count = 0;
        for (char c : num_poly) { if (c == ';') { num_count++; } }
        for (char c : den_poly) { if (c == ';') { den_count++; } }
        context_counter += num_count;
        context_counter += den_count; }

      else if (utype == "loopback")
      { context[ROOT/"solution"/"units"/unit] = (int)context_counter;
        context_counter += (int)circuit[ROOT/"units"/unit/"value"] - 1; } }

    for (std::string net : context.ls(ROOT/"network"))
    { context[ROOT/"solution"/"network"/net] = (int)context_counter;
      context_counter++; }

    context[ROOT/"solution"/"context num"] = (int)context_counter;

    unsigned int step_counter = 0;
    for (std::string step : context.ls(ROOT/"sequence"))
    { std::string type = context[ROOT/"sequence"/step/"type"];
      std::string id = context[ROOT/"sequence"/step/"id"];

      if (type == "input")
      { std::string nid = circuit[ROOT/"inputs"/id/"net"];
        int net = context[ROOT/"solution"/"network"/nid];
        int input = std::stoi(id);
        context[ROOT/"solution"/"sequence"/step_counter/"cmd"] = "net = input";
        context[ROOT/"solution"/"sequence"/step_counter/"net"] = net;
        context[ROOT/"solution"/"sequence"/step_counter/"input"] = input;
        step_counter++; }

      else if (type == "output")
      { std::string nid = circuit[ROOT/"outputs"/id/"net"];
        int net = context[ROOT/"solution"/"network"/nid];
        int output = std::stoi(id);
        context[ROOT/"solution"/"sequence"/step_counter/"cmd"] = "output = net";
        context[ROOT/"solution"/"sequence"/step_counter/"net"] = net;
        context[ROOT/"solution"/"sequence"/step_counter/"output"] = output;
        step_counter++; }

      else if (type == "unit")
      { std::string utype = circuit[ROOT/"units"/id/"type"];

        if (utype == "constant")
        { std::string nid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          int net = context[ROOT/"solution"/"network"/nid];
          float value = circuit[ROOT/"units"/id/"value"];
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = value";
          context[ROOT/"solution"/"sequence"/step_counter/"net"] = net;
          context[ROOT/"solution"/"sequence"/step_counter/"value"] = value;
          step_counter++; }

        else if (utype == "sum")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          int net = context[ROOT/"solution"/"network"/onid];
          int net1 = context[ROOT/"solution"/"network"/i0nid];
          int net2 = context[ROOT/"solution"/"network"/i1nid];
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = net1 + net2";
          context[ROOT/"solution"/"sequence"/step_counter/"net"]  = net;
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = net1;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = net2;
          step_counter++; }

        else if (utype == "difference")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          int net = context[ROOT/"solution"/"network"/onid];
          int net1 = context[ROOT/"solution"/"network"/i0nid];
          int net2 = context[ROOT/"solution"/"network"/i1nid];
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = net1 - net2";
          context[ROOT/"solution"/"sequence"/step_counter/"net"]  = net;
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = net1;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = net2;
          step_counter++; }

        else if (utype == "product")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          int net = context[ROOT/"solution"/"network"/onid];
          int net1 = context[ROOT/"solution"/"network"/i0nid];
          int net2 = context[ROOT/"solution"/"network"/i1nid];
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = net1 * net2";
          context[ROOT/"solution"/"sequence"/step_counter/"net"]  = net;
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = net1;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = net2;
          step_counter++; }

        else if (utype == "division")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          int net = context[ROOT/"solution"/"network"/onid];
          int net1 = context[ROOT/"solution"/"network"/i0nid];
          int net2 = context[ROOT/"solution"/"network"/i1nid];
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = net1 / net2";
          context[ROOT/"solution"/"sequence"/step_counter/"net"]  = net;
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = net1;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = net2;
          step_counter++; }

        else if (utype == "delay")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int value = circuit[ROOT/"units"/id/"value"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          int unit = context[ROOT/"solution"/"units"/id];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = unit[num]";
          context[ROOT/"solution"/"sequence"/step_counter/"net"]  = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
          context[ROOT/"solution"/"sequence"/step_counter/"num"]  = value - 1;
          step_counter++;

          for (unsigned int i = value - 1; i > 0; i--)
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num1] = unit[num2]";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num1"] = (int)i;
            context[ROOT/"solution"/"sequence"/step_counter/"num2"]
            = (int)i - 1;
            step_counter++; }

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "unit[num] = net";
          context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
          context[ROOT/"solution"/"sequence"/step_counter/"num"] = 0;
          context[ROOT/"solution"/"sequence"/step_counter/"net"] = inet;
          step_counter++; }

        else if (utype == "function")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          int unit = context[ROOT/"solution"/"units"/id];
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
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num1] = unit[num2]";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num1"] = i;
            context[ROOT/"solution"/"sequence"/step_counter/"num2"] = i + 1;
            step_counter++; }

            context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num] = net";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num"]
            = num_count - 1;
            context[ROOT/"solution"/"sequence"/step_counter/"net"]  = inet;
            step_counter++;

          // manage denominator delays
          for (int i = 0; i < den_count - 1; i++)
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num1] = unit[num2]";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num1"]
            = num_count + i;
            context[ROOT/"solution"/"sequence"/step_counter/"num2"]
            = num_count + i + 1;
            step_counter++; }

          // calculate output
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "unit[num] = val";
          context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
          context[ROOT/"solution"/"sequence"/step_counter/"num"]
          = num_count + den_count - 1;
          context[ROOT/"solution"/"sequence"/step_counter/"val"] = 0.0f;
          step_counter++;

          // numerator_sum
          for (int i = 0; i < num_count; i++)
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num1] += val * unit[num2]";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num1"]
            = num_count + den_count - 1;
            context[ROOT/"solution"/"sequence"/step_counter/"num2"]
            = num_count - 1 - i;
            context[ROOT/"solution"/"sequence"/step_counter/"val"]
            = num_coeffs[i];
            step_counter++; }

          // denominator sum
          for (int i = 1; i < den_count; i++)
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num1] -= val * unit[num2]";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num1"]
            = num_count + den_count - 1;
            context[ROOT/"solution"/"sequence"/step_counter/"num2"]
            = num_count + den_count - 1 - i;
            context[ROOT/"solution"/"sequence"/step_counter/"val"]
            = den_coeffs[i];
            step_counter++; }

          // finalizing last Y
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "unit[num] /= val";
          context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
          context[ROOT/"solution"/"sequence"/step_counter/"num"] 
          = num_count + den_count - 1;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]
          = den_coeffs[0];
          step_counter++;

          // setting output
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net = unit[num]";
          context[ROOT/"solution"/"sequence"/step_counter/"net"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
          context[ROOT/"solution"/"sequence"/step_counter/"num"]
          = num_count + den_count - 1;
          step_counter++; }

        else if (utype == "loopback")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          int unit = context[ROOT/"solution"/"units"/id];
          int value = circuit[ROOT/"units"/id/"value"];
          if (value == 1)
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "net1 = net2";
            context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
            context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
            step_counter++; }
          else
          { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "net = unit[num]";
            context[ROOT/"solution"/"sequence"/step_counter/"net"] = onet;
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num"] = value - 2;
            step_counter++;

            for (unsigned int i = value - 2; i > 0; i--)
            { context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
              = "unit[num1] = unit[num2]"; 
              context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
              context[ROOT/"solution"/"sequence"/step_counter/"num1"] = (int)i;
              context[ROOT/"solution"/"sequence"/step_counter/"num2"]
              = (int)i - 1;
              step_counter++; }

            context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
            = "unit[num] = net";
            context[ROOT/"solution"/"sequence"/step_counter/"unit"] = unit;
            context[ROOT/"solution"/"sequence"/step_counter/"num"]  = 0;
            context[ROOT/"solution"/"sequence"/step_counter/"net"]  = inet;
            step_counter++; } }

        else if (utype == "coeff")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          float value = circuit[ROOT/"units"/id/"value"];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = val * net2";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]  = value;
          step_counter++; }

        else if (utype == "abs")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = net2 * (net2 > 0)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          step_counter++;

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 -= net2 * (net2 < 0)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          step_counter++; }

        else if (utype == "limit max")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          float value = circuit[ROOT/"units"/id/"value"];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = val * (net2 >= val)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]  = value;
          step_counter++;

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 += net2 * (net2 < val)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]  = value;
          step_counter++; }

        else if (utype == "limit min")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];
          float value = circuit[ROOT/"units"/id/"value"];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = val * (net2 <= val)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]  = value;
          step_counter++;

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 += net2 * (net2 > val)";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          context[ROOT/"solution"/"sequence"/step_counter/"val"]  = value;
          step_counter++; }

        else if (utype == "more")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];

          int onet  = context[ROOT/"solution"/"network"/onid];
          int i1net = context[ROOT/"solution"/"network"/i0nid];
          int i2net = context[ROOT/"solution"/"network"/i1nid];
          
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = net2 > net3";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = i1net;
          context[ROOT/"solution"/"sequence"/step_counter/"net3"] = i2net;
          step_counter++;
        }

        else if (utype == "less")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];

          int onet  = context[ROOT/"solution"/"network"/onid];
          int i1net = context[ROOT/"solution"/"network"/i0nid];
          int i2net = context[ROOT/"solution"/"network"/i1nid];
          
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = net2 < net3";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = i1net;
          context[ROOT/"solution"/"sequence"/step_counter/"net3"] = i2net;
          step_counter++;
        }

        else if (utype == "eq")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];

          int onet  = context[ROOT/"solution"/"network"/onid];
          int i1net = context[ROOT/"solution"/"network"/i0nid];
          int i2net = context[ROOT/"solution"/"network"/i1nid];
          
          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = net2 == net3";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = i1net;
          context[ROOT/"solution"/"sequence"/step_counter/"net3"] = i2net;
          step_counter++;
        }

        else if (utype == "not")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];

          int onet = context[ROOT/"solution"/"network"/onid];
          int inet = context[ROOT/"solution"/"network"/inid];

          context[ROOT/"solution"/"sequence"/step_counter/"cmd"]
          = "net1 = net2 <= 0";
          context[ROOT/"solution"/"sequence"/step_counter/"net1"] = onet;
          context[ROOT/"solution"/"sequence"/step_counter/"net2"] = inet;
          step_counter++;
        }
      
      } } }
  // <--- solve

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
        if (circuit[ROOT/"units"/unit/"type"] == "loopback") { active = true; }
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

// ---> circuit calculator
circuit_calculator& circuit_calculator::u(std::string id)
{ unit u; u.id = id; units.push_back(u); return *this; }

circuit_calculator& circuit_calculator::i(std::string id)
{ unit::pin p; p.id = id; units.back().i.push_back(p); return *this; }

circuit_calculator& circuit_calculator::o(std::string id, bool active)
{ unit::pin p; p.id = id; p.active = active;
  units.back().o.push_back(p); return *this; }

circuit_calculator& circuit_calculator::connect(std::string uid,
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

bool circuit_calculator::schedule(std::list<std::string>& s)
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

bool circuit_calculator::inputs(std::string nid, std::list<unit::pin*> pins)
{ bool res = false;

  for (unit& u : units)
  { for (unit::pin& p : u.i)
    { if (p.nid == nid) { pins.push_back(&p); }
      res = true; } }

  return res; }

bool circuit_calculator::output(std::string nid, unit::pin& pin)
{ for (unit& u : units)
  { for (unit::pin& p : u.o)
    { if (p.nid == nid) { pin = p; return true; } } }

  return false; }
// <--- circuit calculator

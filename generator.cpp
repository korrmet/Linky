#include "generator.hpp"
#include "solver.hpp"
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

// ---> handler
void generator::handler(void* ctx, IM mess)
{ // ---> generate code
  if (mess == "generate code")
  { if (!context(ROOT/"solution")) { return; }

    std::string header; std::string source;
    std::string tmp = context[ROOT/"circuit file path"];
    std::string fname;

    for (char c : tmp)
    { if (c == '.') { break; }
      if (c == '/') { fname.clear(); }
      fname.push_back(c); }
    
    std::string header_name = fname; header_name.append(".h");
    std::string source_name = fname; source_name.append(".c");

    // ---> header
    print(header, "#ifndef %s_h\n", fname.c_str());
    print(header, "#define %s_h\n\n", fname.c_str());

    for (std::string iid : context.ls(ROOT/"solution"/"inputs"))
    { std::string iname = circuit[ROOT/"inputs"/iid/"name"];
      int num = context[ROOT/"inputs"/iid];
      print(header, "#define %s_i_%s %d\n",
            fname.c_str(), iname.c_str(), num); }

    print(header, "\n");
    for (std::string oid : context.ls(ROOT/"solution"/"outputs"))
    { std::string oname = circuit[ROOT/"outputs"/oid/"name"];
      int num = context[ROOT/"outputs"/oid];
      print(header, "#define %s_o_%s %d\n",
            fname.c_str(), oname.c_str(), num); }

    print(header, "\n");
    print(header, "#define %s_inputs_size %d\n",
          fname.c_str(), (int)context[ROOT/"solution"/"inputs num"]);
    print(header, "#define %s_outputs_size %d\n",
          fname.c_str(), (int)context[ROOT/"solution"/"outputs num"]);
    print(header, "#define %s_context_size %d\n",
          fname.c_str(), (int)context[ROOT/"solution"/"context num"]);

    print(header, "\n");
    print(header, "void %s(float* i, float* o, float* ctx);\n", fname.c_str());

    print(header, "\n#endif\n");
    std::ofstream header_file(header_name);
    if (!header_file.is_open()) { PRINT("Can't write generated header!\n"); }
    else { header_file << header; header_file.close(); }
    // <--- header
    
    // ---> source
    print(source, "#include \"%s.h\"\n\n", fname.c_str());
    print(source, "void %s(float* i, float* o, float* ctx)\n", fname.c_str());
    print(source, "{\n");

    for (std::string step : context.ls(ROOT/"solution"/"sequence"))
    { std::string cmd = context[ROOT/"solution"/"sequence"/step/"cmd"];

      if (cmd == "net = input")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int input = context[ROOT/"solution"/"sequence"/step/"input"];
        print(source, "  ctx[%d] = i[%d];\n", net, input); }

      else if (cmd == "output = net")
      { int output = context[ROOT/"solution"/"sequence"/step/"output"];
        int net = context[ROOT/"solution"/"sequence"/step/"net"];
        print(source, "  o[%d] = ctx[%d];\n", output, net); }
      
      else if (cmd == "net = value")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        float value = context[ROOT/"solution"/"sequence"/step/"value"];
        print(source, "  ctx[%d] = %f;\n", net, value); }
      
      else if (cmd == "net = net1 + net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] + ctx[%d];\n",
              net, net1, net2); }
      
      else if (cmd == "net = net1 - net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] - ctx[%d];\n",
              net, net1, net2); }
      
      else if (cmd == "net = net1 * net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] * ctx[%d];\n",
              net, net1, net2); }
      
      else if (cmd == "net = net1 / net2")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] * ctx[%d];\n",
              net, net1, net2); }
      
      else if (cmd == "net = unit[num]")
      { int net = context[ROOT/"solution"/"sequence"/step/"net"];
        int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        print(source, "  ctx[%d] = ctx[%d];\n", net, unit_num); }
      
      else if (cmd == "unit[num1] = unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        print(source, "  ctx[%d] = ctx[%d];\n", unit_num1, unit_num2); }
      
      else if (cmd == "unit[num] = net")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        int net = context[ROOT/"solution"/"sequence"/step/"net"];
        print(source, "  ctx[%d] = ctx[%d];\n", unit_num, net); }
      
      else if (cmd == "unit[num] = val")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] = %f;\n", unit_num, val); }
      
      else if (cmd == "unit[num1] += val * unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] += %f * ctx[%d];\n",
              unit_num1, val, unit_num2); }
      
      else if (cmd == "unit[num1] -= val * unit[num2]")
      { int unit_num1 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num1"];
        int unit_num2 = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                      + (int)context[ROOT/"solution"/"sequence"/step/"num2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] -= %f * ctx[%d];\n",
              unit_num1, val, unit_num2); }
      
      else if (cmd == "unit[num] /= val")
      { int unit_num = (int)context[ROOT/"solution"/"sequence"/step/"unit"]
                     + (int)context[ROOT/"solution"/"sequence"/step/"num"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] /= %f;\n", unit_num, val); }
      
      else if (cmd == "net1 = net2")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d];\n", net1, net2); }
      
      else if (cmd == "net1 = val * net2")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] = %f * ctx[%d];\n", net1, val, net2); }
      
      else if (cmd == "net1 = net2 * (net2 > 0)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] * (ctx[%d] > 0);\n",
              net1, net2, net2); }
      
      else if (cmd == "net1 -= net2 * (net2 < 0)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] -= ctx[%d] * (ctx[%d] < 0);\n",
              net1, net2, net2); }
      
      else if (cmd == "net1 = val * (net2 >= val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] = %f * (ctx[%d] >= %f);\n",
              net1, val, net2, val); }
      
      else if (cmd == "net1 += net2 * (net2 < val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] += ctx[%d] * (ctx[%d] < %f);\n",
              net1, net2, net2, val); }
      
      else if (cmd == "net1 = val * (net2 <= val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] = %f * (ctx[%d] <= %f);\n",
              net1, val, net2, val); }
      
      else if (cmd == "net1 += net2 * (net2 > val)")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        float val = context[ROOT/"solution"/"sequence"/step/"val"];
        print(source, "  ctx[%d] += ctx[%d] * (ctx[%d] > %f);\n",
              net1, net2, net2, val); }

      else if (cmd == "net1 = net2 > net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        print(source, "  ctx[%d] = ctx[%d] > ctx[%d];\n", net1, net2, net3); }

      else if (cmd == "net1 = net2 < net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        print(source, "  ctx[%d] = ctx[%d] < ctx[%d];\n", net1, net2, net3); }

      else if (cmd == "net1 = net2 == net3")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        int net3 = context[ROOT/"solution"/"sequence"/step/"net3"];
        print(source, "  ctx[%d] = ctx[%d] == ctx[%d];\n", net1, net3, net3); }

      else if (cmd == "net1 = net2 <= 0")
      { int net1 = context[ROOT/"solution"/"sequence"/step/"net1"];
        int net2 = context[ROOT/"solution"/"sequence"/step/"net2"];
        print(source, "  ctx[%d] = ctx[%d] <= %f;\n", net1, net2, 0); }
    }

    print(source, "}\n");

    std::ofstream source_file(source_name);
    if (!source_file.is_open()) { PRINT("Can't write generated source!\n"); }
    else { source_file << source; source_file.close(); }
    // <--- source
  }
  // <--- generate code
  
  // ---> generate code
  // this is 100% working code and should be used for reference for nearest
  // time, until bugs would not be solved
  if (mess == "_generate code")
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
      // ---> delay
      if (utype == "delay")
      { print(header, "#define %s_unit_%s %d\n",
              fname.c_str(), unit.c_str(), context_size);
        int value = circuit[ROOT/"units"/unit/"value"];
        print(header, "#define %s_unit_%s_SIZE %d\n",
              fname.c_str(), unit.c_str(), value);
        context_size += value; }
      // <---
      
      // ---> function
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
      // <---
      
      // ---> loopback
      else if (utype == "loopback")
      { int value = circuit[ROOT/"units"/unit/"value"];
        if (value > 1)
        { print(header, "#define %s_unit_%s %d\n",
                fname.c_str(), unit.c_str(), context_size);
          print(header, "#define %s_unit_%s_SIZE %d\n",
                fname.c_str(), unit.c_str(), value - 1);
          context_size += value - 1; } }
      // <---
    }

    for (std::string net : context.ls(ROOT/"network"))
    { print(header, "#define %s_net_%s %d\n",
            fname.c_str(), net.c_str(), context_size);
      context_size++; }

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

    print(source, "\nvoid %s(float* inputs, "
                  " float* outputs, float* context) {\n", fname.c_str());

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
        print(source, "  context[%s_net_%s] = inputs[%s_%s];\n",
              fname.c_str(), nid.c_str(), fname.c_str(), iname.c_str()); }
      // <---

      // ---> output
      else if (type == "output")
      { std::string nid = circuit[ROOT/"outputs"/id/"net"];
        std::string oname = circuit[ROOT/"outputs"/id/"name"];
        print(source, "  outputs[%s_%s] = context[%s_net_%s];\n",
              fname.c_str(), oname.c_str(), fname.c_str(), nid.c_str()); }
      // <---
      
      // ---> units
      else if (type == "unit")
      { // ---> constant
        if (unit_type == "constant")
        { std::string nid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          float value = circuit[ROOT/"units"/id/"value"];
          print(source, "  context[%s_net_%s] = %f;\n",
                fname.c_str(), nid.c_str(), value); }
        // <---
        
        // ---> sum
        else if (unit_type == "sum")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source,
                "  context[%s_net_%s] ="
                " context[%s_net_%s] + context[%s_net_%s];\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), i0nid.c_str(),
                fname.c_str(), i1nid.c_str()); }
        // <---
        
        // ---> difference
        else if (unit_type == "difference")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source,
                "  context[%s_net_%s] ="
                " context[%s_net_%s] - context[%s_net_%s];\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), i0nid.c_str(),
                fname.c_str(), i1nid.c_str()); }
        // <---

        // ---> product
        else if (unit_type == "product")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source,
                "  context[%s_net_%s] ="
                " context[%s_net_%s] * context[%s_net_%s];\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), i0nid.c_str(),
                fname.c_str(), i1nid.c_str()); }
        // <---
        
        // ---> division
        else if (unit_type == "division")
        { std::string onid  = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string i0nid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          std::string i1nid = circuit[ROOT/"units"/id/"inputs"/1/"net"];
          print(source,
                "  context[%s_net_%s] ="
                " context[%s_net_%s] / context[%s_net_%s];\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), i0nid.c_str(),
                fname.c_str(), i1nid.c_str()); }
        // <---

        // ---> delay
        else if (unit_type == "delay")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int value = circuit[ROOT/"units"/id/"value"];

          // set output network
          print(source, "  context[%s_net_%s] = context[%s_unit_%s + %d];\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), id.c_str(),
                value - 1);

          // manage input
          for (unsigned int i = value - 1; i > 0; i--)
          { print(source, "  context[%s_unit_%s + %d] "
                          "= context[%s_unit_%s + %d];\n",
                  fname.c_str(), id.c_str(), i,
                  fname.c_str(), id.c_str(), i - 1); }
          print(source, "  context[%s_unit_%s] = context[%s_net_%s];\n",
                fname.c_str(), id.c_str(), fname.c_str(), inid.c_str()); }
        // <---

        // ---> function
        //      sum[i = 0..n](k_xi * x[n - i]) - sum[j = 1..m](k_yj * y[m - j])
        //  y = ---------------------------------------------------------------
        //      k_y0
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
          print(source, "  context[%s_unit_%s + %d] = context[%s_net_%s];\n",
                fname.c_str(), id.c_str(), num_count - 1,
                fname.c_str(), inid.c_str());

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
          print(source, "  context[%s_net_%s] = context[%s_unit_%s + %d];\n",
                fname.c_str(), onid.c_str(), fname.c_str(), id.c_str(),
                num_count + den_count - 1);
        }
        // <---

        // ---> loopback
        else if (unit_type == "loopback")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          int value = circuit[ROOT/"units"/id/"value"];
          if (value == 1) // simple case with z-1 delay
          { print(source, "  context[%s_net_%s] = context[%s_net_%s];\n",
                  fname.c_str(), onid.c_str(),
                  fname.c_str(), inid.c_str()); }
          else
          { // set output network
            print(source, "  context[%s_net_%s] = context[%s_unit_%s + %d];\n",
                  fname.c_str(), onid.c_str(),
                  fname.c_str(), id.c_str(),
                  value - 2);
           
            // manage input
            for (unsigned int i = value - 2; i > 0; i--)
            { print(source, "  context[%s_unit_%s + %d] "
                            "= context[%s_unit_%s + %d];\n",
                    fname.c_str(), id.c_str(), i,
                    fname.c_str(), id.c_str(), i - 1); }
            print(source, "  context[%s_unit_%s] = context[%s_net_%s];\n",
                  fname.c_str(), id.c_str(), fname.c_str(), inid.c_str()); } }
        // <---
        
        // ---> coefficient
        else if (unit_type == "coeff")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          float value = circuit[ROOT/"units"/id/"value"];

          print(source, "  context[%s_net_%s] = %f * [%s_net_%s];\n",
                fname.c_str(), onid.c_str(),
                value,
                fname.c_str(), inid.c_str()); }
        // <---
        
        // ---> abs
        else if (unit_type == "abs")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];

          print(source, "  context[%s_net_%s]  = "
                        "context[%s_net_%s] * (context[%s_net_%s] > 0);\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), inid.c_str(),
                fname.c_str(), inid.c_str());

          print(source, "  context[%s_net_%s] -= "
                        "context[%s_net_%s] * (context[%s_net_%s] < 0);\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), inid.c_str(),
                fname.c_str(), inid.c_str()); }
        // <---
        
        // ---> limit max
        else if (unit_type == "limit max")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          float value = circuit[ROOT/"units"/id/"value"];

          print(source, "  context[%s_net_%s]  = "
                        "%f * (context[%s_net_%s] >= %f);\n",
                fname.c_str(), onid.c_str(),
                value,
                fname.c_str(), inid.c_str(),
                value);

          print(source, "  context[%s_net_%s] += "
                        "context[%s_net_%s] * (context[%s_net_%s] < %f);\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), inid.c_str(),
                fname.c_str(), inid.c_str(),
                value); }
        // <---
        
        // ---> limit min
        else if (unit_type == "limit min")
        { std::string onid = circuit[ROOT/"units"/id/"outputs"/0/"net"];
          std::string inid = circuit[ROOT/"units"/id/"inputs"/0/"net"];
          float value = circuit[ROOT/"units"/id/"value"];

          print(source, "  context[%s_net_%s]  = "
                        "%f * (context[%s_net_%s] <= %f);\n",
                fname.c_str(), onid.c_str(),
                value,
                fname.c_str(), inid.c_str(),
                value);

          print(source, "  context[%s_net_%s] += "
                        "context[%s_net_%s] * (context[%s_net_%s] > %f);\n",
                fname.c_str(), onid.c_str(),
                fname.c_str(), inid.c_str(),
                fname.c_str(), inid.c_str(),
                value); }
        // <---
      }
      // <---
    }
    print(source, "}\n");

    std::ofstream source_file(source_name);
    if (!source_file.is_open()) { PRINT("Can't write generated source!\n"); }
    else { source_file << source; source_file.close(); }
    // <---
  }
  // <---
}
// <---

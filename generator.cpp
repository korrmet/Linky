#include "generator.hpp"

void generator::handler(void* ctx, IM mess)
{ if (mess == "make network")
  { // TODO: Network algorithm
    //       Scan the wires and look for conjunctions. Add conjuncted wires to
    //       the current network list and delete it from the initial list. When
    //       there is no conjunctions add the network to the network list.
    //       Repeat until all of the wires gone

    std::list<std::string> wires = circuit.ls(root/"wires");
    std::list<std::string> current_net;
    while (wires.size())
    { unsigned int added = 0;
      do
      { for (std::string wire : wires)
        { if (!current_net.size())
          { current_net.push_back(wire); wires.remove(wire);
            added++; continue; }

          for (std::string nwire : current_net)
          {
          }
        }
      } while (added);

      // add the current net to the network
      int netnum = 0;
      for (std::string net : context.ls(root/"network"))
      { if (std::stoi(net) > netnum) { netnum = std::stoi(net); } }
      netnum++;

      for (std::string wire : current_net)
      { context[root/"network"/netnum/"wires"/wire]; }
    }
  } }

#include "circuit_calculator.hpp"

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

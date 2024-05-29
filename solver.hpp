#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "app.hpp"

namespace solver {

void handler(void* ctx, IM mess);

}

#include <string>
#include <list>

// ---> circuit calculator
class circuit_calculator
{ public:

  /** \brief Adds unit to the calculator
   *  \param id Identifier of the unit, should be unique for each */
  circuit_calculator& u(std::string id);

  /** \brief Adds input to last added unit
   *  \param id Identifier of the input, should be unique pin name inside
   *            the unit */
  circuit_calculator& i(std::string id);

  /** \brief Adds output to last added unit
   *  \param id     Identifier of the output, should be unique pin name
   *                inside the unit
   *  \param active This flag says that this output is alerady solved */
  circuit_calculator& o(std::string id, bool active = false);

  /** \brief Connects unit pin to the net
   *  \param uid Unit identifier string
   *  \param pid Pin identifier string
   *  \param nid Net identifier string */
  circuit_calculator& connect(std::string uid,
                              std::string pid,
                              std::string nid);

  /** \brief Make sequence of units calculation
   *  \param s Reference to the list of strings to store the sequence
   *  \retrun Result of scheduling
   *  \retval true Calculations was performed well, the list of strings
   *               contains right sequence of the units
   *  \retval false Something went wrong, some units can't be calculated
   *                because of the possible loops */
  bool schedule(std::list<std::string>& s);

  struct unit
  { struct pin { pin() : active(false) {}
                 bool active; std::string id; std::string nid; };
    unit() : solved(false) {}
    std::string id;
    bool solved;
    std::list<pin> i, o; };

  private:
  std::list<unit> units;

  bool inputs(std::string nid, std::list<unit::pin*> pins);
  bool output(std::string nid, unit::pin& pin); };
// <--- circuit calculator

#endif // SOLVER_HPP

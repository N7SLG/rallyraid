
#include "Competitor.h"

Competitor::Competitor(unsigned int num,
    const std::string& name,
    const std::string& coName,
    const std::string& teamName,
    const std::string& vehicleTypeName,
    unsigned int strength,
    bool ai)
    : num(num),
      name(name),
      coName(coName),
      teamName(teamName),
      vehicleTypeName(vehicleTypeName),
      strength(strength),
      ai(ai)
{
}

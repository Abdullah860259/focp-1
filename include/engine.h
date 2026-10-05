#pragma once
#include <string>

// Used for damage level, water scarcity and food availability
enum Level
{
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2
};

// Everything the user tells us about one relief request
struct HelpRequest
{
    std::string areaName;
    Level damageLevel;
    int familiesAffected;
    bool routeOpen;
    int distanceKm;          // only asked when the route is open, otherwise 0
    Level waterScarcity;
    Level foodAvailability;
    int injuredPeople;
    bool previousAid;
};

void getHelp();
HelpRequest takeInputs();

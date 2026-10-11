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
    int distanceKm; // only asked when the route is open, otherwise 0
    Level waterScarcity;
    Level foodAvailability;
    int injuredPeople;
    bool previousAid;
};

// The four relief packages; the order matches the PACKAGES table below
enum Action
{
    FULL_RELIEF = 0,
    STANDARD_RELIEF = 1,
    EMERGENCY_KIT = 2,
    DEFER = 3
};

// What one package costs to send
struct ReliefPackage
{
    std::string name;
    int trucksNeeded;
    int packsNeeded;
    int waterBottlesNeeded;
    int foodPacketsNeeded;
    int medicineKitsNeeded;
};

const int PACKAGE_COUNT = 4;
extern const ReliefPackage PACKAGES[PACKAGE_COUNT];

bool canAfford(Action action);
bool dispatchPackage(Action action);

int calculateNeedScore(const HelpRequest &request);
void saveRequest(const HelpRequest &request);
void getHelp();
void addStock();
HelpRequest takeInputs();

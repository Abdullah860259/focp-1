// storage.h - saving requests to data/requests.csv.
// Definitions are in src/storage.cpp.
#pragma once
#include "engine.h"

void saveRequest(const HelpRequest &request);   // add one request as a new CSV line

std::string levelToText(Level level);   // LOW -> "Low"
std::string yesNo(bool value);          // true -> "Yes"
void showRequestHistorySummary();       // the "history" command

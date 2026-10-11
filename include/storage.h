#pragma once
#include "engine.h"

void saveRequest(const HelpRequest &request);

std::string levelToText(Level level);
std::string yesNo(bool value);
void showRequestHistorySummary();

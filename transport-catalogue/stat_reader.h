#pragma once
#include "transport_catalogue.h"
#include <algorithm>
#include <vector>

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output);
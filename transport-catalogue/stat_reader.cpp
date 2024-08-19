#include "stat_reader.h"
#include <unordered_set>
#include <numeric>
#include <iostream>

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
    std::ostream& output) {

    auto command = request.substr(0, request.find_first_of(' '));

    if (command == "Bus") {
        auto space = request.find_first_of(' ');
        auto symbol_after_space = space + 1;
        auto name = request.substr(symbol_after_space, request.size() - symbol_after_space);
        auto* bus = transport_catalogue.FindBus(name);

        if (bus) {
            output << request << ": " << bus->stops.size() << " stops on route, "
                << (transport_catalogue.GetUniqueStops(*bus)).size() << " unique stops, "
                << transport_catalogue.AllDistance(bus->stops) << " route length, "
                << transport_catalogue.AllDistance(bus->stops) / transport_catalogue.GetGeoDistance(*bus) << " curvature"
                << "\n";
        }
        else {
            output << request << ": not found" << "\n";
        }
    }
    else {
        std::string_view stop_query = request.substr(request.find_first_of(' ') + 1);
        const TransportCatalogue::Stop* stop_pointer = transport_catalogue.FindStop(stop_query);
        std::set<std::string_view> buses_in_stop = transport_catalogue.GetUniqueBus(stop_pointer);
        if (stop_pointer == nullptr) {
            output << request << ": not found" << std::endl;
        }
        else if (buses_in_stop.empty()) {
            output << request << ": no buses" << std::endl;
        }
        else {
            output << request << ": buses ";
            for (auto& bus : buses_in_stop) {
                output << bus << " ";
            }
            output << std::endl;
        }
    }
}
#include "transport_catalogue.h"
namespace transport_catalogue {
void TransportCatalogue::AddStop(const std::string_view& name, const geo::Coordinates& coords) {
    stops_.push_back({ std::string(name), coords, {} });
    stopname_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view& name, const std::vector<const Stop*> stops, bool circular_route) {
    buses_.push_back({ std::string(name), stops, circular_route });
    busname_[buses_.back().name] = &buses_.back();
    for (const auto& route_stop : stops) {
        for (auto& stop_ : stops_) {
            if (stop_.name == route_stop->name) stop_.buses_by_stop.insert(std::string(name));
        }
    }
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    return stopname_.count(name) ? stopname_.at(name) : nullptr;
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    return busname_.count(name) ? busname_.at(name) : nullptr;
}

void TransportCatalogue::AddDistance(const Stop* from, const Stop* to, const int distance) {
    distance_bus_to_stop_[{from, to}] = distance;
}

int TransportCatalogue::GetDistance(const Stop* first, const Stop* last) const {
    std::pair<const Stop*, const Stop*> key = { first, last };
    auto it = distance_bus_to_stop_.find(key);

    if (it != distance_bus_to_stop_.end()) {
        return it->second;
    }
    else {
        std::pair<const Stop*, const Stop*> rkey = { last, first };
        auto it = distance_bus_to_stop_.find(rkey);
        if (it != distance_bus_to_stop_.end()) {
            return it->second;
        }
    }
    return 0;
}

int TransportCatalogue::AllDistance(const std::vector<const Stop*>& names) const {
    if (distance_bus_to_stop_.empty()) {
        return 0;
    }
    int result = 0;
    for (int i = 0; i < names.size() - 1; ++i) {
        result += GetDistance(names[i], names[i + 1]);
    }
    return result;
}

size_t TransportCatalogue::GetUniqueStops(std::string_view bus) const {
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : busname_.at(bus)->stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}
const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedAllBuses() const {
    std::map<std::string_view, const Bus*> result;
    for (const auto& bus : busname_) {
        result.emplace(bus);
    }
    return result;
}
}//transport_catalogue
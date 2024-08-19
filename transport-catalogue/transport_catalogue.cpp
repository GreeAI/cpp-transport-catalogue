#include "transport_catalogue.h"
<<<<<<< HEAD
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
=======
#include "stat_reader.h"

void TransportCatalogue::AddStop(const std::string& name, const detail::Coordinates& coords) {
    Stop stops = { name, coords };
    stops_.emplace_back(stops);
    const Stop& stop = stops_.back();
    buses_to_stop_[&stop] = {};
    stopname_.emplace(stop.name, stop);

}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string_view>& stops) {
    TransportCatalogue::Bus bus_to_add;
    bus_to_add.name = name;
    for (const std::string_view& stop : stops) {
        auto it = stopname_.find(stop);
        if (it != stopname_.end()) {
            const Stop& stop_ref = it->second;
            bus_to_add.stops.emplace_back(&stop_ref);
        }
    }
    buses_.emplace_back(std::move(bus_to_add));
    const TransportCatalogue::Bus& bus = buses_.back();
    for (const TransportCatalogue::Stop* path : bus.stops) {
        buses_to_stop_[path].emplace(static_cast<std::string_view>(bus.name));
    }
    const TransportCatalogue::Bus& bus_ref = buses_.back();
    busname_.emplace(bus_ref.name, bus_ref);

}

const TransportCatalogue::Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stopname_.find(name);
    return it != stopname_.end() ? &it->second : nullptr;
}

const TransportCatalogue::Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = busname_.find(name);
    return it != busname_.end() ? &it->second : nullptr;
}

void TransportCatalogue::AddDistance(const TransportCatalogue::Stop* first, const TransportCatalogue::Stop* last, int distance) {
    distance_bus_to_stop_[{first, last}] = distance;
}

int TransportCatalogue::GetDistance(const TransportCatalogue::Stop* first, const TransportCatalogue::Stop* last) const {
>>>>>>> b045a1c9fcbf4c2e2d3f3c1ea9804c87b09daab7
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

<<<<<<< HEAD
int TransportCatalogue::AllDistance(const std::vector<const Stop*>& names) const {
=======
int TransportCatalogue::AllDistance(const std::vector<const TransportCatalogue::Stop*>& names) const {
>>>>>>> b045a1c9fcbf4c2e2d3f3c1ea9804c87b09daab7
    if (distance_bus_to_stop_.empty()) {
        return 0;
    }
    int result = 0;
    for (int i = 0; i < names.size() - 1; ++i) {
        result += GetDistance(names[i], names[i + 1]);
    }
    return result;
}

<<<<<<< HEAD
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
=======
double TransportCatalogue::GetGeoDistance(const TransportCatalogue::Bus& bus) const {
    double distance = 0.0;
    if (bus.stops.size() > 1) {
        for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
            distance += ComputeDistance(bus.stops[i]->coordinates, bus.stops[i + 1]->coordinates);
        }
    }
    return distance;
}

std::set<std::string_view> TransportCatalogue::GetUniqueBus(const Stop* name) const {
    std::set<std::string_view> buss;
    if (buses_to_stop_.count(name) == 1) {
        for (const auto& [stop_name, name_bus] : buses_to_stop_) {
            if (stop_name == name) {
                for (const auto& bus : name_bus) {
                    buss.insert(bus);
                }
            }
        }
    }
    return buss;
}

std::unordered_set<const TransportCatalogue::Stop*> TransportCatalogue::GetUniqueStops(const TransportCatalogue::Bus& bus) const {
    return std::unordered_set<const TransportCatalogue::Stop*>(bus.stops.begin(), bus.stops.end());
}
>>>>>>> b045a1c9fcbf4c2e2d3f3c1ea9804c87b09daab7

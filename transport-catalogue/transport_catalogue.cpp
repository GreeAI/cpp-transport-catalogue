#include "transport_catalogue.h"

namespace transport_catalogue {
void TransportCatalogue::AddStop(const std::string_view name, const geo::Coordinates& coords) {
    stops_.push_back({ std::string(name), coords, {} });
    stopname_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<const Stop*> stops, bool circular_route) {
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

int TransportCatalogue::AllDistance(const Stop* from, const Stop* to) const {
    if (distance_bus_to_stop_.count({ from, to })) {
        return distance_bus_to_stop_.at({from, to});
    }
    else if (distance_bus_to_stop_.count({ to, from })) {
        return distance_bus_to_stop_.at({ to, from });
    }
    return 0;
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
    for (const auto& bus : busname_) result.emplace(bus);
    return result;
}
const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedAllStops() const {
    std::map<std::string_view, const Stop*> result;
    for (const auto& stop : stopname_) result.emplace(stop);
    return result;
}
std::optional<transport_catalogue::BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_number) const {
    transport_catalogue::BusStat bus_stat{};
    const transport_catalogue::Bus* bus = FindBus(bus_number);

    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->circular_route) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->circular_route) {
            route_length += AllDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates);
        }
        else {
            route_length += AllDistance(from, to) + AllDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = GetUniqueStops(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}
}//transport_catalogue
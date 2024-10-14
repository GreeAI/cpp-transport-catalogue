#pragma once

#include "geo.h"
#include "domain.h"

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <map>
#include <optional>
#include <stdexcept>

namespace transport_catalogue{

class TransportCatalogue {
public:
    struct DistanceHasher {
        std::hash<const void*> hasher_;

        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
            auto hash_1 = static_cast<const void*>(pair_stops.first);
            auto hash_2 = static_cast<const void*>(pair_stops.second);
            return hasher_(hash_1) * 17 + hasher_(hash_2);
        }
    };
    
    void AddStop(const std::string_view name, const geo::Coordinates& coords);
    void AddBus(const std::string_view name, const std::vector<const Stop*> stops, bool circular_route);
    const Stop* FindStop(std::string_view stop_name) const;
    const Bus* FindBus(std::string_view bus_name) const;
    void AddDistance(const Stop* from, const Stop* to, const int distance);
    int AllDistance(const Stop* from, const Stop* to) const;
    size_t GetUniqueStops(std::string_view bus) const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
    const std::map<std::string_view, const Stop*> GetSortedAllStops() const;
    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view& bus_number) const;
    std::vector<const Stop*> GetStops() const;
    std::vector<const Bus*> GetBuses() const;

private:
    using DistanceContainer = std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher>;

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stopname_;
    std::unordered_map<std::string_view, const Bus*> busname_;
    DistanceContainer distance_bus_to_stop_;
};
}
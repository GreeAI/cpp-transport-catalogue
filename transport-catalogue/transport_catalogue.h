#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>

#include "geo.h"


class TransportCatalogue {
public:

    struct Stop {
        std::string name;
        detail::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
    };

    struct DistanceHasher {
        std::hash<const void*> hasher_;

        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
            auto hash_1 = static_cast<const void*>(pair_stops.first);
            auto hash_2 = static_cast<const void*>(pair_stops.second);
            return hasher_(hash_1) * 17 + hasher_(hash_2);
        }
    };

    void AddStop(const std::string& name, const detail::Coordinates& coords);
    void AddBus(const std::string& bus, const std::vector<std::string_view>& stops);
    const Stop* FindStop(std::string_view stop_name) const;
    const Bus* FindBus(std::string_view bus_name) const;
    void AddDistance(const Stop* first, const Stop* last, int distance);
    int GetDistance(const Stop* first, const Stop* last) const;
    int AllDistance(const std::vector<const Stop*>& names) const;
    double GetGeoDistance(const Bus& bus) const;
    std::set<std::string_view> GetUniqueBus(const Stop* name) const;
    std::unordered_set<const Stop*> GetUniqueStops(const Bus& bus) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop&> stopname_;
    std::unordered_map<std::string_view, const Bus&> busname_;
    std::unordered_map<const Stop*, std::set<std::string_view>> buses_to_stop_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, DistanceHasher> distance_bus_to_stop_;
};
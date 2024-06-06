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
    struct Bus;

    struct Stop {
        std::string name;
        detail::Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
    };
    void AddStop(const std::string& name, const detail::Coordinates& coords);
    void AddBus(const std::string& bus, const std::vector<std::string_view>& stops);
    const Stop* FindStop(std::string_view stop_name) const;
    const Bus* FindBus(std::string_view bus_name) const;
    double GetDistance(const Bus& bus) const;
    std::set<std::string_view> GetUniqueBus(const Stop* name) const;
    std::unordered_set<const Stop*> GetUniqueStops(const Bus& bus) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop&> stopname_;
    std::unordered_map<std::string_view, const Bus&> busname_;
    std::unordered_map<const Stop*, std::set<std::string_view>> buses_to_stop_;
};

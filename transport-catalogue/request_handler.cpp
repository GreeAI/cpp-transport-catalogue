#include "request_handler.h"

std::optional<transport_catalogue::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_number) const {
    transport_catalogue::BusStat bus_stat{};
    const transport_catalogue::Bus* bus = catalogue_.FindBus(bus_number);

    if (!bus) throw std::invalid_argument("bus not found");
    if (bus->circular_route) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->circular_route) {
            route_length += catalogue_.AllDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates);
        }
        else {
            route_length += catalogue_.AllDistance(from, to) + catalogue_.AllDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates,
                to->coordinates) * 2;
        }
    }
    bus_stat.unique_stops_count = catalogue_.GetUniqueStops(bus_number);
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStop(stop_name)->buses_by_stop;
}

bool RequestHandler::IsBus(const std::string_view bus) const {
    return catalogue_.FindBus(bus);
}

bool RequestHandler::IsStop(const std::string_view stop) const {
    return catalogue_.FindStop(stop);
}

svg::Document RequestHandler::RenderMap() const {
    return render_.GetSVG(catalogue_.GetSortedAllBuses());
}
const std::optional<transport::RouteInfo> RequestHandler::GetRouter(const std::string_view from, const std::string_view to) const {
    return router_.GetRoute(from, to);
}

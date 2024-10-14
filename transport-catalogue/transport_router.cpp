#include "transport_router.h"

namespace transport {

    void Router::BuildGraph(const transport_catalogue::TransportCatalogue& catalogue) {
        const auto& all_stops = catalogue.GetStops();
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        graph_ = std::move(stops_graph);

        size_t i = 0;
        for (const auto stop : all_stops) {
            graph::VertexId first_id = i++;
            graph::VertexId second_id = i++;
            stop_ids_[stop->name] = StopIdVertex{ first_id, second_id };
        }
        for (const auto [stop_ptr, ids] : stop_ids_) {
            graph::EdgeId id = graph_.AddEdge(graph::Edge<double>{
                ids.begin, ids.end, static_cast<double>(settings_.bus_wait_time)});
            edges_info_[id] = WaitInfo{ stop_ptr, static_cast<double>(settings_.bus_wait_time) };
        }

        const auto& all_buses = catalogue.GetBuses();
        for (const auto& bus : all_buses) {
            auto stops = bus->stops;
            if (!bus->circular_route) {
                for (auto it = bus->stops.rbegin() + 1; it != bus->stops.rend(); ++it) {
                    stops.push_back(*it);
                }
            }
            for (size_t from = 0; from < stops.size() - 1; ++from) {
                int distance = 0;
                for (size_t to = from + 1; to < stops.size(); ++to) {
                    if (stops[from] == stops[to]) {
                        continue;
                    }

                    int span_count = static_cast<int>(to - from);

                    distance += catalogue.AllDistance(stops[to - 1], stops[to]);
                    double weight = static_cast<double>(distance) / (settings_.bus_velocity * 1000 / 60);

                    const transport_catalogue::Stop* stop_from = stops[from];
                    const transport_catalogue::Stop* stop_to = stops[to];
                    graph::EdgeId edge_id = graph_.AddEdge(graph::Edge<double>{stop_ids_.at(stop_from->name).end, stop_ids_.at(stop_to->name).begin, weight});
                    edges_info_.emplace(edge_id, BusInfo{bus->name, span_count, weight });
                }
            }
        }
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    const std::variant<WaitInfo, BusInfo>& Router::GetEdgesInfo(graph::EdgeId edge_id) const {
        return edges_info_.at(edge_id);
    }

    std::optional<RouteInfo> Router::GetRoute(const std::string_view from, const std::string_view to) const {
        auto route = router_->BuildRoute(stop_ids_.at(from).begin, stop_ids_.at(to).begin);
        if (!route) {
            return std::nullopt;
        }
        RouteInfo route_info;
        route_info.time = route->weight;
        for (const auto& edge_id : route->edges) {
            route_info.edge_info.push_back(GetEdgesInfo(edge_id));
        }

        return route_info;
    }
}

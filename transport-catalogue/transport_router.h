#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <variant>
#include <memory>

namespace transport {

    struct RoutingSettings {
        int bus_wait_time = 1;
        double bus_velocity = 1.;
    };

    struct WaitInfo {
        std::string_view name;
        double time;
    };

    struct BusInfo {
        std::string_view name;
        int span_count;
        double time;
    };
    struct StopIdVertex {
        graph::VertexId begin;
        graph::VertexId end;
    };
    struct RouteInfo {
        double time = 0.;
        std::vector < std::variant<WaitInfo, BusInfo>> edge_info;
    };


    class Router {
    public:

        static constexpr double Coefficient = (100.0 / 6.0);

        Router() = default;
        Router(const RoutingSettings& settings, const transport_catalogue::TransportCatalogue& catalogue)
            :catalogue_(catalogue),
             settings_(settings) {
            BuildGraph(catalogue);
        }


        std::optional<RouteInfo> GetRoute(const std::string_view from, const std::string_view to) const;
        void BuildGraph(const transport_catalogue::TransportCatalogue& catalogue);
        const std::variant<WaitInfo, BusInfo>& GetEdgesInfo(graph::EdgeId edge_id) const;

    private:
        const transport_catalogue::TransportCatalogue& catalogue_;
        const RoutingSettings settings_;
        std::unique_ptr<graph::Router<double>> router_;
        std::map<std::string_view, StopIdVertex> stop_ids_;
        std::unordered_map<graph::EdgeId, std::variant<WaitInfo, BusInfo>> edges_info_;
        graph::DirectedWeightedGraph<double> graph_;
    };
}
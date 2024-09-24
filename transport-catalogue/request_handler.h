#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <optional>

class RequestHandler {
public:
    explicit RequestHandler(const map_render::MapRender& render, transport_catalogue::TransportCatalogue& catalogue, const transport::Router& rout)
        : render_(render)
        , catalogue_(catalogue)
        , router_(rout)
    {}

    // ���������� �������������� ������ ������������ ��������� ����������� ��������. ��������� ����� ���������� ���������, ���������� ���������, ����� ����� �������� � ��� ��������
    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view& bus_number) const;
    // ��� �������� ��������� ���������� ����� ���� ���������� ���������, ������� ����� ��� ��������
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
    // ����� ����� ������� � ��������� � json_reader
    bool IsBus(const std::string_view bus) const;
    bool IsStop(const std::string_view stop) const;

    const std::optional<graph::Router<double>::RouteInfo> GetRouter(const std::string_view from, const std::string_view to) const;
    const graph::DirectedWeightedGraph<double>& GetGraph() const;

    svg::Document RenderMap() const;

private:
    const map_render::MapRender& render_;
    const transport_catalogue::TransportCatalogue& catalogue_;
    const transport::Router& router_;
};
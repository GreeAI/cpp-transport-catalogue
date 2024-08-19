#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>

class RequestHandler {
public:
    explicit RequestHandler(const map_render::MapRender& render, transport_catalogue::TransportCatalogue& catalogue)
        : render_(render)
        , catalogue_(catalogue)
    {}

    // ���������� �������������� ������ ������������ ��������� ����������� ��������. ��������� ����� ���������� ���������, ���������� ���������, ����� ����� �������� � ��� ��������
    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view& bus_number) const;
    // ��� �������� ��������� ���������� ����� ���� ���������� ���������, ������� ����� ��� ��������
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
    // ����� ����� ������� � ��������� � json_reader
    bool IsBus(const std::string_view bus) const;
    bool IsStop(const std::string_view stop) const;

    svg::Document RenderMap() const;

private:
    const map_render::MapRender& render_;
    const transport_catalogue::TransportCatalogue& catalogue_;
};
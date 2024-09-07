#pragma once

#include <sstream>

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}
    // ���������� ���� JSON, ���������� ������� �������
    const json::Node& GetBaseRequests() const;
    // ���������� ���� JSON, ���������� �������������� �������
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;

    // ������������ ������� ������� �� JSON � ��������� ���������� ������� ���������� ����������� �� ���������� � ��������� ���������
    void FillCatalogue(transport_catalogue::TransportCatalogue& catalogue);
    void ProcessStopRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue);
    void ProcessBusRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue);

    svg::Color ExtractColor(const json::Node& color_node) const;
    map_render::MapRender FillRenderSettings(const json::Dict& request_map) const;

    // ������������ ������� ���������� �� �������������� ������������ ������� ��������. �������� ����������� �� ������� �������, � � ����������� �� ���� �������
    void ProcessRequests(const json::Node& stat_requests, RequestHandler& requests) const;
    // ������� ������������ ������ ���������� ��� ����������� �������� ��������. ������� ��� �������� �� ������� � ��������� ����� � ��������
    const json::Node PrintBus(const json::Dict& request_map, RequestHandler& requests) const;
    // ������� ������������ ������ ���������� ��� ���������� ���������. ���� ��������� ������� � ��������, ������� �������� ���������, ���������� ����� ��� ���������
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& requests) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& requests) const;

private:
    json::Document input_; 
    json::Node dummy_ = nullptr;

    // ��������� ���������� �� ����� ��������� �� JSON. 
    // ���������� ������, ���������� �������� ���������, � ���������� � ���������� �� ������ ���������.
    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dict& request_map) const;
    // ��������� ���������� � �������� ���������� ������������ ����� �����������
    void FillStopDistances(transport_catalogue::TransportCatalogue& catalogue) const;
    // ��������� ���������� � �������� �������� �� JSON � ���������� ������
    std::tuple<std::string_view, std::vector<const transport_catalogue::Stop*>, bool> FillRoute(const json::Dict& request_map, transport_catalogue::TransportCatalogue& catalogue) const;
};
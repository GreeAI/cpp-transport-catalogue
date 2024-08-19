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

    // ¬озвращает статистические данные относительно заданного автобусного маршрута. ¬ычисл€ет общее количество остановок, уникальные остановки, общую длину маршрута и его кривизну
    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view& bus_number) const;
    // ƒл€ заданной остановки возвращает набор всех автобусных маршрутов, которые через нее проход€т
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
    // „тобы найти автобус и остановку в json_reader
    bool IsBus(const std::string_view bus) const;
    bool IsStop(const std::string_view stop) const;

    svg::Document RenderMap() const;

private:
    const map_render::MapRender& render_;
    const transport_catalogue::TransportCatalogue& catalogue_;
};
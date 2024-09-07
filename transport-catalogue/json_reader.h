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
    // ¬озвращает узел JSON, содержащий базовые запросы
    const json::Node& GetBaseRequests() const;
    // ¬озвращает узел JSON, содержащий статистические запросы
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;

    // ќбрабатывает базовые запросы из JSON и заполн€ет переданный каталог транспорта информацией об остановках и маршрутах автобусов
    void FillCatalogue(transport_catalogue::TransportCatalogue& catalogue);
    void ProcessStopRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue);
    void ProcessBusRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue);

    svg::Color ExtractColor(const json::Node& color_node) const;
    map_render::MapRender FillRenderSettings(const json::Dict& request_map) const;

    // ќбрабатывает запросы статистику из предварительно сохраненного массива запросов. »тераци€ выполн€етс€ по каждому запросу, и в зависимости от типа запроса
    void ProcessRequests(const json::Node& stat_requests, RequestHandler& requests) const;
    // ‘ункци€ обрабатывает запрос статистики дл€ конкретного маршрута автобуса. ƒостает им€ маршрута из запроса и выполн€ет поиск в каталоге
    const json::Node PrintBus(const json::Dict& request_map, RequestHandler& requests) const;
    // ‘ункци€ обрабатывает запрос статистики дл€ конкретной остановки. ≈сли остановка найдена в каталоге, выводит перечень автобусов, проход€щих через эту остановку
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& requests) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& requests) const;

private:
    json::Document input_; 
    json::Node dummy_ = nullptr;

    // »звлекает информацию об одной остановке из JSON. 
    // ¬озвращает кортеж, содержащий название остановки, еЄ координаты и рассто€ни€ до других остановок.
    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dict& request_map) const;
    // ƒополн€ет информацию в каталоге транспорта рассто€ни€ми между остановками
    void FillStopDistances(transport_catalogue::TransportCatalogue& catalogue) const;
    // »звлекает информацию о маршруте автобуса из JSON и возвращает кортеж
    std::tuple<std::string_view, std::vector<const transport_catalogue::Stop*>, bool> FillRoute(const json::Dict& request_map, transport_catalogue::TransportCatalogue& catalogue) const;
};
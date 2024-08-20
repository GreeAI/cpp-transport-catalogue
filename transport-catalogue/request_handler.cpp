#include "request_handler.h"

std::optional<transport_catalogue::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_number) const {
    return catalogue_.GetBusStat(bus_number);
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
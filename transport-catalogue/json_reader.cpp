#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    auto it = input_.GetRoot().AsMap().find("base_requests");
    if (it == input_.GetRoot().AsMap().end()) return dummy_;
    return it->second;
    //if (!input_.GetRoot().AsMap().count("base_requests")) return dummy_;
    //return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetRenderSettings() const {
    auto it = input_.GetRoot().AsMap().find("render_settings");
    if (it == input_.GetRoot().AsMap().end()) return dummy_;
    return it->second;
    //if (!input_.GetRoot().AsMap().count("render_settings")) return dummy_;
    //return input_.GetRoot().AsMap().at("render_settings");
}

const json::Node& JsonReader::GetStatRequests() const {
    auto it = input_.GetRoot().AsMap().find("stat_requests");
    if (it == input_.GetRoot().AsMap().end()) return dummy_;
    return it->second;
    //if (!input_.GetRoot().AsMap().count("stat_requests")) return dummy_;
    //input_.GetRoot().AsMap().at("stat_requests");
}

void JsonReader::FillCatalogue(transport_catalogue::TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    ProcessStopRequests(arr, catalogue);
    FillStopDistances(catalogue);
    ProcessBusRequests(arr, catalogue);
}
void JsonReader::ProcessStopRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue) {
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates);
        }
    }
}

void JsonReader::ProcessBusRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue) {
    for (const auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsMap();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, catalogue);
            catalogue.AddBus(bus_number, stops, circular_route);
        }
    }
}

map_render::RenderSettings JsonReader::FillRenderSettings(const json::Dict& request_map) const {
    map_render::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();

    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();

    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    render_settings.underlayer_color = ExtractColor(request_map.at("underlayer_color"));
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        render_settings.color_palette.emplace_back(ExtractColor(color_element));
    }

    return render_settings;
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& requests) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, requests).AsMap());
        }
        if (type == "Bus") {
            result.push_back(PrintBus(request_map, requests).AsMap());
        }
        if (type == "Map") {
            result.push_back(PrintMap(request_map, requests).AsMap());
        }
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node JsonReader::PrintBus(const json::Dict& request_map, RequestHandler& requests) const {
    json::Dict result;
    const std::string& bus_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!requests.IsBus(bus_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = requests.GetBusStat(bus_name)->curvature;
        result["route_length"] = requests.GetBusStat(bus_name)->route_length;
        result["stop_count"] = static_cast<int>(requests.GetBusStat(bus_name)->stops_count);
        result["unique_stop_count"] = static_cast<int>(requests.GetBusStat(bus_name)->unique_stops_count);
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& requests) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!requests.IsStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : requests.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& requests) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream foo;
    svg::Document map = requests.RenderMap();
    map.Render(foo);
    result["map"] = foo.str();

    return json::Node{ result };
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsMap();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return { stop_name, coordinates, stop_distances };
}

void JsonReader::FillStopDistances(transport_catalogue::TransportCatalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            for (auto& [to_name, dist] : stop_distances) {
                auto from = catalogue.FindStop(stop_name);
                auto to = catalogue.FindStop(to_name);
                catalogue.AddDistance(from, to, dist);
            }
        }
    }
}

svg::Color JsonReader::ExtractColor(const json::Node& color_node) const {
    if (color_node.IsString()) {
        return color_node.AsString();
    }
    else if (color_node.IsArray()) {
        const json::Array& color_array = color_node.AsArray();
        if (color_array.size() == 3) {
            return svg::Rgb(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt());
        }
        else if (color_array.size() == 4) {
            return svg::Rgba(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt(), color_array[3].AsDouble());
        }
        else {
            throw std::logic_error("wrong color type");
        }
    }
    else {
        throw std::logic_error("wrong color type");
    }
}

std::tuple<std::string_view, std::vector<const transport_catalogue::Stop*>, bool> JsonReader::FillRoute(const json::Dict& request_map, transport_catalogue::TransportCatalogue& catalogue) const {
    std::string_view bus_number = request_map.at("name").AsString();
    std::vector<const transport_catalogue::Stop*> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(catalogue.FindStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();

    return { bus_number, stops, circular_route };
}
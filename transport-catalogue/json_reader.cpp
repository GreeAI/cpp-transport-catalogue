#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    auto it = input_.GetRoot().AsDict().find("base_requests");
    if (it == input_.GetRoot().AsDict().end()) return dummy_;
    return it->second;
    //if (!input_.GetRoot().AsMap().count("base_requests")) return dummy_;
    //return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetRenderSettings() const {
    auto it = input_.GetRoot().AsDict().find("render_settings");
    if (it == input_.GetRoot().AsDict().end()) return dummy_;
    return it->second;
    //if (!input_.GetRoot().AsMap().count("render_settings")) return dummy_;
    //return input_.GetRoot().AsMap().at("render_settings");
}

const json::Node& JsonReader::GetStatRequests() const {
    auto it = input_.GetRoot().AsDict().find("stat_requests");
    if (it == input_.GetRoot().AsDict().end()) return dummy_;
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
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates);
        }
    }
}

void JsonReader::ProcessBusRequests(const json::Array& arr, transport_catalogue::TransportCatalogue& catalogue) {
    for (const auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, catalogue);
            catalogue.AddBus(bus_number, stops, circular_route);
        }
    }
}

map_render::MapRender JsonReader::FillRenderSettings(const json::Dict& request_map) const {
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

    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("wrong underlayer colortype");
    }
    else throw std::logic_error("wrong underlayer color");

    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("wrong color_palette type");
        }
        else throw std::logic_error("wrong color_palette");
    }

    return render_settings;
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& requests) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, requests).AsDict());
        }
        if (type == "Bus") {
            result.push_back(PrintBus(request_map, requests).AsDict());
        }
        if (type == "Map") {
            result.push_back(PrintMap(request_map, requests).AsDict());
        }
    }

    json::Print(json::Document{ result }, std::cout);
}

const json::Node JsonReader::PrintBus(const json::Dict& request_map, RequestHandler& requests) const {
    json::Node result;
    const std::string& route_number = request_map.at("name").AsString();
    const int id = request_map.at("id").AsInt();

    if (!requests.IsBus(route_number)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        const auto& route_info = requests.GetBusStat(route_number);
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("curvature").Value(route_info->curvature)
            .Key("route_length").Value(route_info->route_length)
            .Key("stop_count").Value(static_cast<int>(route_info->stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(route_info->unique_stops_count))
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& requests) const {
    json::Node result;
    const std::string& stop_name = request_map.at("name").AsString();
    const int id = request_map.at("id").AsInt();

    if (!requests.IsStop(stop_name)) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array buses;
        for (const auto& bus : requests.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& requests) const {
    json::Node result;
    const int id = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = requests.RenderMap();
    map.Render(strm);

    result = json::Builder{}
        .StartDict()
        .Key("request_id").Value(id)
        .Key("map").Value(strm.str())
        .EndDict()
        .Build();

    return result;
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsDict();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return { stop_name, coordinates, stop_distances };
}

void JsonReader::FillStopDistances(transport_catalogue::TransportCatalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
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
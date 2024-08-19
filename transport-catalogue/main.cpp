#include "json_reader.h"
#include "request_handler.h"

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    JsonReader json(std::cin);

    json.FillCatalogue(catalogue);

    const auto& stat_requests = json.GetStatRequests();
    const auto& render_settings = json.GetRenderSettings().AsMap();
    const auto& render = json.FillRenderSettings(render_settings);

    RequestHandler rh(render, catalogue);
    rh.RenderMap().Render(std::cout);
}
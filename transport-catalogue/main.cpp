#include "json_reader.h"
#include "request_handler.h"

int main() {
#ifdef __APPLE__
    freopen("input.json", "r", stdin);
    freopen("output.json", "w", stdout);
#endif

    transport_catalogue::TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);

    RequestHandler rh(renderer, catalogue);
    json_doc.ProcessRequests(stat_requests, rh);
}
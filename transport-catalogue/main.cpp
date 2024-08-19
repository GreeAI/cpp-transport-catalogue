<<<<<<< HEAD
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
=======
#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(cin, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    cin >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(cin, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
>>>>>>> b045a1c9fcbf4c2e2d3f3c1ea9804c87b09daab7
}
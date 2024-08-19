#pragma once

#include "geo.h"
#include "svg.h"
#include "json.h"
#include "domain.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
namespace map_render {

    inline const double EPSILON = 1e-6;

    bool IsZero(double value);

    class SphereProjector {
    public:
        // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
            double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            if (points_begin == points_end) {
                return;
            }

            // “очки с минимальной и максимальной долготой
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.longitude < rhs.longitude; });
            min_lon_ = left_it->longitude;
            const double max_lon = right_it->longitude;

            // “очки с минимальной и максимальной широтой
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.latitude < rhs.latitude; });
            const double min_lat = bottom_it->latitude;
            max_lat_ = top_it->latitude;

            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                zoom_coeff_ = *height_zoom;
            }
        }

        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.longitude - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.latitude) * zoom_coeff_ + padding_
            };
        }

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };

    struct RenderSettings {
        double width = 0.0;
        double height = 0.0;
        double padding = 0.0;
        double line_width = 0.0;
        double stop_radius = 0.0;
        int bus_label_font_size = 0;
        svg::Point bus_label_offset = { 0.0, 0.0 };
        int stop_label_font_size = 0;
        svg::Point stop_label_offset = { 0.0, 0.0 };
        svg::Color underlayer_color = { svg::NoneColor };
        double underlayer_width = 0.0;
        std::vector<svg::Color> color_palette{};
    };

    class MapRender {
    public:
        MapRender(const RenderSettings& render_settings)
            : render_settings_(render_settings)
        {}

        std::vector<svg::Polyline> GetRouteLines(const std::map<std::string_view, const transport_catalogue::Bus*>& buses, const SphereProjector& sp) const;
        svg::Document GetSVG(const std::map<std::string_view, const transport_catalogue::Bus*>& buses) const;
        // √енерирует метки дл€ автобусов, основыва€сь на переданном словаре автобусов и проекции сферической карты.
        std::vector<svg::Text> GetBusMapName(const std::map<std::string_view, const transport_catalogue::Bus*>& buses, const SphereProjector& sp) const;
        // Cоздает символы (круги) дл€ остановок в транспортной сети.
        std::vector<svg::Circle> GetStopsCircle(const std::map<std::string_view, const transport_catalogue::Stop*>& stops, const SphereProjector& sp) const;
        // ѕредназначена дл€ создани€ меток дл€ остановок на карте.
        std::vector<svg::Text> GetStopsMapName(const std::map<std::string_view, const transport_catalogue::Stop*>& stops, const SphereProjector& sp) const;

    private:
        const RenderSettings render_settings_;
    };
}//map_render
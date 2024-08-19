<<<<<<< HEAD
#pragma once
#include <cmath>

namespace geo {

    const int EARTH_RADIUS = 6371000;
    const double PI = 3.1415926535;

    struct Coordinates {
        double latitude;
        double longitude;

        bool operator==(const Coordinates& other) const {
             return latitude == other.latitude && longitude == other.longitude;
        }
        bool operator!=(const Coordinates& other) const {
             return !(*this == other);
        }
    };

    double ComputeDistance(Coordinates start, Coordinates end);

}//geo
=======
#pragma once

#include <cmath>
namespace detail {
    struct Coordinates {
        double lat;
        double lng;
        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        if (from == to) {
            return 0;
        }
        static const double dr = 3.1415926535 / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
            + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * 6371000;
    }
}
>>>>>>> b045a1c9fcbf4c2e2d3f3c1ea9804c87b09daab7

#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "highway_map.h"
#include "waypoints.h"

#include "json.hpp"

#include <tuple>

struct Obstacles {

    /** @brief Position of each obstacle in cartesian coordinates. */
    Waypoints cartesian;

    /** @brief Position of each obstacle in frenet coordinates (with `x == s` and `y == d`). */
    Waypoints frenet;

    /** @brief Lane occupied by each obstacle. */
    std::vector<size_t> lanes;

    /** @brief Speeds of obstacles in cartesian and frenet coordinates. */
    struct {
        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> s;
        std::vector<double> d;} speeds;


    /**
     * @brief Return the index and distance to the obstacle ahead closest to the given position along the given lane.
     */
    std::tuple<size_t, double> closestAhead(size_t lane, double s) const;

    /**
     * @brief Return the index and distance to the obstacle behind closest to the given position along the given lane.
     */
    std::tuple<size_t, double> closestBehind(size_t lane, double s) const;

    /**
     * @brief Update this object with data from the give state and JSON node.
     */
    void update(double t, const HighwayMap &highway, const nlohmann::json &json);
//     void update(double t, const State &state, const nlohmann::json &json);

    /**
     * @brief Return the number of obstacles.
     */
    size_t size() const;
};

#endif

#include "path_planner.h"

#include <fstream>
#include <math.h>
#include <uWS/uWS.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "json.hpp"

// for convenience
using json = nlohmann::json;

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_first_of("}");
  if (found_null != std::string::npos) {
    return "";
  } else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 2);
  }
  return "";
}

int main() {
    uWS::Hub h;

    // The max s value before wrapping around the track back to 0
    double max_s = 6945.554;

    PathPlanner planner(3, 4.0);
    std::ifstream map_file("data/highway_map.csv");
    map_file >> planner.highway;

    auto handler = [&](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
        // "42" at the start of the message means there's a websocket message event.
        // The 4 signifies a websocket message
        // The 2 signifies a websocket event
        //auto sdata = string(data).substr(0, length);
        //cout << sdata << endl;
        if (length <= 2 || data[0] != '4' || data[1] != '2') {
            return;
        }

        auto s = hasData(data);
        if (s == "") {
            // Manual driving
            std::string msg = "42[\"manual\",{}]";
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            return;
        }

        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event != "telemetry") {
            return;
        }

        // Main car's localization Data
        State state(j[1]);
        std::cout << state.x << ", " << state.y << ", " << state.o << std::endl;

        // Sensor Fusion Data, a list of all other cars on the same side of the road.
        auto sensor_fusion = j[1]["sensor_fusion"];

        Waypoints previous(
            j[1]["previous_path_x"],
            j[1]["previous_path_y"]
        );

        Waypoints waypoints = planner(state, previous);

        json msgJson;
        msgJson["next_x"] = waypoints.x;
        msgJson["next_y"] = waypoints.y;

        auto msg = "42[\"control\","+ msgJson.dump()+"]";

        //this_thread::sleep_for(chrono::milliseconds(1000));
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
    };

    h.onMessage(handler);

  // We don't need this since we're not using HTTP but if it's removed the
  // program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data,
                     size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1) {
      res->end(s.data(), s.length());
    } else {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}

















































































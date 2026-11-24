/**
 * @file example_app.cpp
 * @author Nicolas Gry (nicolas.gry@inria.fr)
 * @brief A simple passthrough app
 * @version 0.1
 * @date 2026-11-24
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 */
#include <signal.h>

#include <oboe_utils/oboe_utils.hpp>

static volatile std::atomic<bool> keepRunning = true;
static volatile std::atomic<bool> passthrough_on = true;

void intHandler(int dummy) { keepRunning = false; }

int main() {
  signal(SIGINT, intHandler);
  obut::IOStreamHandler<oboe::AudioFormat::Float> io_streams(44000, 128, 4);

  io_streams.start_streams();
  char letter;
  while (keepRunning) {
    std::cout << "Type a letter and type enter to toggle audio" << std::endl;
    std::cin >> letter;
    passthrough_on = !passthrough_on;
    if (passthrough_on) {
      io_streams.start_streams();
    } else {
      io_streams.stop_streams();
    }
  }

  io_streams.stop_streams();
  return 0;
}
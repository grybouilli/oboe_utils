/**
 * @file example_app.cpp
 * @author Nicolas Gry (nicolas.gry@madic.com)
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
static volatile bool keepRunning = true;

void intHandler(int dummy) { keepRunning = false; }

int main(){
  signal(SIGINT, intHandler);
  obut::IOStreamHandler<oboe::AudioFormat::Float> io_streams(44000, 128);

  io_streams.start_streams();
  while (keepRunning);

  io_streams.stop_streams();
  return 0;
}
#include "../src/IO/AppleRunLoopStop.h"
#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;
namespace {
void check(bool value, const char *message) { if (!value) throw std::runtime_error(message); }
void run(bool stopBeforeRun) {
  ii::device::detail::AppleRunLoopStop stop;
  std::promise<bool> attached; auto ready = attached.get_future();
  std::promise<void> start, finished; auto gate = start.get_future(); auto done = finished.get_future();
  std::thread worker([&] {
    const bool ok = stop.attach(); attached.set_value(ok);
    if (!ok) { finished.set_value(); return; }
    gate.wait(); stop.run(); stop.detach(); finished.set_value();
  });
  const bool ok = ready.get();
  if (stopBeforeRun) stop.request_stop();
  start.set_value();
  const bool stayed = stopBeforeRun || done.wait_for(80ms) == std::future_status::timeout;
  stop.request_stop();
  const bool ended = done.wait_for(2s) == std::future_status::ready;
  worker.join();
  check(ok, "Cannot create the native run-loop source");
  check(stayed, "An idle run loop returned without an event");
  check(ended, "Stopping the run loop lost its wakeup");
}
}
int main() { run(false); run(true); }

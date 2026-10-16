#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iiDeviceMultimedia/IO/Result.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace ii::device;
namespace {
void check(bool condition, const char *message) {
  if (!condition)
    throw std::runtime_error(message);
}
}
int main() {
  try {
    bool rejected = false;
    try { Hardware invalid({}); } catch (const std::invalid_argument &) {
      rejected = true;
    }
    check(rejected, "handler must be present");
    HardwareOptions invalid;
    invalid.event_capacity = 0;
    rejected = false;
    try { Hardware devices([](HardwareEvent) {}, invalid); }
    catch (const std::invalid_argument &) { rejected = true; }
    check(rejected, "zero event capacity rejected without a runtime");
    invalid = {};
    invalid.usb_interface_guids = {"invalid"};
    rejected = false;
    try { Hardware devices([](HardwareEvent) {}, invalid); }
    catch (const std::invalid_argument &) { rejected = true; }
    check(rejected, "malformed USB interface GUID rejected");

    std::atomic<int> retired = 0;
    Hardware *owner = nullptr;
    struct Capture final {
      std::atomic<int> &retired;
      Hardware *&owner;
      ~Capture() {
        const auto poll = owner->poll();
        const auto close = owner->close();
        if (!poll && poll.error().code == ErrorCode::closed &&
            !close && close.error().code == ErrorCode::busy)
          ++retired;
      }
    };
    auto capture = std::make_shared<Capture>(retired, owner);
    Hardware devices([capture](HardwareEvent) {});
    owner = &devices;
    capture.reset();
    check(bool(devices.poll()), "standalone collector can be pumped");
    check(bool(devices.poll()), "standalone collector can be pumped repeatedly");
    check(bool(devices.close()) && retired == 1,
          "handler retires outside locks before successful close returns");
    const auto after = devices.poll();
    check(!after && after.error().code == ErrorCode::closed,
          "closed poll has a device-owned error");
    check(bool(devices.close()) && devices.stats().pending == 0,
          "repeated close is successful and clears queued events");
    std::cout << "independent device lifecycle contracts passed\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}

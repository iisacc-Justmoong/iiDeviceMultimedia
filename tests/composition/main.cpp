#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iiSDKCore/IO/Channel.h>
#include <iiSDKCore/Runtime/FrameLoop.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;
bool core_first_headers();
bool device_first_headers();
namespace {
void check(bool condition, const char *message) {
  if (!condition)
    throw std::runtime_error(message);
}
}
int main() {
  try {
    check(core_first_headers() && device_first_headers(), "both include orders");
    ii::runtime::EventLoop events;
    ii::runtime::FrameLoop frames(events);
    std::size_t frame_count = 0, polls = 0, notes = 0;
    auto tick = frames.add([&](const auto &) { ++frame_count; });
    check(bool(tick), "Core frame callback");
    ii::device::Hardware devices([&](ii::device::HardwareEvent event) {
      for (const auto &packet : event.midi) {
        const auto note = ii::device::protocol::note(packet);
        if (note && note->on && note->key == 60)
          ++notes;
      }
    });
    auto capture = std::make_shared<int>(1);
    const std::weak_ptr<int> retired = capture;
    const auto driver = events.every(1ms, [&, capture] {
      check(bool(devices.poll()), "application drives independent devices");
      ++polls;
    });
    check(bool(driver), "application registers Core timer");
    capture.reset();
    auto pump = [&] {
      check(frames.poll().advanced, "Core app/game frame");
      std::this_thread::sleep_for(1ms);
    };
    const auto deadline = std::chrono::steady_clock::now() + 3s;
    while (polls < 3 && std::chrono::steady_clock::now() < deadline)
      pump();
    check(polls >= 3 && frame_count >= 3, "device collection and game frames coexist");

    const ii::device::protocol::USBSetup request{.request = 6, .length = 18};
    const auto encoded = request.encode();
    auto file = ii::io::open_file("composition-roundtrip.bin",
        {.access = ii::io::Access::read_write, .creation = ii::io::Creation::truncate});
    check(bool(file), "Core channel opens independently");
    check(file->write(encoded).value_or(0) == encoded.size(), "device bytes through Core I/O");
    check(bool(file->seek(0)), "Core byte cursor");
    std::array<std::byte, 8> bytes{};
    check(file->read(bytes).value_or(ii::io::Transfer{}).bytes == bytes.size(), "Core reads protocol bytes");
    const auto decoded = ii::device::protocol::USBSetup::decode(bytes);
    check(decoded && decoded->request == 6 && decoded->length == 18, "separate result types and codec roundtrip");
    check(bool(file->close()), "Core channel closes independently");

#if defined(__APPLE__)
    const auto name = "iiSDK composed " + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto input = devices.create_midi_destination(name);
    check(bool(input), "standalone module creates native CoreMIDI endpoint");
    const auto outputs = devices.enumerate(ii::device::DeviceKind::midi_output);
    check(bool(outputs), "native MIDI enumeration");
    const auto found = std::find_if(outputs->begin(), outputs->end(), [&](const auto &entry) {
      return entry.name.find(name) != std::string::npos;
    });
    check(found != outputs->end(), "native endpoint visible");
    const auto output = devices.open_midi(found->id);
    check(bool(output), "native MIDI sender");
    ii::device::protocol::UMP packet;
    packet.size = 2;
    packet.words = {0x40903c00, 0xffff0000, 0, 0};
    check(bool(devices.midi_send(*output, std::span(&packet, 1))), "native UMP send");
    const auto receive_deadline = std::chrono::steady_clock::now() + 3s;
    while (!notes && std::chrono::steady_clock::now() < receive_deadline)
      pump();
    check(notes == 1, "Core timer dispatches native device UMP on its host thread");
    check(bool(devices.close(*output)) && bool(devices.close(*input)), "native endpoints retire between polls");
#endif
    check(driver.token.cancel(), "application cancels its Core timer");
    check(!driver.token.pending() && retired.expired(), "driver captures retire before devices");
    check(bool(devices.close()), "independent device close after driver retirement");
    check(frames.poll().advanced, "Core frames continue after device shutdown");
    std::cout << "independent packages compose: frames=" << frame_count
              << " polls=" << polls << " native_notes=" << notes << '\n';
    return 0;
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}

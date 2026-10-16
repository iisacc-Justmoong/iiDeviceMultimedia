#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iiDeviceMultimedia/IO/Protocols.h>
#include <array>
#include <iostream>

int main() {
  ii::device::Hardware devices([](ii::device::HardwareEvent) {});
  const auto caps = ii::device::hardware_capabilities();
  if (caps.midi2 && !caps.midi) return 1;
  if (!devices.poll()) return 2;
  const ii::device::protocol::USBSetup setup{.request = 6, .length = 18};
  const auto bytes = setup.encode();
  const auto decoded = ii::device::protocol::USBSetup::decode(bytes);
  if (!decoded || decoded->request != 6 || decoded->length != 18) return 4;
  const std::array samples{-.5f, 0.f, .5f};
  auto pcm = ii::device::protocol::pcm_s16le(samples);
  if (!pcm || pcm->size() != 6 || !ii::device::protocol::pcm_s16le(*pcm)) return 5;
  if (!devices.close() || devices.stats().pending != 0) return 6;
  std::cout << "standalone installed device module complete\n";
}

#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iostream>
int main() {
  ii::device::Hardware hardware( [](ii::device::HardwareEvent) {});
  for (const auto kind :
       {ii::device::DeviceKind::usb, ii::device::DeviceKind::hid,
        ii::device::DeviceKind::audio_output, ii::device::DeviceKind::midi_output}) {
    auto devices = hardware.enumerate(kind);
    if (!devices) {
      std::cout << "kind=" << static_cast<int>(kind)
                << " unavailable=" << static_cast<int>(devices.error().code)
                << '\n';
      continue;
    }
    for (const auto &device : *devices)
      std::cout << "kind=" << static_cast<int>(kind) << " id=" << device.id
                << " name=" << device.name << '\n';
  }
  return hardware.close() ? 0 : 1;
}

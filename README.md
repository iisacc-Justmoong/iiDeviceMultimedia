# iiDeviceMultimedia

iiDeviceMultimedia is the standalone Qt-free native C++23 device/media module
for iiSDK. Version 0.2.0 owns USB/HID discovery, hotplug, control transfers and
reports; Bluetooth LE central/GATT; interleaved float32 PCM input/output; and
MIDI UMP transport/decoding. Its public namespace is `ii::device`.

iiDeviceMultimedia and iiSDKCore are independent peer packages. Neither imports,
links or calls the other. Apps can use both: Core supplies scheduling, frames and
generic byte I/O; the app calls device `Hardware::poll()` from its chosen host
loop. There is no duplicated generic runtime or hidden SDK adapter. See the
[independence and composition contract](docs/independence.md).

## Build and install

Use CMake 3.31+, C++23 and standard platform threads. Core is not required:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
cmake --install build --prefix "$PWD/build/stage"
```

Apple uses its native frameworks. Windows requires Windows SDK C++/WinRT device
projections; native MIDI 2.0 additionally requires its optional projections and
service. Linux requires libudev, D-Bus and ALSA >=1.2.10 development packages.
iOS and Emscripten use static libraries. Emscripten publishes C++ exception flags
and defaults `IIDEVICEMULTIMEDIA_WASM_THREADS` to ON; a single-thread host can
explicitly disable it. The Python architecture checker is a build tool, not an
application runtime.

## Consume independently

```cmake
find_package(iiDeviceMultimedia 0.2 CONFIG REQUIRED)
target_link_libraries(app PRIVATE iiDeviceMultimedia::iiDeviceMultimedia)
```

```cpp
#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iiDeviceMultimedia/IO/Protocols.h>

ii::device::Hardware devices([](ii::device::HardwareEvent event) {
    // Runs on the thread that calls poll().
});
const auto result = devices.poll();
const auto closed = devices.close();
```

`IO/Result.h` owns device errors independently of Core's `ii::io` errors. A poll
collects native input and dispatches a bounded batch; the host chooses cadence.
Apps using Core explicitly find/link both packages and own a timer that calls
`devices.poll()`. Examples and teardown requirements are in
[composition](docs/independence.md). The separate `tests/composition` application
verifies both package/header orders, Core FrameLoop/Channel coexistence and native
UMP delivery through an application-owned Core timer.

Compiled capabilities do not prove permissions, device presence or physical
operation. Generic raw USB/HID is unavailable on iOS; Android/WASM hardware host
bridges are unsupported. Protocols and bounded dispatch remain portable. See the
[hardware contract](docs/hardware.md), [0.1-to-0.2 migration](docs/independence.md),
[extraction history](docs/extraction.md) and [validation](docs/validation.md).

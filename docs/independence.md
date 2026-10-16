# Independent packages and application composition

iiDeviceMultimedia 0.2.0 and iiSDKCore 0.12 are peer packages. Neither includes,
links, configures, embeds, imports or calls the other. Both use standard C++23
types and their own native platform needs. The application explicitly chooses
both packages when it needs both responsibilities:

```text
application -> iiSDKCore        (runtime, frames, byte I/O)
application -> iiDeviceMultimedia (devices, media, protocol codecs)
```

Device declarations live in `ii::device` / `ii::device::protocol`. Its `Error`,
`ErrorCode` and `Result<T>` are owned by `IO/Result.h`; they do not alias Core's
`ii::io` errors. Byte spans, PCM spans, chrono durations and C++ callbacks cross
the application boundary without a shared SDK base, inheritance or a bridge
library. Include order and package import order do not change the contract.

## Standalone collection

Construct `Hardware(handler, options)` and call `poll()` from the host. A poll
serializes native collection with device commands, then dispatches at most
`dispatch_batch` events and returns the successful delivery count. Zero is a
successful idle poll. Native callbacks enqueue bounded events and never invoke
the user handler themselves. No application loop, scheduler slot, worker pool
or generic task runtime is created or borrowed. The private Apple MIDI native
notification thread is still necessary to service CFRunLoop and is joined when
its final device hub closes.

Only one poll can be active per hub. Recursive or concurrent polling returns
`ErrorCode::busy`. Handler exceptions propagate after endpoint and poll activity
bookkeeping is released; a subsequent poll is allowed. The caller chooses the
collection cadence and responsiveness. The old `dispatch_interval` option is
removed instead of retaining an unused scheduling knob.

## Composition with Core

Only application/consumer CMake explicitly finds both packages:

```cmake
find_package(iiSDKCore 0.12 CONFIG REQUIRED)
find_package(iiDeviceMultimedia 0.2 CONFIG REQUIRED)
target_link_libraries(app PRIVATE
    iiSDKCore::iiSDKCore iiDeviceMultimedia::iiDeviceMultimedia)
```

For a single host dispatch thread:

```cpp
#include <iiSDKCore/Runtime/EventLoop.h>
#include <iiDeviceMultimedia/IO/Hardware.h>

ii::runtime::EventLoop events;
ii::device::Hardware devices([](ii::device::HardwareEvent event) {
    // Handle on the thread that calls devices.poll().
});
const auto driver = events.every(std::chrono::milliseconds(2), [&] {
    const auto result = devices.poll();
    // Inspect device-owned errors; dispatch must not silently ignore failures.
});
// Check driver, then pump events.poll()/run() or a FrameLoop on this same host.
// After that host dispatch has returned:
(void)driver.token.cancel();
// Retire any active driver and its borrowed captures before destroying devices.
// This single-threaded example has no active callback after poll() returns.
const auto closed = devices.close();
```

The timer belongs to the application. Core can continue running after the
device hub closes, and the device module can run without Core. Hosts can instead
pump directly or from another scheduler. There is no SDK-provided Core adapter,
conditional Core package dependency, or hidden ownership of the external timer.
For concurrent hosts, cancellation alone is not a borrowed-lifetime barrier:
the host must finish active callbacks before destroying the hub. Core's existing
task/lifetime facilities may be used by the application for that purpose.

## Shutdown and ownership

`close(endpoint)` returns busy while any poll is active, without changing that
endpoint. Call it between polls or after the host has stopped its driver. Once
successful, native resources, stale events and endpoint quota have retired.

Own-callback `close()` returns busy without stopping the hub. Foreign `close()`
during an active poll stops new commands/events/polls and returns busy
instead of waiting on another runtime's dispatcher. The current handler may
finish; later queued handlers are suppressed. After the external driver retires,
the owner retries close to release native resources and handler captures. Closed
polls return `ErrorCode::closed`. Concurrent close during resource/capture cleanup
also returns busy. Successful repeated close remains idempotent. Captures retire
outside locks before successful close returns, including reentrant cleanup.

Do not destroy a hub while its close returns busy, from its own handler, or while
another caller is still using it. As before, violating this lifetime contract
terminates instead of abandoning native work. No hard realtime or preemption
guarantee follows from bounded dispatch. Native device/driver calls may block.

## Migration from 0.1

Replace `ii::io::Hardware` and `ii::io::protocol` with the corresponding
`ii::device` names. Replace device error checks with `ii::device::ErrorCode`.
Remove the EventLoop constructor argument and explicitly own the `poll()` driver.
Find/link both packages only in applications that actually use both. Remove
`IIDEVICEMULTIMEDIA_CORE_SOURCE_DIR`; there is no embedded-Core build mode.

The device package is 0.2.0 with SOVERSION 0.2 and SameMinorVersion package
compatibility. Rebuild 0.1 consumers; no compatibility alias or duplicate Core
error definition is introduced. Core's generic source and ABI are unchanged by
this peer-module refactor. The old extraction is documented historically in
[extraction.md](extraction.md); current verification is in [validation.md](validation.md).

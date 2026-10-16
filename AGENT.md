# iiDeviceMultimedia

This module owns USB/HID, BLE central/GATT, float32 PCM and MIDI UMP device I/O
and their protocol codecs. It and iiSDKCore are independent peer packages: no
production include, symbol, target, package or runtime dependency in either
direction. General runtime queues, tasks and Channel I/O remain in Core. Device
collection/dispatch is host-driven through Hardware::poll(); do not duplicate a
general scheduler or task runtime here.

Use native C++23, standard types and private platform bridges without Qt. Preserve
the device/protocol behavior. Public types and errors belong to `ii::device` and
`ii::device::protocol`; headers belong under `iiDeviceMultimedia/`. Public complete records are final and composition is
used instead of inheritance. The always-on architecture gate also preprocesses
Objective-C++ and applies when tests are disabled.

Preserve explicit bounded dispatch, cancellation, endpoint identity, callback
retirement and capture-before-capacity cleanup. Reject recursive/concurrent polls;
foreign active-poll hub close stops admission and returns busy for owner retry.
External driver cancellation/quiescence belongs to the application. Do not alter hardware behavior to
make extraction tests pass. Keep device capabilities separate from permissions,
presence and physical-device proof. Android/WASM hardware operations remain
explicitly unsupported until real host bridges exist.

Update documentation and tests with source changes; build only under build/.
Verify standalone installed consumption with Core package lookup disabled, the
Core-only boundary and the separate composition consumer in both package/include
orders. Integration belongs only in that consumer/application; no optional Core
adapter header or find_dependency is allowed in the device package. The private
Apple notification pump owns a joined standard thread, and native BLE callback
retirement must not depend on a Core TaskScope.
README.md is English. Do not disturb active apps, unrelated changes or hardware.

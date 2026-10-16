# Device/media extraction history

Current contract: iiDeviceMultimedia 0.2.0 and Core 0.12 are independent peers.
Device APIs/errors use `ii::device`, the host owns the `poll()` driver, and no
package imports the other. Follow [independence and migration](independence.md).
The following 0.1 extraction record describes the earlier intermediate boundary;
its Core dependency, constructor, namespace and minimum-version rules are historical.

# Extraction and migration

iiDeviceMultimedia 0.1.0 / iiSDKCore 0.12.0 separate device/media responsibility
from generic runtime and byte I/O. No device algorithms or backend semantics are
rewritten. The moved families are USB/HID, BLE central/GATT, float32 PCM and MIDI
UMP, including their protocol parsers and private platform implementations.

| Former Core file family | New location |
| --- | --- |
| `src/IO/Hardware*` | `iiDeviceMultimedia/src/IO/Hardware*` |
| `src/IO/Protocols.h/.cpp` | `iiDeviceMultimedia/src/IO/Protocols.h/.cpp` |
| Hardware, state and protocol tests | `iiDeviceMultimedia/tests/` |
| Hardware example and contract | `iiDeviceMultimedia/examples/` and `docs/` |

The stable C++ names `ii::io::Hardware`, device records and `ii::io::protocol`
remain unchanged. Update public includes from `iiSDKCore/IO/Hardware.h` and
`iiSDKCore/IO/Protocols.h` to the corresponding `iiDeviceMultimedia/IO/` paths.
Link `iiDeviceMultimedia::iiDeviceMultimedia`; its package imports Core 0.12+.
Do not combine this package with a pre-extraction Core that still exports device
symbols. Existing hardware consumers must rebuild and relink. Generic Core APIs
and behavior remain unchanged; Core-only consumers need no device package.

No forwarding header or Core dependency on the new module is introduced. Core
configuration removes its two stale generated device headers; installing Core
0.12 removes exactly those obsolete headers formerly owned by Core in that
prefix, including DESTDIR staging. It does not remove the prefix or unrelated
files. Validation covers a Core 0.11-to-0.12 prefix upgrade and fresh, relocated
device-package consumption.

Apple frameworks, Windows device/WinRT libraries and Linux libudev/D-Bus/ALSA
dependency discovery move with their backends. Core retains standard threading,
generic file/serial I/O, poll/wake, tasks/queries, FrameLoop and plugins. Public
device headers use only Core's installed API, never its private sources.

The module has its own source/preprocessor architecture gate and regression
fixtures, extracted from the existing proven Core policy. The scanner snapshot
is unchanged; its local CMake target names and public-type fixtures are module
specific. Building from an installed Core does not require Core source/tools.

Current support boundaries are preserved: iOS has no generic raw USB/HID host
API, Android/WASM hardware host bridges are not implemented, and software tests
are not physical-device or every-OS execution proof.

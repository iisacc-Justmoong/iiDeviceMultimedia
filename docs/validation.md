# Independent peer-module validation — 2026-10-15

Version 0.2.0 removes the former dependency from iiDeviceMultimedia to Core.
Neither package imports, includes, links or calls the other. The application
explicitly composes device polling with its chosen runtime. The 0.1 extraction
record below is historical; current architecture and migration are described in
[independence.md](independence.md).

## Independent and composed execution

| Configuration | Result | Scope |
| --- | --- | --- |
| Device macOS arm64 Release/shared | 6/6 CTests | Independent package/source boundary, native hardware, state, protocols, new lifecycle and sealed-record policy |
| Device Debug/static, ASan + UBSan | 6/6 CTests | C++/Objective-C++ instrumentation; C++ warnings as errors |
| Device Debug/static, ThreadSanitizer | 6/6 CTests | Native MIDI thread/poll/close and state/audio-ring concurrency; no race report |
| Core macOS Release/shared | 34/34 CTests | All retained runtime/frame/I/O/value regressions; no Core source implementation change |
| Core-only installed consumer | 13/13 CTests | Device package lookup disabled; standalone Core import and runtime remain intact |
| Relocated standalone device consumer | 3/3 CTests | Core package lookup disabled; no imported Core target, header or library required |
| Device-first composed package consumer | 1/1 CTest | Explicitly imports both packages and compiles both header orders |
| Core-first composed package consumer | 1/1 CTest | Reversed package order; same native integration contract |
| WASM32 pthreads, Node 24 | 6/6 CTests | Portable protocols, polling/lifecycle/state and unsupported hardware; policy executes on host |
| Windows x64 / Linux arm64 / Android arm64 | Device library, four functional tests, inventory example and three public headers cross-built | Native PE/ELF; no Core build target; no target execution |
| iOS arm64 | Device static archive and Apple bridge cross-built | Mach-O IOS, minimum OS 15.0, SDK 27.0; no Core build target or device execution |

Both composed consumers report `frames=7 polls=5 native_notes=1` in their recorded
runs. The application-owned Core timer calls device poll, Core FrameLoop advances,
a device USB setup packet roundtrips through Core Channel file I/O, and native
CoreMIDI virtual endpoints route one MIDI 2.0 note into the host callback. The
application cancels the timer and verifies driver-capture retirement before
closing devices. Core frames continue after device shutdown. Header-order probes
prove device errors/results are distinct from Core's ii::io types.

The standalone native hardware test retains enumeration, output queue/silence,
virtual UMP topology, close/reopen and notification-pump coverage. It now also
checks recursive poll, own-callback hub/endpoint close, and a foreign close while
the user handler is held on another thread. The foreign close stops admission,
returns busy without waiting for that handler, and succeeds on owner retry.
State tests retain payload limits, stale endpoints, FIFO, active-delivery quota
and concurrent audio-ring order; new checks cover stopped admission and exception
accounting/recovery. Lifecycle tests cover standalone validation, polling, typed
closed results, idempotence and reentrant capture retirement outside locks.

## Dependency and preservation evidence

The device dylib contains no Core imports or link dependency. Its owned symbols
use ii::device and ii::device::protocol. Core contains no reverse device link.
The standalone installed executable links only the device SDK; the composed
executable explicitly links both. SDK-wide installer discovery reports no SDK
dependencies for either project, with no edge between them.

The installed device prefix is relocated before consumer configuration. All
three public headers match source and the installed dylib matches the build
byte-for-byte. SOVERSION 0.2 distinguishes the new ABI from 0.1. No compatibility
alias, shared SDK error header, optional Core adapter or embedded-Core option is
used. Composition tests are a separate application project and are absent from
the device package's production dependency graph.

All 65 Core source implementation/header files and its build/package definitions
match their pre-refactor hashes. Core remains 0.12.0. Protocol codec algorithms,
Linux/portable backends and Apple ID parsing match the previous implementation
after namespace/include relocation. Device runtime coupling is replaced narrowly
with its host poll guard, a joined standard Apple MIDI notification thread and
Windows BLE admission locking; no general runtime is copied into this module.

The initial independence guard rejected the former Core dependency, and standalone
lifecycle syntax compilation failed on its Core include before implementation.
Those first-failure logs and the complete previous extraction state are preserved
under `build/independence-baseline/`. A Linux manifest check initially classified
sysroot assets located under another checkout's build directory as an SDK API
dependency; the guard now checks actual source/package references and target
link requirements separately from physical toolchain-directory names.

Machine-readable reports are retained at:

- `build/independence-verification.json`: final source/artifact hashes, test
  results, namespace/linkage, platform formats and explicit limits.
- `build/independence-linkage.json`: no cross-library dependency and standalone
  versus explicitly composed imports.
- `build/independence-native-tests.xml`.
- `build/sanitize/build/independence-sanitizer-tests.xml`.
- `build/tsan/build/independence-tsan-tests.xml`.
- `build/independent-package/consumer/build/independence-installed-tests.xml`.
- `build/independent-package/{device-first,core-first}/build/`: composed XML and
  native observation logs.
- `build/wasm-threads/build/independence-wasm-tests.xml`.

Core-only installed consumption is recorded in Core's
`build/device-package/core-consumer/build/` with device lookup disabled. Core's
full native output is `build/device-independence-core-tests.xml`.

Verification installs remain inside Workspace; global ~/.local packages are
unchanged. Physical USB/HID transfers, BLE peripherals, microphone capture,
audible speaker output and Windows/Linux/Android/iOS runtime execution are not
proved by this pass. Android/WASM hardware bridges and iOS generic raw USB/HID
remain unsupported. The Windows SDK here compiles the MIDI 1.0 fallback; its
optional Midi2 projection path is not covered by this cross-build.

## Historical 0.1.0 extraction validation — 2026-10-15

iiDeviceMultimedia 0.1.0 extracts the existing device/media implementation from
iiSDKCore 0.11.0 and depends on the device-free Core 0.12.0 package. Previous Core
device validation remains historical evidence. The results below were executed
after extraction; no new physical-device coverage is implied by moving the code.

## Regression and package checks

| Configuration | Result | Evidence boundary |
| --- | --- | --- |
| macOS arm64 Release, source Core dependency | 4/4 CTests | Hardware, bounded state, protocols and architecture policy; public headers and inventory example built |
| macOS Debug/static, ASan + UBSan | 4/4 CTests | Core and device C++ instrumented; native Objective-C++ bridge instrumented; C++ warnings treated as errors |
| Independent build against installed Core | 4/4 CTests | No Core source/tools required; imported Core supplies public include and link requirements |
| Relocated installed device/Core consumer | 3/3 CTests | Public API, protocols, native hardware contracts and FrameLoop coexist across the package boundary |
| Core-only native regression | 34/34 CTests | Retained runtime/value/I/O behavior, examples and device-boundary guard |
| Core-only ASan + UBSan | 12/12 focused CTests | FrameLoop/example, Runtime, Query, Thread, shared payloads, realtime, I/O, cleanup, reuse and ownership boundary |
| Relocated Core-only consumer | 13/13 CTests | Upgraded package imports no device target or PkgConfig; retained frame and I/O contracts |
| Values-only installed consumer | 3/3 CTests | No runtime, Threads or device dependency imports |
| Emscripten pthreads, Node 24 | 3/3 CTests | Portable protocol/state and explicit unsupported hardware contracts; no browser device bridge |

The native hardware suite performs available enumeration and validation. On this
macOS host it also exercises CoreMIDI virtual endpoint topology, OS-routed UMP
delivery, repeated close/reopen and the headless notification pump. It checks
native output queue setup and float32 silence writes where an output is present.
BLE scanning and microphone capture are not exercised. It does not prove physical
speaker playback, physical USB/HID transfers or BLE peripheral communication.
Apple missing-autorelease-pool diagnostics stay
enabled for the hardware CTest.

The first sanitizer run passed all three functional suites but reached the
unchanged 60-second architecture-test timeout while cross-builds were running.
The first XML report is retained. A subsequent unchanged full run passed 4/4,
including the architecture test; neither timeout nor policy was relaxed.

## Native target compilation

| Target | Verified output | Execution boundary |
| --- | --- | --- |
| Windows x64 | PE32+ device/Core DLLs, three tests, inventory example and public headers | No Windows execution; this SDK uses the WinRT MIDI 1.0 fallback, so the optional Midi2-projection path is not covered by this cross-build |
| Linux arm64 | ELF device/Core libraries, three tests, inventory example and public headers | libudev/BlueZ D-Bus/ALSA implementation compiled and linked; no Linux execution |
| Android arm64, API 26 | NDK ELF libraries, three tests, inventory example and public headers | Portable unsupported hardware backend; no Android device execution |
| iOS arm64 | Device/Core static archives and native Apple bridge object | Mach-O, platform IOS, minimum OS 15.0, SDK 27.0; no device execution |
| WASM32 pthreads | Static libraries, three test modules, inventory example and public headers | WebAssembly execution in Node; hardware host APIs unsupported |

The mandatory source/preprocessor architecture gate passed nine device
translation units in native/Windows/Linux/Android/WASM test builds and three in
the tests-disabled iOS build. Embedded Core passed 17 translation units, or 18
with its browser driver. Core's standalone native gate passed 85. No policy
bypass, engine, Qt, shared VM or interpreter dependency was introduced. The five
OS targets compile native C++; WASM follows its separate WebAssembly host model.

## Ownership, migration and installation safety

Fourteen moved source/test/example files match their original bytes after only
public include relocation and OS client/queue/test display-name substitutions.
No parser, state machine, backend algorithm or functional test assertion changed.
Eight preexisting frame-loop source/test/example files match their earlier
verification hashes. The other 61 retained tracked Core source files match their
original Git contents. A pre-extraction backup and the existing dirty patch are
preserved in Core's `build/device-extraction-baseline/`.

Core 0.11-to-0.12 upgrade checks removed exactly the obsolete Hardware/Protocols
headers while preserving an unrelated sentinel and the old versioned dylib.
All 45 current Core headers match the generated build headers and its installed
0.12 dylib matches the native build. DESTDIR staging, including a prefix with
spaces, removes only the two obsolete files and preserves an adjacent generic
header without touching the unstaged prefix.

The device package was built independently against that installed Core, installed
to a separate prefix, then relocated before consumer configuration. Its two
installed headers match source. CMake removes the build RPATH during installation;
the native `__TEXT,__text` section is byte-identical. Both libraries use relocatable
`@rpath` install names. Core links only libc++/libSystem on macOS and exports no
Hardware/protocol symbols. The device library exports those symbols and imports
Core plus its native frameworks. Core 0.11 is rejected by the new package's
minimum-version dependency. SDK-wide installation discovery automatically orders
Core before this module with no reverse edge.

Commands use CMake source or installed-Core mode documented in the README. Tests
use `ctest --test-dir build --output-on-failure`, with analogous build subtrees for
sanitizers, cross targets and consumers. Machine-readable evidence is retained at:

- `build/extraction-verification.json`: complete test counts, source/artifact
  hashes, package safety, linkage and execution limits.
- `build/extraction-provenance.json`: normalized extraction comparison and
  preservation of the existing Core work.
- `build/extraction-linkage.json` and `build/extraction-platforms.json`: library
  ownership and target binary formats.
- `build/device-extraction-native-tests.xml` and
  `build/sanitize/build/device-extraction-sanitizer-tests.xml`: full module tests.
- `build/sanitize/build/device-extraction-sanitizer-first-tests.xml`: first run
  with the architecture timeout.
- `build/installed-core/build/device-extraction-installed-core-build-tests.xml`:
  independent source build against the installed Core.
- `build/package-consumer/build/device-extraction-installed-tests.xml`: relocated
  installed consumer.
- `build/wasm-threads/build/device-extraction-wasm-tests.xml`: Node contracts.
- `build/reject-old-core/configure.log`: expected rejection of Core 0.11.

Installation for verification stays inside Workspace in Core's
`build/device-package/relocated/{core,device}`. The global `~/.local` SDK was not
updated. OS permissions, real hardware availability, Windows/Linux/Android/iOS
runtime execution, mobile installation and production endurance remain outside
this verification. See [hardware support and limits](hardware.md).

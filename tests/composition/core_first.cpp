#include <iiSDKCore/IO/Channel.h>
#include <iiSDKCore/Runtime/FrameLoop.h>
#include <iiDeviceMultimedia/IO/Hardware.h>
#include <type_traits>
static_assert(!std::is_same_v<ii::io::Error, ii::device::Error>);
static_assert(!std::is_same_v<ii::io::Result<void>, ii::device::Result<void>>);
bool core_first_headers() { return true; }

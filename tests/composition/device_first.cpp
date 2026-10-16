#include <iiDeviceMultimedia/IO/Hardware.h>
#include <iiSDKCore/Runtime/FrameLoop.h>
#include <type_traits>
static_assert(!std::is_same_v<ii::io::ErrorCode, ii::device::ErrorCode>);
bool device_first_headers() { return true; }

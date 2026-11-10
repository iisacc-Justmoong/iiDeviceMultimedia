#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <atomic>

namespace ii::device::detail {

// Attach on the worker and publish its completion before requesting a stop.
// The owner must join the worker before destroying this object.
class AppleRunLoopStop final {
public:
  AppleRunLoopStop() = default;
  AppleRunLoopStop(const AppleRunLoopStop &) = delete;
  AppleRunLoopStop &operator=(const AppleRunLoopStop &) = delete;

  ~AppleRunLoopStop() {
    if (source_)
      CFRelease(source_);
    if (loop_)
      CFRelease(loop_);
  }

  bool attach() {
    loop_ = CFRunLoopGetCurrent();
    if (!loop_)
      return false;
    CFRetain(loop_);
    CFRunLoopSourceContext context{};
    context.perform = [](void *) {};
    source_ = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &context);
    if (!source_)
      return false;
    CFRunLoopAddSource(loop_, source_, kCFRunLoopDefaultMode);
    return true;
  }

  bool run_once() {
    if (stopping_.load(std::memory_order_acquire))
      return false;
    // The source stays signalled if stop is requested immediately before this
    // call, so entering the wait cannot lose the stop notification.
    const auto result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0e10, true);
    return !stopping_.load(std::memory_order_acquire) &&
           result != kCFRunLoopRunFinished;
  }

  void run() {
    while (run_once()) {
    }
  }

  void request_stop() noexcept {
    stopping_.store(true, std::memory_order_release);
    if (source_)
      CFRunLoopSourceSignal(source_);
    if (loop_)
      CFRunLoopWakeUp(loop_);
  }

  void detach() noexcept {
    if (loop_ && source_)
      CFRunLoopRemoveSource(loop_, source_, kCFRunLoopDefaultMode);
  }

private:
  std::atomic_bool stopping_{false};
  CFRunLoopRef loop_ = nullptr;
  CFRunLoopSourceRef source_ = nullptr;
};

} // namespace ii::device::detail

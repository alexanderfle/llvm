//==-------------------- spinlock.hpp --- Spin lock ------------------------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#pragma once

#include <CL/sycl/detail/defines.hpp>

#include <atomic>
#include <thread>

#if ITT_NOTIFY_ENABLE
#include <ittnotify.h>
#define ITT_SYNC_ACQUIRED(addr) __itt_sync_acquired((void *)addr)
#define ITT_SYNC_RELEASING(addr) __itt_sync_releasing((void *)addr)
#else
#define ITT_SYNC_ACQUIRED(...)
#define ITT_SYNC_RELEASING(...)
#endif

__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {
namespace detail {
/// SpinLock is a synchronization primitive, that uses atomic variable and
/// causes thread trying acquire lock wait in loop while repeatedly check if
/// the lock is available.
///
/// One important feature of this implementation is that std::atomic<bool> can
/// be zero-initialized. This allows SpinLock to have trivial constructor and
/// destructor, which makes it possible to use it in global context (unlike
/// std::mutex, that doesn't provide such guarantees).
class SpinLock {
public:
  void lock() {
    while (MLock.test_and_set(std::memory_order_acquire))
      std::this_thread::yield();
    ITT_SYNC_ACQUIRED(this);
  }
  void unlock() {
    ITT_SYNC_RELEASING(this);
    MLock.clear(std::memory_order_release);
  }

private:
  std::atomic_flag MLock = ATOMIC_FLAG_INIT;
};
} // namespace detail
} // namespace sycl
} // __SYCL_INLINE_NAMESPACE(cl)

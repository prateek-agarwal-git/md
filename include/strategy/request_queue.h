#pragma once
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
namespace strategy {
template <typename T> class Queue {
public:
  T pop();
  void push(T &&pItem);

private:
  static constexpr uint64_t m_ring_mod_mask = 2047;
  static constexpr uint64_t m_size = 2048;

  uint64_t getCount() const;
  bool peek() const;
  bool full() const;

  T mMem_[m_size];
  std::mutex mLock_;
  std::condition_variable condVar_;
  uint64_t mReadPtr_{};
  uint64_t mWritePtr_{};
};

template <typename T> void Queue<T>::push(T &&pItem) {
  std::unique_lock<std::mutex> lock(mLock_);
  condVar_.wait(lock, [this] { return !full(); });
  mMem_[mWritePtr_ & m_ring_mod_mask] = std::move(pItem);
  mWritePtr_++;
  condVar_.notify_all();
}

template <typename T> uint64_t Queue<T>::getCount() const {
  return mWritePtr_ > mReadPtr_ ? mWritePtr_ - mReadPtr_
                                : mReadPtr_ - mWritePtr_;
}
template <typename T> bool Queue<T>::full() const {
  return (getCount() == m_size);
}

template <typename T> bool Queue<T>::peek() const {
  return (mWritePtr_ != mReadPtr_);
}

template <typename T> T Queue<T>::pop() {
  std::unique_lock<std::mutex> lock(mLock_);
  condVar_.wait(lock, [this] { return peek(); });
  T &ret = mMem_[mReadPtr_ & m_ring_mod_mask];
  mReadPtr_++;
  condVar_.notify_one();
  return ret;
}
} // namespace strategy
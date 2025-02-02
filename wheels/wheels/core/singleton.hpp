#pragma once

//////////////////////////////////////////////////////////////////////

// Meyers singleton
// Thread-safe since C++11

template <class T>
T* Singleton() {
  static T instance;
  return &instance;
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <typename T>
class LeakyStorage {
 public:
  LeakyStorage() {
    new (buffer_) T();
  }

  T& GetInstance() {
    return *reinterpret_cast<T*>(buffer_);
  }

 private:
  alignas(T) char buffer_[sizeof(T)];
};

}  // namespace detail

template <typename T>
T& LeakySingleton() {
  static detail::LeakyStorage<T> leaky;
  return leaky.GetInstance();
}

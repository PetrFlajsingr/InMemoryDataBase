//
// Created by Petr Flajsingr on 2019-02-27.
//

#ifndef PROJECT_LAZY_H
#define PROJECT_LAZY_H

#include <functional>

/**
 * Lazy init implementation. Allows for value initialisation using a function.
 *
 * Call to invalidate forces the object to recalculate saved value on next access.
 * @tparam T type of value stored.
 */
template <typename T> class Lazy {
  using InitFnc = std::function<T()>;

public:
  explicit Lazy(const InitFnc &init) : init(init) {}
  /**
   * Calculate value on first call or if invalidate() was called.
   * @return lazily evaluated value
   */
  virtual operator const T &() {
    if (!initialised) {
      value = init();
      initialised = true;
    }
    return value;
  }
  /**
   * Mark value as invalid and force it to recompute on next acess.
   */
  void invalidate() { initialised = false; }

private:
  T value;
  InitFnc init;
  bool initialised = false;
};

#endif // PROJECT_LAZY_H

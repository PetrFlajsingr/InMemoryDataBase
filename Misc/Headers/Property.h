#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "cppcoreguidelines-c-copy-assignment-signature"
//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_PROPERTY_H
#define PROJECT_PROPERTY_H

#include <type_traits>

template<typename T, typename Owner>
class Property final {
  friend Owner;
  using Getter = std::function<T &()>;
  using Setter = std::function<T &(const T &)>;
 public:
  Property() = default;
  Property(const Getter &get, const Setter &set) : get(get), set(set) {}
  ~Property() = default;
  Property<T, Owner>(const Property<T, Owner> &) = delete;
  Property<T, Owner> &operator=(const Property<T, Owner> &) = delete;
  T &operator=(const T &f) { return set(f); }
  operator const T &() const { return get(); }
 private:
  Getter get = [this] { return value; };
  Setter set = [this](const T &val) {
    value = val;
    return value;
  };
  T value;
};

template<typename T, typename Owner>
class ReadOnlyProperty final {
  friend Owner;
  using Getter = std::function<T()>;
 public:
  ReadOnlyProperty() = default;
  explicit ReadOnlyProperty(const Getter &get) : get(get) {}
  ~ReadOnlyProperty() = default;
  ReadOnlyProperty<T, Owner>(const ReadOnlyProperty<T, Owner> &) = delete;
  ReadOnlyProperty<T, Owner> &operator=(const ReadOnlyProperty<T,
                                                               Owner> &) = delete;
  operator T const &() const { return get(); }
  T operator->() const { return get(); }
 private:
  Getter get = [this] { return value; };
  T value;
};

template<typename T, typename Owner>
class WriteOnlyProperty final {
  friend Owner;
  using Setter = std::function<T &(const T &)>;
 public:
  WriteOnlyProperty() = default;
  explicit WriteOnlyProperty(const Setter &set) : set(set) {}
  ~WriteOnlyProperty() = default;
  WriteOnlyProperty<T, Owner>(const WriteOnlyProperty<T, Owner> &) = delete;
  WriteOnlyProperty<T, Owner> &operator=(const WriteOnlyProperty<T,
                                                                 Owner> &) = delete;
  T &operator=(const T &f) { return set(f); }
 private:
  Setter set = [this](const T &val) {
    value = val;
    return value;
  };
  T value;
};
#endif //PROJECT_PROPERTY_H

#pragma clang diagnostic pop
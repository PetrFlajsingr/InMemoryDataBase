//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_PROPERTY_H
#define PROJECT_PROPERTY_H

#include <type_traits>

template<typename T, typename Owner>
class Property {
  friend Owner;
 public:
  virtual ~Property() = default;
  virtual T &operator=(const T &f) { return set(f); }
  virtual operator const T &() const { return get(); }
 protected:
  virtual T const &get() const = 0;
  virtual T &set(const T &f) = 0;
  T value;

  Owner *owner;
 private:
  void setOwner(Owner *owner) {
    this->owner = owner;
  }
};

template<typename T, typename Owner>
class ReadOnlyProperty {
  friend Owner;
 public:
  virtual ~ReadOnlyProperty() = default;
  virtual operator T const &() const { return get(); }
  virtual T operator->() const { return get(); }
 protected:
  virtual T const &get() const {
    return value;
  }
  T value;
};

template<typename T, typename Owner>
class WriteOnlyProperty {
  friend Owner;
 public:
  virtual ~WriteOnlyProperty() = default;
  virtual T &operator=(const T &f) { return set(f); }
 protected:
  virtual float &set(const T &f) = 0;
  T value;
};

#endif //PROJECT_PROPERTY_H

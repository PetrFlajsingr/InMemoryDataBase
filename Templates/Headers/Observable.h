#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "cppcoreguidelines-c-copy-assignment-signature"
//
// Created by Petr Flajsingr on 2019-02-28.
//

#ifndef PROJECT_OBSERVABLE_H
#define PROJECT_OBSERVABLE_H

#include <list>
#include "Property.h"
#include <iostream>
#include <Types.h>

template<typename T>
class Binder;

class ObservableBase;
class Observer {
 public:
  virtual void update(ObservableBase const *obs) = 0;
};

class ObservableBase {
 public:
  void addObserver(Observer *observer) {
    observers.emplace_back(observer);
  }
  void deleteObserver(Observer *observer) {
    if (auto it = std::find(observers.begin(), observers.end(), observer); it
        != observers.end()) {
      observers.erase(it);
    }
  }
  void deleteObservers() {
    observers.clear();
  }
  size_t countObservers() {
    return observers.size();
  }
  bool hasChanged() {
    return changed;
  }
  void disable() {
    enabled = false;
  }
  void enable() {
    enabled = true;
  }
  void notifyObservers() {
    if (changed && enabled) {
      std::for_each(observers.begin(), observers.end(), [this](auto observer) {
        observer->update(this);
      });
      changed = false;
    }
  }
  virtual ~ObservableBase() = default;
 protected:
  bool changed = false;
  bool enabled = true;
  std::list<Observer *> observers;
};

template<typename T>
class Observable
    : public ObservableBase {
 public:
  Observable() = default;
  explicit Observable(T value) : value(value) {}
  typename std::conditional<std::is_compound<T>::value, T &, T>::type operator=(
      const T &f) {
    value = f;
    changed = true;
    notifyObservers();
    return value;
  }
  operator typename std::conditional<std::is_compound<T>::value,
                                     const T &,
                                     T>::type() const { return value; }
 protected:
  T value;
  friend class Binder<T>;
};

#endif //PROJECT_OBSERVABLE_H

#pragma clang diagnostic pop
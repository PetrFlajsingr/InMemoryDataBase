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
/**
 * Interface allowing object to receive an update when watched value is changed.
 */
class Observer {
 public:
  /**
   * Called when an observed value is changed.
   * @param obs Observable whose value has changed
   */
  virtual void update(ObservableBase const *obs) = 0;
};
/**
 * Base for Observable classes. Handles observer pattern.
 * Allows for disabling of notification on change.
 */
class ObservableBase {
 public:
  /**
   * Add observer whose update method will be called when this's value changes.
   * @param observer observer to notify on value change
   */
  void addObserver(Observer *observer) {
    observers.emplace_back(observer);
  }
  /**
   * Delete observer to prevent further update notifications.
   * @param observer Observer to delete from notify list
   */
  void deleteObserver(Observer *observer) {
    if (auto it = std::find(observers.begin(), observers.end(), observer); it
        != observers.end()) {
      observers.erase(it);
    }
  }
  /**
   * Clear all observers, preventing further notifications of change to any of already registered.
   */
  void deleteObservers() {
    observers.clear();
  }
  /**
   *
   * @return count of observers currently watching this's value
   */
  size_t countObservers() {
    return observers.size();
  }
  /**
   *
   * @return true if value has been changed, false otherwise
   */
  bool hasChanged() {
    return changed;
  }
  /**
   * Disable observer notification until enable() is called.
   */
  void disable() {
    enabled = false;
  }
  /**
   * Enable observer notification. On by default.
   */
  void enable() {
    enabled = true;
  }
  /**
   * Notify observers of a value change in this.
   * The changed and enabled flags have to be set for the notifications to go through.
   */
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
/**
 * Concrete implementation of Observable.
 * @tparam T type of observable value
 */
template<typename T>
class Observable
    : public ObservableBase {
 public:
  Observable() = default;
  explicit Observable(T value) : value(value) {}
  /**
   * Assign value and notify observers.
   * Doesn't notify observers if disable() has been called.
   * @param f new value
   */
  typename std::conditional<std::is_compound<T>::value, T &, T>::type operator=(
      const T &f) {
    value = f;
    changed = true;
    notifyObservers();
    return value;
  }
  /**
   * Implicit cast to stored value.
   */
  operator typename std::conditional<std::is_compound<T>::value,
                                     const T &,
                                     T>::type() const { return value; }
 protected:
  T value;
  friend class Binder<T>;
};

#endif //PROJECT_OBSERVABLE_H

#pragma clang diagnostic pop
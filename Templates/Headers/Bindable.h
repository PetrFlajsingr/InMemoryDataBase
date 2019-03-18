//
// Created by Petr Flajsingr on 2019-03-01.
//

#ifndef PROJECT_BINDABLE_H
#define PROJECT_BINDABLE_H

#include <list>

template<typename T>
class Binder;

template<typename T>
class Bindable;
/**
 * Utility class binding two Observables together. Handles one-way binding.
 * Example:
 *  A is bound to B
 *  When A changes value, B value stays the same.
 *  When B changes value, the value is written to A as well.
 * @tparam T type of bound values
 */
template<typename T>
class Binder : public Observer {
 public:
  Binder(Observable<T> *bind, Observable<T> *to) : bind(bind), to(to) {
    to->addObserver(this);
  }
  virtual ~Binder() {
    to->deleteObserver(this);
  }
  void update(ObservableBase const *obs) override {
    bind->disable();
    bind->value = reinterpret_cast<Bindable<T> const *>(obs)->value;
    bind->enable();
  }

 private:
  Observable<T> *to;
  Observable<T> *bind;
};
/**
 * Transformation binding. Allows to use a function to compute new value
 * when any of the two connected Bindables is changed.
 * @tparam T type of first bindable
 * @tparam U type of second bindable
 * @tparam Ret type of return value
 */
template<typename T,
    typename U,
    typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                             T,
                                             U>::type>
class Binding
    : public Bindable<Ret>,
      public Observer {
  using ComputeFnc = std::function<Ret(std::shared_ptr<Bindable<T>>,
                                       std::shared_ptr<Bindable<U>>)>;
  using Observable<Ret>::operator=;
 public:
  Binding(std::shared_ptr<Bindable<T>> a,
          std::shared_ptr<Bindable<U>> b,
          ComputeFnc compute)
      : a(a), b(b), compute(compute) {
    a->addObserver(this);
    b->addObserver(this);
    Observable<Ret>::value = compute(a, b);
  }
  virtual ~Binding() {
    a->deleteObserver(this);
    b->deleteObserver(this);
  }
  void update(ObservableBase const *) override {
    *this = compute(a, b);
  }

 private:
  ComputeFnc compute;
  std::shared_ptr<Bindable<T>> a;
  std::shared_ptr<Bindable<U>> b;
};
/**
 * Similiar to Binding class, this one transforms only a single value.
 * @tparam T return type
 * @tparam U type of Bindable
 */
template<typename T, typename U>
class TransformBinding : public Bindable<T>, public Observer {
  using TransFunc = std::function<T(U)>;
  using Observable<T>::operator=;
 public:
  TransformBinding(const std::shared_ptr<Bindable<U>> &a,
                   const TransFunc &transform)
      : transform(transform), a(a) {
    *this = transform(*a);
    a->addObserver(this);
  }
  void update(ObservableBase const *) override {
    *this = transform(*a);
  }
 private:
  TransFunc transform;
  std::shared_ptr<Bindable<U>> a;
};
/**
 * A value which can be bound to another one via objects above.
 * @tparam T type of value storec
 */
template<typename T>
class Bindable
    : public Observable<T>, public std::enable_shared_from_this<Bindable<T>> {
 public:
  Bindable() {}
  explicit Bindable(T value) : Observable<T>(value) {}
  void bindBidirectional(Bindable<T> *to) {
    to->bind(this);
    bind(to);
  }

  void bind(Bindable<T> *to) {
    binders.emplace_back(this, to);
  }

  void unbind(Bindable<T> *from) {
    if (auto it = std::find(binders.begin(), binders.end(), from); it
        != binders.end()) {
      binders.remove(it);
    }
  }

  using Observable<T>::operator=;

  template<typename U,
      typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                               T,
                                               U>::type>
  std::shared_ptr<Binding<T, U>> add(std::shared_ptr<Bindable<U>> obs) {
    static_assert(
        std::is_fundamental<T>::value && std::is_fundamental<U>::value);
    return std::make_shared<Binding<T, U>>(this->shared_from_this(),
                                           obs,
                                           [](std::shared_ptr<Observable<T>> a,
                                              std::shared_ptr<Observable<U>> b) -> Ret {
                                             return (T) *a + (U) * b;
                                           });
  }

  template<typename U,
      typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                               T,
                                               U>::type>
  std::shared_ptr<Binding<T, U>> diff(std::shared_ptr<Bindable<U>> obs) {
    static_assert(
        std::is_fundamental<T>::value && std::is_fundamental<U>::value);
    return std::make_shared<Binding<T, U>>(this->shared_from_this(),
                                           obs,
                                           [](std::shared_ptr<Bindable<T>> a,
                                              std::shared_ptr<Bindable<U>> b) -> Ret {
                                             return (T) *a - (U) * b;
                                           });
  }

  template<typename U,
      typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                               T,
                                               U>::type>
  std::shared_ptr<Binding<T, U>> mul(std::shared_ptr<Bindable<U>> obs) {
    static_assert(
        std::is_fundamental<T>::value && std::is_fundamental<U>::value);
    return std::make_shared<Binding<T, U>>(this->shared_from_this(),
                                           obs,
                                           [](std::shared_ptr<Bindable<T>> a,
                                              std::shared_ptr<Bindable<U>> b) -> Ret {
                                             return (T) *a * (U) * b;
                                           });
  }

  template<typename U,
      typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                               T,
                                               U>::type>
  std::shared_ptr<Binding<T, U>> div(std::shared_ptr<Bindable<U>> obs) {
    static_assert(
        std::is_fundamental<T>::value && std::is_fundamental<U>::value);
    return std::make_shared<Binding<T, U>>(this->shared_from_this(),
                                           obs,
                                           [](std::shared_ptr<Bindable<T>> a,
                                              std::shared_ptr<Bindable<U>> b) -> Ret {
                                             return (T) *a / (U) * b;
                                           });
  }

  template<typename U>
  std::shared_ptr<TransformBinding<U, T>> transform(std::function<U(T)> func) {
    static_assert(
        std::is_fundamental<U>::value && std::is_fundamental<T>::value);
    return std::make_shared<TransformBinding<U, T>>(this->shared_from_this(),
                                                    func);
  }

 private:
  std::list<Binder<T>> binders;
  friend class Binder<T>;
};

#endif  //PROJECT_BINDABLE_H

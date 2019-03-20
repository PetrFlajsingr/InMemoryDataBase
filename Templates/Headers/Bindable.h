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
  /**
   *
   * @param bind value to be bound
   * @param to value to bind to
   */
  Binder(Observable<T> *bind, Observable<T> *to) : bind(bind), to(to) {
    to->addObserver(this);
  }
  /**
   * Remove observer from observed value.
   */
  virtual ~Binder() {
    to->deleteObserver(this);
  }
  /**
   * Update value without notifying its observers to avoid recursive updates.
   * @param obs
   */
  // TODO: resolve better
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
  using ComputeFnc = std::function<Ret(std::shared_ptr<Bindable<T>>, std::shared_ptr<Bindable<U>>)>;
  using Observable<Ret>::operator=;
 public:
  /**
   * Create observers for both watched values.
   * @param a first value to be watched
   * @param b second value to be watched
   * @param compute function computing a new value
   */
  Binding(std::shared_ptr<Bindable<T>> a, std::shared_ptr<Bindable<U>> b, ComputeFnc compute)
      : a(a), b(b), compute(compute) {
    a->addObserver(this);
    b->addObserver(this);
    Observable<Ret>::value = compute(a, b);
  }
  /**
   * Remove observer from watched values
   */
  virtual ~Binding() {
    a->deleteObserver(this);
    b->deleteObserver(this);
  }
  /**
   * Update stored value and notify observers (using Observable<Ret>::operator=)
   */
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
  /**
   * Add observer to a
   * @param a watched value
   * @param transform transformation function triggered when a's value is changed
   */
  TransformBinding(const std::shared_ptr<Bindable<U>> &a, const TransFunc &transform)
      : transform(transform), a(a) {
    *this = transform(*a);
    a->addObserver(this);
  }
  /**
   * Compute new value and notify observers.
   */
  void update(ObservableBase const *) override {
    *this = transform(*a);
  }

 private:
  TransFunc transform;
  std::shared_ptr<Bindable<U>> a;
};
/**
 * A value which can be bound to another one via objects above.
 * @tparam T type of value stored
 */
template<typename T>
class Bindable : public Observable<T>, public std::enable_shared_from_this<Bindable<T>> {
 public:
  Bindable() {}
  explicit Bindable(T value) : Observable<T>(value) {}
  /**
   * Bind this to provided Bindable and the Bindable to this.
   * When value of this changes the to's value is changed and vice versa.
   * @param to Bindable to bind to
   */
  void bindBidirectional(Bindable<T> *to) {
    to->bind(this);
    bind(to);
  }
  /**
   * Bind this to provided Bindable whose type doesn't match.
   * When value of this changes the to's value is changed and vice versa.
   * @tparam U type stored in the second Bindable
   * @param to Bindable to bind to
   * @param converter Converter between U and T
   */
  template<typename U>
  void bindBidirectional(Bindable<U> *to, Converter<U, T> *converter) {
    // TODO
    throw NotImplementedException();
  }
  /**
   * Bind value of this to 'to'.
   * When value of to changes the value of this is modified accordingly.
   * @param to Bindable to bind to
   */
  void bind(Bindable<T> *to) {
    binders.emplace_back(this, to);
  }
  /**
   * Break binding on 'from'.
   * If this was not bound to 'from' nothing happens.
   * @param from Bindable to break bind with
   */
  void unbind(Bindable<T> *from) {
    if (auto it = std::find(binders.begin(), binders.end(), from); it
        != binders.end()) {
      binders.remove(it);
    }
  }
  using Observable<T>::operator=;
  /**
   * Create a transformation binding containing an addition of this and obs.
   * @see Binding
   * @tparam U type of second Bindable
   * @tparam Ret return type of binding
   * @param obs second Bindable
   * @return Binding doing addition
   */
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
  /**
   * Create a transformation binding containing a substraction of this and obs.
   * @see Binding
   * @tparam U type of second Bindable
   * @tparam Ret return type of binding
   * @param obs second Bindable
   * @return Binding doing substraction
   */
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
  /**
   * Create a transformation binding containing a multiplication of this and obs.
   * @see Binding
   * @tparam U type of second Bindable
   * @tparam Ret return type of binding
   * @param obs second Bindable
   * @return Binding doing multiplication
   */
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
  /**
   * Create a transformation binding containing a division of this and obs.
   * @see Binding
   * @tparam U type of second Bindable
   * @tparam Ret return type of binding
   * @param obs second Bindable
   * @return Binding doing division
   */
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
  /**
   * Create a transform binding on this.
   * @see TransformBinding
   * @tparam U return value of a transform function
   * @param func transform function
   * @return TransformBinding transformating this's value
   */
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

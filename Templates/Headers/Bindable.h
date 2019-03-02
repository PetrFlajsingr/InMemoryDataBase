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

template<typename T,
    typename U,
    typename Ret = typename std::conditional<std::is_floating_point<T>::value,
                                             T,
                                             U>::type>
class Binding
    : public Bindable<Ret>,
      public Observer {
  using ComputeFnc = std::function<Ret(std::shared_ptr<Bindable<T>> a,
                                       std::shared_ptr<Bindable<U>> b)>;
 public:
  using Observable<Ret>::operator=;
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

template<typename T>
class Bindable
    : public Observable<T>, public std::enable_shared_from_this<Bindable<T>> {
 public:
  Bindable() = default;
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

 private:
  std::list<Binder<T>> binders;
  friend class Binder<T>;
};

#endif  //PROJECT_BINDABLE_H

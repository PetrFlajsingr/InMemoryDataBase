#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma ide diagnostic ignored "cppcoreguidelines-c-copy-assignment-signature"
//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_PROPERTY_H
#define PROJECT_PROPERTY_H

#include <type_traits>
#include <iostream>

template<typename T>
struct is_shared_ptr : std::false_type {};
template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<class T>
struct ptr_type { typedef T type; };
template<class T>
struct ptr_type<std::shared_ptr<T>> {
  typedef T *type;
};

enum PropType {
  R, W, RW
};

template<typename T, typename Owner, PropType SetGet>
class Property final {
  friend Owner;
  using Getter = std::function<T &()>;
  using Setter = std::function<void(const T &)>;
 public:
  Property() = default;
  template<PropType Type = SetGet,
      typename = typename std::enable_if<Type == PropType::RW,
                                         void>::type>
  Property(const Getter &get,
           const Setter &set) : get(get), set(set) {}
  template<PropType Type = SetGet,
      typename = typename std::enable_if<Type == PropType::R, void>::type>
  Property(const Getter &get) : get(get) {}
  template<PropType Type = SetGet,
      typename = typename std::enable_if<Type == PropType::W, void>::type>
  Property(const Setter &set) : set(set) {}
  ~Property() = default;
  Property<T, Owner>(const Property<T, Owner, SetGet> &) = delete;
  Property<T, Owner, SetGet> &operator=(const Property<T,
                                                       Owner,
                                                       SetGet> &) = delete;
  template<PropType Type = SetGet,
      typename = typename std::enable_if<Type != PropType::R, void>::type>
  typename std::conditional<std::is_compound<T>::value, T &, T>::type operator=(
      const T &f) {
    set(f);
    return value;
  }
  template<PropType Type = SetGet>
  typename std::enable_if<
      Type != PropType::W && (std::is_pointer<T>::value || is_shared_ptr<T>{}),
      typename std::conditional<std::is_pointer<T>::value,
                                T,
                                typename std::conditional<is_shared_ptr<T>{},
                                                          typename ptr_type<
                                                              T>::type,
                                                          void>::type>::type>::type
  operator->() const { return &*get(); }

  template<PropType Type = SetGet,
      typename = typename std::enable_if<Type != PropType::W, void>::type>
  operator typename std::conditional<std::is_compound<T>::value,
                                     const T &,
                                     T>::type() const {
    return get();
  }

  template<PropType Type = SetGet>
  typename std::enable_if<
      Type != PropType::W && (std::is_pointer<T>::value || is_shared_ptr<T>{}),
      typename std::conditional<std::is_pointer<T>::value,
                                typename std::remove_pointer<T>::type,
                                typename std::conditional<is_shared_ptr<T>{},
                                                          typename std::remove_pointer<
                                                              typename ptr_type<
                                                                  T>::type>::type,
                                                          void>::type>::type>::type
  operator*() const {
    return *value;
  }
 private:
  Getter get =
      [this]() -> T & { return value; };
  Setter set = [this](const T &val) {
    value = val;
  };
  T value;
};
#endif //PROJECT_PROPERTY_H

#pragma clang diagnostic pop
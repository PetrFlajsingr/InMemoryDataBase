#pragma clang diagnostic push
//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_PROPERTY_H
#define PROJECT_PROPERTY_H

#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T> struct is_shared_ptr : std::false_type {};
template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <class T> struct ptr_type { typedef T type; };
template <class T> struct ptr_type<std::shared_ptr<T>> { typedef T *type; };
/**
 * Type of property.
 * R    -   read only
 * W    -   write only
 * RW   -   read write
 */
enum PropType { R, W, RW };

/**
 * Emulation of c# properties.
 * @tparam T type stored inside property
 * @tparam Owner owner type (in order to access inner value)
 * @tparam SetGet type of property - read/write only, read write
 */
template <typename T, typename Owner, PropType SetGet> class Property final {
  friend Owner;
  using Getter = std::function<T &()>;
  using Setter = std::function<void(const T &)>;

public:
  Property() = default;
  /**
   * Create read write property
   * @param get getter
   * @param set setter
   */
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::RW, void>::type>
  Property(const Getter &get, const Setter &set) : get(get), set(set) {}

  /**
   * Create ready only property
   * @param get getter
   */
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::R, void>::type>
  explicit Property(const Getter &get) : get(get) {}
  /**
   * Create write only property
   * @param set setter
   */
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::W, void>::type>
  explicit Property(const Setter &set) : set(set) {}
  ~Property() = default;
  Property<T, Owner, SetGet>(const Property<T, Owner, SetGet> &) = delete;
  Property<T, Owner, SetGet> &operator=(const Property<T, Owner, SetGet> &) = delete;
  /**
   * Set value using Setter function
   */
  template <PropType Type = SetGet, typename = typename std::enable_if<Type != PropType::R, void>::type>
  T &operator=(const T &f) {
    set(f);
    return value;
  }
  template <PropType Type = SetGet>
  typename std::enable_if<
      Type != PropType::W && (std::is_pointer<T>::value || is_shared_ptr<T>{}),
      typename std::conditional<
          std::is_pointer<T>::value, T,
          typename std::conditional<is_shared_ptr<T>{}, typename ptr_type<T>::type, void>::type>::type>::type
  operator->() const {
    return &*get();
  }

  template <PropType Type = SetGet, typename = typename std::enable_if<Type != PropType::W, void>::type>
  operator T &() {
    return get();
  }

  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::R>::type>
  const T &operator*() const {
    return value;
  }

  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::RW>::type> T &operator*() {
    return value;
  }

  template <typename... Args> static Property<T, Owner, SetGet> make_property(Getter getter, Args... args) {
    static_assert(std::is_constructible<T, Args...>::value, "Can't construct in make_property");
    static_assert(SetGet != R, "Property is not read only");
    return Property<T, Owner, SetGet>(getter, T(std::forward<Args>(args)...));
  }

  template <typename... Args> static Property<T, Owner, SetGet> make_property(Setter getter, Args... args) {
    static_assert(std::is_constructible<T, Args...>::value, "Can't construct in make_property");
    static_assert(SetGet != W, "Property is not write only");
    return Property<T, Owner, SetGet>(getter, T(std::forward<Args>(args)...));
  }

  template <typename... Args>
  static Property<T, Owner, SetGet> make_property(Getter getter, Setter setter, Args... args) {
    static_assert(std::is_constructible<T, Args...>::value, "Can't construct in make_property");
    static_assert(SetGet != RW, "Property is not read write");
    return Property<T, Owner, SetGet>(getter, setter, T(std::forward<Args>(args)...));
  }

  template <typename... Args> static Property<T, Owner, SetGet> make_property(Args... args) {
    return Property<T, Owner, SetGet>(T(std::forward<Args>(args)...));
  }

private:
  Getter get = [this]() -> T & { return value; };
  Setter set = [this](const T &val) { value = val; };
  T value;

  Property(T val) : value(val) {}
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::RW, void>::type>
  Property(const Getter &get, const Setter &set, T val) : get(get), set(set), value(val) {}
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::R, void>::type>
  Property(const Getter &get, T val) : get(get), value(val) {}
  template <PropType Type = SetGet, typename = typename std::enable_if<Type == PropType::W, void>::type>
  Property(const Setter &set, T val) : set(set), value(val) {}
};

template<typename Class>
struct UseProperty {
  template <typename T, PropType type>
  using Property = Property<T, Class, type>;
};

template <typename T, typename Owner, typename... Args, typename Getter = std::function<const T &()>>
Property<T, Owner, R> make_property(Getter getter, Args... args) {
  return Property<T, Owner, R>::make_property(getter, std::forward<Args>(args)...);
}

template <typename T, typename Owner, typename... Args, typename Setter = std::function<void(const T &)>>
Property<T, Owner, W> make_property(Setter setter, Args... args) {
  return Property<T, Owner, W>::make_property(setter, std::forward<Args>(args)...);
}

template <typename T, typename Owner, typename... Args, typename Getter = std::function<const T &()>,
          typename Setter = std::function<void(const T &)>>
Property<T, Owner, RW> make_property(Getter getter, Setter setter, Args... args) {
  return Property<T, Owner, RW>::make_property(getter, setter, std::forward<Args>(args)...);
}

template <typename T, typename Owner, PropType SetGet, typename... Args>
Property<T, Owner, SetGet> make_property(Args... args) {
  return Property<T, Owner, SetGet>::make_property(std::forward<Args>(args)...);
}

#endif // PROJECT_PROPERTY_H

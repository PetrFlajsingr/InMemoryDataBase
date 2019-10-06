//
// Created by Petr Flajsingr on 2019-02-24.
//

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include <Exceptions.h>
#include <Utilities.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Config in files using categories & key value pairs
 */
class Config {
  template <typename T> std::string toString(T val) {
    if constexpr (std::is_integral<T>{} || std::is_floating_point<T>{}) {
      return std::to_string(val);
    }
    if constexpr (std::is_same<T, std::string>{} || std::is_same<T, char *>{} || std::is_same<T, const char *>{}) {
      return val;
    }
  }

  template <typename T> T fromString(std::string val) {
    if constexpr (std::is_integral<T>{}) {
      return Utilities::stringToInt(val);
    }
    if constexpr (std::is_floating_point<T>{}) {
      return Utilities::stringToDouble(val);
    }
    if constexpr (std::is_same<T, std::string>{}) {
      return val;
    }
  }

public:
  /**
   *
   * @param path path to config file
   * @param autoCommit save changes automatically
   */
  explicit Config(const std::string &path, bool autoCommit = false);
  virtual ~Config();
  /**
   *
   * @tparam T requested data type
   * @param category
   * @param key
   * @param defaultVal
   * @return required value if found, else defaultVal
   */
  template <typename T> T getValue(const std::string &category, const std::string &key, T defaultVal) {
    if (categories.find(category) != categories.end()) {
      auto cat = categories[category];
      if (cat.find(key) != cat.end()) {
        return fromString<T>(cat[key]);
      }
    }
    categories[category][key] = toString(defaultVal);
    if (autoCommit) {
      std::ofstream file(path);
      save();
    }
    return defaultVal;
  }
  /**
   * Get value of requested type. Throw ResourceNotFoundException when no value is found.
   * @tparam T requested data type
   * @param category
   * @param key
   * @return required value
   */
  template <typename T> T getValue(std::string_view category, std::string_view key) {
    if (categories.find(std::string(category)) != categories.end()) {
      auto cat = categories[std::string(category)];
      if (cat.find(std::string(key)) != cat.end()) {
        return fromString<T>(cat[std::string(key)]);
      }
    }
    throw ResourceNotFoundException("Resource [" + std::string(category) + "[" + std::string(key) + "]] not found");
  }
  /**
   * Save value on autocommit
   * @tparam T
   * @param category
   * @param key
   * @param value
   */
  template <typename T> void setValue(const std::string &category, const std::string &key, T value) {
    categories[category][key] = toString(value);
    if (autoCommit) {
      save();
    }
  }
  /**
   * Save data to file if autocommit is off.
   */
  void commit();

private:
  std::string path;
  bool autoCommit;

  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> categories;
  void load();
  void save();
};

#endif // PROJECT_CONFIG_H

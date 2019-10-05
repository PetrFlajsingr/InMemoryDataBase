//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_RESOURCEMANAGER_H
#define PROJECT_RESOURCEMANAGER_H

#include <string>
#include <Exceptions.h>
#include "Config.h"
#include "FileManager.h"

enum ResourceType {
  Image, Text, Csv, Archive
};
/**
 * Access point for resource handling.
 */
class ResourceManager {
 public:
  /**
   * Return a string resource [category[name]]
   * @tparam T type of resource
   * @param resString resource string indetifying the value
   * @return value saved under provided key
   */
  template<typename T>
  T getResource(std::string_view resString) {
    if (resString[0] == '[' && resString.back() == ']' && resString[resString.size() - 2] == ']') {
      auto sub = resString.substr(1, resString.size() - 3);
      auto pos = sub.find('[');
      if (pos != std::string::npos) {
        auto cat = sub.substr(0, pos);
        auto key = sub.substr(pos + 1, sub.size() - pos - 1);

        return config.getValue<T>(cat, key);
      }
    }
    throw InvalidArgumentException("Invalid resource format.");
  }

  File getResourceFile(std::string_view fileName);

 private:
  Config config = Config("./test.conf");

    Folder resourceFolder = FileManager::getFolder("./resources");
};

#endif //PROJECT_RESOURCEMANAGER_H

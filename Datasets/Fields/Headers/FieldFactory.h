//
// Created by Petr Flajsingr on 2019-02-04.
//

#ifndef PROJECT_FIELDFACTORY_H
#define PROJECT_FIELDFACTORY_H

#include <BaseField.h>
#include <IntegerField.h>
#include <DoubleField.h>
#include <StringField.h>
#include <CurrencyField.h>
#include <DateTimeField.h>

namespace DataSets {

class FieldFactory {
 public:
  static FieldFactory &Get() {
    static FieldFactory instance;
    return instance;
  }

  std::shared_ptr<DataSets::BaseField> CreateField(std::string_view name,
                                                   gsl::index index,
                                                   ValueType type,
                                                   BaseDataSet *dataSet);

 private:
  FieldFactory() = default;

  FieldFactory(const FieldFactory &) = default;

  FieldFactory &operator=(const FieldFactory &) = default;
};

}  // namespace DataSets

#endif //PROJECT_FIELDFACTORY_H

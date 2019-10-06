//
// Created by Petr Flajsingr on 2019-02-04.
//

#ifndef PROJECT_FIELDFACTORY_H
#define PROJECT_FIELDFACTORY_H

#include <BaseField.h>
#include <CurrencyField.h>
#include <DateTimeField.h>
#include <DoubleField.h>
#include <IntegerField.h>
#include <StringField.h>

namespace DataSets {

/**
 * Create specific fields and return them as shared_ptr.
 * Singleton.
 */
class FieldFactory {
public:
  static FieldFactory &GetInstance() {
    static FieldFactory instance;
    return instance;
  }
  /**
   * Create field based on provided parameters.
   * @param name name of the field (column)
   * @param index index in data set
   * @param type type of store data
   * @param dataSet owner data set
   * @return field created based on provided parameters
   */
  static std::shared_ptr<DataSets::BaseField> CreateField(std::string_view name, gsl::index index, ValueType type,
                                                          BaseDataSet *dataSet);

  FieldFactory(const FieldFactory &) = delete;
  FieldFactory &operator=(const FieldFactory &) = delete;

private:
  FieldFactory() = default;
};

} // namespace DataSets

#endif // PROJECT_FIELDFACTORY_H

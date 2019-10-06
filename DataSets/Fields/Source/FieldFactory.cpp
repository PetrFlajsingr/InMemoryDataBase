//
// Created by Petr Flajsingr on 2019-02-04.
//

#include <FieldFactory.h>

std::shared_ptr<DataSets::BaseField> DataSets::FieldFactory::CreateField(std::string_view name, gsl::index index,
                                                                         ValueType type, BaseDataSet *dataSet) {
  switch (type) {
  case ValueType::Integer:
    return std::make_shared<IntegerField>(name, index, dataSet);
  case ValueType::Double:
    return std::make_shared<DoubleField>(name, index, dataSet);
  case ValueType::String:
    return std::make_shared<StringField>(name, index, dataSet);
  case ValueType::Currency:
    return std::make_shared<CurrencyField>(name, index, dataSet);
  case ValueType::DateTime:
    return std::make_shared<DateTimeField>(name, index, dataSet);
  default:
    throw InvalidArgumentException("Field type not supported");
  }
}

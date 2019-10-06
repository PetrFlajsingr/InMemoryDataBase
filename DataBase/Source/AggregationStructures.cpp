//
// Created by Petr Flajsingr on 2019-03-21.
//
#include <AggregationStructures.h>
#include <CurrencyField.h>
#include <DoubleField.h>
#include <IntegerField.h>

bool DataBase::Unique::check(const DataContainer &newVal) {
  int cmpResult;
  switch (field->getFieldType()) {
  case ValueType::Integer:
    cmpResult = lastVal.compare<int>(newVal);
    break;
  case ValueType::Double:
    cmpResult = lastVal.compare<double>(newVal);
    break;
  case ValueType::String:
    cmpResult = lastVal.compare<gsl::zstring<>>(newVal);
    break;
  case ValueType::Currency:
    cmpResult = lastVal.compare<Currency>(newVal);
    break;
  case ValueType::DateTime:
    cmpResult = lastVal.compare<DateTime>(newVal);
    break;
  }
  return cmpResult != 0;
}
DataBase::Unique::Unique(DataSets::BaseField *field) : BaseAgr(field) {
  switch (field->getFieldType()) {
  case ValueType::String:
    lastVal = "";
    break;
  case ValueType::Currency:
    lastVal = Currency(0);
    break;
  case ValueType::DateTime:
    lastVal = DateTime();
    break;
  default:
    break;
  }
}
DataBase::Sum::Sum(DataSets::BaseField *field) : BaseAgr(field) {
  switch (field->getFieldType()) {
  case ValueType::Integer:
    sum = 0;
    break;
  case ValueType::Double:
    sum = 0.0;
    break;
  case ValueType::Currency:
    sum = Currency(0);
    break;
  default:
    throw std::logic_error("DataBase::Sum constructor.");
  }
}

void DataBase::Sum::accumulate(const DataContainer &val) {
  switch (field->getFieldType()) {
  case ValueType::Integer:
    sum._integer += val._integer;
    break;
  case ValueType::Double:
    sum._double += val._double;
    break;
  case ValueType::Currency:
    *sum._currency += *val._currency;
    break;
  default:
    throw std::runtime_error("Sum::accumulate()");
  }
}
void DataBase::Sum::reset() {
  switch (field->getFieldType()) {
  case ValueType::Integer:
    sum = 0;
    break;
  case ValueType::Double:
    sum = 0.0;
    break;
  case ValueType::Currency:
    sum = Currency(0);
    break;
  default:
    throw std::runtime_error("Sum::reset()");
  }
}
DataBase::BaseAgr::BaseAgr(DataSets::BaseField *field) : field(field) {
  fieldIndex = DataSets::BaseField::convertIndex(*field);
}

DataBase::Min::Min(DataSets::BaseField *field) : BaseAgr(field) { reset(); }
void DataBase::Min::check() {
  switch (field->getFieldType()) {
  case ValueType::Integer: {
    auto val = dynamic_cast<DataSets::IntegerField *>(field)->getAsInteger();
    if (val < min._integer) {
      min._integer = val;
    }
  } break;
  case ValueType::Double: {
    auto val = dynamic_cast<DataSets::DoubleField *>(field)->getAsDouble();
    if (val < min._double) {
      min._double = val;
    }
  } break;
  case ValueType::Currency: {
    auto val = dynamic_cast<DataSets::CurrencyField *>(field)->getAsCurrency();
    if (val < *min._currency) {
      *min._currency = val;
    }
  } break;
  default:
    throw std::runtime_error("Min::check()");
  }
}
void DataBase::Min::reset() {
  switch (field->getFieldType()) {
  case ValueType::Integer:
    min._integer = std::numeric_limits<int>::min();
    break;
  case ValueType::Double:
    min._double = std::numeric_limits<double>::lowest();
    break;
  case ValueType::Currency:
    *min._currency = DEC_MIN_INT64;
    break;
  default:
    throw std::runtime_error("Min::reset()");
  }
}

DataBase::Max::Max(DataSets::BaseField *field) : BaseAgr(field) { reset(); }
void DataBase::Max::check() {
  switch (field->getFieldType()) {
  case ValueType::Integer: {
    auto val = dynamic_cast<DataSets::IntegerField *>(field)->getAsInteger();
    if (val > max._integer) {
      max._integer = val;
    }
  } break;
  case ValueType::Double: {
    auto val = dynamic_cast<DataSets::DoubleField *>(field)->getAsDouble();
    if (val > max._double) {
      max._double = val;
    }
  } break;
  case ValueType::Currency: {
    auto val = dynamic_cast<DataSets::CurrencyField *>(field)->getAsCurrency();
    if (val > *max._currency) {
      *max._currency = val;
    }
  } break;
  default:
    throw std::runtime_error("Max::check()");
  }
}
void DataBase::Max::reset() {
  switch (field->getFieldType()) {
  case ValueType::Integer:
    max._integer = std::numeric_limits<int>::max();
    break;
  case ValueType::Double:
    max._double = std::numeric_limits<double>::max();
    break;
  case ValueType::Currency:
    *max._currency = DEC_MAX_INT64;
    break;
  default:
    throw std::runtime_error("Max::reset()");
  }
}

DataBase::Count::Count(DataSets::BaseField *field) : BaseAgr(field) {}
void DataBase::Count::add() { count++; }
void DataBase::Count::reset() { count = 0; }

DataBase::Avg::Avg(DataSets::BaseField *field) : Sum(field), Count(field) {}
void DataBase::Avg::accumulate(const DataContainer &val) {
  Sum::accumulate(val);
  Count::add();
}
void DataBase::Avg::reset() {
  Sum::reset();
  Count::reset();
}
DataContainer DataBase::Avg::getResult() {
  DataContainer result;
  switch (Sum::field->getFieldType()) {
  case ValueType::Integer:
    result._double = Sum::sum._integer / (double)Count::count;
    break;
  case ValueType::Double:
    result._double = Sum::sum._double / Count::count;
    break;
  case ValueType::Currency:
    *result._currency = *Sum::sum._currency / (int64_t)Count::count;
    break;
  default:
    throw std::runtime_error("Avg::getResult()");
  }
  return result;
}

//
// Created by Petr Flajsingr on 2019-02-10.
//

#ifndef PROJECT_VIEWDATASET_H
#define PROJECT_VIEWDATASET_H

#include "BaseDataSet.h"

namespace DataSets {

// read only
class ViewDataSet : public BaseDataSet {
public:
  explicit ViewDataSet(const std::string_view &dataSetName);
};
} // namespace DataSets

#endif // PROJECT_VIEWDATASET_H

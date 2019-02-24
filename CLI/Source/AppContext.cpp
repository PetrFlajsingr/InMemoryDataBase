//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <AppContext.h>

#include "AppContext.h"
AppContext &AppContext::GetInstance() {
  static AppContext instance;
  return instance;
}

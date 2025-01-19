#pragma once

#include "sparkbox/core/core_driver.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/router.h"

namespace sparkbox {

class CoreManager : public Manager {
 public:
  CoreManager(Router& router, CoreDriver& core_driver)
      : Manager(router, Destination::kCoreManager), core_driver_(core_driver) {}

 private:
  CoreDriver& core_driver_;
  void HandleMessage(Message& message) final;
};

}  // namespace sparkbox
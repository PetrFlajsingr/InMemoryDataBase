//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <CLIController.h>
#include "Templates/Headers/Observable.h"
#include <Bindable.h>

void terminate_handler();

class Test {
 public:
  Test() {
    dataSet.value = DataSets::MemoryDataSet("test");
    ptr = &dataSet.value;
  }

  Property<DataSets::MemoryDataSet, Test, RW> dataSet =
      make_property<DataSets::MemoryDataSet, Test, RW>("man");

  void foo() {
    std::cout << "get" << std::endl;
  }

  DataSets::MemoryDataSet *ptr;

  std::shared_ptr<Bindable<int>> a = std::make_shared<Bindable<int>>(2);
  std::shared_ptr<Bindable<float>> b = std::make_shared<Bindable<float>>(2.0f);
  std::shared_ptr<Bindable<int>> c = std::make_shared<Bindable<int>>(2);
  std::shared_ptr<Bindable<double>> d = std::make_shared<Bindable<double>>(2);
};

class ARTest : public std::enable_shared_from_this<ARTest> {
 public:
  std::shared_ptr<ARTest> getPtr() {
    return shared_from_this();
  }
  virtual ~ARTest() {
    std::cout << "~ARTest called" << std::endl;
  }
};

int main(int argc, char **argv) {
  //CLIController cl;
  //cl.runApp();

  Test test;
  auto binding = test.a->transform<int>([](int a) {
    return 10 * a;
  });
  std::cout << *binding << std::endl;
  *test.a = 100;
  std::cout << *binding << std::endl;

  (*test.dataSet).setName("hihihi");

  std::cout << (*test.dataSet).getName();



  /*moor::ArchiveReader reader("/Users/petr/Downloads/libarchive-3.3.3.tar");
  bool ex = true;
  while (ex) {
    auto va = reader.ExtractNext();
    if (va.first.empty()) {
      ex = false;
    }
    std::cout << va.first <<std::endl;
  }

  return 0;*/
  /*FileDownloadManager man;
  Obs obs;
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp",
                      obs, false);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/language",
                      obs, false);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/memory_model",
                      obs, true);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/namespace_alias",
                      obs, false);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/operator_other",
                      obs, false);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/adl",
                      obs, true);
  man.enqueueDownload("/Users/petr/Desktop/dl/",
                      "https://en.cppreference.com/w/cpp/partial_specialization",
                      obs,
                      false);

  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;*/
}

void terminate_handler() {
  try {
    auto unknown = std::current_exception();
    if (unknown) {
      std::rethrow_exception(unknown);
    } else {
      std::cerr << "normal termination" << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Exc type: " << typeid(e).name() << "\nExc.what: " << e.what()
              << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception of type" << std::endl;
  }
  abort();
}
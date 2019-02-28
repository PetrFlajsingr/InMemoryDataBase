//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <CLIController.h>
#include <Lazy.h>

void terminate_handler();

class LazyTest {
 public:
  LazyTest() {
    bool1.value = true;
    bool2.value = false;
    dt.value = DateTime();
  }
  Property<bool, LazyTest, RW> bool1{
      [this]() -> bool & { return bool1.value; },
      [this](const bool &value) {
        bool1.value = value;
        isTrue.invalidate();
      }
  };

  Property<DateTime, LazyTest, RW> dt;

  Property<bool, LazyTest, R> bool2{
      [this]() -> bool & { return bool2.value; }
  };

  Property<bool, LazyTest, W> bool3{
      [this](const bool &value) -> bool & {
        bool3.value = value;
        return bool3.value;
      }
  };

  Lazy<bool> isTrue = Lazy<bool>([this] {
    return (bool) bool1 | (bool) bool2;
  });
};

int main(int argc, char **argv) {
  CLIController cl;
  cl.runApp();
  LazyTest test;
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
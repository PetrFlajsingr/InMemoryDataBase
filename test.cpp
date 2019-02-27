//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>
#include <SemanticAnalyser.h>
#include <MemoryDataBase.h>
#include <JoinMaker.h>
#include <CsvReader.h>
#include <XlsxWriter.h>
#include <CLIController.h>
#include <CsvWriter.h>
#include <Config.h>
#include <ThreadPool.h>
#include <FileDownloader.h>
#include <FileDownloadManager.h>
#include <moor/archive_reader.hpp>
#include <Message.h>
#include <MessageManager.h>
#include <MessageReceiver.h>
#include <MessageSender.h>
#include <regex>
#include <Lazy.h>


void terminate_handler();

class LazyTest {
 public:
  LazyTest() {
    bool1.setOwner(this);
    bool2.setOwner(this);
    bool1.value = true;
    bool2.value = false;
  }
  class : public Property<bool, LazyTest> {
   public:
    using Property<bool, LazyTest>::operator=;
   protected:
    bool const &get() const override {
      return value;
    }
    bool &set(const bool &f) override {
      value = f;
      owner->isTrue.invalidate();
      return value;
    }
  } bool1;

  class : public Property<bool, LazyTest> {
   public:
    using Property<bool, LazyTest>::operator=;
   protected:
    bool const &get() const override {
      return value;
    }
    bool &set(const bool &f) override {
      value = f;
      owner->isTrue.invalidate();
      return value;
    }
  } bool2;

  Lazy<bool> isTrue = Lazy<bool>([this] {
    return bool1 | bool2;
  });

};

int main(int argc, char **argv) {
  LazyTest test;
  std::cout << test.isTrue << std::endl;
  std::cout << test.isTrue << std::endl;
  std::cout << test.bool1 << std::endl;
  test.bool1 = false;
  std::cout << test.bool1 << std::endl;
  std::cout << test.isTrue << std::endl;

  return 0;
  CLIController cl;
  cl.runApp();
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
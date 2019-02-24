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

void terminate_handler();

int cnt = 0;

class Obs : public FileDownloadObserver {
 public:
  void onDownloadStarted(std::string_view fileName) override {
    std::cout << "Started " << fileName << std::endl;
  }
  void onDownloadFailed(std::string_view fileName,
                        std::string_view errorMessage) override {
    std::cout << "failed " << fileName << std::endl;
  }
  void onDownloadFinished(std::string_view fileName,
                          std::string_view filePath) override {
    std::cout << "finished " << fileName << " downloaded to: " << filePath
              << std::endl;
  }
};

int main(int argc, char **argv) {
  FileDownloadManager man;
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
  return 0;
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
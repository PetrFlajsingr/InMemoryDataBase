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

void terminate_handler();

std::string replaceResources(std::string input) {
  Config config("./test.conf", true);
  std::regex
      rx(R"(\[[a-zA-Z0-9]*\[[a-zA-Z0-9]*\]\])", std::regex_constants::icase);
  std::smatch matches;
  std::string output;
  std::cout << input.length() << std::endl;
  while (std::regex_search(input, matches, rx)) {
    for (gsl::index i = 0; i < matches.size(); ++i) {
      output += input.substr(0, matches.position(i));

      std::string match = matches[i];
      auto index = match.find_last_of('[');
      auto index2 = match.find(']');
      output += config.getValue<std::string>(match.substr(1, index - 1),
                                             match.substr(index + 1,
                                                          index2 - index - 1));
    }
    input = matches.suffix().str();
  }
  output += input;
  return output;
}

void man() {
  std::regex
      rx(R"(\[[a-zA-Z0-9]*\[[a-zA-Z0-9]*\]\])", std::regex_constants::icase);
  std::smatch OuMatches;
  std::string input("test [ye[ma]] sad [hi[d]]dfdsfdsfsd");
  while (std::regex_search(input, OuMatches, rx)) {
    for (auto x:OuMatches) std::cout << x << " ";
    std::cout << std::endl;
    input = OuMatches.suffix().str();
  }
}
int main(int argc, char **argv) {
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
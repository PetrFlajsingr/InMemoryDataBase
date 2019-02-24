//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <FileDownloader.h>
#include <Utilities.h>
#include <fstream>

FileDownloader::FileDownloader(std::string_view downloadLocation)
    : downloadLocation(downloadLocation) {

}
bool FileDownloader::downloadFile(std::string_view fileName) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  std::string readBuffer;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, fileName);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    notifyDownloadStarted(fileName);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      notifyDownloadFailed(fileName, "dunno");
      return false;
    }

    auto parts = Utilities::splitStringByDelimiter(fileName, "/");
    auto name = parts.back();
    saveFile(readBuffer, downloadLocation + name);

    notifyDownloadFinished(fileName, downloadLocation + name);

    curl_easy_cleanup(curl);
  }
  return true;
}
void FileDownloader::addObserver(gsl::not_null<FileDownloadObserver *> observer) {
  observers.emplace_back(observer);
}
void FileDownloader::removeObserver(FileDownloadObserver *observer) {
  if (auto found = std::find(observers.begin(), observers.end(), observer);
      found != observers.end()) {
    observers.erase(found);
  }
}
void FileDownloader::notifyDownloadStarted(std::string_view fileName) {
  for (auto observer : observers) {
    observer->onDownloadStarted(fileName);
  }
}
void FileDownloader::notifyDownloadFailed(std::string_view fileName,
                                          std::string_view errorMessage) {
  for (auto observer : observers) {
    observer->onDownloadFailed(fileName, errorMessage);
  }
}
void FileDownloader::notifyDownloadFinished(std::string_view fileName,
                                            std::string_view filePath) {
  for (auto observer : observers) {
    observer->onDownloadFinished(fileName, filePath);
  }
}
size_t FileDownloader::WriteCallback(void *contents,
                                     size_t size,
                                     size_t nmemb,
                                     void *userp) {
  ((std::string *) userp)->append((char *) contents, size * nmemb);
  return size * nmemb;
}
void FileDownloader::saveFile(std::string_view content,
                              std::string_view location) {
  auto tmp = std::string(location);
  std::ofstream file(tmp);
  file << content;
}
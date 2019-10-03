//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <FileDownloader.h>
#include <Utilities.h>
#include <fstream>

FileDownloader::FileDownloader(std::string_view downloadLocation) : downloadLocation(downloadLocation) {

}
bool FileDownloader::downloadFile(std::string_view url) {
  CURL *curl;
  CURLcode res;

  // curl = curl_easy_init();
  std::string readBuffer;
  if (curl) {
    // curl_easy_setopt(curl, CURLOPT_URL, url);
    // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    auto parts = Utilities::splitStringByDelimiter(url, "/");
    auto name = parts.back();
    //saveFile(readBuffer, downloadLocation + name);
    FILE *fp;
    fp = fopen((downloadLocation + name).c_str(), "wb");

    // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    notifyDownloadStarted(url);
    // res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      notifyDownloadFailed(url, "dunno");
      fclose(fp);
      return false;
    }

    notifyDownloadFinished(url, downloadLocation + name);

    fclose(fp);
    // curl_easy_cleanup(curl);
  }
  return true;
}
void FileDownloader::addObserver(gsl::not_null<FileDownloadObserver *> observer) {
  observers.emplace_back(observer);
}
void FileDownloader::removeObserver(FileDownloadObserver *observer) {
  if (auto found = std::find(observers.begin(), observers.end(), observer); found != observers.end()) {
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
size_t FileDownloader::WriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written;
  written = fwrite(ptr, size, nmemb, stream);
  return written;
}
void FileDownloader::saveFile(std::string_view content, std::string_view location) {
  auto tmp = std::string(location);
  std::ofstream file(tmp);
  file << content;
}
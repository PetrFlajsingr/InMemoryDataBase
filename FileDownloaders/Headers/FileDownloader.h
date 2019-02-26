//
// Created by Petr Flajsingr on 19/10/2018.
//

#ifndef FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_
#define FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

#include <vector>
#include <string>
#include "Exceptions.h"
#include "FileDownloadObserver.h"
#include <curl/curl.h>

class FileDownloader {
 public:
  explicit FileDownloader(std::string_view downloadLocation);

  /**
   * Stazeni vybraneho souboru na disk. Cesta k ulozeni je urcena
   * v konstruktoru objektu.
   * @param fileName Nazev souboru ke stazeni
   */
  bool downloadFile(std::string_view fileName);

  void addObserver(gsl::not_null<FileDownloadObserver *> observer);

  void removeObserver(FileDownloadObserver *observer);

 protected:
  std::vector<FileDownloadObserver *> observers;
  std::string downloadLocation;

  void notifyDownloadStarted(std::string_view fileName);

  void notifyDownloadFailed(std::string_view fileName,
                            std::string_view errorMessage);

  void notifyDownloadFinished(std::string_view fileName,
                              std::string_view filePath);

  static size_t WriteCallback(void *ptr,
                              size_t size,
                              size_t nmemb,
                              FILE *stream);

  void saveFile(std::string_view content, std::string_view location);
};

#endif  // FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

//
// Created by Petr Flajsingr on 19/10/2018.
//

#ifndef FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_
#define FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

#include <vector>
#include <string>
#include "Exceptions.h"
#include "FileDownloadObserver.h"

/**
 * Zakladni trida pro stahovani souboru ze serveru.
 * Pripravuje metody pro:
 *  - Stazeni souboru (predpoklada se implementace v jinem vlakne)
 *  - Sledovani prubehu stahovani pomoci [observers]
 *
 * Pouziti:
 * class FINMFileDownloader : BaseFileDownloader {
 *  public:
 *   void downloadFile(size_t fileIndex) override;
 * };
 *
 * Je nutne implementovat:
 *  Nacteni nazvu dostupnych souboru do vector availableFiles (asi v konstruktoru).
 *  Implementace stazeni souboru na disk. Pri stahovani je nutne volat notify metody.
 *  Umisteni pro stazeni bude definovano v konstruktoru.
 *
 */
class BaseFileDownloader {
 public:
  explicit BaseFileDownloader(std::string_view downloadLocation)
      : downloadLocation(downloadLocation) {}

  virtual ~BaseFileDownloader() = default;

  /**
   *
   * @return Nazvy souboru dostupnych ke stazeni
   */
  virtual std::vector<std::string> getAvailableFiles() const {
    return availableFiles;
  }

  /**
   * Stazeni vybraneho souboru na disk. Cesta k ulozeni je urcena
   * v konstruktoru objektu.
   * @param fileName Nazev souboru ke stazeni
   */
  virtual void downloadFile(std::string_view fileName) {
    auto result = std::find(availableFiles.begin(),
                            availableFiles.end(),
                            fileName);

    if (result == availableFiles.end()) {
      throw InvalidArgumentException(
          ("The file " + fileName + " is not available").c_str());
    }
  }

  /**
   * Stazeni vybraneho souboru podle jeho vnitrniho indexu.
   * @param fileIndex Index souboru ke stazeni
   */
  virtual void downloadFile(size_t fileIndex) = 0;

  void addObserver(gsl::not_null<FileDownloadObserver *> observer) {
    observers.emplace_back(observer);
  }

  void removeObserver(FileDownloadObserver *observer) {
    if (auto found = std::find(observers.begin(), observers.end(), observer);
        found != observers.end()) {
      observers.erase(found);
    }
  }

 protected:
  std::vector<FileDownloadObserver *> observers;
  std::vector<std::string> availableFiles;
  std::string downloadLocation;

  void notifyDownloadStarted(std::string_view fileName) {
    for (auto observer : observers) {
      observer->onDownloadStarted(fileName);
    }
  }

  void notifyDownloadFailed(std::string_view fileName,
                            std::string_view errorMessage) {
    for (auto observer : observers) {
      observer->onDownloadFailed(fileName, errorMessage);
    }
  }

  void notifyDownloadFinished(std::string_view fileName,
                              std::string_view filePath) {
    for (auto observer : observers) {
      observer->onDownloadFinished(fileName, filePath);
    }
  }
};

#endif  // FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

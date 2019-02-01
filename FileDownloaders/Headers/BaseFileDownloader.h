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
 protected:
  std::vector<FileDownloadObserver*> observers;
  std::vector<std::string> availableFiles;
  std::string downloadLocation;

  void notifyDownloadStarted(const std::string &fileName) {
    for (auto observer : observers) {
      observer->onDownloadStarted(fileName);
    }
  }

  void notifyDownloadFailed(const std::string &fileName,
                            const std::string &errorMessage) {
    for (auto observer : observers) {
      observer->onDownloadFailed(fileName, errorMessage);
    }
  }

  void notifyDownloadFinished(const std::string &fileName,
                              const std::string &filePath) {
    for (auto observer : observers) {
      observer->onDownloadFinished(fileName, filePath);
    }
  }

 public:
  explicit BaseFileDownloader(const std::string &downloadLocation)
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
  virtual void downloadFile(const std::string &fileName) {
    auto result = std::find(availableFiles.begin(),
        availableFiles.end(),
        fileName);

    if (result == availableFiles.end()) {
      throw InvalidArgumentException(
          ("The file " + fileName + " is not available").c_str());
    }

    downloadFile(result - availableFiles.begin());
  }

  /**
   * Stazeni vybraneho souboru podle jeho vnitrniho indexu.
   * @param fileIndex Index souboru ke stazeni
   */
  virtual void downloadFile(size_t fileIndex) = 0;

  virtual void addObserver(FileDownloadObserver* observer) {
    observers.push_back(observer);
  }
};

#endif  // FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

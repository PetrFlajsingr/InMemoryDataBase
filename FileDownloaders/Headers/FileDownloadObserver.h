//
// Created by Petr Flajsingr on 19/10/2018.
//

#ifndef FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_
#define FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_

#include <string>

class FileDownloadObserver {
public:
  virtual void onDownloadStarted(std::string_view fileName) = 0;
  virtual void onDownloadFailed(std::string_view fileName, std::string_view errorMessage) = 0;
  virtual void onDownloadFinished(std::string_view fileName, std::string_view filePath) = 0;
};

#endif // FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_

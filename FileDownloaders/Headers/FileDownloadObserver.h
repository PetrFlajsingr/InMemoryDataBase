//
// Created by Petr Flajsingr on 19/10/2018.
//

#ifndef FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_
#define FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_

#include <string>

class FileDownloadObserver {
 public:
  virtual void onDownloadStarted(const std::string &fileName) = 0;

  virtual void onDownloadFailed(const std::string &fileName,
                            const std::string &errorMessage) = 0;

  virtual void onDownloadFinished(const std::string &fileName,
                              const std::string &filePath) = 0;
};

#endif  //FILEDOWNLOADERS_HEADERS_FILEDOWNLOADOBSERVER_H_

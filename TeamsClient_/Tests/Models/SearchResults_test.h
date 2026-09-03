#ifndef SEARCH_RESULTS_TEST_H
#define SEARCH_RESULTS_TEST_H

#include <QObject>

class SearchResultsTest : public QObject {
  Q_OBJECT

 private slots:
  void startsEmptyAndFindReturnsInvalidUser();
  void addUserExposesRolesAndGetMap();
  void setUsersReplacesContents();
  void removeAndClearUpdateModel();
  void searchLoadedReplacesResults();
};

#endif
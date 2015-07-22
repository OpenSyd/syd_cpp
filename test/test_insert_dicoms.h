
#include "sydDicomSerieBuilder.h"


void insert_dicoms(ext::ExtendedDatabase * db, syd::Injection::pointer injection, std::string folder) {

  int a = Log::LogLevel();
  Log::LogLevel() = 0;

  syd::DicomSerieBuilder b(db);
  b.SetInjection(injection);
  b.SetForcePatientFlag(true);
  OFList<OFString> files;
  b.SearchForFilesInFolder(folder, files);
  for(auto f:files) b.CreateDicomSerieFromFile(f.c_str());
  b.InsertDicomSeries();

  Log::LogLevel() = a;
}

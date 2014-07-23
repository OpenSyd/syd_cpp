/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

#ifndef SYDQUERY_H
#define SYDQUERY_H

// syd
#include "sydCommon.h"

// odb
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>

#include "sydPatient-odb.hxx"
#include "sydStudy-odb.hxx"
#include "sydRoiStudy-odb.hxx"
#include "sydRoiType-odb.hxx"
#include "sydRoiSerie-odb.hxx"
#include "sydSerie-odb.hxx"
#include "sydProperty-odb.hxx"

// clitk
#include <clitkCommon.h>
#include <clitkImageCommon.h>

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
namespace syd {

  class sydQuery {

  public:
    sydQuery();
    ~sydQuery();

    void SetVerboseFlag(bool b) { mVerboseFlag = b; }
    bool GetVerboseFlag() const { return mVerboseFlag; }

    void SetVerboseDBFlag(bool b) { mVerboseDBFlag = b; }
    bool GetVerboseDBFlag() const { return mVerboseDBFlag; }

    void SetVerboseQueryFlag(bool b) { mVerboseQueryFlag = b; }
    bool GetVerboseQueryFlag() const { return mVerboseQueryFlag; }

    void OpenDatabase();

    // Commands
    void ComputeRoiTimeActivity(RoiStudy roistudy);
    void ComputeRoiTimeActivity(Study study);
    void ComputeRoiCumulActivity(RoiStudy roistudy);
    void ComputeRoiInfo(RoiStudy r);
    void ComputeRoiInfo(Study study, std::string roiname);
    void ComputeTiming(Study study);
    void ComputeTiming(Study study, Serie serie);
    RoiStudy InsertRoi(Study study, std::string roiname, std::string mhd);
    void InsertCT(Study study, std::string mhd);
    Serie InsertSPECT(Study study, std::string dcm, std::string mhd);
    void DumpStudy(Study study);
    void DumpStudy2(Study study);
    void DumpCalibrationFactor(Study study);
    void DumpCalibrationFactor();

    // FIXME
    void DumpRoiInformation(unsigned long SynfrizzId, std::vector<std::string> roinames);
    void DumpRoiInformation(RoiStudy roistudy);

    // Call back for tracer
    void TraceCallback(const char* sql);

    // Convenient functions
    RoiStudy GetRoiStudy(Study study, std::string roiname);
    Study GetStudy(unsigned long SynfrizzId, unsigned long StudyNb);
    Study GetStudy(char ** inputs);
    void GetListOfPatients(std::string SynfrizzId, std::vector<unsigned long> & ids);
    void GetListOfRois(std::string roiname, std::vector<std::string> & rois);

    // Image type
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType, 3> MaskImageType;
    typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> StatisticsImageFilterType;

  protected:
    bool mVerboseFlag;
    bool mVerboseQueryFlag;
    bool mVerboseDBFlag;
    std::string mDatabaseFilename;
    std::string mDataPath;
    odb::sqlite::database * db;

    // convenients fct
    template<class T>
    void LoadVector(std::vector<T> & list, const odb::query<T> & q);
    template<class T>
    void Load(T & t, const odb::query<T> & q);

    // For tracing SQL queries
    std::string mCurrentSQLQuery;

  };

#include "sydQuery.txx"
} // end namespace

// --------------------------------------------------------------------
// http://stackoverflow.com/questions/1607368/sql-query-logging-for-sqlite
void trace_callback( void* udp, const char* sql );
// --------------------------------------------------------------------


// --------------------------------------------------------------------

#endif

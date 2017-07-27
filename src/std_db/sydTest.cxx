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

// syd
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydTableOfRecords.h"
#include "sydPrintTable.h"
#include "sydField.h"

//#include "sydTestTemp9.h"
#include "gdcmReader.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);
  // -----------------------------------------------------------------

  while (1) {
    DD("test");
    syd::File::vector files;
    db->Query<syd::File>(files);
    DD(files.size());
  }

  // std::string filename = args_info.inputs[0];
  // DD(filename);

  // gdcm::Reader RTreader;
  // RTreader.SetFileName(filename.c_str());
  // if (!RTreader.Read()) {
  //   std::cout << "Problem reading file: " << filename << std::endl;
  // }
  // const gdcm::DataSet& ds = RTreader.GetFile().GetDataSet();
  // gdcm::MediaStorage ms;
  // ms.SetFromFile(RTreader.GetFile());
  // // (3006,0020) SQ (Sequence with explicit length #=4)      # 370, 1 StructureSetROISequence  
  // gdcm::Tag tssroisq(0x3006,0x0020);
  // if( !ds.FindDataElement( tssroisq ) )
  //   {
  //     std::cout << "Problem locating 0x3006,0x0020 - Is this a valid RT Struct file?" << std::endl;
  //   }
  // gdcm::Tag troicsq(0x3006,0x0039);
  // if( !ds.FindDataElement( troicsq ) )
  //   {
  //     std::cout << "Problem locating 0x3006,0x0039 - Is this a valid RT Struct file?" << std::endl;
  //   }
  // DD("ok");




  // -----------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------

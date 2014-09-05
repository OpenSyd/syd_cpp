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
#include "syd_Dump_ROI_Activity_Command.h"

// clitk
#include <clitkResampleImageWithOptionsFilter.h>
#include <clitkCropLikeImageFilter.h>

// itk
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>

// --------------------------------------------------------------------
syd::Dump_ROI_Activity_Command::
Dump_ROI_Activity_Command(sydQuery & _db):Command(_db)
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Dump_ROI_Activity_Command::
SetArgs(char ** inputs, int n)
{
  // FIXME check nb of args

  // Get studies for all wanted patients
  db.GetStudies(inputs[0], "all", studies);
  // Get all wanted types of roi
  db.GetRoiTypes(inputs[1], roitypes);

  // Get parameters

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Dump_ROI_Activity_Command::
Run()
{
  // Get values to display
  int nb_row = studies.size();
  int nb_column = roitypes.size();
  std::vector<std::string> row_names(nb_row);
  std::vector<std::string> column_names(nb_column);
  std::vector<int> column_widths(nb_column);
  std::vector<unsigned long> row_ids(nb_row);
  std::vector<unsigned long> column_ids(nb_column);

  // Columns settings
  std::fill(column_widths.begin(), column_widths.end(), 14);
  for(auto j=0; j<nb_column; j++) {
    column_names[j] = roitypes[j].Name.substr(0,12); // 12 first char
    column_ids[j] = roitypes[j].Id;
  }

  // Row header (first column)
  for(auto i=0; i<nb_row; i++) {
    Patient p = db.GetById<Patient>(studies[i].PatientId);
    row_names[i] = toString<int>(p.SynfrizzId);
    row_ids[i] = studies[i].Id;
  }

  // table -> 2 list of Ids, 1 for row, 1 for column
  DumpSimpleTable t(nb_row, nb_column);
  t.set_row_ids(row_ids);
  t.set_column_ids(column_ids);
  t.set_row_names(row_names);
  t.set_column_names(column_names);
  t.set_title("Pat");
  t.set_precision(1);
  t.set_column_widths(5, column_widths);
  t.FillValues(new GetActivityFromRoiStudy(&db)); // with F taking 2 ids, loadin roitype etc

  std::ostream & os = std::cout;
  t.Dump(os);


}
// --------------------------------------------------------------------

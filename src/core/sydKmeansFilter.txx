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

// #include "itkWeightedCentroidKdTreeGenerator.h"
// #include "itkImageKmeansModelEstimator.h"

// --------------------------------------------------------------------
/*template<int N>
void syd::KmeansFilter::RunWithDim()
{
  // Create the initial list of samples
  typedef itk::Vector<double, N> VectorPixelType;
  typedef itk::Statistics::ListSample<VectorPixelType> SampleType;
  typename SampleType::Pointer sample = SampleType::New();
  sample->SetMeasurementVectorSize(N);
  for(auto p:points) {
    VectorPixelType v;
    for(auto i=0; i<N; i++) v[i] = p[i];
    sample->PushBack(v);
  }

  // kmeans kdtree
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< SampleType > TreeGeneratorType;
  typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  treeGenerator->SetSample(sample);
  treeGenerator->SetBucketSize(50);
  int n = treeGenerator->GetMeasurementVectorSize();
  treeGenerator->Update();
  //  treeGenerator->GetOutput()->PlotTree(std::cout);

  typedef typename TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  typename EstimatorType::Pointer estimator = EstimatorType::New();
  typename EstimatorType::ParametersType initialMeans(K*n);
  initialMeans.Fill(0.0f); // FIXME starting points ?
  for(auto i=0; i<K; i++) {
    for(auto j=0;j<n; j++)
      initialMeans[n*i+j] = 1.0/(double)K * (double)i;
  }
  DDS(initialMeans);

  estimator->SetParameters( initialMeans );
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 500 );
  estimator->SetCentroidPositionChangesThreshold(0);
  estimator->StartOptimization();

  // Copy estimated centers
  typename EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  centers = NDimPoints::New();
  centers->SetPointDimension(N);
  for(auto i=0 ; i<K ; i++) {
    double * p = new double[N];
    for(auto j=0; j<n; j++) p[j] = estimatedMeans[n*i+j];
    centers->push_back(p);
    DDV(p,N);
  }
}
*/
// --------------------------------------------------------------------

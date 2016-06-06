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
template<int N>
void syd::KmeansFilter::RunWithDim()
{
  DD(N);
  DD(points->size());

  // Create the initial list of samples
  typedef itk::Vector<double, N> VectorPixelType;
  typedef itk::Statistics::ListSample<VectorPixelType> SampleType;
  typename SampleType::Pointer sample = SampleType::New();
  sample->SetMeasurementVectorSize(N);
  for(auto p=points->begin(); p<points->end(); ++p) {
    VectorPixelType v;
    for(auto i=0; i<N; i++) v[i] = (*p)[i];
    sample->PushBack(v);
  }
  DD(sample->Size());

  // kmeans kdtree
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< SampleType > TreeGeneratorType;
  typename TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  treeGenerator->SetSample(sample);
  treeGenerator->SetBucketSize(5);
  std::cout << "Tree measurement vector size: " << treeGenerator->GetMeasurementVectorSize() << std::endl;
  int n = treeGenerator->GetMeasurementVectorSize();
  DD(n);
  treeGenerator->Update();
  DD("update done");

  typedef typename TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  typename EstimatorType::Pointer estimator = EstimatorType::New();
  typename EstimatorType::ParametersType initialMeans(K*n);
  initialMeans.Fill(0.0f);
  DD(n);
  //    for(auto i=0; i<nbClasses; i++) initialMeans[i] = i;
  DDS(initialMeans);
  estimator->SetParameters( initialMeans );
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 500 );
  estimator->SetCentroidPositionChangesThreshold(0);
  DD("before start");
  estimator->StartOptimization();

  typename EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  std::cout << "estimatedMeans: " << estimatedMeans << std::endl;

  centers = NDimPoints::New();
  centers->SetPointDimension(N);
  for(auto i=0 ; i<K ; i++) {
    double * p = new double[N];
    for(auto j=0; j<n; j++) p[j] = estimatedMeans[n*i+j];
    centers->push_back(p);
    DDV(p,N);
  }
}
// --------------------------------------------------------------------

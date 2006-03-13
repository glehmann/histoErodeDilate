#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkBasicMorphologicalGradientImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkSimpleFilterWatcher.h"


int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;
  kernel.SetRadius( 10 );
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit = 0;
    }
  SRType::Iterator kit=kernel.Begin();
  *kit = 1;
  *(++kit) = 1;
  
  typedef itk::MovingHistogramMorphologicalGradientImageFilter< IType, IType, SRType > MorphologicalGradientType;
  MorphologicalGradientType::Pointer hgradient = MorphologicalGradientType::New();
  hgradient->SetInput( reader->GetOutput() );
  hgradient->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(hgradient, "filter");

  typedef itk::BasicMorphologicalGradientImageFilter< IType, IType, SRType > HMorphologicalGradientType;
  HMorphologicalGradientType::Pointer gradient = HMorphologicalGradientType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetKernel( kernel );
  
  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( gradient->GetOutput() );
  writer->SetFileName( argv[2] );
  
  WriterType::Pointer hwriter = WriterType::New();
  hwriter->SetInput( hgradient->GetOutput() );
  hwriter->SetFileName( argv[3] );

  writer->Update();
  hwriter->Update();
  
  return 0;
}


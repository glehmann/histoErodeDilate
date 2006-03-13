#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMorphologicalGradientImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkCommand.h"
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
  kernel.SetRadius( 1 );
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit = 0;
    }
  SRType::Iterator kit=kernel.Begin();
  *kit = 1;
  *(++kit) = 1;
  
  typedef itk::MorphologicalGradientImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer gradient = DilateType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(gradient, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( gradient->GetOutput() );

  gradient->SetNameOfBackendFilterClass("BasicMorphologicalGradientImageFilter");
  writer->SetFileName( argv[2] );
  writer->Update();
  
  gradient->SetNameOfBackendFilterClass("MovingHistogramMorphologicalGradientImageFilter");
  writer->SetFileName( argv[3] );
  writer->Update();
  
  try
    {
    gradient->SetNameOfBackendFilterClass("a wrong class name");
    return EXIT_FAILURE;
    }
  catch (itk::ExceptionObject & e)
    {}  
  
  return 0;
}


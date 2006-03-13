#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"


int main(int, char * argv[])
{
  const int dim = 3;
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
  
  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer hdilate = DilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  hdilate->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(hdilate, "filter");

  typedef itk::BasicDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer dilate = HDilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );
  
  WriterType::Pointer hwriter = WriterType::New();
  hwriter->SetInput( hdilate->GetOutput() );
  hwriter->SetFileName( argv[3] );

  writer->Update();
  hwriter->Update();
  
  return 0;
}


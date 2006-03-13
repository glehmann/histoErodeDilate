#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkBasicErodeImageFilter.h"
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
  
  typedef itk::MovingHistogramErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer herode = ErodeType::New();
  herode->SetInput( reader->GetOutput() );
  herode->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(herode, "filter");

  typedef itk::BasicErodeImageFilter< IType, IType, SRType > HErodeType;
  HErodeType::Pointer erode = HErodeType::New();
  erode->SetInput( reader->GetOutput() );
  erode->SetKernel( kernel );
  
  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( erode->GetOutput() );
  writer->SetFileName( argv[2] );
  
  WriterType::Pointer hwriter = WriterType::New();
  hwriter->SetInput( herode->GetOutput() );
  hwriter->SetFileName( argv[3] );

  writer->Update();
  hwriter->Update();
  
  return 0;
}


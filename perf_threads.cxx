#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkTimeProbe.h"
#include <vector>

int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBallStructuringElement< PType, dim > SRType;
  SRType kernel;
  kernel.SetRadius( 4 );
  kernel.CreateStructuringElement();
  
  typedef itk::HistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  hdilate->SetKernel( kernel );
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
/*  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );*/
  
  reader->Update();
  
  for( int t=1; t<=4; t++ )
    {
    itk::TimeProbe time;
    itk::TimeProbe htime;
  
    hdilate->SetNumberOfThreads( t );
    hdilate->SetNumberOfThreads( t );
    
    for( int i=0; i<50; i++ )
      {
      time.Start();
      dilate->Update();
      time.Stop();
      
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      dilate->Modified();
      hdilate->Modified();
      }
      
    std::cout << t << "\t" << time.GetMeanTime() << "\t" << htime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


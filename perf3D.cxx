#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkTimeProbe.h"
#include <vector>

int main(int, char * argv[])
{
  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBallStructuringElement< PType, dim > SRType;
  
  typedef itk::HistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  hdilate->SetNumberOfThreads( 1 );
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetNumberOfThreads( 1 );
  
/*  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );*/
  
  reader->Update();
  
  std::vector< int > radiusList;
  for( int s=1; s<=10; s++)
    { radiusList.push_back( s ); }
  radiusList.push_back( 15 );
  radiusList.push_back( 20 );
  
  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe time;
    itk::TimeProbe htime;
  
    SRType kernel;
    kernel.SetRadius( *it );
    kernel.CreateStructuringElement();
  
    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
    for( int i=0; i<10; i++ )
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
      
    std::cout << *it << "\t" << time.GetMeanTime() << "\t" << htime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


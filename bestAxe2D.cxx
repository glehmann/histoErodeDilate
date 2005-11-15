#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkNeighborhood.h"
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
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;
  
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
  
  long radMax = 20;
 
  for( int x=0; x<=radMax; x++)
    {
    itk::TimeProbe time;
    itk::TimeProbe htime;
    
    SRType::RadiusType radius;
    radius[0] = x;
    // compute radius y
    long y = ((2*radMax+1)*(2*radMax+1) / (2*x+1) - 1) / 2;
    radius[1] = y;

    kernel.SetRadius( radius );

    for( SRType::Iterator it=kernel.Begin(); it!=kernel.End(); it++ )
      {
      *it = 1;
      }
  
    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
    for( int i=0; i<5; i++ )
      {
      time.Start();
      //dilate->Update();
      time.Stop();
      
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      dilate->Modified();
      hdilate->Modified();
      }
      
    std::cout << x<<"\tx "<<y << "\t" << htime.GetMeanTime() << std::endl;
    }
  
  
  for( int y=radMax-1; y>=0; y--)
    {
    itk::TimeProbe time;
    itk::TimeProbe htime;
    
    SRType::RadiusType radius;
    radius[1] = y;
    // compute radius x
    long x = ((2*radMax+1)*(2*radMax+1) / (2*y+1) - 1) / 2;
    radius[0] = x;

    kernel.SetRadius( radius );

    for( SRType::Iterator it=kernel.Begin(); it!=kernel.End(); it++ )
      {
      *it = 1;
      }
  
    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
    for( int i=0; i<5; i++ )
      {
      time.Start();
      //dilate->Update();
      time.Stop();
      
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      dilate->Modified();
      hdilate->Modified();
      }
      
    std::cout << x<<"\tx "<<y << "\t" << htime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkMultiThreader.h"
#include <iomanip>

int main(int, char * argv[])
{
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);

  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;
  
  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
/*  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );*/
  
  reader->Update();
  
  long radMax = 20;
 
  std::cout << "#rx" << "\t" 
    << "ry" << "\t"
    << "n" << "\t"
    << "p/t" << "\t"
    << "hd" << std::endl;

  for( int x=0; x<=radMax; x++)
    {
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
  
    hdilate->SetKernel( kernel );
    
    for( int i=0; i<5; i++ )
      {
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      hdilate->Modified();
      }
      
    std::cout << std::setprecision(3)
      << x << "\t"
      << y << "\t"
      << (2*x+1)*(2*y+1) << "\t"
      << hdilate->GetPixelsPerTranslation() << "\t"
      << htime.GetMeanTime() << std::endl;
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
  
    hdilate->SetKernel( kernel );
    
    for( int i=0; i<5; i++ )
      {
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      hdilate->Modified();
      }
      
    std::cout << std::setprecision(3)
      << x << "\t"
      << y << "\t"
      << (2*x+1)*(2*y+1) << "\t"
      << hdilate->GetPixelsPerTranslation() << "\t"
      << htime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


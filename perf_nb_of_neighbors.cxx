#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkMultiThreader.h"

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
  
  typedef itk::BasicDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  
  reader->Update();

  int radius = 3;
  int total = (radius*2+1)*(radius*2+1);

  
  std::cout << "#radius" << "\t" 
            << "rep" << "\t" 
            << "total" << "\t" 
            << "nb" << "\t" 
            << "hnb" << "\t" 
            << "d" << "\t" 
            << "hd" << std::endl;

  for( int s=1; s<=total; s++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe hdtime;

    kernel.SetRadius( radius );
    unsigned long nbOfNeighbors = 0;
    for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
      {
      if( nbOfNeighbors <= s )
        { *kit = 1; }
      else
        { *kit = 0; }
      nbOfNeighbors++;
      }
    *kernel.Begin() = 1;

    // compute the number of activated neighbors in the structuring element
  
    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );

    int nbOfRepeats = 5;

    for( int i=0; i<nbOfRepeats; i++ )
      {
      dtime.Start();
      dilate->Update();
      dtime.Stop();
      dilate->Modified();
      hdtime.Start();
      hdilate->Update();
      hdtime.Stop();
      hdilate->Modified();
      }
      
    std::cout << radius << "\t" 
              << nbOfRepeats << "\t" 
              << total << "\t" 
              << s << "\t"
              << hdilate->GetPixelsPerTranslation() << "\t" 
              << dtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


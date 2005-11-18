#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkHistogramErodeImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkHistogramMorphologicalGradientImageFilter.h"
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
  
  typedef itk::HistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  
  reader->Update();
  
  std::vector< int > radiusList;
  for( int s=1; s<=10; s++)
    { radiusList.push_back( s ); }
  for( int s=15; s<=30; s+=5)
    { radiusList.push_back( s ); }
  radiusList.push_back( 40 );
  radiusList.push_back( 50 );
  radiusList.push_back( 100 );
  
  std::cout << "#radius" << "\t" 
            << "rep" << "\t" 
            << "total" << "\t" 
            << "nb" << "\t" 
            << "hnb" << "\t" 
            << "d" << "\t" 
            << "hd" << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe hdtime;

    kernel.SetRadius( *it );
    for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
      {
      *kit = 0;
      }
    *kernel.Begin() = 1;

    // compute the number of activated neighbors in the structuring element
    unsigned long nbOfNeighbors = 1;
  
    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );

    int nbOfRepeats;
    if( *it <= 10 )
      { nbOfRepeats = 20; }
    else if( *it <= 20 )
      { nbOfRepeats = 10; }
    else if( *it <= 50 )
      { nbOfRepeats = 5; }
    else
      { nbOfRepeats = 2; }
    //nbOfRepeats = 1;

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
      
    std::cout << *it << "\t" 
              << nbOfRepeats << "\t" 
              << (*it*2+1)*(*it*2+1) << "\t" 
              << nbOfNeighbors << "\t"
              << hdilate->GetPixelsPerTranslation() << "\t" 
              << dtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkHistogramErodeImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkHistogramMorphologicalGradientImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"
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
  
  typedef itk::HistogramErodeImageFilter< IType, IType, SRType > HErodeType;
  HErodeType::Pointer herode = HErodeType::New();
  herode->SetInput( reader->GetOutput() );
  herode->SetNumberOfThreads( 1 );
  
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( reader->GetOutput() );
  erode->SetNumberOfThreads( 1 );
  
  typedef itk::HistogramMorphologicalGradientImageFilter< IType, IType, SRType > HMorphologicalGradientType;
  HMorphologicalGradientType::Pointer hgradient = HMorphologicalGradientType::New();
  hgradient->SetInput( reader->GetOutput() );
  hgradient->SetNumberOfThreads( 1 );
  
  typedef itk::MorphologicalGradientImageFilter< IType, IType, SRType > MorphologicalGradientType;
  MorphologicalGradientType::Pointer gradient = MorphologicalGradientType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetNumberOfThreads( 1 );
  
  reader->Update();
  
  std::vector< int > radiusList;
  for( int s=1; s<=10; s++)
    { radiusList.push_back( s ); }
  radiusList.push_back( 15 );
  radiusList.push_back( 20 );
  
  std::cout << "radius" << "\t" << "nb" << "\t" << "d" << "\t" << "hd" << "\t" << "e" << "\t" << "he" << "\t" << "g" << "\t" << "hg" << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe hdtime;

    itk::TimeProbe etime;
    itk::TimeProbe hetime;
  
    itk::TimeProbe gtime;
    itk::TimeProbe hgtime;
  
    SRType kernel;
    kernel.SetRadius( *it );
    kernel.CreateStructuringElement();
  
    // compute the number of activated neighbors in the structuring element
    unsigned long nbOfNeighbors = 0;
    for( SRType::Iterator nit=kernel.Begin(); nit!=kernel.End(); nit++ )
      {
      if( *nit > 0 )
        { nbOfNeighbors++; }
      }


    dilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
    erode->SetKernel( kernel );
    herode->SetKernel( kernel );
    
    gradient->SetKernel( kernel );
    hgradient->SetKernel( kernel );

    int nbOfRepeats;
    if( *it <= 5 )
      { nbOfRepeats = 10; }
    else if( *it <= 10 )
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
      
      etime.Start();
      erode->Update();
      etime.Stop();
      erode->Modified();
      hetime.Start();
      herode->Update();
      hetime.Stop();
      herode->Modified();
      
      gtime.Start();
      gradient->Update();
      gtime.Stop();
      gradient->Modified();
      hgtime.Start();
      hgradient->Update();
      hgtime.Stop();
      hgradient->Modified();
      }
      
    std::cout << *it << "\t" << nbOfNeighbors << "\t" << dtime.GetMeanTime() << "\t" << hdtime.GetMeanTime() << "\t" << etime.GetMeanTime() << "\t" << hetime.GetMeanTime()<< "\t" << gtime.GetMeanTime() << "\t" << hgtime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}


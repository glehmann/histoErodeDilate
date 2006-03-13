#include "itkImageFileReader.h"

#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"

#include "itkMovingHistogramErodeImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"

#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkBasicMorphologicalGradientImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkMultiThreader.h"
#include <iomanip>

int main(int, char * argv[])
{
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);

  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBallStructuringElement< PType, dim > SRType;
  
  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
  typedef itk::BasicDilateImageFilter< IType, IType, SRType > BDilateType;
  BDilateType::Pointer bdilate = BDilateType::New();
  bdilate->SetInput( reader->GetOutput() );
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );

  
  typedef itk::MovingHistogramErodeImageFilter< IType, IType, SRType > HErodeType;
  HErodeType::Pointer herode = HErodeType::New();
  herode->SetInput( reader->GetOutput() );
  
  typedef itk::BasicErodeImageFilter< IType, IType, SRType > BErodeType;
  BErodeType::Pointer berode = BErodeType::New();
  berode->SetInput( reader->GetOutput() );
  
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( reader->GetOutput() );

  
  typedef itk::MovingHistogramMorphologicalGradientImageFilter< IType, IType, SRType > HMorphologicalGradientType;
  HMorphologicalGradientType::Pointer hgradient = HMorphologicalGradientType::New();
  hgradient->SetInput( reader->GetOutput() );
  
  typedef itk::MorphologicalGradientImageFilter< IType, IType, SRType > MorphologicalGradientType;
  MorphologicalGradientType::Pointer gradient = MorphologicalGradientType::New();
  gradient->SetInput( reader->GetOutput() );
  
  typedef itk::BasicMorphologicalGradientImageFilter< IType, IType, SRType > BMorphologicalGradientType;
  BMorphologicalGradientType::Pointer bgradient = BMorphologicalGradientType::New();
  bgradient->SetInput( reader->GetOutput() );
  
  reader->Update();
  
  std::vector< int > radiusList;
  for( int s=0; s<=10; s++)
    { radiusList.push_back( s ); }
  radiusList.push_back( 15 );
  radiusList.push_back( 20 );
  
  std::cout << std::setprecision(3) << "#radius" << "\t" 
            << "rep" << "\t" 
            << "total" << "\t" 
            << "nb" << "\t" 
            << "hnb" << "\t" 
            << "bd" << "\t" 
            << "hd" << "\t" 
            << "d" << "\t" 
            << "be" << "\t" 
            << "he" << "\t" 
            << "e" << "\t" 
            << "bg" << "\t" 
            << "hg" << "\t" 
            << "g" << "\t" 
            << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe bdtime;
    itk::TimeProbe hdtime;

    itk::TimeProbe etime;
    itk::TimeProbe hetime;
    itk::TimeProbe betime;
  
    itk::TimeProbe gtime;
    itk::TimeProbe bgtime;
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
    bdilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
    erode->SetKernel( kernel );
    berode->SetKernel( kernel );
    herode->SetKernel( kernel );
    
    gradient->SetKernel( kernel );
    bgradient->SetKernel( kernel );
    hgradient->SetKernel( kernel );

    int nbOfRepeats;
    if( *it <= 3 )
      { nbOfRepeats = 5; }
    else if( *it <= 5 )
      { nbOfRepeats = 2; }
    else
      { nbOfRepeats = 1; }
    //nbOfRepeats = 1;

    for( int i=0; i<nbOfRepeats; i++ )
      {
      dtime.Start();
      dilate->Update();
      dtime.Stop();
      dilate->Modified();
      bdtime.Start();
      bdilate->Update();
      bdtime.Stop();
      bdilate->Modified();
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
      betime.Start();
      berode->Update();
      betime.Stop();
      berode->Modified();
      
      gtime.Start();
      gradient->Update();
      gtime.Stop();
      gradient->Modified();
      bgtime.Start();
      bgradient->Update();
      bgtime.Stop();
      bgradient->Modified();
      hgtime.Start();
      hgradient->Update();
      hgtime.Stop();
      hgradient->Modified();
      }
      
    std::cout << *it << "\t" 
              << nbOfRepeats << "\t" 
              << (*it*2+1)*(*it*2+1)*(*it*2+1) << "\t" 
              << nbOfNeighbors << "\t"
              << hdilate->GetPixelsPerTranslation() << "\t" 
              << bdtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << "\t" 
              << dtime.GetMeanTime() << "\t" 
              << betime.GetMeanTime() << "\t" 
              << hetime.GetMeanTime()<< "\t" 
              << etime.GetMeanTime()<< "\t" 
              << bgtime.GetMeanTime() << "\t" 
              << hgtime.GetMeanTime() << "\t"
              << gtime.GetMeanTime() << "\t" 
//<< dilate->GetNameOfBackendFilterClass()
              <<std::endl;
    }
  
  
  return 0;
}


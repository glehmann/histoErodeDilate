#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkCropImageFilter.h"
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
  
  typedef itk::CropImageFilter< IType, IType > CropType;
  CropType::Pointer crop = CropType::New();
  crop->SetInput( reader->GetOutput() );
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;
  kernel.SetRadius( 5 );
  unsigned long nbOfNeighbors = 0;
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    { *kit = 1; }
  
  typedef itk::HistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( crop->GetOutput() );
  hdilate->SetKernel( kernel );
  
  typedef itk::BasicDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( crop->GetOutput() );
  dilate->SetKernel( kernel );
  
  reader->Update();
  
  std::cout << "#radius" << "\t" 
            << "rep" << "\t" 
            << "total" << "\t" 
            << "nb" << "\t" 
            << "hnb" << "\t" 
            << "im size" << "\t" 
            << "d" << "\t" 
            << "hd" << std::endl;

  IType::SizeType imSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  IType::SizeType size = imSize;

  for( int i=0; i<=25; i++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe hdtime;

    IType::SizeType cropSize = imSize - size;
    for(int j=0; j<dim; j++)
      { cropSize[j] = cropSize[j] / 2; }
    crop->SetBoundaryCropSize( cropSize );

    int nbOfRepeats = 5;

    for( int i=0; i<nbOfRepeats; i++ )
      {
      dtime.Start();
      dilate->UpdateLargestPossibleRegion();
      dtime.Stop();
      dilate->Modified();

      hdtime.Start();
      hdilate->UpdateLargestPossibleRegion();
      hdtime.Stop();
      hdilate->Modified();
      }
      
    unsigned long imageSize = 1;
    for( int j=0; j<dim; j++)
      { imageSize *= crop->GetOutput()->GetLargestPossibleRegion().GetSize()[j]; }

    std::cout << 5 << "\t" 
              << nbOfRepeats << "\t" 
              << 121 << "\t" 
              << 121 << "\t"
              << hdilate->GetPixelsPerTranslation() << "\t" 
              << imageSize << "\t" 
              << dtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << std::endl;

    for(int j=0; j<dim; j++)
      { size[j] = static_cast<unsigned long>(size[j] * 0.85f); }
    }
  
    
  
  return 0;
}


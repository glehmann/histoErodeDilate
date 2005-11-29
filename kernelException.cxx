#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkNeighborhood.h"

int main(int, char * argv[])
{
  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;
  kernel.SetRadius( 1 );
  // build an empty kernel
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit = 0;
    }

  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  
  typedef itk::MovingHistogramErodeImageFilter< IType, IType, SRType > HErodeType;
  HErodeType::Pointer herode = HErodeType::New();
  
  typedef itk::MovingHistogramMorphologicalGradientImageFilter< IType, IType, SRType > HMorphologicalGradientType;
  HMorphologicalGradientType::Pointer hgradient = HMorphologicalGradientType::New();
  
  try
    {
    hdilate->SetKernel( kernel );
    return EXIT_FAILURE;
    }
  catch (itk::ExceptionObject & e)
    {}  

  try
    {
    herode->SetKernel( kernel );
    return EXIT_FAILURE;
    }
  catch (itk::ExceptionObject & e)
    {}  

  try
    {
    hgradient->SetKernel( kernel );
    return EXIT_FAILURE;
    }
  catch (itk::ExceptionObject & e)
    {}  

  
  return 0;
}


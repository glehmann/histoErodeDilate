#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkConstantBoundaryCondition.h"

int main(int, char * argv[])
{
  const int dim = 3;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  typedef itk::Neighborhood<PType, dim> SRType;
  SRType kernel;

  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();

  typedef itk::BasicDilateImageFilter< IType, IType, SRType > BDilateType;
  BDilateType::Pointer bdilate = BDilateType::New();
  
  typedef itk::BasicErodeImageFilter< IType, IType, SRType > BErodeType;
  BErodeType::Pointer berode = BErodeType::New();

  typedef itk::ConstantBoundaryCondition< IType > ConditionType;
  ConditionType condition;
  condition.SetConstant( 12 );

  PType dilateDefault = dilate->GetBoundary();
  PType erodeDefault = erode->GetBoundary();
  
  // to be sure that default values are the same
  if( dilateDefault != dynamic_cast<ConditionType*>( bdilate->GetBoundaryCondition() )->GetConstant() )
    throw "Invalid boundary value";
  if( erodeDefault != dynamic_cast<ConditionType*>( berode->GetBoundaryCondition() )->GetConstant() )
    throw "Invalid boundary value";

  dilate->OverrideBoundaryCondition( &condition );
  erode->OverrideBoundaryCondition( &condition );
  
  if( 12 != dilate->GetBoundary() )
    throw "Invalid boundary value";
  if( 12 != erode->GetBoundary() )
    throw "Invalid boundary value";

  dilate->GetBoundaryCondition();
  erode->GetBoundaryCondition();
  
  dilate->ResetBoundaryCondition();
  erode->ResetBoundaryCondition();
  
  if( dilateDefault != dilate->GetBoundary() )
    throw "Invalid boundary value";
  if( erodeDefault != erode->GetBoundary() )
    throw "Invalid boundary value";

  return 0;
}


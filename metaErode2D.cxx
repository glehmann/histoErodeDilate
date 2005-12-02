#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkNeighborhood.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkCommand.h"

template < class TFilter >
class ProgressCallback : public itk::Command
{
public:
  typedef ProgressCallback   Self;
  typedef itk::Command  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkTypeMacro( IterationCallback, Superclass );
  itkNewMacro( Self );

  /** Type defining the optimizer. */
  typedef    TFilter     FilterType;

  /** Method to specify the optimizer. */
  void SetFilter( FilterType * filter )
    {
    m_Filter = filter;
    m_Filter->AddObserver( itk::ProgressEvent(), this );
    }

  /** Execute method will print data at each iteration */
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object *, const itk::EventObject & event)
    {
    std::cout << m_Filter->GetNameOfClass() << ": " << m_Filter->GetProgress() << std::endl;
    }

protected:
  ProgressCallback() {};
  itk::WeakPointer<FilterType>   m_Filter;
};

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
  kernel.SetRadius( 1 );
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit = 0;
    }
  SRType::Iterator kit=kernel.Begin();
  *kit = 1;
  *(++kit) = 1;
  
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( reader->GetOutput() );
  erode->SetKernel( kernel );
  
  typedef ProgressCallback< ErodeType > ProgressType;
  ProgressType::Pointer progress = ProgressType::New();
  progress->SetFilter(erode);

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( erode->GetOutput() );

  erode->SetNameOfBackendFilterClass("BasicErodeImageFilter");
  writer->SetFileName( argv[2] );
  writer->Update();
  
  erode->SetNameOfBackendFilterClass("MovingHistogramErodeImageFilter");
  writer->SetFileName( argv[3] );
  writer->Update();
  
  try
    {
    erode->SetNameOfBackendFilterClass("a wrong class name");
    return EXIT_FAILURE;
    }
  catch (itk::ExceptionObject & e)
    {}  
  
  return 0;
}


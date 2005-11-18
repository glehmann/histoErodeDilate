#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramDilateImageFilter.h"
#include "itkGrayscaleDilateImageFilter.h"
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
  kernel.SetRadius( 10 );
  for( SRType::Iterator kit=kernel.Begin(); kit!=kernel.End(); kit++ )
    {
    *kit = 0;
    }
  *kernel.Begin() = 1;
  
  typedef itk::HistogramDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer hdilate = DilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  hdilate->SetKernel( kernel );
  
  typedef ProgressCallback< DilateType > ProgressType;
  ProgressType::Pointer progress = ProgressType::New();
  progress->SetFilter(hdilate);

  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer dilate = HDilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );
  
  WriterType::Pointer hwriter = WriterType::New();
  hwriter->SetInput( hdilate->GetOutput() );
  hwriter->SetFileName( argv[3] );

  writer->Update();
  hwriter->Update();
  
  return 0;
}


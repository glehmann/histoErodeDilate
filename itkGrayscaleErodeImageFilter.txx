/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayscaleErodeImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleErodeImageFilter_txx
#define __itkGrayscaleErodeImageFilter_txx

#include "itkGrayscaleErodeImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressAccumulator.h"
#include <string>

namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::GrayscaleErodeImageFilter()
  : m_Kernel()
{
  m_BasicFilter = BasicFilterType::New();
  m_HistogramFilter = HistogramFilterType::New();
  m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass();

  this->SetBoundary( itk::NumericTraits< PixelType >::max() );
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = 
    const_cast< TInputImage * >( this->GetInput() );
  
  if ( !inputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Kernel.GetRadius() );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    OStringStream msg;
    msg << static_cast<const char *>(this->GetNameOfClass())
        << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetKernel( const KernelType& kernel )
{
  m_HistogramFilter->SetKernel( kernel );
  m_BasicFilter->SetKernel( kernel );

  m_Kernel = kernel;
  // Delegate to a dilate filter.
  if( m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 5.4 )
    { m_NameOfBackendFilterClass = m_BasicFilter->GetNameOfClass(); }
  else
    { m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass(); }

}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetBoundary( const PixelType value )
{
  m_Boundary = value;
  m_HistogramFilter->SetBoundary( value );
  
  m_BoundaryCondition.SetConstant( value );
  m_BasicFilter->OverrideBoundaryCondition( &m_BoundaryCondition );
}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetNameOfBackendFilterClass( const char * name )
{
  if( strcmp( m_NameOfBackendFilterClass, name ) )
    {
    if( !strcmp( name, m_BasicFilter->GetNameOfClass() ) )
      { m_NameOfBackendFilterClass = m_BasicFilter->GetNameOfClass(); }
    else if( !strcmp( name, m_HistogramFilter->GetNameOfClass() ) )
      { m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass(); }
    else
      { itkExceptionMacro( << "Invalid name of class." ); }
    this->Modified();
    }
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateData() 
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();

  // Delegate to a dilate filter.
  if( !strcmp( m_NameOfBackendFilterClass, m_BasicFilter->GetNameOfClass() ) )
    {
    m_BasicFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicFilter, 1.0f );
    
    m_BasicFilter->GraftOutput( this->GetOutput() );
    m_BasicFilter->Update();
    this->GraftOutput( m_BasicFilter->GetOutput() );
    }
  else
    {
    m_HistogramFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_HistogramFilter, 1.0f );
    
    m_HistogramFilter->GraftOutput( this->GetOutput() );
    m_HistogramFilter->Update();
    this->GraftOutput( m_HistogramFilter->GetOutput() );
    }

}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::Modified() const
{
  Superclass::Modified();
  m_BasicFilter->Modified();
  m_HistogramFilter->Modified();
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
  os << indent << "Boundary: " <<  static_cast<typename NumericTraits<PixelType>::PrintType>( m_Boundary ) << std::endl;
  os << indent << "NameOfBackendFilterClass: " << m_NameOfBackendFilterClass << std::endl;
}

}// end namespace itk
#endif

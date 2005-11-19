/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBasicMorphologicalGradientImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:16:38 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBasicMorphologicalGradientImageFilter_txx
#define __itkBasicMorphologicalGradientImageFilter_txx

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkBasicMorphologicalGradientImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkProgressAccumulator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"


namespace itk {

template <class TInputImage, class TOutputImage, class TKernel>
BasicMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::BasicMorphologicalGradientImageFilter()
  : m_Kernel()
{
}

template <class TInputImage, class TOutputImage, class TKernel>
void 
BasicMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
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


template <class TInputImage, class TOutputImage, class TKernel>
void 
BasicMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateData()
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();
  
  // Delegate to a dilate filter.
  typename BasicDilateImageFilter<TInputImage, TInputImage, TKernel>::Pointer
    dilate = BasicDilateImageFilter<TInputImage, TInputImage, TKernel>::New();
  dilate->SetInput( this->GetInput() );
  dilate->SetKernel(this->m_Kernel);
  dilate->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(dilate,.45f);
  
  // Delegate to an erode filter.
  typename BasicErodeImageFilter<TInputImage, TInputImage, TKernel>::Pointer
    erode = BasicErodeImageFilter<TInputImage, TInputImage, TKernel>::New();
  erode->SetInput( this->GetInput() );
  erode->SetKernel(this->m_Kernel);
  erode->SetNumberOfThreads( this->GetNumberOfThreads() );
  progress->RegisterInternalFilter(erode,.45f);
  
  // Need to subtract the eroded image from the dialted one
  typename SubtractImageFilter<TInputImage, TInputImage, TOutputImage>::Pointer
    subtract=SubtractImageFilter<TInputImage,TInputImage,TOutputImage>::New();
  subtract->SetInput1( dilate->GetOutput() );
  subtract->SetInput2( erode->GetOutput() );
  subtract->SetNumberOfThreads( this->GetNumberOfThreads() );

  // graft our output to the subtract filter to force the proper regions
  // to be generated
  subtract->GraftOutput( this->GetOutput() );

  // run the algorithm
  progress->RegisterInternalFilter(subtract,.1f);

  subtract->Update();

  // graft the output of the subtract filter back onto this filter's
  // output. this is needed to get the appropriate regions passed
  // back.
  this->GraftOutput( subtract->GetOutput() );

}

template<class TInputImage, class TOutputImage, class TKernel>
void
BasicMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
}

}// end namespace itk
#endif

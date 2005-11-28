/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHistogramMorphologicalGradientImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHistogramMorphologicalGradientImageFilter_txx
#define __itkHistogramMorphologicalGradientImageFilter_txx

#include "itkHistogramMorphologicalGradientImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"


namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::HistogramMorphologicalGradientImageFilter()
  : m_Kernel()
{
  m_PixelsPerTranslation = 0;
}

template<class TInputImage, class TOutputImage, class TKernel>
void
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
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
HistogramMorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel>
::SetKernel( const KernelType& kernel )
{
  // first, build the list of offsets of added and removed pixels when the 
  // structuring element move of 1 pixel on 1 axe; do it for the 2 directions
  // on each axes.
  
  // transform the structuring element in an image for an easier
  // access to the data
  typedef Image< bool, TInputImage::ImageDimension > BoolImageType;
  typename BoolImageType::Pointer tmpSEImage = BoolImageType::New();
  tmpSEImage->SetRegions( kernel.GetSize() );
  tmpSEImage->Allocate();
  RegionType tmpSEImageRegion = tmpSEImage->GetRequestedRegion();
  ImageRegionIteratorWithIndex<BoolImageType> kernelImageIt;
  kernelImageIt = ImageRegionIteratorWithIndex<BoolImageType>(tmpSEImage, tmpSEImageRegion);
  kernelImageIt.GoToBegin();
  KernelIteratorType kernel_it = kernel.Begin();
  OffsetListType kernelOffsets;

  // create a center index to compute the offset
  IndexType centerIndex;
  for( int axe=0; axe<ImageDimension; axe++)
    { centerIndex[axe] = kernel.GetSize()[axe] / 2; }
  
  unsigned long count = 0;
  while( !kernelImageIt.IsAtEnd() )
    {
    kernelImageIt.Set( *kernel_it > 0 );
    if( *kernel_it > 0 )
      {
      kernelImageIt.Set( true );
      kernelOffsets.push_front( kernelImageIt.GetIndex() - centerIndex );
      count++;
      }
    else
      { kernelImageIt.Set( false ); }
    ++kernelImageIt;
    ++kernel_it;
    }
    

  // verify that the kernel contain at least one point
  if( count == 0 )
    { itkExceptionMacro( << "The kernel must contain at least one point." ); }

  // no attribute should be modified before here to avoid setting the filter in a bad status
  // store the kernel !!
  m_Kernel = kernel;

  // clear the already stored values
  m_AddedOffsets.clear();
  m_RemovedOffsets.clear();
  //m_Axes

  // store the kernel offset list
  m_KernelOffsets = kernelOffsets;

  typename itk::FixedArray< unsigned long, ImageDimension > axeCount;
  axeCount.Fill( 0 );

  for( int axe=0; axe<ImageDimension; axe++)
    {
    OffsetType refOffset;
    refOffset.Fill( 0 );
    for( int direction=-1; direction<=1; direction +=2)
      {
      refOffset[axe] = direction;
      for( kernelImageIt.GoToBegin(); !kernelImageIt.IsAtEnd(); ++kernelImageIt)
        {
        IndexType idx = kernelImageIt.GetIndex();
        
        if( kernelImageIt.Get() )
          {
          // search for added pixel during a translation
          IndexType nextIdx = idx + refOffset;
          if( tmpSEImageRegion.IsInside( nextIdx ) )
            {
            if( !tmpSEImage->GetPixel( nextIdx ) )
              {
                m_AddedOffsets[refOffset].push_front( nextIdx - centerIndex );
                axeCount[axe]++;
              }
            }
          else
            {
              m_AddedOffsets[refOffset].push_front( nextIdx - centerIndex );
              axeCount[axe]++;
            }
          // search for removed pixel during a translation
          IndexType prevIdx = idx - refOffset;
          if( tmpSEImageRegion.IsInside( prevIdx ) )
            {
            if( !tmpSEImage->GetPixel( prevIdx ) )
              {
                m_RemovedOffsets[refOffset].push_front( idx - centerIndex );
                axeCount[axe]++;
              }
            }
          else
            {
              m_RemovedOffsets[refOffset].push_front( idx - centerIndex );
              axeCount[axe]++;
            }

          }
        }
      }
    }
    
    // search for the best axe
    typedef typename std::multimap<unsigned long, int, typename std::greater< PixelType > > MapCountType;
    MapCountType invertedCount;
    for( int i=0; i<ImageDimension; i++ )
      {
      invertedCount.insert( typename MapCountType::value_type( axeCount[i], i ) );
      }

    int i=0;
    for( typename MapCountType::iterator it=invertedCount.begin(); it!=invertedCount.end(); it++, i++ )
      {
      m_Axes[i] = it->second;
      }

    m_PixelsPerTranslation = axeCount[m_Axes[ImageDimension - 1]] / 2;  // divided by 2 because there is 2 directions on the axe
}

template<class TInputImage, class TOutputImage, class TKernel>
void
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId) 
{
    ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
    
    // declare the type used to store the histogram, and instanciate the histogram
    typedef typename std::map< PixelType, unsigned long > HistogramType;
    HistogramType histogram;
    
    OutputImageType* outputImage = this->GetOutput();
    const InputImageType* inputImage = this->GetInput();
    RegionType inputRegion = inputImage->GetRequestedRegion();
    
    // initialize the histogram
    for( typename OffsetListType::iterator listIt = m_KernelOffsets.begin(); listIt != m_KernelOffsets.end(); listIt++ )
      {
      IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
      if( inputRegion.IsInside( idx ) )
        { histogram[inputImage->GetPixel(idx)]++; }
      }
    // and set the first point of the image
    if( !histogram.empty() )
      { outputImage->SetPixel( outputRegionForThread.GetIndex(), static_cast< OutputPixelType >( histogram.rbegin()->first - histogram.begin()->first ) ); }
    else
      { outputImage->SetPixel( outputRegionForThread.GetIndex(), 0 ); }
    progress.CompletedPixel();
    
    // now move the histogram
    itk::FixedArray<short, ImageDimension> direction;
    direction.Fill(1);
    IndexType currentIdx = outputRegionForThread.GetIndex();
    int axe = ImageDimension - 1;
    OffsetType offset;
    offset.Fill( 0 );
    RegionType stRegion;
    stRegion.SetSize( m_Kernel.GetSize() );
    stRegion.PadByRadius( 1 ); // must pad the region by one because of the translation
  
    OffsetType centerOffset;
    for( int axe=0; axe<ImageDimension; axe++)
      { centerOffset[axe] = stRegion.GetSize()[axe] / 2; }

    // init the offset and get the lists for the best axe
    offset[m_Axes[axe]] = direction[m_Axes[axe]];
    // it's very important for performances to get a pointer and not a copy
    const OffsetListType* addedList = &m_AddedOffsets[offset];;
    const OffsetListType* removedList = &m_RemovedOffsets[offset];

    while( axe >= 0 )
      {
      if( outputRegionForThread.IsInside( currentIdx + offset ) )
        {
        stRegion.SetIndex( currentIdx + offset - centerOffset );
        if( inputRegion.IsInside( stRegion ) )
          {
          // update the histogram
          for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
            { histogram[ inputImage->GetPixel( currentIdx + (*addedIt) ) ]++; }
          for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
            { histogram[ inputImage->GetPixel( currentIdx + (*removedIt) ) ]--; }
          }
        else
          {
          // update the histogram
          for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
            {
            IndexType idx = currentIdx + (*addedIt);
            if( inputRegion.IsInside( idx ) )
              { histogram[inputImage->GetPixel( idx )]++; }
            }
          for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
            {
            IndexType idx = currentIdx + (*removedIt);
            if( inputRegion.IsInside( idx ) )
              { histogram[ inputImage->GetPixel( idx ) ]--; }
            }
           }
            
        typename HistogramType::iterator mapIt = histogram.begin();
        while( mapIt != histogram.end() )
          {
          if( mapIt->second == 0 )
            { 
            // this value must be removed from the histogram
            // The value must be stored and the iterator updated before removing the value
            // or the iterator is invalidated.
            PixelType toErase = mapIt->first;
            mapIt++;
            histogram.erase(toErase);
            }
          else
            {
            mapIt++;
            // don't remove all the zero value found, just remove the one before the current maximum value
            // the histogram may become quite big on real type image, but it's an important increase of performances
            // break;
            }
          }
            
        // histogram is fully uptodate
        // get the highest value
        OutputPixelType value = 0;
        if( !histogram.empty() )
          { value = static_cast< OutputPixelType >( histogram.rbegin()->first - histogram.begin()->first ); }
                    
        // store the new index
        currentIdx += offset;
        
        outputImage->SetPixel( currentIdx, value );
        progress.CompletedPixel();
        
        if( axe != ImageDimension - 1 )
          {
          offset[m_Axes[axe]] = 0;
          // the axe must be the last one
          axe = ImageDimension - 1;
          offset[m_Axes[axe]] = direction[m_Axes[axe]];
          addedList = &m_AddedOffsets[offset];;
          removedList = &m_RemovedOffsets[offset];
          }
        }
      else
        {
        // the next offset is not in the right region,
        // we need to switch to another axe
        
        // invert the direction of the current axe
        direction[m_Axes[axe]] *= -1;
        // set the offset of the current axe to 0
        // -> offset == [0]*dim
        offset[m_Axes[axe]] = 0;
        // and switch to another axe
        axe--;
        
        if( axe >= 0 )
          {
          offset[m_Axes[axe]] = direction[m_Axes[axe]];
          addedList = &m_AddedOffsets[offset];;
          removedList = &m_RemovedOffsets[offset];
          }
        }
      }
}

template<class TInputImage, class TOutputImage, class TKernel>
void
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
}

}// end namespace itk
#endif

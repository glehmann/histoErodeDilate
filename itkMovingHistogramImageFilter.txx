/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramImageFilter_txx
#define __itkMovingHistogramImageFilter_txx

#include "itkMovingHistogramImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


namespace itk {


template<class TInputImage, class TOutputImage, class TKernel, class TValueFunctor, class THistogram>
MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, TValueFunctor, THistogram>
::MovingHistogramImageFilter()
{
}


template<class TInputImage, class TOutputImage, class TKernel, class TValueFunctor, class THistogram>
void
MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, TValueFunctor, THistogram>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId) 
{
    ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
    
    // instanciate the histogram
    THistogram histogram;

    // instanciate the value function
    TValueFunctor valueFunction;
    
    OutputImageType* outputImage = this->GetOutput();
    const InputImageType* inputImage = this->GetInput();
    RegionType inputRegion = inputImage->GetRequestedRegion();
    
    // initialize the histogram
    for( typename OffsetListType::iterator listIt = this->m_KernelOffsets.begin(); listIt != this->m_KernelOffsets.end(); listIt++ )
      {
      IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
      if( inputRegion.IsInside( idx ) )
        { histogram[inputImage->GetPixel(idx)]++; }
      }
    // and set the first point of the image
    outputImage->SetPixel( outputRegionForThread.GetIndex(), static_cast< OutputPixelType >( valueFunction( histogram ) ) );
    progress.CompletedPixel();
    
    // now move the histogram
    itk::FixedArray<short, ImageDimension> direction;
    direction.Fill(1);
    IndexType currentIdx = outputRegionForThread.GetIndex();
    int axe = ImageDimension - 1;
    OffsetType offset;
    offset.Fill( 0 );
    RegionType stRegion;
    stRegion.SetSize( this->m_Kernel.GetSize() );
    stRegion.PadByRadius( 1 ); // must pad the region by one because of the translation
  
    OffsetType centerOffset;
    for( int axe=0; axe<ImageDimension; axe++)
      { centerOffset[axe] = stRegion.GetSize()[axe] / 2; }

    // init the offset and get the lists for the best axe
    offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
    // it's very important for performances to get a pointer and not a copy
    const OffsetListType* addedList = &this->m_AddedOffsets[offset];;
    const OffsetListType* removedList = &this->m_RemovedOffsets[offset];

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
            
        typename THistogram::iterator mapIt = histogram.begin();
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
          offset[this->m_Axes[axe]] = 0;
          // the axe must be the last one
          axe = ImageDimension - 1;
          offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
          addedList = &this->m_AddedOffsets[offset];;
          removedList = &this->m_RemovedOffsets[offset];
          }
        }
      else
        {
        // the next offset is not in the right region,
        // we need to switch to another axe
        
        // invert the direction of the current axe
        direction[this->m_Axes[axe]] *= -1;
        // set the offset of the current axe to 0
        // -> offset == [0]*dim
        offset[this->m_Axes[axe]] = 0;
        // and switch to another axe
        axe--;
        
        if( axe >= 0 )
          {
          offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
          addedList = &this->m_AddedOffsets[offset];;
          removedList = &this->m_RemovedOffsets[offset];
          }
        }
      }
}

}// end namespace itk
#endif

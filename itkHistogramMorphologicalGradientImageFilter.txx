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


namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::HistogramMorphologicalGradientImageFilter()
  : m_Kernel()
{
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
  // store the kernel !!
  m_Kernel = kernel;

  // clear the already stored values
  m_AddedOffsets.clear();
  m_RemovedOffsets.clear();
  m_KernelOffsets.clear();
  //m_Axes

  // first, build the list of offsets of added and removed pixels when the 
  // structuring element move of 1 pixel on 1 axe; do it for the 2 directions
  // on each axes.
  
  // transform the structuring element in an image for an easier
  // access to the data
  typedef Image< bool, TInputImage::ImageDimension > BoolImageType;
  typename BoolImageType::Pointer tmpSEImage = BoolImageType::New();
  tmpSEImage->SetRegions( m_Kernel.GetSize() );
  tmpSEImage->Allocate();
  RegionType tmpSEImageRegion = tmpSEImage->GetRequestedRegion();
  ImageRegionIteratorWithIndex<BoolImageType> kernelImageIt;
  kernelImageIt = ImageRegionIteratorWithIndex<BoolImageType>(tmpSEImage, tmpSEImageRegion);
  kernelImageIt.GoToBegin();
  KernelIteratorType kernel_it = m_Kernel.Begin();

  // create a center index to compute the offset
  IndexType centerIndex;
  for( int axe=0; axe<ImageDimension; axe++)
    { centerIndex[axe] = m_Kernel.GetSize()[axe] / 2; }
  
  while( !kernelImageIt.IsAtEnd() )
    {
    kernelImageIt.Set( *kernel_it > 0 );
    if( *kernel_it > 0 )
      {
      kernelImageIt.Set( true );
      m_KernelOffsets.push_front( kernelImageIt.GetIndex() - centerIndex );
      }
    else
      {
      kernelImageIt.Set( false );
      }
    ++kernelImageIt;
    ++kernel_it;
    }
    
/*  std::cout << "image copied" << std::endl;*/

  typename itk::FixedArray< unsigned long, ImageDimension > axeCount;
  axeCount.Fill( 0 );

  for( int axe=0; axe<ImageDimension; axe++)
    {
/*  std::cout << "axe: " << axe << std::endl;*/
    OffsetType refOffset;
    refOffset.Fill( 0 );
    for( int direction=-1; direction<=1; direction +=2)
      {
      refOffset[axe] = direction;
/*  std::cout << "refOffset: " << refOffset << std::endl;*/
      for( kernelImageIt.GoToBegin(); !kernelImageIt.IsAtEnd(); ++kernelImageIt)
        {
        IndexType idx = kernelImageIt.GetIndex();
/*  std::cout << "idx: " << idx << std::endl;*/
        
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
/*    std::cout << "--" << axeCount << "--" << std::endl;*/
    typedef typename std::multimap<unsigned long, int, typename std::greater< unsigned long > > MapCountType;
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
}

template<class TInputImage, class TOutputImage, class TKernel>
void
HistogramMorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId) 
{
/*std::cout << "++" << axes << "--" << std::endl;*/
    
    // declare the type used to store the histogram, and instanciate the histogram
    typedef typename std::map< PixelType, unsigned long > HistogramType;
    HistogramType histogram;
    
/*    typedef ImageRegionIteratorWithIndex< OutputImageType > OutputIteratorType;
    OutputIteratorType oIt = OutputIteratorType( this->GetOutput(), outputRegionForThread );
    oIt.GoToBegin();*/
    
    OutputImageType* outputImage = this->GetOutput();
    const InputImageType* inputImage = this->GetInput();
    RegionType inputRegion = inputImage->GetRequestedRegion();
    
    // initialize the histogram
    for( typename OffsetListType::iterator listIt = m_KernelOffsets.begin(); listIt != m_KernelOffsets.end(); listIt++ )
      {
      IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
      if( inputRegion.IsInside( idx ) )
        {
        histogram[inputImage->GetPixel(idx)]++;
        }
      
      }
    // and set the first point of the image
    outputImage->SetPixel( outputRegionForThread.GetIndex(), histogram.begin()->first );
    
//         for( typename HistogramType::iterator mapIt = histogram.begin(); mapIt != histogram.end(); mapIt++ )
//           {
//           std::cout << "  " << 0.0 + mapIt->first << "-" << mapIt->second ;
//           }
//           std::cout << std::endl;

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
  
    while( axe >= 0 )
      {
      // increment the value on the current axe
      offset[m_Axes[axe]] = direction[m_Axes[axe]];
      if( outputRegionForThread.IsInside( currentIdx + offset ) )
        {
        stRegion.SetIndex( currentIdx + offset - centerOffset );
        if( inputRegion.IsInside( stRegion ) )
          {
          // update the histogram
          const OffsetListType* addedList = &m_AddedOffsets[offset]; // it's very important for performances to get a pointer and not a copy
          for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
            { histogram[ inputImage->GetPixel( currentIdx + (*addedIt) ) ]++; }
          const OffsetListType* removedList = &m_RemovedOffsets[offset];
          for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
            { histogram[ inputImage->GetPixel( currentIdx + (*removedIt) ) ]--; }
          }
        else
          {
          // update the histogram
          const OffsetListType* addedList = &m_AddedOffsets[offset];
          for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
            {
            IndexType idx = currentIdx + (*addedIt);
            if( inputRegion.IsInside( idx ) )
              { histogram[inputImage->GetPixel( idx )]++; }
            }
          const OffsetListType* removedList = &m_RemovedOffsets[offset];
          for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
            {
            IndexType idx = currentIdx + (*removedIt);
            if( inputRegion.IsInside( idx ) )
              { histogram[ inputImage->GetPixel( idx ) ]--; }
            }
           }
            
        for( typename HistogramType::iterator mapIt = histogram.begin(); mapIt != histogram.end(); mapIt++ )
          {
          // std::cout << "  " << 0.0 + mapIt->first << "-" << mapIt->second ;
          if( mapIt->second == 0 )
            { histogram.erase(mapIt->first); } // more efficient than histogram.erase(mapIt)... strange but true
          else
            {
            // don't remove all the zero value found, just remove the one before the current maximum value
            // the histogram may become quite big on real type image, but it's an important increase of performances
/*            break;*/
            }
          }
/*          std::cout << std::endl;*/
            
/*        for( typename HistogramType::reverse_iterator mapIt = histogram.rbegin(); mapIt != histogram.rend(); mapIt++ )
          {
          // std::cout << "  " << 0.0 + mapIt->first << "-" << mapIt->second ;
          if( mapIt->second == 0 )
            { histogram.erase(mapIt->first); }
          else
            {
            // don't remove all the zero value found, just remove the one before the current maximum value
            // the histogram may become quite big on real type image, but it's an important increase of performances
            break;
            }
          }*/
/*          std::cout << std::endl;*/
            
        // histogram is fully uptodate
        // get the highest value
        PixelType value = histogram.rbegin()->first - histogram.begin()->first;
                    
        // store the new index
        currentIdx += offset;
        
/*        oIt += offset;
        oIt.Set( value );*/
        outputImage->SetPixel( currentIdx, value );

        offset[m_Axes[axe]] = 0;
        // the axe must be the last one
        axe = ImageDimension - 1;
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

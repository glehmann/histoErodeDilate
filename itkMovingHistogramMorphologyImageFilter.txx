/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramMorphologyImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramMorphologyImageFilter_txx
#define __itkMovingHistogramMorphologyImageFilter_txx

#include "itkMovingHistogramMorphologyImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


namespace itk {

template<class TInputImage, class TOutputImage, class TKernel, class THistogram>
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, THistogram>
::MovingHistogramMorphologyImageFilter()
{
  // default m_boundary should be set by subclasses. Just provide a default
  // value to always get the same behavior if it is not done
  m_Boundary = itk::NumericTraits< PixelType >::Zero;
}

// #if 0
// 
// template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
// void
// MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
// ::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
//                        int threadId) 
// {
//     ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
//     
//     // declare the type used to store the histogram, and instanciate the histogram
//     typedef typename std::map< PixelType, unsigned long, TCompare > HistogramType;
//     HistogramType histogram;
//     
//     OutputImageType* outputImage = this->GetOutput();
//     const InputImageType* inputImage = this->GetInput();
//     RegionType inputRegion = inputImage->GetRequestedRegion();
//     
//     // initialize the histogram
//     for( typename OffsetListType::iterator listIt = this->m_KernelOffsets.begin(); listIt != this->m_KernelOffsets.end(); listIt++ )
//       {
//       IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
//       if( inputRegion.IsInside( idx ) )
//         { histogram[inputImage->GetPixel(idx)]++; }
//       else
//         { histogram[m_Boundary]++; }
//       }
//     // and set the first point of the image
//     outputImage->SetPixel( outputRegionForThread.GetIndex(), static_cast< OutputPixelType >( histogram.begin()->first ) );
//     progress.CompletedPixel();
// 
//     // now move the histogram
//     itk::FixedArray<short, ImageDimension> direction;
//     direction.Fill(1);
//     IndexType currentIdx = outputRegionForThread.GetIndex();
//     int axe = ImageDimension - 1;
//     OffsetType offset;
//     offset.Fill( 0 );
//     RegionType stRegion;
//     stRegion.SetSize( this->m_Kernel.GetSize() );
//     stRegion.PadByRadius( 1 ); // must pad the region by one because of the translation
//   
//     OffsetType centerOffset;
//     for( int axe=0; axe<ImageDimension; axe++)
//       { centerOffset[axe] = stRegion.GetSize()[axe] / 2; }
// 
//     // init the offset and get the lists for the best axe
//     offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
//     // it's very important for performances to get a pointer and not a copy
//     const OffsetListType* addedList = &this->m_AddedOffsets[offset];;
//     const OffsetListType* removedList = &this->m_RemovedOffsets[offset];
// 
// 
//     while( axe >= 0 )
//       {
//       if( outputRegionForThread.IsInside( currentIdx + offset ) )
//         {
//         stRegion.SetIndex( currentIdx + offset - centerOffset );
//         if( inputRegion.IsInside( stRegion ) )
//           {
//           // update the histogram
//           for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
//             { histogram[ inputImage->GetPixel( currentIdx + (*addedIt) ) ]++; }
//           for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
//             { histogram[ inputImage->GetPixel( currentIdx + (*removedIt) ) ]--; }
//           }
//         else
//           {
//           // update the histogram
//           for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
//             {
//             IndexType idx = currentIdx + (*addedIt);
//             if( inputRegion.IsInside( idx ) )
//               { histogram[inputImage->GetPixel( idx )]++; }
//             else
//               { histogram[m_Boundary]++; }
//             }
//           for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
//             {
//             IndexType idx = currentIdx + (*removedIt);
//             if( inputRegion.IsInside( idx ) )
//               { histogram[ inputImage->GetPixel( idx ) ]--; }
//             else
//               { histogram[m_Boundary]--; }
//             }
//            }
//             
//         typename HistogramType::iterator mapIt = histogram.begin();
//         while( mapIt != histogram.end() )
//           {
//           if( mapIt->second == 0 )
//             { 
//             // this value must be removed from the histogram
//             // The value must be stored and the iterator updated before removing the value
//             // or the iterator is invalidated.
//             PixelType toErase = mapIt->first;
//             mapIt++;
//             histogram.erase(toErase);
//             }
//           else
//             {
//             //mapIt++;
//             // don't remove all the zero value found, just remove the one before the current maximum value
//             // the histogram may become quite big on real type image, but it's an important increase of performances
//             break;
//             }
//           }
//             
//         // histogram is fully uptodate
//         // get the highest value
//         OutputPixelType value = static_cast< OutputPixelType >( histogram.begin()->first );
//                     
//         // store the new index
//         currentIdx += offset;
//         
//         outputImage->SetPixel( currentIdx, value );
//         progress.CompletedPixel();
// 
//         if( axe != ImageDimension - 1 )
//           {
//           offset[this->m_Axes[axe]] = 0;
//           // the axe must be the last one
//           axe = ImageDimension - 1;
//           offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
//           addedList = &this->m_AddedOffsets[offset];;
//           removedList = &this->m_RemovedOffsets[offset];
//           }
//         }
//       else
//         {
//         // the next offset is not in the right region,
//         // we need to switch to another axe
//         
//         // invert the direction of the current axe
//         direction[this->m_Axes[axe]] *= -1;
//         // set the offset of the current axe to 0
//         // -> offset == [0]*dim
//         offset[this->m_Axes[axe]] = 0;
//         // and switch to another axe
//         axe--;
//         
//         if( axe >= 0 )
//           {
//           offset[this->m_Axes[axe]] = direction[this->m_Axes[axe]];
//           addedList = &this->m_AddedOffsets[offset];;
//           removedList = &this->m_RemovedOffsets[offset];
//           }
//         }
//       }
// }
// #else
// // a modified version that uses line iterators and only moves the
// // histogram in one direction. Hopefully it will be a bit simpler and
// // faster due to improved memory access and a tighter loop.
// template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
// void
// MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
// ::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
//                        int threadId) 
// {
//     
//     // instanciate the histogram
//     HistogramType histogram;
//     
//     OutputImageType* outputImage = this->GetOutput();
//     const InputImageType* inputImage = this->GetInput();
//     RegionType inputRegion = inputImage->GetRequestedRegion();
//     
//     // initialize the histogram
//     for( typename OffsetListType::iterator listIt = this->m_KernelOffsets.begin(); listIt != this->m_KernelOffsets.end(); listIt++ )
//       {
//       IndexType idx = outputRegionForThread.GetIndex() + (*listIt);
//       if( inputRegion.IsInside( idx ) )
//         { histogram[inputImage->GetPixel(idx)]++; }
//       else
//         { histogram[m_Boundary]++; }
//       }
// 
//     // now move the histogram
//     itk::FixedArray<short, ImageDimension> direction;
//     direction.Fill(1);
//     IndexType currentIdx = outputRegionForThread.GetIndex();
//     int axe = ImageDimension - 1;
//     OffsetType offset;
//     offset.Fill( 0 );
//     RegionType stRegion;
//     stRegion.SetSize( this->m_Kernel.GetSize() );
//     stRegion.PadByRadius( 1 ); // must pad the region by one because of the translation
// 
//     OffsetType centerOffset;
//     for( int axe=0; axe<ImageDimension; axe++)
//       { centerOffset[axe] = stRegion.GetSize()[axe] / 2; }
// 
//     int BestDirection = this->m_Axes[axe];
//     // Report progress every line instead of every pixel
//     ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels()/outputRegionForThread.GetSize()[BestDirection]);
//     // init the offset and get the lists for the best axe
//     offset[BestDirection] = direction[BestDirection];
//     // it's very important for performances to get a pointer and not a copy
//     const OffsetListType* addedList = &this->m_AddedOffsets[offset];;
//     const OffsetListType* removedList = &this->m_RemovedOffsets[offset];
// 
//     typedef typename itk::ImageLinearConstIteratorWithIndex<InputImageType> InputLineIteratorType;
//     InputLineIteratorType InLineIt(inputImage, outputRegionForThread);
//     InLineIt.SetDirection(BestDirection);
//     
//     typedef typename itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
//     //OutputIteratorType oit(outputImage, outputRegionForThread);
//     InLineIt.GoToBegin();
//     IndexType LineStart;
//     //PrevLineStart = InLineIt.GetIndex();
//     InLineIt.GoToBegin();
// 
//     typedef typename std::vector<HistogramType> HistVecType;
//     HistVecType HistVec(ImageDimension);
//     typedef typename std::vector<IndexType> IndexVecType;
//     IndexVecType PrevLineStartVec(ImageDimension);
// 
//     for (int i=0;i<ImageDimension;i++)
//       {
//       HistVec[i] = histogram;
//       PrevLineStartVec[i] = InLineIt.GetIndex();
//       }
// 
//     while(!InLineIt.IsAtEnd())
//       {
//       HistogramType *histRef = &(HistVec[BestDirection]);
// //      PrevLineStart = InLineIt.GetIndex();
//       for (InLineIt.GoToBeginOfLine(); !InLineIt.IsAtEndOfLine(); ++InLineIt)
// 	{
// 	
// 	// Update the historgram
// 	IndexType currentIdx = InLineIt.GetIndex();
// 	outputImage->SetPixel(currentIdx, static_cast< OutputPixelType >( histRef->begin()->first ));
// 	stRegion.SetIndex( currentIdx - centerOffset );
// 	pushHistogram(*histRef, addedList, removedList, inputRegion, 
// 		      stRegion, inputImage, currentIdx);
// 
// 	cleanHistogram(*histRef);
// 	}
//       InLineIt.NextLine();
//       if (InLineIt.IsAtEnd())
// 	{
// 	break;
// 	}
//       LineStart = InLineIt.GetIndex();
//       // This section updates the histogram for the next line
//       // Since we aren't zig zagging we need to figure out which
//       // histogram to update and the direction in which to push
//       // it. Then we need to copy that histogram to the relevant
//       // places
//       OffsetType LineOffset;
//       // Figure out which stored histogram to move and in
//       // which direction
//       int LineDirection;
//       IndexType PrevLineStart = PrevLineStartVec[BestDirection];
// 
//       // This function deals with changing planes etc
//       GetDirAndOffset(LineStart, PrevLineStart, ImageDimension,
// 		      LineOffset, LineDirection);
// 
//       const OffsetListType* addedListLine = &this->m_AddedOffsets[LineOffset];;
//       const OffsetListType* removedListLine = &this->m_RemovedOffsets[LineOffset];
//       HistogramType *tmpHist = &(HistVec[LineDirection]);
//       stRegion.SetIndex(PrevLineStartVec[LineDirection] - centerOffset);
//       // Now move the histogram
//       pushHistogram(*tmpHist, addedListLine, removedListLine, inputRegion, 
// 		    stRegion, inputImage, PrevLineStartVec[LineDirection]);
// 
//       cleanHistogram(*tmpHist);
//       PrevLineStartVec[LineDirection] = LineStart;
//       // copy the updated histogram and line start entries to the
//       // relevant directions. When updating direction 2, for example,
//       // new copies of directions 0 and 1 should be made.
//       for (int i=0;i<ImageDimension;i++) 
// 	{
// 	int idx = this->m_Axes[i];
// 	if (idx< LineDirection)
// 	  {
// 	  PrevLineStartVec[idx] = LineStart;
// 	  HistVec[idx] = HistVec[LineDirection];
// 	  }
// 	}
//       progress.CompletedPixel();
//       }
// 
// }
/*
template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
void
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
::pushHistogram(HistogramType &histogram, 
		const OffsetListType* addedList,
		const OffsetListType* removedList,
		const RegionType &inputRegion,
		const RegionType &kernRegion,
		const InputImageType* inputImage,
		const IndexType currentIdx)
{

  if( inputRegion.IsInside( kernRegion ) )
    {
    // update the histogram
    for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
      { 
      histogram[ inputImage->GetPixel( currentIdx + (*addedIt) ) ]++; 
      }
    for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
      { 
      histogram[ inputImage->GetPixel( currentIdx + (*removedIt) ) ]--; 
      }
    }
  else
    {
    // update the histogram
    for( typename OffsetListType::const_iterator addedIt = addedList->begin(); addedIt != addedList->end(); addedIt++ )
      {
      IndexType idx = currentIdx + (*addedIt);
      if( inputRegion.IsInside( idx ) )
	{ histogram[inputImage->GetPixel( idx )]++; }
      else
	{ histogram[m_Boundary]++; }
      }
    for( typename OffsetListType::const_iterator removedIt = removedList->begin(); removedIt != removedList->end(); removedIt++ )
      {
      IndexType idx = currentIdx + (*removedIt);
      if( inputRegion.IsInside( idx ) )
	{ histogram[ inputImage->GetPixel( idx ) ]--; }
      else
	{ histogram[m_Boundary]--; }
      }
    }
}

template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
void
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
::cleanHistogram(HistogramType &histogram)
{
  // Remove empty entries from the start of the histogram
  typename HistogramType::iterator mapIt = histogram.begin();
  while( mapIt != histogram.end() )
    {
    if( mapIt->second == 0 )
      { 
      // this value must be removed from the histogram
      // The value must be stored and the iterator updated before removing the value
      // or the iterator is invalidated.
      typename HistogramType::iterator toErase = mapIt;
      mapIt++;
      histogram.erase(toErase);
      }
    else
      {
      //mapIt++;
      // don't remove all the zero value found, just remove the one before the current maximum value
      // the histogram may become quite big on real type image, but it's an important increase of performances
      break;
      }
    }
  
  // histogram is fully up-to-date
}

#if 1
template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
void
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
::printHist(const HistogramType &H)
{
  std::cout << "Hist = " ;
  typename HistogramType::const_iterator mapIt;
  for (mapIt = H.begin(); mapIt != H.end(); mapIt++) 
    {
    std::cout << "V= " << int(mapIt->first) << " C= " << int(mapIt->second) << " ";
    }
  std::cout << std::endl;
}
#endif

template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
void 
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, TCompare>
::GetDirAndOffset(const IndexType LineStart, 
		  const IndexType PrevLineStart,
		  const int ImageDimension,
		  OffsetType &LineOffset,
		  int &LineDirection)
{
  // when moving between lines in the same plane there should be only
  // 1 non zero (positive) entry in LineOffset.
  // When moving between planes there will be some negative ones too.
  LineOffset = LineStart - PrevLineStart;
  for (int y=0;y<ImageDimension;y++) 
    {
    if (LineOffset[y] > 0)
      {
      LineOffset[y]=1;  // should be 1 anyway
      LineDirection=y;
      }
    else
      {
      LineOffset[y]=0;
      }
    }
}

#endif */



template<class TInputImage, class TOutputImage, class TKernel, class THistogram>
THistogram
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, THistogram>
::NewHistogram()
{
  THistogram histogram;
  histogram.SetBoundary( m_Boundary );
  return histogram;
}


template<class TInputImage, class TOutputImage, class TKernel, class THistogram>
void
MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel, THistogram>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Boundary: " << m_Boundary << std::endl;
}

}// end namespace itk
#endif

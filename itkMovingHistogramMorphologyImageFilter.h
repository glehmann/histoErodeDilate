/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramMorphologyImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramMorphologyImageFilter_h
#define __itkMovingHistogramMorphologyImageFilter_h

#include "itkMovingHistogramImageFilter.h"
#include <list>
#include <map>
#include "itkOffsetLexicographicCompare.h"

namespace itk {

namespace Function {
template <class TInputPixel, class TCompare>
class MorphologyHistogram
{
public:
  MorphologyHistogram(){}
  ~MorphologyHistogram(){}

  typedef typename std::map< TInputPixel, unsigned long, TCompare > HistogramType;

  inline TInputPixel operator()( const HistogramType &histogram )
  {
    if( !histogram.empty() )
      { return histogram.rbegin()->first - histogram.begin()->first; }
    return 0;
  }

  inline void AddBoundary()
    { m_Map[ m_Boundary ]++; }

  inline void RemoveBoundary()
    { m_Map[ m_Boundary ]--; }

  inline void AddPixel( const TInputPixel &p )
    { m_Map[ p ]++; }

  inline void RemovePixel( const TInputPixel &p )
    { m_Map[ p ]--; }

  inline TInputPixel GetValue()
    {
    // clean the map
    typename HistogramType::iterator mapIt = m_Map.begin();
    while( mapIt != m_Map.end() )
      {
      if( mapIt->second == 0 )
        { 
        // this value must be removed from the histogram
        // The value must be stored and the iterator updated before removing the value
        // or the iterator is invalidated.
        TInputPixel toErase = mapIt->first;
        mapIt++;
        m_Map.erase( toErase );
        }
      else
        {
        mapIt++;
        // don't remove all the zero value found, just remove the one before the current maximum value
        // the histogram may become quite big on real type image, but it's an important increase of performances
        break;
        }
      }

    // and return the value
    return m_Map.begin()->first;
    }

  void SetBoundary( const TInputPixel & val )
    { m_Boundary = val; }

  HistogramType m_Map;

  TInputPixel m_Boundary;
};
} // end namespace Function



/**
 * \class MovingHistogramMorphologyImageFilter
 * \brief base class for MovingHistogramDilateImageFilter and MovingHistogramErodeImageFilter
 *
 * This class is similar to MovingHistogramImageFilter but add support
 * for boundaries and don't fully update the histogram to enhance performances.
 * 
 * \sa MovingHistogramImageFilter, MovingHistogramDilateImageFilter, MovingHistogramErodeImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel, class THistogram>
class ITK_EXPORT MovingHistogramMorphologyImageFilter : 
    public MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, THistogram>
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramMorphologyImageFilter Self;
  typedef MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, THistogram> Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingHistogramMorphologyImageFilter, 
               ImageToImageFilter);
  
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename TOutputImage::PixelType OutputPixelType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Kernel typedef. */
  typedef TKernel KernelType;
  
  /** Kernel (structuring element) iterator. */
  typedef typename KernelType::ConstIterator KernelIteratorType ;
  
  /** n-dimensional Kernel radius. */
  typedef typename KernelType::SizeType RadiusType ;

  typedef typename std::list< OffsetType > OffsetListType;

  typedef typename std::map< OffsetType, OffsetListType, typename Functor::OffsetLexicographicCompare<ImageDimension> > OffsetMapType;

  /** Set/Get the boundary value. */
  itkSetMacro(Boundary, PixelType);
  itkGetMacro(Boundary, PixelType);
  
protected:
  MovingHistogramMorphologyImageFilter();
  ~MovingHistogramMorphologyImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Multi-thread version GenerateData. */
//   void  ThreadedGenerateData (const OutputImageRegionType& 
//                               outputRegionForThread,
//                               int threadId) ;

  /** needed to pass the boundary value to the histogram object */
  virtual THistogram NewHistogram();

  PixelType m_Boundary;

private:
  MovingHistogramMorphologyImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

// #if 1
//   // declare the type used to store the histogram
//   typedef typename std::map< PixelType, unsigned long, TCompare > HistogramType;
// 
//   void pushHistogram(HistogramType &histogram, 
// 		     const OffsetListType* addedList,
// 		     const OffsetListType* removedList,
// 		     const RegionType &inputRegion,
// 		     const RegionType &kernRegion,
// 		     const InputImageType* inputImage,
// 		     const IndexType currentIdx);
// 
//   void cleanHistogram(HistogramType &histogram);
// 
//   void printHist(const HistogramType &H);
// 
//   void GetDirAndOffset(const IndexType LineStart, 
// 		       const IndexType PrevLineStart,
// 		       const int ImageDimension,
// 		       OffsetType &LineOffset,
// 		       int &LineDirection);
// 
// #endif

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMovingHistogramMorphologyImageFilter.txx"
#endif

#endif



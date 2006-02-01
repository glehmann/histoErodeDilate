/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramMorphologicalGradientImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramMorphologicalGradientImageFilter_h
#define __itkMovingHistogramMorphologicalGradientImageFilter_h

#include "itkMovingHistogramImageFilter.h"
#include <map>

namespace itk {

namespace Function {
template <class TInputPixel>
class MorphologicalGradientHistogram
{
public:
  MorphologicalGradientHistogram(){}
  ~MorphologicalGradientHistogram(){}

  typedef typename std::map< TInputPixel, unsigned long > HistogramType;

  inline TInputPixel operator()( const HistogramType &histogram )
  {
    if( !histogram.empty() )
      { return histogram.rbegin()->first - histogram.begin()->first; }
    return 0;
  }

  inline void AddBoundary() {}

  inline void RemoveBoundary() {}

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
        m_Map.erase(toErase);
        }
      else
        {
        mapIt++;
        }
      }

    // and return the value
    if( !m_Map.empty() )
      { return m_Map.rbegin()->first - m_Map.begin()->first; }
    return 0;
    }

  HistogramType m_Map;
};
} // end namespace Function

/**
 * \class MovingHistogramMorphologicalGradientImageFilter
 * \brief Morphological gradients enhance the variation of pixel intensity in a given neighborhood.
 *
 * Morphological gradient is described in Chapter 3.8.1 of Pierre Soille's book 
 * "Morphological Image Analysis: Principles and Applications", 
 * Second Edition, Springer, 2003.
 * 
 * \author Gaëtan Lehmann. Biologie du Développement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */


template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT MovingHistogramMorphologicalGradientImageFilter : 
    public MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel,
      typename  Function::MorphologicalGradientHistogram< typename TInputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramMorphologicalGradientImageFilter Self;
  typedef MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel,
      typename  Function::MorphologicalGradientHistogram< typename TInputImage::PixelType > >  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingHistogramMorphologicalGradientImageFilter, 
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

  typedef typename Function::MorphologicalGradientHistogram< PixelType > HistogramType;


  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      

protected:
  MovingHistogramMorphologicalGradientImageFilter() {};
  ~MovingHistogramMorphologicalGradientImageFilter() {};


private:
  MovingHistogramMorphologicalGradientImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

} ; // end of class

} // end namespace itk
  
#endif



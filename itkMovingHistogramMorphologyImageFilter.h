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

#include "itkMovingHistogramImageFilterBase.h"
#include <list>
#include <map>
#include "itkOffsetLexicographicCompare.h"

namespace itk {

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

template<class TInputImage, class TOutputImage, class TKernel, class TCompare>
class ITK_EXPORT MovingHistogramMorphologyImageFilter : 
    public MovingHistogramImageFilterBase<TInputImage, TOutputImage, TKernel>
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramMorphologyImageFilter Self;
  typedef MovingHistogramImageFilterBase<TInputImage, TOutputImage, TKernel> Superclass;
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
  void  ThreadedGenerateData (const OutputImageRegionType& 
                              outputRegionForThread,
                              int threadId) ;

  PixelType m_Boundary;

private:
  MovingHistogramMorphologyImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMovingHistogramMorphologyImageFilter.txx"
#endif

#endif



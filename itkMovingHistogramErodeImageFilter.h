/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramErodeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramErodeImageFilter_h
#define __itkMovingHistogramErodeImageFilter_h

#include "itkMovingHistogramImageFilterBase.h"
#include <list>
#include <map>
#include "itkOffsetLexicographicCompare.h"

namespace itk {

/**
 * \class MovingHistogramErodeImageFilter
 * \brief gray scale dilation of an image
 *
 * Erode an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionErodeImageFilter, BinaryErodeImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT MovingHistogramErodeImageFilter : 
    public MovingHistogramImageFilterBase<TInputImage, TOutputImage, TKernel>
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramErodeImageFilter Self;
  typedef MovingHistogramImageFilterBase<TInputImage, TOutputImage, TKernel> Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingHistogramErodeImageFilter, 
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
  MovingHistogramErodeImageFilter();
  ~MovingHistogramErodeImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const OutputImageRegionType& 
                              outputRegionForThread,
                              int threadId) ;


private:
  MovingHistogramErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PixelType m_Boundary;
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMovingHistogramErodeImageFilter.txx"
#endif

#endif



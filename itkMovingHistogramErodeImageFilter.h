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

#include "itkMovingHistogramMorphologyImageFilter.h"

namespace itk {

/**
 * \class MovingHistogramErodeImageFilter
 * \brief gray scale dilation of an image
 *
 * MorphologicalGradient an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionMorphologicalGradientImageFilter, BinaryMorphologicalGradientImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */


template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT MovingHistogramErodeImageFilter : 
    public MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel,
      typename Function::MorphologyHistogram < typename TInputImage::PixelType, typename std::less<typename TInputImage::PixelType> > >
{
public:
  /** Standard class typedefs. */
  typedef MovingHistogramErodeImageFilter Self;
  typedef MovingHistogramMorphologyImageFilter<TInputImage, TOutputImage, TKernel,
      typename Function::MorphologyHistogram < typename TInputImage::PixelType, typename std::less<typename TInputImage::PixelType> > >  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingHistogramErodeImageFilter, 
               MovingHistogramMorphologyImageFilter);
  
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
                      

protected:
  MovingHistogramErodeImageFilter()
  {
    this->m_Boundary = itk::NumericTraits< PixelType >::max();
  }
  ~MovingHistogramErodeImageFilter() {};


private:
  MovingHistogramErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

} ; // end of class

} // end namespace itk
  
#endif



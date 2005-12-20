/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMorphologicalGradientImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMorphologicalGradientImageFilter_h
#define __itkMorphologicalGradientImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkMovingHistogramMorphologicalGradientImageFilter.h"
#include "itkBasicMorphologicalGradientImageFilter.h"
#include "itkConstantBoundaryCondition.h"

namespace itk {

/**
 * \class MorphologicalGradientImageFilter
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
class ITK_EXPORT MorphologicalGradientImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef MorphologicalGradientImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MorphologicalGradientImageFilter, 
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

  typedef MovingHistogramMorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel > HistogramFilterType;
  typedef BasicMorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel > BasicFilterType;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Kernel typedef. */
  typedef TKernel KernelType;
  
  /** Set kernel (structuring element). */
  void SetKernel( const KernelType& kernel );

  /** Get the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);
  
  /** Set/Get the backend filter class. */
  void SetNameOfBackendFilterClass( const char * name );
  itkGetMacro(NameOfBackendFilterClass, const char*);
  
  /** MorphologicalGradientImageFilter need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  /** MorphologicalGradientImageFilter need to set its internal filters as modified */
  virtual void Modified() const;

protected:
  MorphologicalGradientImageFilter();
  ~MorphologicalGradientImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  void GenerateData();

private:
  MorphologicalGradientImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** kernel or structuring element to use. */
  KernelType m_Kernel ;

  // the filters used internally
  typename HistogramFilterType::Pointer m_HistogramFilter;
  typename BasicFilterType::Pointer m_BasicFilter;

  // and the name of the filter
  const char* m_NameOfBackendFilterClass;

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMorphologicalGradientImageFilter.txx"
#endif

#endif



/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBasicMorphologicalGradientImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2005/08/23 15:16:38 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBasicMorphologicalGradientImageFilter_h
#define __itkBasicMorphologicalGradientImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk {

/** \class BasicMorphologicalGradientImageFilter
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
class ITK_EXPORT BasicMorphologicalGradientImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef BasicMorphologicalGradientImageFilter Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage>
  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename OutputImageType::Pointer        OutputImagePointer;
  typedef typename OutputImageType::ConstPointer   OutputImageConstPointer;
  typedef typename OutputImageType::RegionType     OutputImageRegionType;
  typedef typename OutputImageType::PixelType      OutputImagePixelType;
  
 /** Kernel typedef. */
  typedef TKernel KernelType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(BasicMorphologicalGradientImageFilter, 
               ImageToImageFilter);

  /** Set kernel (structuring element). */
  itkSetMacro(Kernel, KernelType);
  
  /** Get the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);

protected:
  BasicMorphologicalGradientImageFilter();
  ~BasicMorphologicalGradientImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** BasicMorphologicalGradientImageFilter need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  void GenerateData();
  

private:
  BasicMorphologicalGradientImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** kernel or structuring element to use. */
  KernelType m_Kernel ;
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBasicMorphologicalGradientImageFilter.txx"
#endif

#endif



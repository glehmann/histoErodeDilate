/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayscaleDilateImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleDilateImageFilter_h
#define __itkGrayscaleDilateImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkConstantBoundaryCondition.h"

namespace itk {

/**
 * \class GrayscaleDilateImageFilter
 * \brief gray scale dilation of an image
 *
 * Dilate an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionDilateImageFilter, BinaryDilateImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT GrayscaleDilateImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef GrayscaleDilateImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(GrayscaleDilateImageFilter, 
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

  typedef MovingHistogramDilateImageFilter< TInputImage, TOutputImage, TKernel > HistogramFilterType;
  typedef BasicDilateImageFilter< TInputImage, TOutputImage, TKernel > BasicFilterType;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Typedef for boundary conditions. */
  typedef ImageBoundaryCondition<InputImageType> *ImageBoundaryConditionPointerType;
  typedef ImageBoundaryCondition<InputImageType> const *ImageBoundaryConditionConstPointerType;
  typedef ConstantBoundaryCondition<InputImageType> DefaultBoundaryConditionType;


  /** Kernel typedef. */
  typedef TKernel KernelType;
  
  /** Set kernel (structuring element). */
  void SetKernel( const KernelType& kernel );

  /** Get the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);
  
  /** Set/Get the boundary value. */
  void SetBoundary( const PixelType value );
  itkGetMacro(Boundary, PixelType);
  
  /** Set/Get the backend filter class. */
  void SetNameOfBackendFilterClass( const char * name );
  itkGetMacro(NameOfBackendFilterClass, const char*);
  
  /** GrayscaleDilateImageFilter need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  /** GrayscaleDilateImageFilter need to set its internal filters as modified */
  virtual void Modified() const;

  /** \deprecated
  * Allows a user to override the internal boundary condition. Care should be
  * be taken to ensure that the overriding boundary condition is a persistent
  * object during the time it is referenced. The overriding condition
  * can be of a different type than the default type as long as it is
  * a subclass of ImageBoundaryCondition. */
  void OverrideBoundaryCondition(const DefaultBoundaryConditionType* i)
    {
    itkLegacyBody(itk::GrayscaleDilateImageFilter::OverrideBoundaryCondition, 2.8);
    SetBoundary( i->GetConstant() );
    }

  /** \deprecated
   * Get the current boundary condition.
   */
  const DefaultBoundaryConditionType* GetBoundaryCondition()
    {
    itkLegacyBody(itk::GrayscaleDilateImageFilter::GetBoundaryCondition, 2.8);
    return &m_BoundaryCondition;
    }

  /** \deprecated
   * Rest the boundary condition to the default
   */
  void ResetBoundaryCondition()
    {
    itkLegacyBody(itk::GrayscaleDilateImageFilter::ResetBoundaryCondition, 2.8);
    SetBoundary( itk::NumericTraits< PixelType >::NonpositiveMin() );
    }


protected:
  GrayscaleDilateImageFilter();
  ~GrayscaleDilateImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  void GenerateData();

private:
  GrayscaleDilateImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** kernel or structuring element to use. */
  KernelType m_Kernel ;

  PixelType m_Boundary;

  // the filters used internally
  typename HistogramFilterType::Pointer m_HistogramFilter;
  typename BasicFilterType::Pointer m_BasicFilter;

  // and the name of the filter
  const char* m_NameOfBackendFilterClass;

  // the boundary condition need to be stored here
  DefaultBoundaryConditionType m_BoundaryCondition;
  
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGrayscaleDilateImageFilter.txx"
#endif

#endif



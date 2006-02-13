#Change PROJECT_NAME to the name of your project
PROJECT(histoErodeDilate)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

FIND_PROGRAM(IMAGE_COMPARE ImageCompare)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
)


# install devel files
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project
SET(CurrentExe "perf2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf3D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_threads")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_binary")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_nb_of_neighbors")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_strel_size")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_image_size")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate3D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_dimension")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "erode2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "erode3D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "gradient2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "gradient3D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "kernelException")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "metaErode2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "metaDilate2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "metaGradient2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.

ADD_TEST(Dilate2D dilate2D ${CMAKE_SOURCE_DIR}/images/cthead1.png bdilate2D.png hdilate2D.png)
ADD_TEST(Dilate2DCompare ${IMAGE_COMPARE} bdilate2D.png hdilate2D.png)

ADD_TEST(Dilate3D dilate3D ${CMAKE_SOURCE_DIR}/images/ESCells.hdr bdilate3D.tif hdilate3D.tif)
ADD_TEST(Dilate3DCompare ${IMAGE_COMPARE} bdilate3D.tif hdilate3D.tif)

ADD_TEST(Erode2D erode2D ${CMAKE_SOURCE_DIR}/images/cthead1.png berode2D.png herode2D.png)
ADD_TEST(Erode2DCompare ${IMAGE_COMPARE} berode2D.png herode2D.png)

ADD_TEST(Erode3D erode3D ${CMAKE_SOURCE_DIR}/images/ESCells.hdr berode3D.tif herode3D.tif)
ADD_TEST(Erode3DCompare ${IMAGE_COMPARE} berode3D.tif herode3D.tif)

ADD_TEST(Gradient2D gradient2D ${CMAKE_SOURCE_DIR}/images/cthead1.png bgradient2D.png hgradient2D.png)
ADD_TEST(Gradient2DCompare ${IMAGE_COMPARE} bgradient2D.png hgradient2D.png)

ADD_TEST(Gradient3D gradient3D ${CMAKE_SOURCE_DIR}/images/ESCells.hdr bgradient3D.tif hgradient3D.tif)
ADD_TEST(Gradient3DCompare ${IMAGE_COMPARE} bgradient3D.tif hgradient3D.tif)

ADD_TEST(KernelException kernelException)

ADD_TEST(MetaErode2D metaErode2D ${CMAKE_SOURCE_DIR}/images/cthead1.png bmetaErode2D.png hmetaErode2D.png)
ADD_TEST(MetaErode2DCompare ${IMAGE_COMPARE} bmetaErode2D.png hmetaErode2D.png)

ADD_TEST(MetaDilate2D metaDilate2D ${CMAKE_SOURCE_DIR}/images/cthead1.png bmetaDilate2D.png hmetaDilate2D.png)
ADD_TEST(MetaDilate2DCompare ${IMAGE_COMPARE} bmetaDilate2D.png hmetaDilate2D.png)

ADD_TEST(MetaGradient2D metaGradient2D ${CMAKE_SOURCE_DIR}/images/cthead1.png bmetaGradient2D.png hmetaGradient2D.png)
ADD_TEST(MetaGradient2DCompare ${IMAGE_COMPARE} bmetaGradient2D.png hmetaGradient2D.png)


#Change PROJECT_NAME to the name of your project
PROJECT(histoErodeDilate)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

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

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project
SET(CurrentExe "perf2D")
ADD_EXECUTABLE(${CurrentExe} perf2D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf3D")
ADD_EXECUTABLE(${CurrentExe} perf3D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "perf_threads")
ADD_EXECUTABLE(${CurrentExe} perf_threads.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate2D")
ADD_EXECUTABLE(${CurrentExe} dilate2D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate3D")
ADD_EXECUTABLE(${CurrentExe} dilate3D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "bestAxe2D")
ADD_EXECUTABLE(${CurrentExe} bestAxe2D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "erode2D")
ADD_EXECUTABLE(${CurrentExe} erode2D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "erode3D")
ADD_EXECUTABLE(${CurrentExe} erode3D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "gradient2D")
ADD_EXECUTABLE(${CurrentExe} gradient2D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "gradient3D")
ADD_EXECUTABLE(${CurrentExe} gradient3D.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.

ADD_TEST(Dilate2D dilate2D ${CMAKE_SOURCE_DIR}/cthead1.tif dilate2d.tif hdilate2d.tif)
ADD_TEST(Dilate2DCompare ImageCompare dilate2d.tif hdilate2d.tif)

ADD_TEST(Dilate3D dilate3D ${CMAKE_SOURCE_DIR}/ESCells.hdr dilate3d.tif hdilate3d.tif)
ADD_TEST(Dilate3DCompare ImageCompare dilate3d.tif hdilate3d.tif)

ADD_TEST(Erode2D erode2D ${CMAKE_SOURCE_DIR}/cthead1.tif erode2d.tif herode2d.tif)
ADD_TEST(Erode2DCompare ImageCompare erode2d.tif herode2d.tif)

ADD_TEST(Erode3D erode3D ${CMAKE_SOURCE_DIR}/ESCells.hdr erode3d.tif herode3d.tif)
ADD_TEST(Erode3DCompare ImageCompare erode3d.tif herode3d.tif)

ADD_TEST(Gradient2D gradient2D ${CMAKE_SOURCE_DIR}/cthead1.tif gradient2d.tif hgradient2d.tif)
ADD_TEST(Gradient2DCompare ImageCompare gradient2d.tif hgradient2d.tif)

ADD_TEST(Gradient3D gradient3D ${CMAKE_SOURCE_DIR}/ESCells.hdr gradient3d.tif hgradient3d.tif)
ADD_TEST(Gradient3DCompare ImageCompare gradient3d.tif hgradient3d.tif)


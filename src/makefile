CC = g++
CCFLAGS = -std=c++11

OPENCV_INC_PATH = C:\msys64\mingw64\include
OPENCV_LIB_PATH = C:\msys64\mingw64\lib
LDFLAGS = -L$(OPENCV_LIB_PATH) -llibopencv_calib3d341 -llibopencv_core341 -llibopencv_dnn341 -llibopencv_features2d341 -llibopencv_flann341 -llibopencv_highgui341 -llibopencv_imgcodecs341 -llibopencv_imgproc341 -llibopencv_ml341 -llibopencv_objdetect341 -llibopencv_photo341 -llibopencv_shape341 -llibopencv_stitching341 -llibopencv_superres341 -llibopencv_video341 -llibopencv_videoio341 -llibopencv_videostab341
 

all: snapcut

snapcut: main.cpp
	g++ main.cpp frame.cpp util.cpp window.cpp gmm.cpp graphcut.cpp -std=c++17 -L$(OPENCV_LIB_PATH) -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_video -lopencv_videoio -lopencv_features2d -lopencv_calib3d -I$(OPENCV_INC_PATH)
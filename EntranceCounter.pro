TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS -= -mmacosx-version-min=10.6
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_LFLAGS -= -mmacosx-version-min=10.6
QMAKE_LFLAGS += -stdlib=libc++

SOURCES += main.cpp \
    tracking_person.cpp \
    people_tracker.cpp \
    people_detector.cpp \
    image_holder.cpp

unix: LIBS += -L/usr/opencv/lib/\
-lopencv_core\
-lopencv_highgui\
-lopencv_video\
-lopencv_imgproc\
-lopencv_objdetect\
-lopencv_features2d

INCLUDEPATH += /usr/opencv/include
DEPENDPATH += /usr/opencv/include

HEADERS += \
    tracking_person.h \
    people_tracker.h \
    people_detector.h \
    image_holder.h

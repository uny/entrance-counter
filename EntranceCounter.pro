TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -Wc++11-extensions -Wgnu

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

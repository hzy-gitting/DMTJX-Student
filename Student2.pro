QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += __STDC_LIMIT_MACROS

INCLUDEPATH += \
    C:\Users\hzy\Desktop\MyCode\Teacher\network \
    C:\Users\hzy\Desktop\SDL-devel-1.2.15-VC\SDL-1.2.15\Include \
    C:\Users\hzy\Desktop\ffmpeg_win64_dev\Include

LIBS += -LC:\Users\hzy\Desktop\ffmpeg_win64_dev\lib -lavcodec -lavdevice -lavformat -lswscale -lavutil
LIBS += -LC:\Users\hzy\Desktop\SDL-devel-1.2.15-VC\SDL-1.2.15\lib\x64 -lsdl

SOURCES += \
    ffmpegDecoder.cpp \
    filedatahandler.cpp \
    filercvwindow.cpp \
    filereceiver.cpp \
    main.cpp \
    netmessage.cpp \
    networkdatahandler.cpp \
    stumsgwindow.cpp \
    videodatabuffer.cpp \
    widget.cpp \

HEADERS += \
    SDLMaster.h \
    ffmpeg.h \
    ffmpegDecoder.h \
    filedatahandler.h \
    filercvwindow.h \
    filereceiver.h \
    netmessage.h \
    networkdatahandler.h \
    stumsgwindow.h \
    videodatabuffer.h \
    widget.h

FORMS += \
    filercvwindow.ui \
    stumsgwindow.ui \
    widget.ui

LIBS+=-ladvapi32
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

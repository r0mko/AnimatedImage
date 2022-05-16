QT += qml quick

CONFIG += qmltypes
CONFIG += file_copies
CONFIG -= app_bundle

QML_IMPORT_NAME = AnimatedImageTexture
QML_IMPORT_MAJOR_VERSION = 1

HEADERS += \
    animatedimage.h \
    animatedimagefborenderer.h
SOURCES += \
    animatedimage.cpp \
    animatedimagefborenderer.cpp \
    main.cpp

OTHER_FILES += \
    main.qml \
    assets/elephant_00.png \
    assets/elephant_01.png \
    assets/elephant_02.png \
    assets/elephant_03.png \
    assets/elephant_04.png \
    assets/elephant_05.png \
    assets/elephant_06.png \
    assets/elephant_07.png \
    assets/elephant_08.png \
    assets/elephant_09.png \
    assets/elephant_10.png \
    assets/elephant_11.png \
    assets/elephant_12.png \
    assets/elephant_13.png \
    assets/elephant_14.png \
    assets/elephant_15.png \
    assets/elephant_16.png \
    assets/elephant_17.png \
    assets/elephant_18.png \
    assets/elephant_19.png \
    assets/elephant_20.png \
    assets/elephant_21.png \
    assets/elephant_22.png \
    assets/elephant_23.png \
    assets/elephant_24.png \
    assets/elephant_25.png \
    assets/elephant_26.png \
    assets/elephant_27.png \
    assets/elephant_28.png \
    assets/elephant_29.png \
    assets/elephant_30.png \
    assets/elephant_31.png \
    assets/elephant_32.png \
    assets/elephant_33.png \
    shaders/checker.frag \
    shaders/checker.frag.qsb \
    shaders/shader.frag \
    shaders/shader.vert \

assets.files += \
    assets/elephant_00.png \
    assets/elephant_01.png \
    assets/elephant_02.png \
    assets/elephant_03.png \
    assets/elephant_04.png \
    assets/elephant_05.png \
    assets/elephant_06.png \
    assets/elephant_07.png \
    assets/elephant_08.png \
    assets/elephant_09.png \
    assets/elephant_10.png \
    assets/elephant_11.png \
    assets/elephant_12.png \
    assets/elephant_13.png \
    assets/elephant_14.png \
    assets/elephant_15.png \
    assets/elephant_16.png \
    assets/elephant_17.png \
    assets/elephant_18.png \
    assets/elephant_19.png \
    assets/elephant_20.png \
    assets/elephant_21.png \
    assets/elephant_22.png \
    assets/elephant_23.png \
    assets/elephant_24.png \
    assets/elephant_25.png \
    assets/elephant_26.png \
    assets/elephant_27.png \
    assets/elephant_28.png \
    assets/elephant_29.png \
    assets/elephant_30.png \
    assets/elephant_31.png \
    assets/elephant_32.png \
    assets/elephant_33.png \

assets.path = $$OUT_PWD/assets

shaders.files += \
    shaders/shader.vert \
    shaders/shader.frag \
    shaders/checker.frag \
    shaders/checker.frag.qsb \

shaders.path = $$OUT_PWD/shaders

qml.files += \
    main.qml

qml.path = $$OUT_PWD/qml


COPIES += assets shaders qml

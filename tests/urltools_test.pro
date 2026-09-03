QT += core testlib
CONFIG += console testcase
TEMPLATE = app
TARGET = urltools_test
SOURCES += urltools_test.cpp \
           ../urltools.cpp
HEADERS += ../urltools.h
INCLUDEPATH += ..

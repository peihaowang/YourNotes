#-------------------------------------------------
#
# Project created by QtCreator 2018-01-05T15:08:53
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = YourNotes
TEMPLATE = app


SOURCES += main.cpp\
    app_settings.cpp \
    ColorPicker.cpp \
    MenuColor.cpp \
    NoteList.cpp \
    NotePage.cpp \
    OptionsPage.cpp \
    MyStyle.cpp \
    MainWindow.cpp \
    HelpWidget.cpp \
    DlgSetReminder.cpp \
    NoteStg.cpp

HEADERS  += \
    app_constants.h \
    app_settings.h \
    app_types.h \
    app_utilities.h \
    common_headers.h \
    cpp_utilities.h \
    dll_utilities.h \
    encoding_specific.h \
    file_utilities.h \
    platform_specific.h \
    string_utilities.h \
    type_defs.h \
    ColorPicker.h \
    MenuColor.h \
    MainWindow.h \
    MyStyle.h \
    NoteList.h \
    NotePage.h \
    OptionsPage.h \
    HelpWidget.h \
    NoteStg.h \
    DlgSetReminder.h

RESOURCES += \
    res.qrc

QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += LIBRARAY_VCF_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        DB_Schema.cpp \
        Database_GTF.cpp \
        Database_GTFPrivate.cpp \
        Database_VCF.cpp \
        Database_VCFPrivate.cpp \
        Variant_Call_Format.cpp \
        Variant_Call_FormatPrivate.cpp

HEADERS += \
	DB_Schema.h \
	Database_GTF.h \
	Database_GTFPrivate.h \
	Database_VCF.h \
	Database_VCFPrivate.h \
	Libraray_VCF_global.h \
	Variant_Call_Format.h \
	Variant_Call_FormatPrivate.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

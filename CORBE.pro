#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T17:03:25
#
#-------------------------------------------------

test {

    QT += core gui network
    QT += testlib
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

    TARGET = UnitTests

    SOURCES += Main/AppTest/main.cpp

    include(Forms/Forms.pri)
    include(Headers/Headers.pri)
    include(Tests/Tests.pri)
    include(Sources/Sources.pri)

} else {

    QT       += core gui network
    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

    TARGET = CORBEClient
    TEMPLATE = app

    SOURCES += Main/App/main.cpp

    include(Forms/Forms.pri)
    include(Sources/Sources.pri)
    include(Headers/Headers.pri)
}

#-------------------------------------------------
#
# Project created by QtCreator 2017-04-11T15:20:34
#
#/**
# *
# * Project: Solitaire Klondike
# * Authors: Vladimír Jeřábek (xjerab21)
# *          Petr Jůda  (xjudap00)
# *
# * Class representing one game in GUI
# * It contains all stacks, button and layout of the game
# *
# */
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hra2017
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += card.cpp\
    CardDeck.cpp\    
    CardStack.cpp\
    FactoryKlondike.cpp\    
    GameCore.cpp\    
    main.cpp\
    target.cpp\   
    working.cpp\
    mainwidget.cpp \
    gamegi.cpp \
    cardgi.cpp \
    wrkdckgi.cpp



HEADERS  += card.hpp\
			CardDeck.hpp\
			CardStack.h\
			FactoryKlondike.hpp\
			GameCore.h\
			target.h\
			working.h \
    mainwidget.h \
    gamegi.h \
    cardgi.h \
    wrkdckgi.h



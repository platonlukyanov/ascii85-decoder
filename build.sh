#!/bin/bash

build_app() {
    g++ -o ascii85 main.cpp
}

build_tests() {
    g++ -DTESTING -o tests test/test.cpp main.cpp \
        -lgtest -lgtest_main -lpthread
}

case "$1" in
    "test") build_tests ;;
    *) build_app ;;
esac

cmake_minimum_required(VERSION 3.13)
project(OperativeSystem C)

set(CMAKE_C_STANDARD 99)

add_executable(OperativeSystem main/include/bthread.h main/include/bthread_private.h main/src/bthread.c main/include/tqueue.h
        main/src/tqueue.c test/src/main.c test/src/bthread_test.c test/src/tqueue_test.c)
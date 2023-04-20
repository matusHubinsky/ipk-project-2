/**
 * @file main.h
 * @author Matus Hubinsky
 * @brief 
 * @date 17.04.2023
 * @version 1.0.1
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "arg.h"
#include "udp.h"

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/ip.h>	

#include <stdlib.h>
#include <stdbool.h>
#include <argp.h>
#include <string.h>

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        #define OS "windows"
    #elif defined(__linux__) || defined(__gnu_linux__)
        #define OS "linux"
    #else 
        #define OS "unknown"
    #endif

#endif

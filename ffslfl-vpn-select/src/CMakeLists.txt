cmake_minimum_required(VERSION 2.8.8)

project(VPN_SELECT C)

set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)

find_library(UCI_LIBRARY NAMES uci)
find_library(UCLIENT_LIBRARY NAMES uclient)

find_package(PkgConfig REQUIRED QUIET)

add_executable(vpn_select
        vpn_select.c
        settings.c
        util.c
        uclient.c
        )

set_property(TARGET vpn_select PROPERTY COMPILE_FLAGS "-std=c99 -Wall")

target_link_libraries(vpn_select
        ${UCI_LIBRARY}
        ${UCLIENT_LIBRARY}
        )

install(TARGETS vpn_select RUNTIME DESTINATION sbin)
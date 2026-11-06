cmake_minimum_required(VERSION 3.16)

project(SpaceShooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Localização da SFML 2.6.1
set(SFML_DIR "C:/Bibliotecas/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(SpaceShooter main.cpp)

# Copia as imagens para a pasta de execução
add_custom_command(TARGET SpaceShooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:SpaceShooter>/assets"
)

target_link_libraries(SpaceShooter
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)

include(GNUInstallDirs)

install(TARGETS SpaceShooter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
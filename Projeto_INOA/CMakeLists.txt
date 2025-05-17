cmake_minimum_required(VERSION 3.10)
project(StockPriceApp LANGUAGES CXX)

# Aponte pro toolchain do vcpkg que você já clonou em C:/dev/vcpkg 
# (ajuste se o seu caminho for outro)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")

# Peça ao CMake para achar as bibliotecas que você instalou
find_package(CURL           REQUIRED)
find_package(nlohmann_json  REQUIRED)

# Seu executável e linkagem
add_executable(Projeto_INOA main.cpp)
target_link_libraries(Projeto_INOA
    PRIVATE
      CURL::libcurl
      nlohmann_json::nlohmann_json
)

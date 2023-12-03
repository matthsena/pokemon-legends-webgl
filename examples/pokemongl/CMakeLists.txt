project(pokemongl)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp window.cpp model.cpp pokemon.cpp pokeball.cpp)
enable_abcg(${PROJECT_NAME})
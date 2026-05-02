#!/bin/bash
# bootstrap.sh – MagistralaCAN3: szkielet projektu (bez README.md)
# Uruchom w katalogu ~/MagistralaCAN3

set -e
echo "🚀 MagistralaCAN3 – inicjalizacja szkieletu projektu"

# Katalogi
mkdir -p src/can src/gui src/sniffer src/ml src/util

# .gitignore
cat > .gitignore << 'GITEOF'
build/
*.o
*.pro.user
.qmake.stash
*.swp
*.swo
GITEOF

# CMakeLists.txt
cat > CMakeLists.txt << 'CMAKE_EOF'
cmake_minimum_required(VERSION 3.16)
project(MagistralaCAN3 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Test)

set(SOURCES
    src/main.cpp
    src/can/can_worker.cpp
    src/can/socketcan_adapter.cpp
    src/gui/can_frame_model.cpp
    src/gui/throttled_frame_model.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core)
CMAKE_EOF

# Tymczasowe pliki źródłowe, aby projekt się kompilował
cat > src/main.cpp << 'MAINEOF'
#include <QApplication>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    return app.exec();
}
MAINEOF

for f in can_worker socketcan_adapter can_frame_model throttled_frame_model; do
    echo "// placeholder" > src/can/${f}.cpp 2>/dev/null || echo "// placeholder" > src/gui/${f}.cpp 2>/dev/null
done

# Dodajemy oryginalny socketcan_adapter z MagistralaCAN2 (jeśli istnieje)
if [ -f ~/MagistralaCAN2/src/can/socketcan_adapter.h ]; then
    cp ~/MagistralaCAN2/src/can/socketcan_adapter.h src/can/
    cp ~/MagistralaCAN2/src/can/socketcan_adapter.cpp src/can/
    echo "✔ Skopiowano SocketCanAdapter z MagistralaCAN2"
fi

git add .
git commit -m "Szkielet projektu MagistralaCAN3"
git branch -M main
git push -u origin main

echo ""
echo "✅ Szkielet gotowy i wypchnięty na GitHub."
echo "   Oczekuj na kolejne skrypty z właściwą implementacją."

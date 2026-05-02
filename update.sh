#!/bin/bash
# fix_lua_filter_include.sh – MagistralaCAN3: dodaje brakujące includy w LuaEngine
# Uruchom w katalogu ~/MagistralaCAN3

set -e
echo "🔧 MagistralaCAN3 – naprawa includów w LuaEngine"

if [ ! -f "src/lua/lua_engine.h" ] || [ ! -f "src/lua/lua_engine.cpp" ]; then
    echo "❌ Nie znaleziono plików LuaEngine. Uruchom w ~/MagistralaCAN3"
    exit 1
fi

cp src/lua/lua_engine.h src/lua/lua_engine.h.bak_fix_$(date +%Y%m%d_%H%M%S)
cp src/lua/lua_engine.cpp src/lua/lua_engine.cpp.bak_fix_$(date +%Y%m%d_%H%M%S)

# 1. Dodaj forward declaration CanFrameModel w nagłówku
if ! grep -q "class CanFrameModel;" src/lua/lua_engine.h; then
    sed -i '/#include "can\/can_interface.h"/a class CanFrameModel;' src/lua/lua_engine.h
    echo "✔ Forward declaration dodana"
fi

# 2. Dodaj #include "gui/can_frame_model.h" w .cpp
if ! grep -q '#include "gui/can_frame_model.h"' src/lua/lua_engine.cpp; then
    sed -i '/#include "lua_engine.h"/a #include "gui/can_frame_model.h"' src/lua/lua_engine.cpp
    echo "✔ include gui/can_frame_model.h dodany"
fi

# 3. Dodaj #include <QColor> w .cpp (jeśli nie ma)
if ! grep -q '#include <QColor>' src/lua/lua_engine.cpp; then
    sed -i '/#include <QDateTime>/a #include <QColor>' src/lua/lua_engine.cpp
    echo "✔ include QColor dodany"
fi

# Kompilacja
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "✅ Kompilacja zakończona. Możesz wczytać scripts/filter_example.lua i testować."

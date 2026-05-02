#include "lua_engine.h"
#include "gui/can_frame_model.h"
#include <QDebug>
#include <QDateTime>
#include <QColor>
#include <QFileInfo>

extern "C" {
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
}

// Makro pomocnicze do pobrania wskaźnika na LuaEngine z rejestru Lua
#define GET_ENGINE(L) \
    lua_getglobal(L, "__engine_ptr"); \
    LuaEngine *engine = reinterpret_cast<LuaEngine*>(lua_touserdata(L, -1)); \
    lua_pop(L, 1);

LuaEngine::LuaEngine(QObject *parent) : QObject(parent) {}

LuaEngine::~LuaEngine() {
    if (L_) lua_close(L_);
}

bool LuaEngine::loadScript(const QString &filePath) {
    if (L_) {
        lua_close(L_);
        L_ = nullptr;
        loaded_ = false;
    }

    L_ = luaL_newstate();
    if (!L_) {
        emit scriptError("Nie można utworzyć stanu Lua");
        return false;
    }

    luaL_openlibs(L_);

    // Zapisujemy wskaźnik this, aby funkcje API miały do niego dostęp
    lua_pushlightuserdata(L_, this);
    lua_setglobal(L_, "__engine_ptr");

    registerApi();

    // Wykonaj plik
    if (luaL_dofile(L_, filePath.toUtf8().constData()) != LUA_OK) {
        QString err = QString::fromUtf8(lua_tostring(L_, -1));
        emit scriptError(err);
        lua_pop(L_, 1);
        lua_close(L_);
        L_ = nullptr;
        return false;
    }

    loaded_ = true;
    QFileInfo fi(filePath);
    emit scriptOutput(QString("Skrypt załadowany: %1").arg(fi.fileName()));
    return true;
}

bool LuaEngine::isLoaded() const {
    return loaded_;
}

void LuaEngine::handleFrame(const CanFrame &frame) {
    if (!L_ || !loaded_) return;

    lua_getglobal(L_, "onFrame");
    if (!lua_isfunction(L_, -1)) {
        lua_pop(L_, 1);
        return;
    }

    // Przygotuj argumenty
    lua_pushinteger(L_, frame.id);
    lua_pushlstring(L_, reinterpret_cast<const char*>(frame.data), frame.dlc);
    double ts = frame.timestamp.tv_sec + frame.timestamp.tv_usec / 1e6;
    lua_pushnumber(L_, ts);

    // Wywołaj z zabezpieczeniem błędu
    if (lua_pcall(L_, 3, 0, 0) != LUA_OK) {
        QString err = QString::fromUtf8(lua_tostring(L_, -1));
        emit scriptError(err);
        lua_pop(L_, 1);
    }
}

void LuaEngine::sendCanFrame(uint32_t id, const QByteArray &data) {
    emit sendFrameRequested(id, data);
}

void LuaEngine::registerApi() {
    lua_register(L_, "log", lua_log);
    lua_register(L_, "sendFrame", lua_sendFrame);
    lua_register(L_, "getTick", lua_getTick);
}

int LuaEngine::lua_log(lua_State *L) {
    GET_ENGINE(L);
    const char *msg = luaL_checkstring(L, 1);
    emit engine->scriptOutput(QString::fromUtf8(msg));
    return 0;
}

int LuaEngine::lua_sendFrame(lua_State *L) {
    GET_ENGINE(L);
    uint32_t id = luaL_checkinteger(L, 1);
    size_t len;
    const char *data = luaL_checklstring(L, 2, &len);
    if (len > 8) len = 8;
    engine->sendCanFrame(id, QByteArray(data, len));
    return 0;
}

int LuaEngine::lua_getTick(lua_State *L) {
    lua_pushnumber(L, QDateTime::currentMSecsSinceEpoch());
    return 1;
}

void LuaEngine::setModel(CanFrameModel *model) {
    model_ = model;
}

void LuaEngine::setColorForId(uint32_t id, int r, int g, int b) {
    if (model_) {
        model_->setColorForId(id, QColor(r, g, b));
    }
}

void LuaEngine::hideId(uint32_t id) {
    if (model_) {
        model_->hideId(id);
    }
}

int LuaEngine::lua_setColor(lua_State *L) {
    GET_ENGINE(L);
    uint32_t id = luaL_checkinteger(L, 1);
    int r = luaL_checkinteger(L, 2);
    int g = luaL_checkinteger(L, 3);
    int b = luaL_checkinteger(L, 4);
    engine->setColorForId(id, r, g, b);
    return 0;
}

int LuaEngine::lua_hideFrame(lua_State *L) {
    GET_ENGINE(L);
    uint32_t id = luaL_checkinteger(L, 1);
    engine->hideId(id);
    return 0;
}

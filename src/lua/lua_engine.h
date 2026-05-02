#ifndef LUA_ENGINE_H
#define LUA_ENGINE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include "can/can_interface.h"
class CanFrameModel;

struct lua_State;

class LuaEngine : public QObject {
    Q_OBJECT
public:
    explicit LuaEngine(QObject *parent = nullptr);
    ~LuaEngine() override;

    bool loadScript(const QString &filePath);
    bool isLoaded() const;

    // Wywołaj callback onFrame dla ramki
    void handleFrame(const CanFrame &frame);

    // Wyślij ramkę CAN z poziomu Lua (wywołuje sygnał sendFrameRequested)
    void sendCanFrame(uint32_t id, const QByteArray &data);
    void setModel(CanFrameModel *model);
    void setColorForId(uint32_t id, int r, int g, int b);
    void hideId(uint32_t id);

signals:
    void scriptOutput(const QString &text);
    void scriptError(const QString &error);
    void sendFrameRequested(uint32_t id, const QByteArray &data);

private:
    void registerApi();
    static int lua_log(lua_State *L);
    static int lua_sendFrame(lua_State *L);
    static int lua_setColor(lua_State *L);
    static int lua_hideFrame(lua_State *L);
    static int lua_getTick(lua_State *L);

    lua_State *L_ = nullptr;
    bool loaded_ = false;
    CanFrameModel *model_ = nullptr;
};

#endif

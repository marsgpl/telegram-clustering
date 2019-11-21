#include "tgnews.h"

static lua_State *globalL = NULL; // for tgnews_on_sigint

int main(int argc, const char **argv) {
    if (argc != 3) fail(USAGE);

    const char *task = argv[1];
    const char *src_dir = argv[2];

    lua_State *L = globalL = luaL_newstate();
    if (L == NULL) fail("luaL_newstate returned NULL");

    signal(SIGINT, tgnews_on_sigint);

    lua_pushboolean(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, "LUA_NOENV");

    luaL_openlibs(L);

    lua_getglobal(L, "package");
    lua_pushstring(L, PACKAGE_PATH);
    lua_setfield(L, -2, "path");
    lua_pushstring(L, PACKAGE_CPATH);
    lua_setfield(L, -2, "cpath");
    lua_pop(L, 1);

    lua_createtable(L, 0, 2);
    lua_pushstring(L, task);
    lua_setfield(L, -2, "task");
    lua_pushstring(L, src_dir);
    lua_setfield(L, -2, "src_dir");
    lua_setglobal(L, "args");

    tgnews_lua_require(L, "class");
    tgnews_lua_require(L, "trace");

    lua_pushcfunction(L, tgnews_lua_call_handler);

    int status = luaL_loadfilex(L, ENTRY_POINT_FILE, "b");
    if (status != LUA_OK) fail_lua(L);

    if (lua_pcall(L, 0, 0, -2) != LUA_OK) fail_lua(L);

    lua_close(L);

    return 0;
}

static int tgnews_lua_call_handler(lua_State *L) {
    const char *msg = lua_tostring(L, 1);

    if (msg == NULL) { // error object is not a string
        if (luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING) {
            // error object produces a string via __tostring metatable method call
            // return this result w/o traceback
            return 1;
        } else {
            msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
        }
    }

    // append traceback to msg
    luaL_traceback(L, L, msg, 1);

    return 1; // return traceback
}

static void tgnews_on_sigint(int sig) {
    signal(sig, SIG_DFL); // if another SIGINT happens, terminate process (double ^C)
    lua_sethook(globalL, tgnews_lua_stop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}

static void tgnews_lua_stop(lua_State *L, lua_Debug *ar) {
    (void)ar; // unused
    lua_sethook(L, NULL, 0, 0); // reset hook
    luaL_error(L, "interrupted!");
}

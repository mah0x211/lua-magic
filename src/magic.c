/**
 *  Copyright (C) 2013 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  magic.c
 *  lua-magic
 *
 *  Created by Masatoshi Teruya on 13/04/09.
 */
#include "magic.h"
#include "config.h"
#include <errno.h>
#include <lauxlib.h>
#include <string.h>

// MARK: lua binding
#define MODULE_MT "magic"

// set function at table-field
#define lstate_fn2tbl(L, k, v)                                                 \
 do {                                                                          \
  lua_pushstring(L, k);                                                        \
  lua_pushcfunction(L, v);                                                     \
  lua_rawset(L, -3);                                                           \
 } while (0)

#define lstate_flag2tbl(L, flg)                                                \
 lua_pushstring(L, #flg);                                                      \
 lua_pushinteger(L, MAGIC_##flg);                                              \
 lua_rawset(L, -3)

typedef struct {
    magic_t mgc;
} lmagic_t;

static int file_lua(lua_State *L)
{
    lmagic_t *magic  = luaL_checkudata(L, 1, MODULE_MT);
    const char *path = luaL_checkstring(L, 2);

    lua_pushstring(L, magic_file(magic->mgc, path));

    return 1;
}

static int descriptor_lua(lua_State *L)
{
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);
    int fd          = luaL_checkinteger(L, 2);

    lua_pushstring(L, magic_descriptor(magic->mgc, fd));

    return 1;
}

static int buffer_lua(lua_State *L)
{
    size_t size;
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);
    const char *buf = luaL_checklstring(L, 2, &size);

    lua_pushstring(L, magic_buffer(magic->mgc, buf, size));

    return 1;
}

static int error_lua(lua_State *L)
{
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);

    lua_pushstring(L, magic_error(magic->mgc));

    return 1;
}

static int setflags_lua(lua_State *L)
{
    int argc        = lua_gettop(L);
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);
    int flgs        = MAGIC_NONE;

    if (argc > 1) {
        int i = 2;

        for (; i <= argc; i++) {
            flgs |= luaL_checkinteger(L, i);
        }
    }

    lua_pushinteger(L, magic_setflags(magic->mgc, flgs));

    return 1;
}

static int load_lua(lua_State *L)
{
    int argc         = lua_gettop(L);
    lmagic_t *magic  = luaL_checkudata(L, 1, MODULE_MT);
    const char *path = DEFAULT_LUA_MAGIC_FILE;

    if (argc > 1) {
        path = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, magic_load(magic->mgc, path));

    return 1;
}

static int compile_lua(lua_State *L)
{
    int argc         = lua_gettop(L);
    lmagic_t *magic  = luaL_checkudata(L, 1, MODULE_MT);
    const char *path = NULL;

    if (argc > 1) {
        path = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, magic_compile(magic->mgc, path));

    return 1;
}

static int check_lua(lua_State *L)
{
    int argc         = lua_gettop(L);
    lmagic_t *magic  = luaL_checkudata(L, 1, MODULE_MT);
    const char *path = DEFAULT_LUA_MAGIC_FILE;

    if (argc > 1) {
        path = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, magic_check(magic->mgc, path));

    return 1;
}

static int list_lua(lua_State *L)
{
    int argc         = lua_gettop(L);
    lmagic_t *magic  = luaL_checkudata(L, 1, MODULE_MT);
    const char *path = DEFAULT_LUA_MAGIC_FILE;

    if (argc > 1) {
        path = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, magic_list(magic->mgc, path));

    return 1;
}

static int errno_lua(lua_State *L)
{
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);

    lua_pushinteger(L, magic_errno(magic->mgc));

    return 1;
}

static int gc_lua(lua_State *L)
{
    lmagic_t *magic = lua_touserdata(L, 1);

    magic_close(magic->mgc);

    return 0;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, MODULE_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

// make error
static int newindex_lua(lua_State *L)
{
    return luaL_error(L, "could not change member of instance");
}

static void define_mt(lua_State *L, struct luaL_Reg mmethod[],
                      struct luaL_Reg method[])
{
    int i = 0;

    // create table __metatable
    luaL_newmetatable(L, MODULE_MT);
    // metamethods
    while (mmethod[i].name) {
        lstate_fn2tbl(L, mmethod[i].name, mmethod[i].func);
        i++;
    }
    // methods
    lua_pushstring(L, "__index");
    lua_newtable(L);
    i = 0;
    while (method[i].name) {
        lstate_fn2tbl(L, method[i].name, method[i].func);
        i++;
    }
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

static int getpath_lua(lua_State *L)
{
    lua_pushstring(L, magic_getpath(DEFAULT_LUA_MAGIC_FILE, 0));

    return 1;
}

static int open_lua(lua_State *L)
{
    int argc           = lua_gettop(L);
    int flgs           = MAGIC_NONE;
    lmagic_t *magic    = NULL;
    magic_t mgc        = NULL;
    const char *errbuf = NULL;

    if (argc > 0) {
        int i = 1;

        for (; i <= argc; i++) {
            flgs |= luaL_checkinteger(L, i);
        }
    }

    if (!(mgc = magic_open(flgs))) {
        errbuf = strerror(errno);
    } else if (!(magic = lua_newuserdata(L, sizeof(lmagic_t)))) {
        errbuf = lua_tostring(L, -1);
        magic_close(mgc);
    } else {
        magic->mgc = mgc;
        luaL_getmetatable(L, MODULE_MT);
        lua_setmetatable(L, -2);
        return 1;
    }

    return luaL_error(L, "failed to magic.open() - %s", errbuf);
}

LUALIB_API int luaopen_magic(lua_State *L)
{
    struct luaL_Reg mmethod[] = {
        {"__gc",       gc_lua      },
        {"__tostring", tostring_lua},
        {"__newindex", newindex_lua},
        {NULL,         NULL        }
    };
    struct luaL_Reg method[] = {
  // method
        {"file",       file_lua      },
        {"descriptor", descriptor_lua},
        {"buffer",     buffer_lua    },
        {"error",      error_lua     },
        {"setFlags",   setflags_lua  },
        {"load",       load_lua      },
        {"compile",    compile_lua   },
        {"check",      check_lua     },
        {"list",       list_lua      },
        {"errno",      errno_lua     },
        {NULL,         NULL          }
    };

    // create metatable
    define_mt(L, mmethod, method);

    // register
    lua_newtable(L);
    // functions
    lstate_fn2tbl(L, "getPath", getpath_lua);
    lstate_fn2tbl(L, "open", open_lua);
    // flags
    lstate_flag2tbl(L, NONE);
    lstate_flag2tbl(L, DEBUG);
    lstate_flag2tbl(L, SYMLINK);
    lstate_flag2tbl(L, COMPRESS);
    lstate_flag2tbl(L, DEVICES);
    lstate_flag2tbl(L, MIME_TYPE);
    lstate_flag2tbl(L, CONTINUE);
    lstate_flag2tbl(L, CHECK);
    lstate_flag2tbl(L, PRESERVE_ATIME);
    lstate_flag2tbl(L, RAW);
    lstate_flag2tbl(L, ERROR);
    lstate_flag2tbl(L, MIME_ENCODING);
    lstate_flag2tbl(L, MIME);
    lstate_flag2tbl(L, APPLE);

    lstate_flag2tbl(L, NO_CHECK_COMPRESS);
    lstate_flag2tbl(L, NO_CHECK_TAR);
    lstate_flag2tbl(L, NO_CHECK_SOFT);
    lstate_flag2tbl(L, NO_CHECK_APPTYPE);
    lstate_flag2tbl(L, NO_CHECK_ELF);
    lstate_flag2tbl(L, NO_CHECK_TEXT);
    lstate_flag2tbl(L, NO_CHECK_CDF);
    lstate_flag2tbl(L, NO_CHECK_TOKENS);
    lstate_flag2tbl(L, NO_CHECK_ENCODING);
    // backwards copatibility(rename)
    lstate_flag2tbl(L, NO_CHECK_ASCII);
    // backwards copatibility(do nothing)
    lstate_flag2tbl(L, NO_CHECK_FORTRAN);
    lstate_flag2tbl(L, NO_CHECK_TROFF);

    return 1;
}

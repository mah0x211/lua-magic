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
#include <lualib.h>
#include <stdio.h>
#include <string.h>

// MARK: lua binding
#define MODULE_MT "magic"

static inline void push_fn2tbl(lua_State *L, const char *k, lua_CFunction f)
{
    lua_pushstring(L, k);
    lua_pushcfunction(L, f);
    lua_rawset(L, -3);
}

static inline void push_int2tbl(lua_State *L, const char *k, lua_Integer v)
{
    lua_pushstring(L, k);
    lua_pushinteger(L, v);
    lua_rawset(L, -3);
}

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

static int filehandle_lua(lua_State *L)
{
    lmagic_t *magic = luaL_checkudata(L, 1, MODULE_MT);

#if LUA_VERSION_NUM >= 502
    luaL_Stream *stream = luaL_checkudata(L, 2, LUA_FILEHANDLE);
    FILE *fp            = stream->f;
#else
    FILE *fp = *(FILE **)luaL_checkudata(L, 2, LUA_FILEHANDLE);
#endif

    lua_pushstring(L, magic_descriptor(magic->mgc, fileno(fp)));

    return 1;
}

static int buffer_lua(lua_State *L)
{
    size_t size     = 0;
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

static int getpath_lua(lua_State *L)
{
    lua_pushstring(L, magic_getpath(DEFAULT_LUA_MAGIC_FILE, 0));

    return 1;
}

static int open_lua(lua_State *L)
{
    int argc        = lua_gettop(L);
    int flgs        = MAGIC_NONE;
    lmagic_t *magic = NULL;

    if (argc > 0) {
        int i = 1;

        for (; i <= argc; i++) {
            flgs |= luaL_checkinteger(L, i);
        }
    }

    magic      = lua_newuserdata(L, sizeof(lmagic_t));
    magic->mgc = magic_open(flgs);
    if (!magic->mgc) {
        return luaL_error(L, "failed to magic.open() - %s", strerror(errno));
    }

    luaL_getmetatable(L, MODULE_MT);
    lua_setmetatable(L, -2);
    return 1;
}

LUALIB_API int luaopen_magic(lua_State *L)
{
    if (luaL_newmetatable(L, MODULE_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {"__newindex", newindex_lua},
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"file",       file_lua      },
            {"descriptor", descriptor_lua},
            {"filehandle", filehandle_lua},
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
        struct luaL_Reg *ptr = mmethod;

        for (; ptr->name; ptr++) {
            push_fn2tbl(L, ptr->name, ptr->func);
        }
        // methods
        lua_pushstring(L, "__index");
        lua_newtable(L);
        for (ptr = method; ptr->name; ptr++) {
            push_fn2tbl(L, ptr->name, ptr->func);
        }
        lua_rawset(L, -3);
        lua_pop(L, 1);
    }

    // export functions and constants
    lua_newtable(L);
    // functions
    push_fn2tbl(L, "getpath", getpath_lua);
    push_fn2tbl(L, "open", open_lua);
    // flags
    push_int2tbl(L, "NONE", MAGIC_NONE);
    push_int2tbl(L, "DEBUG", MAGIC_DEBUG);
    push_int2tbl(L, "SYMLINK", MAGIC_SYMLINK);
    push_int2tbl(L, "COMPRESS", MAGIC_COMPRESS);
    push_int2tbl(L, "DEVICES", MAGIC_DEVICES);
    push_int2tbl(L, "MIME_TYPE", MAGIC_MIME_TYPE);
    push_int2tbl(L, "CONTINUE", MAGIC_CONTINUE);
    push_int2tbl(L, "CHECK", MAGIC_CHECK);
    push_int2tbl(L, "PRESERVE_ATIME", MAGIC_PRESERVE_ATIME);
    push_int2tbl(L, "RAW", MAGIC_RAW);
    push_int2tbl(L, "ERROR", MAGIC_ERROR);
    push_int2tbl(L, "MIME_ENCODING", MAGIC_MIME_ENCODING);
    push_int2tbl(L, "MIME", MAGIC_MIME);
    push_int2tbl(L, "APPLE", MAGIC_APPLE);
    push_int2tbl(L, "EXTENSION", MAGIC_EXTENSION);

    push_int2tbl(L, "NO_CHECK_COMPRESS", MAGIC_NO_CHECK_COMPRESS);
    push_int2tbl(L, "NO_CHECK_TAR", MAGIC_NO_CHECK_TAR);
    push_int2tbl(L, "NO_CHECK_SOFT", MAGIC_NO_CHECK_SOFT);
    push_int2tbl(L, "NO_CHECK_APPTYPE", MAGIC_NO_CHECK_APPTYPE);
    push_int2tbl(L, "NO_CHECK_ELF", MAGIC_NO_CHECK_ELF);
    push_int2tbl(L, "NO_CHECK_TEXT", MAGIC_NO_CHECK_TEXT);
    push_int2tbl(L, "NO_CHECK_CDF", MAGIC_NO_CHECK_CDF);
    push_int2tbl(L, "NO_CHECK_TOKENS", MAGIC_NO_CHECK_TOKENS);
    push_int2tbl(L, "NO_CHECK_ENCODING", MAGIC_NO_CHECK_ENCODING);
    // backwards copatibility(rename)
    push_int2tbl(L, "NO_CHECK_ASCII", MAGIC_NO_CHECK_ASCII);
    // backwards copatibility(do nothing)
    push_int2tbl(L, "NO_CHECK_FORTRAN", MAGIC_NO_CHECK_FORTRAN);
    push_int2tbl(L, "NO_CHECK_TROFF", MAGIC_NO_CHECK_TROFF);

    return 1;
}

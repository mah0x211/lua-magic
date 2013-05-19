/*
 *  Copyright (C) 2013 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */
/*
 *  magic.c
 *  lua-magic
 *
 *  Created by Masatoshi Teruya on 13/04/09.
 *
 */
#include <string.h>
#include <errno.h>
#include <lauxlib.h>
#include "magic.h"

// MARK: lua binding
#define MAGIC_LUA        "magic"

// open table to table-field(nested table)
#define openL_ttable(L,k) \
    lua_pushstring(L,k); \
    lua_newtable(L)

// close table
#define closeL_ttable(L)    lua_rawset(L,-3)

// set function at table-field
#define setL_tfunction(L,k,v) \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3)

#define setL_flag(L,flg) \
    lua_pushstring(L,#flg); \
    lua_pushinteger(L, MAGIC_##flg); \
    lua_rawset(L,-3)

typedef struct {
    magic_t magic;
} MagicLua_t;

static int magic_getpath_lua( lua_State *L )
{
    lua_pushstring( L, magic_getpath( NULL, 0 ) );
    
    return 1;
}

static int magic_open_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    int flgs = MAGIC_NONE;
    magic_t magic = NULL;
    MagicLua_t *wrap = NULL;
    const char *errbuf = NULL;
    
    if( argc > 0 )
    {
        int i = 1;
        
        for(; i <= argc; i++ ){
            flgs |= luaL_checkinteger( L, i );
        }
    }
    
    if( !( magic = magic_open( flgs ) ) ){
        errbuf = strerror( errno );
    }
    else if( !( wrap = lua_newuserdata( L, sizeof( MagicLua_t ) ) ) ){
        errbuf = lua_tostring( L, -1 );
        magic_close( magic );
    }
    else {
        wrap->magic = magic;
        luaL_getmetatable( L, MAGIC_LUA );
        lua_setmetatable( L, -2 );
        return 1;
    }

    return luaL_error( L, "failed to magic.open() - %s", errbuf );
}

static int magic_close_lua( lua_State *L )
{
    MagicLua_t *wrap = lua_touserdata( L, 1 );
    
    magic_close( wrap->magic );
    
    return 0;
}

static int magic_file_lua( lua_State *L )
{
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    const char *path = luaL_checkstring( L, 2 );
    
    lua_pushstring( L, magic_file( wrap->magic, path ) );
    
    return 1;
}

static int magic_descriptor_lua( lua_State *L )
{
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    int fd = luaL_checkinteger( L, 2 );
    
    lua_pushstring( L, magic_descriptor( wrap->magic, fd ) );
    
    return 1;
}

static int magic_error_lua( lua_State *L )
{
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    
    lua_pushstring( L, magic_error( wrap->magic ) );
    
    return 1;
}

static int magic_setflags_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    int flgs = MAGIC_NONE;
    
    if( argc > 1 )
    {
        int i = 2;
        
        for(; i <= argc; i++ ){
            flgs |= luaL_checkinteger( L, i );
        }
    }
    
    lua_pushinteger( L, magic_setflags( wrap->magic, flgs ) );
    
    return 1;
}

static int magic_load_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    const char *path = NULL;
    
    if( argc > 1 ){
        path = luaL_checkstring( L, 2 );
    }
    
    lua_pushinteger( L, magic_load( wrap->magic, path ) );
    
    return 1;
}

static int magic_compile_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    const char *path = NULL;
    
    if( argc > 1 ){
        path = luaL_checkstring( L, 2 );
    }
    
    lua_pushinteger( L, magic_compile( wrap->magic, path ) );
    
    return 1;
}

static int magic_check_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    const char *path = NULL;
    
    if( argc > 1 ){
        path = luaL_checkstring( L, 2 );
    }
    
    lua_pushinteger( L, magic_check( wrap->magic, path ) );
    
    return 1;
}

static int magic_list_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    const char *path = NULL;
    
    if( argc > 1 ){
        path = luaL_checkstring( L, 2 );
    }
    
    lua_pushinteger( L, magic_list( wrap->magic, path ) );
    
    return 1;
}

static int magic_errno_lua( lua_State *L )
{
    MagicLua_t *wrap = luaL_checkudata( L, 1, MAGIC_LUA );
    
    lua_pushinteger( L, magic_errno( wrap->magic ) );
    
    return 1;
}

// make error
static int const_newindex( lua_State *L ){
    return luaL_error( L, "could not change member of instance" );
}
    
LUALIB_API int luaopen_magic( lua_State *L )
{
    int top = lua_gettop( L );
    
    // create metatable
    luaL_newmetatable( L, MAGIC_LUA );
    openL_ttable( L, "__index" );
    setL_tfunction( L, "file", magic_file_lua );
    setL_tfunction( L, "descriptor", magic_descriptor_lua );
    setL_tfunction( L, "error", magic_error_lua );
    setL_tfunction( L, "setFlags", magic_setflags_lua );
    setL_tfunction( L, "load", magic_load_lua );
    setL_tfunction( L, "compile", magic_compile_lua );
    setL_tfunction( L, "check", magic_check_lua );
    setL_tfunction( L, "list", magic_list_lua );
    setL_tfunction( L, "errno", magic_errno_lua );
    closeL_ttable( L );
    setL_tfunction( L, "__gc", magic_close_lua );
    setL_tfunction( L, "__newindex", const_newindex );
    lua_settop( L, top );
    
    // register: flags
    lua_newtable( L );
    setL_flag( L, NONE );
    setL_flag( L, DEBUG );
    setL_flag( L, SYMLINK );
    setL_flag( L, COMPRESS );
    setL_flag( L, DEVICES );
    setL_flag( L, MIME_TYPE );
    setL_flag( L, CONTINUE );
    setL_flag( L, CHECK );
    setL_flag( L, PRESERVE_ATIME );
    setL_flag( L, RAW );
    setL_flag( L, ERROR );
    setL_flag( L, MIME_ENCODING );
    setL_flag( L, MIME );
    setL_flag( L, APPLE );
    
    setL_flag( L, NO_CHECK_COMPRESS );
    setL_flag( L, NO_CHECK_TAR );
    setL_flag( L, NO_CHECK_SOFT );
    setL_flag( L, NO_CHECK_APPTYPE );
    setL_flag( L, NO_CHECK_ELF );
    setL_flag( L, NO_CHECK_TEXT );
    setL_flag( L, NO_CHECK_CDF );
    setL_flag( L, NO_CHECK_TOKENS );
    setL_flag( L, NO_CHECK_ENCODING );
    // backwards copatibility(rename)
    setL_flag( L, NO_CHECK_ASCII );
    // backwards copatibility(do nothing)
    setL_flag( L, NO_CHECK_FORTRAN );
    setL_flag( L, NO_CHECK_TROFF );
    // register: functions
    setL_tfunction( L, "getPath", magic_getpath_lua );
    setL_tfunction( L, "open", magic_open_lua );

    return 1;
}




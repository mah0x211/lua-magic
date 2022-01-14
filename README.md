lua-magic
=========

[![test](https://github.com/mah0x211/lua-magic/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-magic/actions/workflows/test.yml)

**MODULE HAS BEEN RENAMED TO https://github.com/mah0x211/lua-libmagic**

`libmagic` binding for lua.  
see `man libmagic` for more details.

***

## Dependencies

- libmagic: https://github.com/file/file


## Installation

```sh
luarocks install mah0x211/magic
```


## Usage

```lua
local magic = require('magic')
local m = magic.open( magic.MIME_TYPE, magic.NO_CHECK_COMPRESS )
assert(m:load()) -- load the default database

print(m:file('README.md'))  -- output "text/plain"
```

## Constants

`magic` module has the following constants.

- magic.VERSION
- magic.NONE
- magic.DEBUG
- magic.SYMLINK
- magic.COMPRESS
- magic.DEVICES
- magic.MIME_TYPE
- magic.CONTINUE
- magic.CHECK
- magic.PRESERVE_ATIME
- magic.RAW
- magic.ERROR
- magic.MIME_ENCODING
- magic.MIME
- magic.APPLE
- magic.EXTENSION
- magic.COMPRESS_TRANSP
- magic.NODESC
- magic.NO_CHECK_COMPRESS
- magic.NO_CHECK_TAR
- magic.NO_CHECK_SOFT
- magic.NO_CHECK_APPTYPE
- magic.NO_CHECK_ELF
- magic.NO_CHECK_TEXT
- magic.NO_CHECK_CDF
- magic.NO_CHECK_TOKENS
- magic.NO_CHECK_ENCODING
- magic.NO_CHECK_JSON
- magic.NO_CHECK_ASCII
- magic.NO_CHECK_FORTRAN
- magic.NO_CHECK_TROFF


## Create an instance of magic

```lua
local magic = require('magic')
local m = magic.open(magic.MIME_TYPE)
```

## Methods

### str, err = m:file( pathname )

returns a textual description of the contents of the file.

**Parameters**

- `pathname:string`: pathname of the file.

**Returns**

- `str:string`: a textual description.
- `err:string`: error string.


### str, err = m:descriptor( fd )

returns a textual description of the contents of the file descriptor.

**Parameters**

- `fd:integer`: file descriptor.

**Returns**

- `str:string`: a textual description.
- `err:string`: error string.


### str, err = m:filehandle( fh )

returns a textual description of the contents of the filehandle.

**Parameters**

- `fh:filehandle`: lua filehandle.

**Returns**

- `str:string`: a textual description.
- `err:string`: error string.


### str, err= m:buffer( buf:string )

returns a textual description of the contents of the buffer.

**Parameters**

- `buf:string`: buffer string.

**Returns**

- `str:string`: a textual description.
- `err:string`: error string.


### err = m:error()

returns a textual explanation of the last error.

**Returns**

- `err:string`: error string.


### ok = m:setflags( [flag, ...] )

set flags.

**Returns**

- `ok:boolean`: `true` on success.


### ok, err = m:load( [pathnames:string] )

loads the database files from the colon separated list of database files.

**Parameters**

- `pathnames:string`: the colon separated list of database files.

**Returns**

- `ok:boolean`: `true` on success.
- `err:string`: error string.


### ok, err = m:compile( pathnames:string )

compile the colon separated list of database files.

**Parameters**

- `pathnames:string`: the colon separated list of database files.

**Returns**

- `ok:boolean`: `true` on success.
- `err:string`: error string.


### ok, err = m:check( [pathnames:string] )

check the validity of entries in the colon separated database files.

**Parameters**

- `pathnames:string`: the colon separated list of database files.

**Returns**

- `ok:boolean`: `true` on success.
- `err:string`: error string.


### ok, err = m:list( [pathnames:string] )

dumps all magic entries in a human readable format

**Returns**

- `ok:boolean`: `true` on success.
- `err:string`: error string.


### errno = m:errno()

returns the last operating system error number.

**Returns**

- `errno:integer`: system error number (`errno`).


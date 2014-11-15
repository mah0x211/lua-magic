lua-magic
=========

libmagic binding for lua.  
see `man libmagic` for more details.

***

## Dependencies

- libmagic: https://github.com/file/file


## Installation

```sh
luarocks install --from=http://mah0x211.github.io/rocks/ magic
```


## example

    local magic = require('magic');
    local mgc = magic.open( magic.MIME_TYPE, magic.NO_CHECK_COMPRESS );
    local filepath = '/path/to/file';
    local rc = mgc:load();

    if rc ~= 0 then
        print( rc, mgc:error() );
    else
        print( mgc:file( filepath ) );
    end


### constants
    magic.NONE
    magic.DEBUG
    magic.SYMLINK
    magic.COMPRESS
    magic.DEVICES
    magic.MIME_TYPE
    magic.CONTINUE
    magic.CHECK
    magic.PRESERVE_ATIME
    magic.RAW
    magic.ERROR
    magic.MIME_ENCODING
    magic.MIME
    magic.APPLE
    magic.NO_CHECK_COMPRESS
    magic.NO_CHECK_TAR
    magic.NO_CHECK_SOFT
    magic.NO_CHECK_APPTYPE
    magic.NO_CHECK_ELF
    magic.NO_CHECK_TEXT
    magic.NO_CHECK_CDF
    magic.NO_CHECK_TOKENS
    magic.NO_CHECK_ENCODING
    -- backwards copatibility(rename)
    magic.NO_CHECK_ASCII
    -- backwards copatibility(do nothing)
    magic.NO_CHECK_FORTRAN
    magic.NO_CHECK_TROFF

### class methods
    magic.getpath();
    magic.open( ...:constants );

### instance methods
    mgc:file( path:string );
    mgc:descriptor( fd:integer );
    mgc:error();
    mgc:setflags( [...:constants] );
    mgc:load( path:string );
    mgc:compile( path:string );
    mgc:check( path:string );
    mgc:list();
    mgc:errno();


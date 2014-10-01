package = "magic"
version = "1.0-0"
source = {
    url = "git://github.com/mah0x211/lua-magic.git",
    tag = "v1.0.0"
}
description = {
    summary = "bindings for libmagic",
    homepage = "https://github.com/mah0x211/lua-magic", 
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1"
}
external_dependencies = {
    MAGIC = {
        header = "magic.h",
        library = "magic"
    }
}
build = {
    type = "builtin",
    modules = {
        magic = {
            sources = { "magic.c" },
            libraries = { "magic" },
            incdirs = { 
                "$(MAGIC_INCDIR)"
            },
            libdirs = { 
                "$(MAGIC_LIBDIR)",
            }
        }
    }
}


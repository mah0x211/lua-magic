package = "magic"
version = "scm-1"
source = {
    url = "git://github.com/mah0x211/lua-magic.git"
}
description = {
    summary = "bindings for libmagic",
    detailed = [[]],
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


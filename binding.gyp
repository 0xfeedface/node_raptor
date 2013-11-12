{ "targets": [ {
    "target_name": "bindings",
    "variables": {
        "raptor_prefix": "/usr/local"
    },
    "include_dirs": [
        "<(raptor_prefix)/include/raptor2"
    ],
    "sources": [
        "src/bindings.cc",
        "src/parser.cc",
        "src/parser_wrapper.cc",
        "src/serializer.cc",
        "src/serializer_wrapper.cc",
        "src/statement.cc",
        "src/statement_wrapper.cc",
        "src/uri.cc",
        "src/world.cc",
        "src/message.cc"
    ],
    "cflags!": [ "-fno-exceptions" ],
    "cflags_cc!": [ "-std=c++11", "-fno-exceptions" ],
    "link_settings": {
        "libraries": [ "-lraptor2" ]
    },
    "conditions": [ [
        "OS=='mac'", {
            "xcode_settings": {
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "OTHER_CPLUSPLUSFLAGS": [
                    "-std=c++11",
                    "-stdlib=libc++",
                    "-mmacosx-version-min=10.7"
                ]
            }
        }
    ] ]
} ] }

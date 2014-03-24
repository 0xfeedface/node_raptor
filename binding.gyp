{ "targets": [ {
    "target_name": "bindings",
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
    "link_settings": {
        "libraries": [ "-lraptor2" ]
    },
    "conditions": [
        [ "OS!='win'", {
            "variables": {
                "raptor_prefix": "/usr"
            },
        "cflags_cc": [ "-std=c++11", "-fexceptions" ]
        } ],
        [ "OS=='mac'", {
        "variables": {
        "raptor_prefix": "/usr/local"
        },
            "xcode_settings": {
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "OTHER_CPLUSPLUSFLAGS": [
                    "-std=c++11",
                    "-stdlib=libc++",
                    "-mmacosx-version-min=10.7"
                ]
            }
        } ]
    ]
} ] }

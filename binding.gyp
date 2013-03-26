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
        "src/serializer.cc",
        "src/statement.cc"
      ],
      "link_settings": {
        "libraries": [ "-lraptor2" ]
      }
    }
  ]
}

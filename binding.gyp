{
  "targets": [
    {
      "target_name": "bindings",
      "include_dirs": [ "/usr/local/include/raptor2" ],
      "sources": [ "src/bindings.cc", "src/parser.cc", "src/serializer.cc", "src/statement.cc" ],
      "link_settings": {
        "libraries": [ "-lraptor2" ]
      }
    }
  ]
}

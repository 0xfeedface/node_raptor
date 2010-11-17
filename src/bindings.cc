#include <node.h>

#include "parser.h"
#include "serializer.h"

using namespace v8;
using namespace node;

/* extern "C" to prevent name mangling */
extern "C" void init(Handle<Object> target) {
    NODE_SET_METHOD(target, "newParser", Parser::Initialize);
    // NODE_SET_METHOD(target, "newSerializer", Serializer::Initialize);
}

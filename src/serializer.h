#ifndef NODE_RAPTOR_SERIALIZER_H_
#define NODE_RAPTOR_SERIALIZER_H_

#include <node.h>
#include <node_events.h>
#include <raptor.h>

#include "statics.h"

using namespace v8;
using namespace node;

class Serializer : public EventEmitter {
public:
    static Handle<Value> Initialize(const Arguments& args);
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Start(const Arguments& args);
    
    // static Handle<Value> SetNamespace(const Arguments& args);
    static Handle<Value> SerializeStatement(const Arguments& args);
    // static Handle<Value> SerializeDescription(const Arguments& args);
    Serializer(const char* syntax_name);
    ~Serializer();
protected:
    raptor_serializer* serializer_;
    char* syntax_name_;
};

#endif
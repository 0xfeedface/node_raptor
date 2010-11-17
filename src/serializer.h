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
    static Handle<Value> End(const Arguments& args);
    
    static Handle<Value> SetNamespace(const Arguments& args);
    static Handle<Value> SerializeStatement(const Arguments& args);
    // static Handle<Value> SerializeDescription(const Arguments& args);
    Serializer(const char* syntax_name);
    ~Serializer();
protected:
    void Start(const char* filename);
    void End();
    void SetNamespace(const char* prefix, const char* nspace);
    void SerializeStatement(const raptor_statement* statement);
    static Handle<Value>* ExtractArguments(const Arguments& args, Handle<Value>* arguments);
    
    raptor_serializer* serializer_;
    char* syntax_name_;
};

#endif
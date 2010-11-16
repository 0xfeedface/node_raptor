#ifndef NODE_RAPTOR_PARSER_H_
#define NODE_RAPTOR_PARSER_H_

#include <node.h>
#include <node_events.h>
#include <raptor.h>

#include "statics.h"

using namespace v8;
using namespace node;

class Parser : public EventEmitter {
public:
    static Handle<Value> Initialize(const Arguments& args);
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Parse(const Arguments& args);
    static Handle<Value> Abort(const Arguments& args);
    static Handle<Value> SetOption(const Arguments& args);
    static Handle<Value> GetName (Local<String> property, const AccessorInfo& info);
    static void CallbackWrapper(void* user_data, raptor_statement* statement);
    static void CallbackWrapper(void* user_data, raptor_namespace *nspace);
    static void CallbackWrapper(void *user_data, raptor_log_message* message);
    Parser(const char* name);
    ~Parser();
protected:
    void Abort();
    void Parse(const char* filename);
    void DoParse(eio_req* request);
    static Handle<Value>* ExtractArguments(const Arguments& args, Handle<Value>* arguments);
    const char* GetName();
    void SetOption(const char* option_name, const char* string_option_value);
    void SetOption(const char* option_name, const char* string_option_value, bool bool_option_value);
    void StatementHandler(raptor_statement* statement);
    void NamespaceHandler(raptor_namespace *nspace);
    void LogMessageHandler(raptor_log_message* message);
    
    raptor_parser* parser_;
    char* parser_name_;
};

#endif
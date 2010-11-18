#ifndef NODE_RAPTOR_PARSER_H_
#define NODE_RAPTOR_PARSER_H_

#include <node.h>
#include <node_events.h>
#include <raptor.h>

#include "statics.h"

enum parser_state {
    PARSER_STATE_INIT = 0, 
    PARSER_STATE_STARTED, 
    PARSER_STATE_PARSING, 
};

using namespace v8;
using namespace node;

class Parser : public EventEmitter {
public:
    static Handle<Value> Initialize(const Arguments& args);
    static Handle<Value> New(const Arguments& args);
    
    static Handle<Value> ParseFile(const Arguments& args);
    static Handle<Value> ParseURI(const Arguments& args);
    static Handle<Value> ParseStart(const Arguments& args);
    static Handle<Value> ParseBuffer(const Arguments& args);
    
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
    
    void ParseFile(const char* filename, const char* base);
    void ParseURI(const char* uri, const char* base);
    
    void ParseStart(const char* base);
    void ParseBuffer(const char* buffer, size_t buffer_length, bool end);
    
    static Handle<Value>* ExtractArguments(const Arguments& args, Handle<Value>* arguments);
    const char* GetName();
    void SetOption(const char* option_name, const char* string_option_value);
    void SetOption(const char* option_name, const char* string_option_value, bool bool_option_value);
    void StatementHandler(raptor_statement* statement);
    void NamespaceHandler(raptor_namespace *nspace);
    void LogMessageHandler(raptor_log_message* message);
    
    raptor_parser* parser_;
    char* syntax_name_;
    parser_state state_;
};

#endif
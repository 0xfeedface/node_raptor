/*
 * Copyright 2010 Norman Heino <norman.heino@gmail.com>
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef NODE_RAPTOR_PARSER_H_
#define NODE_RAPTOR_PARSER_H_

#include <node.h>
#include <node_object_wrap.h>
//#include <node_events.h>
#include <raptor.h>

#include "statics.h"

enum parser_state {
    PARSER_STATE_INIT = 0, 
    PARSER_STATE_STARTED, 
    PARSER_STATE_PARSING, 
};

using namespace v8;
using namespace node;

//class Parser : public EventEmitter {
class Parser : public ObjectWrap {
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

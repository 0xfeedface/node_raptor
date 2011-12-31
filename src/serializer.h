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

#ifndef NODE_RAPTOR_SERIALIZER_H_
#define NODE_RAPTOR_SERIALIZER_H_

#include <node.h>
//#include <node_events.h>
#include <node_object_wrap.h>
#include <raptor.h>

using namespace v8;
using namespace node;

enum serializer_state {
    SERIALIZER_STATE_INIT = 0, 
    SERIALIZER_STATE_SERIALIZING, 
};

class Serializer : public ObjectWrap {
public:
    static Handle<Value> Initialize(const Arguments& args);
    static Handle<Value> New(const Arguments& args);
    
    static Handle<Value> SerializeToFile(const Arguments& args);
    
    static Handle<Value> SerializeStart(const Arguments& args);
    static Handle<Value> SerializeEnd(const Arguments& args);
    
    static Handle<Value> SetNamespace(const Arguments& args);
    static Handle<Value> SerializeStatement(const Arguments& args);
    // static Handle<Value> SerializeDescription(const Arguments& args);
    static Handle<Value> GetName(Local<String> property, const AccessorInfo& info);
    Serializer(const char* syntax_name);
    ~Serializer();
protected:
    void SerializeToFile(const char* filename);
    void SerializeStart(const char* base);
    void SerializeEnd();
    void SetNamespace(const char* prefix, const char* nspace);
    void SerializeStatement(const raptor_statement* statement);
    const char* GetName();
    static Handle<Value>* ExtractArguments(const Arguments& args, Handle<Value>* arguments);
    
    raptor_serializer* serializer_;
    char* syntax_name_;
    serializer_state state_;
    raptor_iostream* statement_stream_;
    char* statement_string_;
    size_t statement_string_length_;
};

#endif

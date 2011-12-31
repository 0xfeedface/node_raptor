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

#include <cstring>  /* strlen, strcpy */
#include <cstdio>

#include <node_buffer.h>

#include "statics.h"
#include "statement.h"
#include "parser.h"

Handle<Value> Parser::Initialize(const Arguments& args) {
    HandleScope scope;
    
    Handle<FunctionTemplate> t = FunctionTemplate::New(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "parseFile", ParseFile);
    NODE_SET_PROTOTYPE_METHOD(t, "parseURI", ParseURI);
    
    NODE_SET_PROTOTYPE_METHOD(t, "parseStart", ParseStart);
    NODE_SET_PROTOTYPE_METHOD(t, "parseBuffer", ParseBuffer);
    
    NODE_SET_PROTOTYPE_METHOD(t, "abort", Abort);
    NODE_SET_PROTOTYPE_METHOD(t, "setOption", SetOption);
    
    t->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), 
                                       GetName, 
                                       NULL, 
                                       Handle<Value>(), 
                                       DEFAULT, 
                                       ReadOnly);

    Handle<Function> function = t->GetFunction();
    Handle<Value> *arguments = new Handle<Value> [args.Length()];
    ExtractArguments(args, arguments);
    Handle<Object> parser = function->NewInstance(args.Length(), arguments);
    delete arguments;
    
    if (args.Length() == 2 && args[1]->IsFunction()) {
        const int argc = 1;
        Handle<Value> argv[argc] = {parser};
        Handle<Function> callback = Handle<Function>::Cast(args[1]);
        callback->Call(Context::GetCurrent()->Global(), argc, argv);
    }
    
    return scope.Close(parser);
}

Handle<Value> Parser::New(const Arguments& args) {
    HandleScope scope;
    
    
    Handle<String> parser_name;
    if (args.Length() == 0) {
        parser_name = String::New("rdfxml");
    } else {
        parser_name = args[0]->ToString();
    }
    
    String::Utf8Value name(parser_name);
    Parser* parser = new Parser(*name);
    parser->Wrap(args.This());
    
    return scope.Close(args.This());
}

Handle<Value> Parser::ParseFile(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    
    if (!args.Length() >= 1 || !args[0]->IsString()) {
        return ThrowException(Exception::Error(
            String::New("First parameter should be filename.")));
    }
    
    HandleScope scope;
    
    String::Utf8Value filename(args[0]->ToString());
    
    if (args.Length() >= 2 && args[1]->IsString()) {
        String::Utf8Value base(args[1]->ToString());
        parser->ParseFile(*filename, *base);
    } else {
        parser->ParseFile(*filename, NULL);
    }
    
    return Undefined();
}

Handle<Value> Parser::ParseURI(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    
    if (args.Length() == 0 || !args[0]->IsString()) {
        return ThrowException(Exception::Error(
            String::New("First parameter should be URI.")));
    }
    
    HandleScope scope;
    
    String::Utf8Value uri_string(args[0]->ToString());
    
    if (args.Length() >= 2 && args[1]->IsString()) {
        String::Utf8Value base(args[1]->ToString());
        parser->ParseURI(*uri_string, *base);
    } else {
      parser->ParseURI(*uri_string, NULL);
    }
    
    return Undefined();
}

Handle<Value> Parser::ParseStart(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    
    HandleScope scope;
    
    if (!parser->state_ == PARSER_STATE_INIT) {
        return ThrowException(Exception::Error(
            String::New("Parsing already started.")));
    }
    
    if (args.Length() >= 1 && args[0]->IsString()) {
        String::Utf8Value base(args[0]->ToString());
        parser->ParseStart(*base);
    } else {
        parser->ParseStart(NULL);
    }
    
    parser->state_ = PARSER_STATE_STARTED;
    
    return Undefined();
}

Handle<Value> Parser::ParseBuffer(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    
    HandleScope scope;
    
    if (args.Length() >= 1) {
        if (!parser->state_ == PARSER_STATE_STARTED) {
            return ThrowException(Exception::Error(
                String::New("Parsing a buffer, parseStart needs to be called first.")));
        }
        
        // chunk
        Handle<Value> buffer_h = args[0];
        if (!Buffer::HasInstance(buffer_h)) {
            return ThrowException(Exception::TypeError(
                String::New("Argument should be a buffer")));
        }
        
        Handle<Object> buffer_obj = buffer_h->ToObject();
        char* buffer_data = Buffer::Data(buffer_obj);
        size_t buffer_length = Buffer::Length(buffer_obj);
        
        parser->ParseBuffer(buffer_data, buffer_length, false);
        parser->state_ = PARSER_STATE_PARSING;
    } else {
        // end
        if (parser->state_ == PARSER_STATE_PARSING) {
            parser->ParseBuffer(NULL, 0, true);
        }
        
        parser->state_ = PARSER_STATE_INIT;
    }
    
    return Undefined();
}

Handle<Value> Parser::Abort(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    parser->Abort();
    
    return Undefined();
}

Handle<Value> Parser::SetOption(const Arguments& args) {
    if (args.Length() != 2 || !args[0]->IsString() || !(args[1]->IsBoolean() || args[1]->IsString())) {
        // TODO: exception
        return Undefined();
    }
    
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    
    String::Utf8Value optionName(args[0]->ToString());
    if (args[1]->IsString()) {
        String::Utf8Value optionValue(args[1]->ToString());
        parser->SetOption(*optionName, *optionValue);
    } else /* if (args[1]->IsBoolean()) */ {
        parser->SetOption(*optionName, NULL, args[1]->ToBoolean()->Value());
    }
}

Handle<Value> Parser::GetName (Local<String> property, const AccessorInfo& info) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(info.This());
    HandleScope scope;
    
    Handle<String> name = String::New(parser->GetName());
    
    return scope.Close(name);
}

void Parser::CallbackWrapper(void* user_data, raptor_statement* statement) {
    Parser* p = static_cast<Parser*>(user_data);
    p->StatementHandler(statement);
}

void Parser::CallbackWrapper(void* user_data, raptor_namespace* nspace) {
    Parser* p = static_cast<Parser*>(user_data);
    p->NamespaceHandler(nspace);
}

void Parser::CallbackWrapper(void* user_data, raptor_log_message* message) {
    Parser* p = static_cast<Parser*>(user_data);
    p->LogMessageHandler(message);
}

Parser::Parser(const char* name) {
    if (raptor_world_is_parser_name(world, name)) {
        size_t name_len = strlen(name);
        syntax_name_ = new char[name_len+1];
        memcpy(syntax_name_, const_cast<char*>(name), name_len);
        // set termination manually
        syntax_name_[name_len] = '\0';
    } else {
        syntax_name_ = NULL;    // will create default parser
    }
    
    raptor_parser* parser = raptor_new_parser(world, syntax_name_);
    assert(parser != NULL);
    raptor_parser_set_statement_handler(parser, this, Parser::CallbackWrapper);
    raptor_parser_set_namespace_handler(parser, this, Parser::CallbackWrapper);
    
    parser_ = parser;
    
    // log handling per world
    raptor_world_set_log_handler(world, this, Parser::CallbackWrapper);
    
    state_ = PARSER_STATE_INIT;
}

Parser::~Parser() {
    if (parser_) {
        raptor_free_parser(parser_);
    }
    
    if (syntax_name_) {
        delete syntax_name_;
    }
    
    raptor_world_set_log_handler(world, NULL, NULL);
}

void Parser::Abort() {
    if (parser_) {
        raptor_parser_parse_abort(parser_);
    }
}

void Parser::ParseFile(const char* filename, const char* base) {
    unsigned char* file_uri_string = NULL;
    file_uri_string = raptor_uri_filename_to_uri_string(filename);
    
    raptor_uri* file_uri = NULL;
    file_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(file_uri_string));
    
    if (file_uri_string) {
        raptor_free_memory(file_uri_string);
    }
    
    raptor_uri* base_uri = NULL;
    if (base) {
        base_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(base));
    }
    
    assert(parser_ != NULL);
    raptor_parser_parse_file(parser_, file_uri, base_uri);
    
    Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
    assert(emit_v->IsFunction());
    Local<Function> emit_f = emit_v.As<Function>();

    Handle<Value> argv[1] = {
      String::New("end")
    };

    TryCatch tc;

    emit_f->Call(this->handle_, 1, argv);
    
    //Emit(end_symbol, 0, NULL);
    
    if (base_uri) {
        raptor_free_uri(base_uri);
    }
    
    if (file_uri) {
        raptor_free_uri(file_uri);
    }

    if (tc.HasCaught())
      FatalException(tc);

}

void Parser::ParseURI(const char* uri_string, const char* base) {
    raptor_uri* uri = NULL;
    uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(uri_string));
    
    raptor_uri* base_uri = NULL;
    if (base) {
        base_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(base));
    }
    
    assert(parser_ != NULL);
    raptor_parser_parse_uri(parser_, uri, base_uri);
    
    Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
    assert(emit_v->IsFunction());
    Local<Function> emit_f = emit_v.As<Function>();

    Handle<Value> argv[1] = {
      String::New("end")
    };

    TryCatch tc;

    emit_f->Call(this->handle_, 1, argv);

    //Emit(end_symbol, 0, NULL);
    
    if (uri) {
        raptor_free_uri(uri);
    }
    
    if (base_uri) {
        raptor_free_uri(base_uri);
    }

    if (tc.HasCaught())
      FatalException(tc);

}

void Parser::ParseStart(const char* base) {
    raptor_uri* base_uri = NULL;
    base_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(base));
    
    assert(parser_ != NULL);
    raptor_parser_parse_start(parser_, base_uri);
    
    if (base_uri) {
        raptor_free_uri(base_uri);
    }
}

void Parser::ParseBuffer(const char* buffer, size_t buffer_length, bool end) {
    assert(parser_ != NULL);
    raptor_parser_parse_chunk(parser_, reinterpret_cast<const unsigned char*>(buffer), buffer_length, end);
    
    if (end) {

      Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
      assert(emit_v->IsFunction());
      Local<Function> emit_f = emit_v.As<Function>();

      Handle<Value> argv[1] = {
        String::New("end")
      };

      TryCatch tc;

      emit_f->Call(this->handle_, 1, argv);

      if (tc.HasCaught())
        FatalException(tc);

      //Emit(end_symbol, 0, NULL);
    }
}

Handle<Value>* Parser::ExtractArguments(const Arguments& args, Handle<Value>* arguments) {
    for (int i = 0; i < args.Length(); ++i) {
        arguments[i] = args[i];
    }
    
    return arguments;
}

const char* Parser::GetName() {
    return syntax_name_;
}

void Parser::SetOption(const char* option_name, const char* string_option_value) {
    // set option in parser
}

void Parser::SetOption(const char* option_name, const char* string_option_value, bool bool_option_value) {
    // set option in parser
}

void Parser::StatementHandler(raptor_statement* statement) {
    HandleScope scope;

    // unsigned char *s = raptor_term_to_string(statement->subject);
    // printf("New subject %p %s\n", reinterpret_cast<void *>(statement->subject), s);
    
    // get new statement object
    Handle<Object> statement_instance = Statement::NewInstance();
    
    // external objects needs to be copied
    // FIXME: probably leaks
    raptor_statement* statement_copy = raptor_statement_copy(statement);
    Handle<External> statement_instance_ptr = External::New(statement_copy);
    statement_instance->SetInternalField(0, statement_instance_ptr);
    
    //Handle<Value> args[1];
    //args[0] = statement_instance;
    
    Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
    assert(emit_v->IsFunction());
    Local<Function> emit_f = emit_v.As<Function>();

    Handle<Value> argv[2] = {
      String::New("statement"),
      statement_instance
    };

    TryCatch tc;

    emit_f->Call(this->handle_, 2, argv);

    if (tc.HasCaught())
      FatalException(tc);

    //Emit(stmt_symbol, 1, args);
}

void Parser::NamespaceHandler(raptor_namespace* nspace) {
    raptor_uri* namespace_uri = raptor_namespace_get_uri(nspace);
    unsigned char* namespace_uri_string = raptor_uri_to_string(namespace_uri);
    
    size_t length;
    const unsigned char* namespace_prefix = raptor_namespace_get_counted_prefix(nspace, &length);
    
    HandleScope scope;
    
    //Handle<Value> args[2];
    //args[0] = String::New(reinterpret_cast<const char*>(namespace_prefix), length);
    //args[1] = String::New(reinterpret_cast<const char*>(namespace_uri_string));
    
    if (namespace_uri_string) {
        raptor_free_memory(namespace_uri_string);
    }
    
    Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
    assert(emit_v->IsFunction());
    Local<Function> emit_f = emit_v.As<Function>();

    Handle<Value> argv[3] = {
      String::New("namespace"),
      String::New(reinterpret_cast<const char*>(namespace_prefix), length),
      String::New(reinterpret_cast<const char*>(namespace_uri_string))
    };

    TryCatch tc;

    emit_f->Call(this->handle_, 3, argv);

    if (tc.HasCaught())
      FatalException(tc);

    //Emit(nmspc_symbol, 2, args);
}

void Parser::LogMessageHandler(raptor_log_message* message) {
    HandleScope scope;
    
    Handle<String> type;
    switch (message->level) {
    case RAPTOR_LOG_LEVEL_DEBUG:
        type = debug_symbol;
        break;
    case RAPTOR_LOG_LEVEL_INFO:
        type = info_symbol;
        break;
    case RAPTOR_LOG_LEVEL_WARN:
        type = warn_symbol;
        break;
    case RAPTOR_LOG_LEVEL_ERROR:
        type = error_symbol;
        break;
    case RAPTOR_LOG_LEVEL_FATAL:
        type = fatal_symbol;
        break;
    }
    
    //Handle<Value> args[3];
    //args[0] = type;
    //args[1] = String::New(message->text);
    //args[2] = Integer::New(message->code);
    

    Local<Value> emit_v = this->handle_->Get(String::NewSymbol("emit"));
    assert(emit_v->IsFunction());
    Local<Function> emit_f = emit_v.As<Function>();

    Handle<Value> argv[4] = {
      String::New("namespace"),
      type,
      String::New(message->text),
      Integer::New(message->code)
    };

    TryCatch tc;

    emit_f->Call(this->handle_, 4, argv);

    if (tc.HasCaught())
      FatalException(tc);

    //Emit(message_symbol, 3, args);
}

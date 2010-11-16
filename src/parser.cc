#include <cstring>  /* strlen, strcpy */

#include "statics.h"
#include "statement.h"
#include "parser.h"

Handle<Value> Parser::Initialize(const Arguments& args) {
    HandleScope scope;

    Handle<FunctionTemplate> t = FunctionTemplate::New(New);

    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "parse", Parse);
    NODE_SET_PROTOTYPE_METHOD(t, "abort", Abort);
    NODE_SET_PROTOTYPE_METHOD(t, "setOption", SetOption);
    
    t->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), GetName);

    Handle<Function> function = t->GetFunction();
    Handle<Value> arguments[args.Length()];
    ExtractArguments(args, arguments);
    Handle<Object> parser = function->NewInstance(args.Length(), arguments);
    
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

Handle<Value> Parser::Parse(const Arguments& args) {
    Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
    HandleScope scope;
    
    if (!args.Length() == 1 || !args[0]->IsString()) {
        return Undefined();
    }
    
    Handle<String> file = args[0]->ToString();
    
    String::Utf8Value filename(file);
    parser->Parse(*filename);
    
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
    Parser *p = static_cast<Parser*>(user_data);
    p->StatementHandler(statement);
}

void Parser::CallbackWrapper(void* user_data, raptor_namespace *nspace) {
    Parser *p = static_cast<Parser*>(user_data);
    p->NamespaceHandler(nspace);
}

void Parser::CallbackWrapper(void *user_data, raptor_log_message* message) {
    Parser *p = static_cast<Parser*>(user_data);
    p->LogMessageHandler(message);
}

Parser::Parser(const char* name) {
    // keep parser name; actual parser created lazily
    parser_name_ = new char[strlen(name)];
    strcpy(parser_name_, const_cast<char*>(name));
}

Parser::~Parser() {
    raptor_free_parser(parser_);
    delete parser_name_;
    raptor_world_set_log_handler(world, NULL, NULL);
}

void Parser::Abort() {
    if (parser_) {
        raptor_parser_parse_abort(parser_);
    }
}

void Parser::Parse(const char* filename) {
    /* TODO: parse asynchonously */
    raptor_parser* parser = raptor_new_parser(world, parser_name_);
    parser_ = parser;
    raptor_parser_set_statement_handler(parser, this, Parser::CallbackWrapper);
    raptor_parser_set_namespace_handler(parser, this, Parser::CallbackWrapper);
    
    raptor_world_set_log_handler(world, this, Parser::CallbackWrapper);
    
    uint8_t* file_uri_string = raptor_uri_filename_to_uri_string(filename);
    raptor_uri* file_uri = raptor_new_uri(world, file_uri_string);
    
    if (file_uri) {
        raptor_parser_parse_file(parser, file_uri, NULL);
        raptor_free_uri(file_uri);
        file_uri = NULL;
    }
    
    if (file_uri_string) {
        raptor_free_memory(file_uri_string);
    }

    
    Emit(end_symbol, 0, NULL);
}

void Parser::DoParse(eio_req* request) {
    
}

Handle<Value>* Parser::ExtractArguments(const Arguments& args, Handle<Value>* arguments) {
    for (int i = 0; i < args.Length(); ++i) {
        arguments[i] = args[i];
    }
    
    return arguments;
}

const char* Parser::GetName() {
    return parser_name_;
}

void Parser::SetOption(const char* option_name, const char* string_option_value) {
    // set option in parser
}

void Parser::SetOption(const char* option_name, const char* string_option_value, bool bool_option_value) {
    // set option in parser
}

void Parser::StatementHandler(raptor_statement* statement) {
    HandleScope scope;
    
    // get new statement object
    Handle<Object> statement_instance = Statement::NewInstance();
    
    // external objects needs to be copied
    // FIXME: probably leakes
    raptor_statement* statement_copy = raptor_statement_copy(statement);
    Handle<External> statement_instance_ptr = External::New(statement_copy);
    statement_instance->SetInternalField(0, statement_instance_ptr);
    
    Handle<Value> args[1];
    args[0] = statement_instance;
    
    Emit(stmt_symbol, 1, args);
}

void Parser::NamespaceHandler(raptor_namespace *nspace) {
    raptor_uri* namespace_uri = raptor_namespace_get_uri(nspace);
    unsigned char* namespace_uri_string = raptor_uri_to_string(namespace_uri);
    
    size_t length;
    const unsigned char* namespace_prefix = raptor_namespace_get_counted_prefix(nspace, &length);
    
    HandleScope scope;
    
    Handle<Value> args[2];
    args[0] = String::New(reinterpret_cast<const char*>(namespace_prefix), length);
    args[1] = String::New(reinterpret_cast<const char*>(namespace_uri_string));
    
    if (namespace_uri_string) {
        raptor_free_memory(namespace_uri_string);
    }
    
    Emit(nmspc_symbol, 2, args);
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
    
    Handle<Value> args[3];
    args[0] = type;
    args[1] = String::New(message->text);
    args[2] = Integer::New(message->code);
    
    Emit(message_symbol, 3, args);
}

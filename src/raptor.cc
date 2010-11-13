#include <node.h>
#include <node_events.h>
#include <raptor.h>
#include <cstring>
#include <cstdio>

using namespace v8;
using namespace node;

static Persistent<String> subject_symbol  = NODE_PSYMBOL("subject");
static Persistent<String> pred_symbol     = NODE_PSYMBOL("predicate");
static Persistent<String> object_symbol   = NODE_PSYMBOL("object");
static Persistent<String> type_symbol     = NODE_PSYMBOL("type");
static Persistent<String> value_symbol    = NODE_PSYMBOL("value");
static Persistent<String> uri_symbol      = NODE_PSYMBOL("uri");
static Persistent<String> literal_symbol  = NODE_PSYMBOL("literal");
static Persistent<String> tliteral_symbol = NODE_PSYMBOL("typed-literal");
static Persistent<String> bnode_symbol    = NODE_PSYMBOL("bnode");
static Persistent<String> dtype_symbol    = NODE_PSYMBOL("datatype");
static Persistent<String> lang_symbol     = NODE_PSYMBOL("lang");
static Persistent<String> stmt_symbol     = NODE_PSYMBOL("statement");
static Persistent<String> nmspc_symbol    = NODE_PSYMBOL("namespace");
static Persistent<String> message_symbol  = NODE_PSYMBOL("message");
static Persistent<String> end_symbol      = NODE_PSYMBOL("end");
static Persistent<String> debug_symbol    = NODE_PSYMBOL("debug");
static Persistent<String> info_symbol     = NODE_PSYMBOL("info");
static Persistent<String> warn_symbol     = NODE_PSYMBOL("warning");
static Persistent<String> error_symbol    = NODE_PSYMBOL("error");
static Persistent<String> fatal_symbol    = NODE_PSYMBOL("fatal");

static raptor_world* world = raptor_new_world();

/**
 * Raptor statement wrapper class
 */
class Statement {
public:
    static Handle<Object> NewInstance() {
        HandleScope scope;

        if (template_.IsEmpty()) {
            Handle<ObjectTemplate> t = ObjectTemplate::New();
            t->SetInternalFieldCount(1);
            t->SetAccessor(subject_symbol, SubjectAccessor);
            t->SetAccessor(pred_symbol, PredicateAccessor);
            t->SetAccessor(object_symbol, ObjectAccessor);
            
            NODE_SET_METHOD(t, "toString", ToString);

            template_ = Persistent<ObjectTemplate>::New(t);
        }
        
        Handle<Object> i = template_->NewInstance();

        return scope.Close(i);
    }
    
    static Handle<Value> ToString(const Arguments& args) {
        HandleScope scope;
        
        // get raptor_statement out of info
        Handle<External> field = Handle<External>::Cast(args.Holder()->GetInternalField(0));
        raptor_statement* statement = static_cast<raptor_statement*>(field->Value());
        
        raptor_iostream* iostream;
        void* statement_string = NULL;
        size_t statement_string_len;
        iostream = raptor_new_iostream_to_string(statement->world, 
                                                 &statement_string, 
                                                 &statement_string_len, 
                                                 malloc);
        if (!iostream) {
            ThrowException(String::New("Error serializing statement."));
            return Undefined();
        }
        
        int ret_val;
        ret_val = raptor_statement_ntriples_write(statement, iostream, 0);
        raptor_free_iostream(iostream);
        
        if (ret_val > 0) {
            if (statement_string) {
                free(statement_string);
                statement_string = NULL;
            }
        }
        
        Handle<String> result;
        if (statement_string) {
            result = String::New(reinterpret_cast<char*>(statement_string), 
                                 statement_string_len - 1 /* remove trailing newline */);
            free(statement_string);
            statement_string = NULL;
        }
        
        return scope.Close(result);
    }
    
    static Handle<Value> SubjectAccessor(Local<String> property, const AccessorInfo& info) {
        HandleScope scope;
        
        Handle<Object> result = Object::New();
        
        // get raptor_statement out of info
        Handle<External> field = Handle<External>::Cast(info.Holder()->GetInternalField(0));
        raptor_statement* statement = static_cast<raptor_statement*>(field->Value());
        
        // get value out of statement
        raptor_term_value* subject_term = &statement->subject->value;
        
        Handle<String> type;
        Handle<String> value;
        
        raptor_uri* uri_value;
        raptor_term_blank_value* blank_value;
        
        if (statement->subject->type == RAPTOR_TERM_TYPE_URI) {
            uri_value = subject_term->uri;
            value = String::New(reinterpret_cast<char*>(raptor_uri_as_string(uri_value)));
            type = uri_symbol;
        } else if (statement->subject->type == RAPTOR_TERM_TYPE_BLANK) {
            blank_value = &subject_term->blank;
            value = String::New(reinterpret_cast<char*>(blank_value->string), 
                                                        blank_value->string_len);
            type = bnode_symbol;
        }
        
        result->Set(type_symbol, type);
        result->Set(value_symbol, value);
        
        return scope.Close(result);
    }
    
    static Handle<Value> PredicateAccessor(Local<String> property, const AccessorInfo& info) {
        HandleScope scope;
        
        Handle<Object> result = Object::New();
        
        // get raptor_statement out of info
        Handle<External> field = Handle<External>::Cast(info.Holder()->GetInternalField(0));
        raptor_statement* statement = static_cast<raptor_statement*>(field->Value());
        
        // get value out of statement
        raptor_term_value* predicate_term = &statement->predicate->value;
        raptor_uri* uri_value = predicate_term->uri;
        Handle<String> value = String::New(reinterpret_cast<char*>(raptor_uri_as_string(uri_value)));
        
        result->Set(type_symbol, uri_symbol);
        result->Set(value_symbol, value);
        
        return scope.Close(result);
    }
    
    static Handle<Value> ObjectAccessor(Local<String> property, const AccessorInfo& info) {
        HandleScope scope;
        
        Handle<Object> result = Object::New();
        
        // get raptor_statement out of info
        Handle<External> field = Handle<External>::Cast(info.Holder()->GetInternalField(0));
        raptor_statement* statement = static_cast<raptor_statement*>(field->Value());
        
        // get value out of statement
        raptor_term_value* object_term = &statement->object->value;
        
        Handle<String> type;
        Handle<String> value;
        
        raptor_uri* uri_value;
        raptor_term_literal_value* literal_value;
        raptor_term_blank_value* blank_value;
        
        switch (statement->object->type) {
        case RAPTOR_TERM_TYPE_URI:
            uri_value = object_term->uri;
            value = String::New(reinterpret_cast<char*>(raptor_uri_as_string(uri_value)));
            type = uri_symbol;
            break;
        case RAPTOR_TERM_TYPE_LITERAL:
            literal_value = &object_term->literal;
            value = String::New(reinterpret_cast<char*>(literal_value->string), 
                                literal_value->string_len);
            type = literal_symbol;
            
            if (literal_value->datatype) {
                raptor_uri* datatype_uri = literal_value->datatype;
                unsigned char* datatype_uri_string = raptor_uri_to_string(datatype_uri);
                result->Set(dtype_symbol, String::New(reinterpret_cast<char*>(datatype_uri_string)));
                if (datatype_uri_string) {
                    raptor_free_memory(datatype_uri_string);
                    datatype_uri_string = NULL;
                }
                
                
                type = tliteral_symbol;
            } else if (literal_value->language) {
                result->Set(lang_symbol, String::New(reinterpret_cast<char*>(literal_value->language), 
                                                     literal_value->language_len));
            }
            break;
        case RAPTOR_TERM_TYPE_BLANK:
            blank_value = &object_term->blank;
            value = String::New(reinterpret_cast<char*>(blank_value->string), 
                                                        blank_value->string_len);
            type = bnode_symbol;
            break;
        }
        
        result->Set(type_symbol, type);
        result->Set(value_symbol, value);
        
        return scope.Close(result);
    }
private:
    static Handle<ObjectTemplate> template_;
};

Handle<ObjectTemplate> Statement::template_;

/**
 * Raptor parser wrapper class
 */
class Parser : public EventEmitter {
public:
    static Handle<Value> Initialize(const Arguments& args) {
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
    
    static Handle<Value> New(const Arguments& args) {
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
    
    static Handle<Value> Parse(const Arguments& args) {
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
    
    static Handle<Value> Abort(const Arguments& args) {
        Parser* parser = ObjectWrap::Unwrap<Parser>(args.This());
        parser->Abort();
        
        return Undefined();
    }
    
    static Handle<Value> SetOption(const Arguments& args) {
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
    
    static Handle<Value> GetName (Local<String> property, const AccessorInfo& info) {
        Parser* parser = ObjectWrap::Unwrap<Parser>(info.This());
        HandleScope scope;
        
        Handle<String> name = String::New(parser->GetName());
        
        return scope.Close(name);
    }
    
    static void CallbackWrapper(void* user_data, raptor_statement* statement) {
        Parser *p = static_cast<Parser*>(user_data);
        p->StatementHandler(statement);
    }
    
    static void CallbackWrapper(void* user_data, raptor_namespace *nspace) {
        Parser *p = static_cast<Parser*>(user_data);
        p->NamespaceHandler(nspace);
    }
    
    static void CallbackWrapper(void *user_data, raptor_log_message* message) {
        Parser *p = static_cast<Parser*>(user_data);
        p->LogMessageHandler(message);
    }
    
    Parser(const char* name) {
        // keep parser name; actual parser created lazily
        parser_name_ = new char[strlen(name)];
        strcpy(parser_name_, const_cast<char*>(name));
    }
    
    ~Parser() {
        raptor_free_parser(parser_);
        delete parser_name_;
        raptor_world_set_log_handler(world, NULL, NULL);
    }
protected:
    raptor_parser* parser_;
    char* parser_name_;
    
    void Abort() {
        if (parser_) {
            raptor_parser_parse_abort(parser_);
        }
    }
    
    void Parse(const char* filename) {
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
    
    void DoParse(eio_req* request) {
        
    }
    
    static Handle<Value>* ExtractArguments(const Arguments& args, Handle<Value>* arguments) {
        for (int i = 0; i < args.Length(); ++i) {
            arguments[i] = args[i];
        }
        
        return arguments;
    }
    
    const char* GetName() {
        return parser_name_;
    }
    
    void SetOption(const char* option_name, const char* string_option_value) {
        // set option in parser
    }
    
    void SetOption(const char* option_name, const char* string_option_value, bool bool_option_value) {
        // set option in parser
    }
    
    void StatementHandler(raptor_statement* statement) {
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
    
    void NamespaceHandler(raptor_namespace *nspace) {
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
    
    void LogMessageHandler(raptor_log_message* message) {
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
};

/* extern "C" to prevent name mangling */
extern "C" void init(Handle<Object> target) {
    NODE_SET_METHOD(target, "newParser", Parser::Initialize);
    // NODE_SET_METHOD(target, "newParserForContent", Raptor::newParserForContent);
}
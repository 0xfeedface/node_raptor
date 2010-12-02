#include <cstring>  /* strlen, strcpy */
#include <cstdlib>

#include "serializer.h"
#include "statement.h"
#include "statics.h"

Handle<Value> Serializer::Initialize(const Arguments& args) {
    HandleScope scope;

    Handle<FunctionTemplate> t = FunctionTemplate::New(New);

    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "serializeToFile", SerializeToFile);
    NODE_SET_PROTOTYPE_METHOD(t, "serializeStart", SerializeStart);
    NODE_SET_PROTOTYPE_METHOD(t, "serializeEnd", SerializeEnd);
    NODE_SET_PROTOTYPE_METHOD(t, "setNamespace", SetNamespace);
    NODE_SET_PROTOTYPE_METHOD(t, "serializeStatement", SerializeStatement);
    // NODE_SET_PROTOTYPE_METHOD(t, "setOption", SetOption);
    
    t->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), 
                                       GetName, 
                                       NULL, 
                                       Handle<Value>(), 
                                       DEFAULT, 
                                       ReadOnly);

    Handle<Function> function = t->GetFunction();
    Handle<Value> arguments[args.Length()];
    ExtractArguments(args, arguments);
    Handle<Object> serializer = function->NewInstance(args.Length(), arguments);
    
    if (args.Length() == 2 && args[1]->IsFunction()) {
        const int argc = 1;
        Handle<Value> argv[argc] = {serializer};
        Handle<Function> callback = Handle<Function>::Cast(args[1]);
        callback->Call(Context::GetCurrent()->Global(), argc, argv);
    }
    
    return scope.Close(serializer);
}

Handle<Value> Serializer::New(const Arguments& args) {
    HandleScope scope;
    
    Handle<String> syntax_name;
    if (args.Length() == 0) {
        syntax_name = String::New("rdfxml");
    } else {
        syntax_name = args[0]->ToString();
    }
    
    String::Utf8Value name(syntax_name);
    Serializer* serializer = new Serializer(*name);
    serializer->Wrap(args.This());
    
    return scope.Close(args.This());
}

Handle<Value> Serializer::SerializeToFile(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    if (serializer->state_ != SERIALIZER_STATE_INIT) {
        return ThrowException(Exception::Error(
            String::New("Previous serialization needs to be finished first.")));
    }
    
    if (!args.Length() >= 1 || !args[0]->IsString()) {
        return ThrowException(Exception::Error(
            String::New("First parameter should be filename.")));
    }
    
    String::Utf8Value filename(args[0]->ToString());
    
    serializer->SerializeToFile(*filename);
    
    serializer->state_ = SERIALIZER_STATE_SERIALIZING;
    
    return Undefined();
}

Handle<Value> Serializer::SerializeStart(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    if (serializer->state_ != SERIALIZER_STATE_INIT) {
        return ThrowException(Exception::Error(
            String::New("Previous serialization needs to be finished first.")));
    }
    
    if (!args.Length() == 1 || !args[0]->IsString()) {
        // return ThrowException(Exception::Error(
        //     String::New("First parameter should be base URI.")));
        serializer->SerializeStart(NULL);
    } else {
        Handle<String> base_h = args[0]->ToString();
        String::Utf8Value base(base_h);
        serializer->SerializeStart(*base);
    }
    
    serializer->state_ = SERIALIZER_STATE_SERIALIZING;
    
    return Undefined();
}

Handle<Value> Serializer::SerializeEnd(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    serializer->SerializeEnd();
    
    serializer->state_ = SERIALIZER_STATE_INIT;
    
    return Undefined();
}

Handle<Value> Serializer::SetNamespace(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    if (!args.Length() == 2 || !args[0]->IsString() || !args[1]->IsString()) {
        return Undefined();
    }
    
    Handle<String> prefix = args[0]->ToString();
    Handle<String> nspace = args[1]->ToString();
    
    String::Utf8Value prefix_s(prefix);
    String::Utf8Value nspace_s(nspace);
    
    serializer->SetNamespace(*prefix_s, *nspace_s);
    
    return Undefined();
}

Handle<Value> Serializer::SerializeStatement(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    Handle<Value> exception;
    
    if (!args.Length() == 1 || !args[0]->IsObject()) {
        ThrowException(Exception::Error(String::New("First should be a JSON triples structure.")));
        return Undefined();
    }
    
    Handle<Object> statement_object = args[0]->ToObject();
    
    if (!statement_object->HasRealNamedProperty(subject_symbol)) {
        ThrowException(Exception::Error(String::New("Statement is missing 'subject' key.")));
        return Undefined();
    }
    
    if (!statement_object->HasRealNamedProperty(pred_symbol)) {
        ThrowException(Exception::Error(String::New("Statement is missing 'predicate' key.")));
        return Undefined();
    }
    
    if (!statement_object->HasRealNamedProperty(object_symbol)) {
        ThrowException(Exception::Error(String::New("Statement is missing 'object' key.")));
        return Undefined();
    }
    
    raptor_statement* statement = Statement::ConvertObjectToRaptorStatement(statement_object);
    
    if (!statement) {
        ThrowException(Exception::Error(String::New("Unable to create statement.")));
        return Undefined();
    }
    
    serializer->SerializeStatement(statement);
    
    raptor_free_statement(statement);
    
    return Undefined();
}

Handle<Value> Serializer::GetName(Local<String> property, const AccessorInfo& info) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(info.This());
    HandleScope scope;
    
    Handle<String> name = String::New(serializer->GetName());
    
    return scope.Close(name);
}

Serializer::Serializer(const char* syntax_name) {
    // keep syntax name; actual serializer is created lazily
    syntax_name_ = new char[strlen(syntax_name)];
    state_ = SERIALIZER_STATE_INIT;
    strcpy(syntax_name_, const_cast<char*>(syntax_name));
}

Serializer::~Serializer() {
    raptor_free_serializer(serializer_);
    delete syntax_name_;
    raptor_world_set_log_handler(world, NULL, NULL);
}

void Serializer::SerializeToFile(const char* filename) {
    serializer_ = raptor_new_serializer(world, syntax_name_);
    raptor_serializer_start_to_filename(serializer_, filename);
}

void Serializer::SerializeStart(const char* base) {
    serializer_ = raptor_new_serializer(world, syntax_name_);
    
    raptor_iostream* iostream = NULL;
    iostream = raptor_new_iostream_to_string(world, 
                                             reinterpret_cast<void**>(&statement_string_), 
                                             &statement_string_length_, 
                                             malloc);
    if (!iostream) {
        return;/* ThrowException(Exception::Error(
            String::New("Error creating serialization stream."))); */
    }
    
    raptor_uri* base_uri = NULL;
    if (base) {
        base_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(base));
    }
    
    raptor_serializer_start_to_iostream(serializer_, base_uri, iostream);
    
    if (base_uri) {
        raptor_free_uri(base_uri);
    }
    
    statement_stream_ = iostream;
}

void Serializer::SerializeEnd() {
    raptor_serializer_serialize_end(serializer_);
    if (statement_stream_) {
        raptor_free_iostream(statement_stream_);
    }
    
    HandleScope scope;
    
    // TODO: make external?
    Handle<String> data;
    if (statement_string_) {
        data = String::New(reinterpret_cast<char*>(statement_string_), 
                           statement_string_length_);
        free(statement_string_);
        statement_string_ = NULL;
    }
    
    Handle<Value> args[1];
    args[0] = data;
    
    // TODO: serialize in chunks w/ raptor_new_iostream_from_handler?
    Emit(data_symbol, 1, args);
    Emit(end_symbol, 0, NULL);
}

void Serializer::SetNamespace(const char* prefix, const char* nspace) {
    raptor_uri* nspace_uri = raptor_new_uri(world, reinterpret_cast<const unsigned char*>(nspace));
    raptor_serializer_set_namespace(serializer_, 
                                    nspace_uri, 
                                    reinterpret_cast<const unsigned char*>(prefix));
}

void Serializer::SerializeStatement(const raptor_statement* statement) {
    raptor_serializer_serialize_statement(serializer_, const_cast<raptor_statement*>(statement));
}

Handle<Value>* Serializer::ExtractArguments(const Arguments& args, Handle<Value>* arguments) {
    for (int i = 0; i < args.Length(); ++i) {
        arguments[i] = args[i];
    }
    
    return arguments;
}

const char* Serializer::GetName() {
    return syntax_name_;
}

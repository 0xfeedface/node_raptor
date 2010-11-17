#include <cstring>  /* strlen, strcpy */

#include "serializer.h"
#include "statement.h"

Handle<Value> Serializer::Initialize(const Arguments& args) {
    HandleScope scope;

    Handle<FunctionTemplate> t = FunctionTemplate::New(New);

    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "start", Start);
    NODE_SET_PROTOTYPE_METHOD(t, "end", End);
    NODE_SET_PROTOTYPE_METHOD(t, "setNamespace", SetNamespace);
    NODE_SET_PROTOTYPE_METHOD(t, "serializeStatement", SerializeStatement);
    // NODE_SET_PROTOTYPE_METHOD(t, "setOption", SetOption);
    
    // t->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), GetName);

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

Handle<Value> Serializer::Start(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    if (!args.Length() == 1 || !args[0]->IsString()) {
        return Undefined();
    }
    
    Handle<String> file = args[0]->ToString();
    
    String::Utf8Value filename(file);
    serializer->Start(*filename);
    
    return Undefined();
}

Handle<Value> Serializer::End(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    serializer->End();
    
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
    
    if (!args.Length() == 1 || !args[0]->IsObject()) {
        return Undefined();
    }
    
    raptor_statement* statement = Statement::ConvertObjectToRaptorStatement(args[0]->ToObject());
    if (statement) {
        serializer->SerializeStatement(statement);
    }
    
    return Undefined();
}

Serializer::Serializer(const char* syntax_name) {
    // keep syntax name; actual serializer is created lazily
    syntax_name_ = new char[strlen(syntax_name)];
    strcpy(syntax_name_, const_cast<char*>(syntax_name));
}

Serializer::~Serializer() {
    raptor_free_serializer(serializer_);
    delete syntax_name_;
    raptor_world_set_log_handler(world, NULL, NULL);
}

void Serializer::Start(const char* filename) {
    serializer_ = raptor_new_serializer(world, syntax_name_);
    raptor_serializer_start_to_filename(serializer_, filename);
}

void Serializer::End() {
    raptor_serializer_serialize_end(serializer_);
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

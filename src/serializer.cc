#include "serializer.h"

static Handle<Value> Serializer::Initialize(const Arguments& args) {
    HandleScope scope;

    Handle<FunctionTemplate> t = FunctionTemplate::New(New);

    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(t, "start", Start);
    NODE_SET_PROTOTYPE_METHOD(t, "end", End);
    // NODE_SET_PROTOTYPE_METHOD(t, "setOption", SetOption);
    
    t->InstanceTemplate()->SetAccessor(String::NewSymbol("name"), GetName);

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

static Handle<Value> Serializer::New(const Arguments& args) {
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

static Handle<Value> Serializer::Start(const Arguments& args) {
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

static Handle<Value> SerializeStatement(const Arguments& args) {
    Serializer* serializer = ObjectWrap::Unwrap<Serializer>(args.This());
    HandleScope scope;
    
    if (!args.Length() == 1 || !args[0]->IsObject()) {
        return Undefined();
    }
    
    Handle<Object> statement = args[0]->ToObject();
    
    Handle<Object> subject = statement->GetNamedProperty()
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

void Serializer::SerializeStatement(const raptor_statement* statement) {
    
}

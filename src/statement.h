#ifndef NODE_RAPTOR_STATEMENT_H_
#define NODE_RAPTOR_STATEMENT_H_

#include <node.h>

class Statement {
public:
    static v8::Handle<v8::Object> NewInstance();
    static v8::Handle<v8::Value> ToString(const v8::Arguments& args);
    static v8::Handle<v8::Value> SubjectAccessor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> PredicateAccessor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> ObjectAccessor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static raptor_statement* ConvertObjectToRaptorStatement(Handle<Object> obj);
private:
    static v8::Handle<v8::ObjectTemplate> template_;
};

#endif

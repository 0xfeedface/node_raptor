#ifndef NODE_RAPTOR_STATEMENT_H_
#define NODE_RAPTOR_STATEMENT_H_

#include <node.h>

using namespace v8;

class Statement {
public:
    static Handle<Object> NewInstance();
    static Handle<Value> ToString(const Arguments& args);
    static Handle<Value> SubjectAccessor(Local<String> property, const AccessorInfo& info);
    static Handle<Value> PredicateAccessor(Local<String> property, const AccessorInfo& info);
    static Handle<Value> ObjectAccessor(Local<String> property, const AccessorInfo& info);
    static raptor_statement* ConvertObjectToRaptorStatement(Handle<Object> obj);
private:
    static Handle<ObjectTemplate> template_;
};

#endif

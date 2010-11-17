#include <raptor.h>

#include "statics.h"
#include "statement.h"

Handle<ObjectTemplate> Statement::template_;

Handle<Object> Statement::NewInstance() {
    HandleScope scope;

    if (template_.IsEmpty()) {
        Handle<FunctionTemplate> f = FunctionTemplate::New();
        Handle<ObjectTemplate> proto = f->PrototypeTemplate();
        proto->Set(tostring_symbol, FunctionTemplate::New(ToString));
        
        // Handle<ObjectTemplate> t = ObjectTemplate::New();
        Handle<ObjectTemplate> t = f->InstanceTemplate();
        t->SetInternalFieldCount(1);
        t->SetAccessor(subject_symbol, SubjectAccessor);
        t->SetAccessor(pred_symbol, PredicateAccessor);
        t->SetAccessor(object_symbol, ObjectAccessor);

        template_ = Persistent<ObjectTemplate>::New(t);
    }
    
    Handle<Object> i = template_->NewInstance();

    return scope.Close(i);
}

Handle<Value> Statement::ToString(const Arguments& args) {
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

Handle<Value> Statement::SubjectAccessor(Local<String> property, const AccessorInfo& info) {
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

Handle<Value> Statement::PredicateAccessor(Local<String> property, const AccessorInfo& info) {
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

Handle<Value> Statement::ObjectAccessor(Local<String> property, const AccessorInfo& info) {
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

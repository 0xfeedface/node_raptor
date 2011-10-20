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

#include <cstdio>
#include <cstdlib>

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
        return ThrowException(Exception::Error(
            String::New("Error serializing statement.")));
    }
    
    int ret_val;
    ret_val = raptor_statement_ntriples_write(statement, iostream, 0);
    raptor_free_iostream(iostream);
    
    if (ret_val > 0) {
        if (statement_string) {
            free(statement_string);
            statement_string = NULL;
        }
        // error
        return Undefined();
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

raptor_statement* Statement::ConvertObjectToRaptorStatement(Handle<Object> obj) {
    HandleScope scope;
    
    Handle<Object> subject = obj->Get(subject_symbol)->ToObject();
    Handle<String> subject_type = subject->Get(type_symbol)->ToString();
    Handle<String> subject_value = subject->Get(value_symbol)->ToString();
    String::Utf8Value subject_string(subject_value);
    
    raptor_term* subject_term;
    if (subject_type->Equals(uri_symbol)) {
        // have URI
        raptor_uri* uri = raptor_new_uri_from_counted_string(world, 
                                                             reinterpret_cast<unsigned char*>(*subject_string), 
                                                             subject_value->Length());
        if (uri) {
            subject_term = raptor_new_term_from_uri(world, uri);
            raptor_free_uri(uri);
        }
    } else if (subject_type->Equals(bnode_symbol)) {
        // have bnode
        subject_term = raptor_new_term_from_counted_blank(world, 
                                                          reinterpret_cast<unsigned char*>(*subject_string), 
                                                          subject_value->Length());
    } else {
        // error
    }
    
    Handle<Object> predicate = obj->Get(pred_symbol)->ToObject();
    Handle<String> predicate_type = predicate->Get(type_symbol)->ToString();
    Handle<String> predicate_value = predicate->Get(value_symbol)->ToString();
    String::Utf8Value predicate_string(predicate_value);
    
    raptor_term* predicate_term;
    if (predicate_type->Equals(uri_symbol)) {
        // have URI
        raptor_uri* uri = raptor_new_uri_from_counted_string(world, 
                                                             reinterpret_cast<unsigned char*>(*predicate_string), 
                                                             predicate_value->Length());
        if (uri) {
            predicate_term = raptor_new_term_from_uri(world, uri);
            raptor_free_uri(uri);
        }
    } else {
        // error
    }
    
    Handle<Object> object = obj->Get(object_symbol)->ToObject();
    Handle<String> object_type = object->Get(type_symbol)->ToString();
    Handle<String> object_value = object->Get(value_symbol)->ToString();
    String::Utf8Value object_string(object_value);
    
    raptor_term* object_term;
    if (object_type->Equals(uri_symbol)) {
        // have URI
        raptor_uri* uri = raptor_new_uri_from_counted_string(world, 
                                                             reinterpret_cast<unsigned char*>(*object_string), 
                                                             object_value->Length());
        if (uri) {
            object_term = raptor_new_term_from_uri(world, uri);
            raptor_free_uri(uri);
        }
    } else if (object_type->Equals(bnode_symbol)) {
        // have bnode
        object_term = raptor_new_term_from_counted_blank(world, 
                                                         reinterpret_cast<unsigned char*>(*object_string), 
                                                         object_value->Length());
    } else if (object_type->Equals(literal_symbol)) {
        // literal
        Handle<String> object_language;
        if (object->Has(lang_symbol)) {
            object_language = object->Get(lang_symbol)->ToString();
            String::Utf8Value lang_string(object_language);
            
            // if (!object_language.IsEmpty()) {
                object_term = raptor_new_term_from_counted_literal(world, 
                                                                   reinterpret_cast<unsigned char*>(*object_string), 
                                                                   object_value->Length(), 
                                                                   NULL, 
                                                                   reinterpret_cast<unsigned char*>(*lang_string), 
                                                                   object_language->Length());
            // }
        } else {
            object_term = raptor_new_term_from_counted_literal(world, 
                                                               reinterpret_cast<unsigned char*>(*object_string), 
                                                               object_value->Length(), 
                                                               NULL, 
                                                               NULL, 
                                                               0);
        }
    } else if (object_type->Equals(tliteral_symbol)) {
        // typed literal
        Handle<String> object_datatype;
        if (object->Has(dtype_symbol)) {
            object_datatype = object->Get(dtype_symbol)->ToString();
            String::Utf8Value dtype_string(object_datatype);
            
            raptor_uri* dtype_uri = raptor_new_uri_from_counted_string(world, 
                                                                       reinterpret_cast<unsigned char*>(*dtype_string), 
                                                                       object_datatype->Length());
            
           if (dtype_uri) {
               object_term = raptor_new_term_from_counted_literal(world, 
                                                                  reinterpret_cast<unsigned char*>(*object_string), 
                                                                  object_value->Length(), 
                                                                  dtype_uri, 
                                                                  NULL, 
                                                                  NULL);
               raptor_free_uri(dtype_uri);
           } else {
               // error
           }
        }
    } else {
        // error
        return NULL;
    }
    
    raptor_statement* statement;
    if (subject_term && predicate_term && object_term) {
        raptor_statement* result = raptor_new_statement_from_nodes(world, 
                                               subject_term, 
                                               predicate_term, 
                                               object_term, 
                                               NULL);
        
        // raptor_free_term(subject_term);
        // raptor_free_term(predicate_term);
        // raptor_free_term(object_term);
        
        return result;
    }
    
    return NULL;
}

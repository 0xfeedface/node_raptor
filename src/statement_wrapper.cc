/*
 * Copyright 2010–2013 Norman Heino <norman.heino@gmail.com>
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

#include <iostream>
#include <string>
#include <utility>
#include "statement_wrapper.h"
#include "uri.h"

v8::Persistent<v8::ObjectTemplate> StatementWrapper::statementTemplate_;

v8::Persistent<v8::String> StatementWrapper::kSubjectSymbol      = NODE_PSYMBOL("subject");
v8::Persistent<v8::String> StatementWrapper::kPredicateSymbol    = NODE_PSYMBOL("predicate");
v8::Persistent<v8::String> StatementWrapper::kObjectSymbol       = NODE_PSYMBOL("object");
v8::Persistent<v8::String> StatementWrapper::kTypeSymbol         = NODE_PSYMBOL("type");
v8::Persistent<v8::String> StatementWrapper::kValueSymbol        = NODE_PSYMBOL("value");
v8::Persistent<v8::String> StatementWrapper::kLiteralSymbol      = NODE_PSYMBOL("literal");
v8::Persistent<v8::String> StatementWrapper::kTypedLiteralSymbol = NODE_PSYMBOL("typed-literal");
v8::Persistent<v8::String> StatementWrapper::kURISymbol          = NODE_PSYMBOL("uri");
v8::Persistent<v8::String> StatementWrapper::kBNodeSymbol        = NODE_PSYMBOL("bnode");
v8::Persistent<v8::String> StatementWrapper::kDTypeSymbol        = NODE_PSYMBOL("dtype");
v8::Persistent<v8::String> StatementWrapper::kLangSymbol         = NODE_PSYMBOL("lang");

StatementWrapper::StatementWrapper(Statement&& statement)
  : statement_(std::forward<Statement&&>(statement))
{
  v8::V8::AdjustAmountOfExternalAllocatedMemory(sizeof(StatementWrapper));
}

StatementWrapper::~StatementWrapper()
{
  v8::V8::AdjustAmountOfExternalAllocatedMemory(-sizeof(StatementWrapper));
}

StatementWrapper& StatementWrapper::Unwrap(v8::Handle<v8::Object> object)
{
  StatementWrapper* wrapper(static_cast<StatementWrapper*>(object->GetPointerFromInternalField(0)));
  return *wrapper;
}

void StatementWrapper::Initialize()
{
  v8::Handle<v8::FunctionTemplate> constructorFunction = v8::FunctionTemplate::New();
  v8::Handle<v8::ObjectTemplate> prototypeTemplate = constructorFunction->PrototypeTemplate();
  prototypeTemplate->Set(v8::String::NewSymbol("toString"), v8::FunctionTemplate::New(ToString));

  v8::Handle<v8::ObjectTemplate> objectTemplate = constructorFunction->InstanceTemplate();
  objectTemplate->SetInternalFieldCount(1);
  objectTemplate->SetAccessor(kSubjectSymbol, Subject);
  objectTemplate->SetAccessor(kPredicateSymbol, Predicate);
  objectTemplate->SetAccessor(kObjectSymbol, Object);

  statementTemplate_ = v8::Persistent<v8::ObjectTemplate>::New(objectTemplate);
}

v8::Handle<v8::Object> StatementWrapper::NewInstance(Statement&& statement)
{
  v8::HandleScope scope;

  StatementWrapper *wrapper = new StatementWrapper(std::forward<Statement&&>(statement));
  v8::Persistent<v8::Object> instance(v8::Persistent<v8::Object>::New(statementTemplate_->NewInstance()));
  instance->SetPointerInInternalField(0, wrapper);
  instance.MakeWeak(wrapper, TurnedWeak);
  instance.MarkIndependent();

  return scope.Close(instance);
}

void StatementWrapper::TurnedWeak(v8::Persistent<v8::Value> value, void* data)
{
  v8::HandleScope scope;
  value.ClearWeak();
  StatementWrapper* wrapper = static_cast<StatementWrapper*>(data);
  delete wrapper;
  v8::Persistent<v8::Object>::Cast(value)->SetPointerInInternalField(0, nullptr);
  value.Dispose();
  value.Clear();
}

v8::Handle<v8::Value> StatementWrapper::ToString(v8::Arguments const& args)
{
  v8::HandleScope scope;
  StatementWrapper& wrapper(Unwrap(args.Holder()));
  std::string value(wrapper.statement_.toString());
  return scope.Close(v8::String::New(value.c_str(), value.length()));
}

v8::Handle<v8::Value> StatementWrapper::Subject(v8::Local<v8::String> property, v8::AccessorInfo const& info)
{
  v8::HandleScope scope;
  StatementWrapper& wrapper(Unwrap(info.Holder()));

  v8::Local<v8::Object> result(v8::Object::New());
  result->Set(kTypeSymbol, v8::String::New(wrapper.statement_.subjectType().c_str()));
  result->Set(kValueSymbol, v8::String::New(wrapper.statement_.subjectValue().c_str()));

  return scope.Close(result);
}

v8::Handle<v8::Value> StatementWrapper::Predicate(v8::Local<v8::String> property, v8::AccessorInfo const& info)
{
  v8::HandleScope scope;
  StatementWrapper& wrapper(Unwrap(info.Holder()));

  v8::Local<v8::Object> result(v8::Object::New());
  result->Set(kTypeSymbol, v8::String::New(wrapper.statement_.predicateType().c_str()));
  result->Set(kValueSymbol, v8::String::New(wrapper.statement_.predicateValue().c_str()));

  return scope.Close(result);
}

v8::Handle<v8::Value> StatementWrapper::Object(v8::Local<v8::String> property, v8::AccessorInfo const& info)
{
  v8::HandleScope scope;
  StatementWrapper& wrapper(Unwrap(info.Holder()));

  v8::Local<v8::Object> result(v8::Object::New());
  result->Set(kTypeSymbol, v8::String::New(wrapper.statement_.objectType().c_str()));
  result->Set(kValueSymbol, v8::String::New(wrapper.statement_.objectValue().c_str()));

  if (wrapper.statement_.objectHasDatatype()) {
    result->Set(kDTypeSymbol, v8::String::New(wrapper.statement_.objectDatatype().c_str()));
  } else if (wrapper.statement_.objectHasLanguageTag()) {
    result->Set(kLangSymbol, v8::String::New(wrapper.statement_.objectLanguage().c_str()));
  }

  return scope.Close(result);
}

Statement StatementWrapper::StatementWithObject(v8::Handle<v8::Object> const& object)
{
  v8::Local<v8::Object> subjectValue   = object->Get(kSubjectSymbol)->ToObject();
  v8::Local<v8::Object> predicateValue = object->Get(kPredicateSymbol)->ToObject();
  v8::Local<v8::Object> objectValue    = object->Get(kObjectSymbol)->ToObject();

  raptor_term* subjectTerm   = newSubjectTermWithObject(subjectValue);
  raptor_term* predicateTerm = newPredicateTermWithObject(predicateValue);
  raptor_term* objectTerm    = newObjectTermWithObject(objectValue);

  if (!subjectTerm || !predicateTerm || !objectTerm) {
    throw std::runtime_error("invalid terms");
  }

  raptor_statement* statement;
  statement = raptor_new_statement_from_nodes(World::raptorWorld(),
                                              subjectTerm,
                                              predicateTerm,
                                              objectTerm,
                                              NULL);

  Statement result(statement);
  raptor_free_statement(statement);
  // FIXME:
  // raptor_free_term(subjectTerm);
  // raptor_free_term(predicateTerm);
  // raptor_free_term(objectTerm);
  return result;
}

raptor_term* StatementWrapper::newSubjectTermWithObject(v8::Handle<v8::Object> const& object)
{
  v8::Local<v8::String> subjectType  = object->Get(kTypeSymbol)->ToString();
  v8::Local<v8::String> subjectValue = object->Get(kValueSymbol)->ToString();

  raptor_term* subjectTerm = nullptr;
  v8::String::Utf8Value subjectString(subjectValue);
  if (subjectType->Equals(kURISymbol)) {
    URI subjectURI(*subjectString, subjectValue->Utf8Length());
    subjectTerm = raptor_new_term_from_uri(World::raptorWorld(), subjectURI);
  } else if (subjectType->Equals(kBNodeSymbol)) {
    subjectTerm = raptor_new_term_from_counted_blank(World::raptorWorld(),
                                                     reinterpret_cast<byte_t*>(*subjectString),
                                                     subjectValue->Length());
  }
  return subjectTerm;
}

raptor_term* StatementWrapper::newPredicateTermWithObject(v8::Handle<v8::Object> const& object)
{
  v8::Local<v8::String> predicateType  = object->Get(kTypeSymbol)->ToString();
  v8::Local<v8::String> predicateValue = object->Get(kValueSymbol)->ToString();

  raptor_term* predicateTerm = nullptr;
  v8::String::Utf8Value predicateString(predicateValue);
  if (predicateType->Equals(kURISymbol)) {
    URI predicateURI(*predicateString, predicateValue->Utf8Length());
    predicateTerm = raptor_new_term_from_uri(World::raptorWorld(), predicateURI);
  }
  return predicateTerm;
}

raptor_term* StatementWrapper::newObjectTermWithObject(v8::Handle<v8::Object> const& object)
{
  v8::Local<v8::String> objectType  = object->Get(kTypeSymbol)->ToString();
  v8::Local<v8::String> objectValue = object->Get(kValueSymbol)->ToString();

  raptor_term* objectTerm = nullptr;
  v8::String::Utf8Value objectString(objectValue);
  if (objectType->Equals(kURISymbol)) {
    URI objectURI(*objectString, objectValue->Length());
    objectTerm = raptor_new_term_from_uri(World::raptorWorld(), objectURI);
  } else if (objectType->Equals(kBNodeSymbol)) {
    objectTerm = raptor_new_term_from_counted_blank(World::raptorWorld(),
                                                    reinterpret_cast<byte_t*>(*objectString),
                                                    objectValue->Utf8Length());
  } else if (objectType->Equals(kLiteralSymbol)) {
    if (object->Has(kLangSymbol)) {
      v8::Local<v8::String> objectLang = object->Get(kLangSymbol)->ToString();
      v8::Local<v8::String> langString = v8::Local<v8::String>(objectLang);
      objectTerm = raptor_new_term_from_counted_literal(World::raptorWorld(),
                                                        reinterpret_cast<byte_t*>(*objectString),
                                                        objectValue->Utf8Length(),
                                                        NULL,
                                                        reinterpret_cast<byte_t*>(*v8::String::Utf8Value(langString)),
                                                        objectLang->Length());
    } else {
      objectTerm = raptor_new_term_from_counted_literal(World::raptorWorld(),
                                                        reinterpret_cast<byte_t*>(*objectString),
                                                        objectValue->Utf8Length(),
                                                        NULL,
                                                        NULL,
                                                        0);
    }
  } else if (objectType->Equals(kTypedLiteralSymbol)) {
    v8::Local<v8::String> objectDType;
    v8::Local<v8::String> dtypeString;
    if (object->Has(kDTypeSymbol)) {
      objectDType = object->Get(kDTypeSymbol)->ToString();
      dtypeString = v8::Local<v8::String>(objectDType);
      URI dtypeURI(*v8::String::Utf8Value(dtypeString), objectDType->Utf8Length());
      objectTerm = raptor_new_term_from_counted_literal(World::raptorWorld(),
                                                        reinterpret_cast<byte_t*>(*objectString),
                                                        objectValue->Utf8Length(),
                                                        dtypeURI,
                                                        NULL,
                                                        0);
    }
  }
  return objectTerm;
}

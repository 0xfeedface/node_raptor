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
#include <utility>
#include "statement_wrapper.h"

v8::Persistent<v8::ObjectTemplate> StatementWrapper::statementTemplate_;

v8::Persistent<v8::String> StatementWrapper::kTypeSymbol  = NODE_PSYMBOL("type");
v8::Persistent<v8::String> StatementWrapper::kValueSymbol = NODE_PSYMBOL("value");

void StatementWrapper::Initialize()
{
  v8::Handle<v8::FunctionTemplate> constructorFunction = v8::FunctionTemplate::New();
  v8::Handle<v8::ObjectTemplate> prototypeTemplate = constructorFunction->PrototypeTemplate();
  prototypeTemplate->Set(v8::String::NewSymbol("toString"), v8::FunctionTemplate::New(ToString));
    
  v8::Handle<v8::ObjectTemplate> objectTemplate = constructorFunction->InstanceTemplate();
  objectTemplate->SetInternalFieldCount(1);
  objectTemplate->SetAccessor(v8::String::NewSymbol("subject"), Subject);
  objectTemplate->SetAccessor(v8::String::NewSymbol("predicate"), Predicate);
  objectTemplate->SetAccessor(v8::String::NewSymbol("object"), Object);

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
    result->Set(v8::String::NewSymbol("dtype"), v8::String::New(wrapper.statement_.objectDatatype().c_str()));
  } else if (wrapper.statement_.objectHasLanguageTag()) {
    result->Set(v8::String::NewSymbol("lang"), v8::String::New(wrapper.statement_.objectLanguage().c_str()));
  }

  return scope.Close(result);
}

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

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

#include <exception>
#include <string>
#include <node_buffer.h>
#include "serializer_wrapper.h"
#include "statement_wrapper.h"

v8::Persistent<v8::Function> SerializerWrapper::constructor;

SerializerWrapper::SerializerWrapper(v8::Handle<v8::Value> syntaxName)
  : serializer_(*v8::String::AsciiValue(syntaxName))
{
  StatementWrapper::Initialize();

  serializer_.setDataHandler([this](byte_t const* bytes, std::size_t size) {
    if (!dataHandler_.IsEmpty()) {
      v8::HandleScope scope;
      const unsigned argc = 1;
      v8::Local<v8::Value> argv[argc] {
        v8::Local<v8::Value>::New(v8::String::New(reinterpret_cast<char const*>(bytes), size))
      };

      // calling into user code
      v8::TryCatch tryCatch;
      dataHandler_->Call(v8::Context::GetCurrent()->Global(), argc, argv);
      if (tryCatch.HasCaught()) {
        node::FatalException(tryCatch);
      }
    }
  });

  serializer_.setEndHandler([this]() {
    if (!endHandler_.IsEmpty()) {
      v8::HandleScope scope;
      v8::Local<v8::Value> argv[] {};

      // calling into user code
      v8::TryCatch tryCatch;
      endHandler_->Call(v8::Context::GetCurrent()->Global(), 0, argv);
      if (tryCatch.HasCaught()) {
        node::FatalException(tryCatch);
      }
    }
  });
}

SerializerWrapper::~SerializerWrapper()
{
  if (!dataHandler_.IsEmpty()) {
    dataHandler_.Dispose();
  }
  if (!endHandler_.IsEmpty()) {
    endHandler_.Dispose();
  }
}

void SerializerWrapper::Initialize(v8::Handle<v8::Object>& exports)
{
  // initialize
  v8::Local<v8::FunctionTemplate> constructorTemplate(v8::FunctionTemplate::New(New));
  constructorTemplate->SetClassName(v8::String::NewSymbol("Serializer"));

  // object properties
  v8::Handle<v8::ObjectTemplate> instanceTemplate = constructorTemplate->InstanceTemplate();
  instanceTemplate->SetInternalFieldCount(1);

  // prototype methods
  node::SetPrototypeMethod(constructorTemplate, "serializeStart", SerializeStart);
  node::SetPrototypeMethod(constructorTemplate, "serializeStatement", SerializeStatement);
  node::SetPrototypeMethod(constructorTemplate, "serializeEnd", SerializeEnd);

  node::SetPrototypeMethod(constructorTemplate, "setDataHandler", SetDataHandler);
  node::SetPrototypeMethod(constructorTemplate, "setEndHandler", SetEndHandler);

  // constructor function
  constructor = v8::Persistent<v8::Function>::New(constructorTemplate->GetFunction());
  exports->Set(v8::String::NewSymbol("Serializer"), constructor);
}

v8::Handle<v8::Value> SerializerWrapper::New(v8::Arguments const& args)
{
  v8::HandleScope scope;

  if (!args.IsConstructCall()) {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }

  v8::Local<v8::String> syntaxName;
  if (args.Length()) {
      syntaxName = args[0]->ToString();
  }
  SerializerWrapper *wrapper;
  try {
    wrapper = new SerializerWrapper(syntaxName);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }
  wrapper->Wrap(args.This());
  return args.This();
}

v8::Handle<v8::Value> SerializerWrapper::SetDataHandler(v8::Arguments const& args)
{
  if (args[0]->IsFunction()) {
    v8::HandleScope scope;
    SerializerWrapper *serializerWrapper = Unwrap<SerializerWrapper>(args.This());
    v8::Local<v8::Function> localHandler(v8::Local<v8::Function>::Cast(args[0]));
    if (!serializerWrapper->dataHandler_.IsEmpty()) {
      serializerWrapper->dataHandler_.Dispose();
    }
    serializerWrapper->dataHandler_ = v8::Persistent<v8::Function>::New(localHandler);
  }
  return v8::Undefined();
}

v8::Handle<v8::Value> SerializerWrapper::SetEndHandler(v8::Arguments const& args)
{
  if (args[0]->IsFunction()) {
    v8::HandleScope scope;
    SerializerWrapper *serializerWrapper = Unwrap<SerializerWrapper>(args.This());
    v8::Local<v8::Function> localHandler(v8::Local<v8::Function>::Cast(args[0]));
    if (!serializerWrapper->endHandler_.IsEmpty()) {
      serializerWrapper->endHandler_.Dispose();
    }
    serializerWrapper->endHandler_ = v8::Persistent<v8::Function>::New(localHandler);
  }
  return v8::Undefined();
}

v8::Handle<v8::Value> SerializerWrapper::SerializeStart(v8::Arguments const& args)
{
  SerializerWrapper* serializerWrapper = Unwrap<SerializerWrapper>(args.This());

  v8::HandleScope scope;

  if (!args.Length() || !args[0]->IsString()) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New("first argument must be string")));
    return v8::ThrowException(error);
  }

  v8::String::Utf8Value base(args[0]->ToString());
  URI baseURI(*base);

  try {
    serializerWrapper->serializer_.serializeStart(baseURI);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> SerializerWrapper::SerializeStatement(v8::Arguments const& args)
{
  SerializerWrapper* serializerWrapper = Unwrap<SerializerWrapper>(args.This());

  v8::HandleScope scope;

  if (!args.Length() || !args[0]->IsObject()) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New("first argument must be object")));
    return v8::ThrowException(error);
  }

  v8::Local<v8::Object> statementObject = args[0]->ToObject();
  Statement statement(StatementWrapper::StatementWithObject(statementObject));

  try {
    serializerWrapper->serializer_.serializeStatement(statement);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> SerializerWrapper::SerializeEnd(v8::Arguments const& args)
{
  SerializerWrapper* serializerWrapper = Unwrap<SerializerWrapper>(args.This());

  try {
    serializerWrapper->serializer_.serializeEnd();
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

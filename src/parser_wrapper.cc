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
#include <utility>
#include <node_buffer.h>
#include "parser_wrapper.h"
#include "statement_wrapper.h"

v8::Persistent<v8::Function> ParserWrapper::constructor;

void ParserWrapper::Initialize(v8::Handle<v8::Object>& exports)
{
  // initialize
  v8::Local<v8::FunctionTemplate> constructorTemplate(v8::FunctionTemplate::New(New));
  constructorTemplate->SetClassName(v8::String::NewSymbol("Parser"));

  // object properties
  v8::Handle<v8::ObjectTemplate> instanceTemplate = constructorTemplate->InstanceTemplate();
  instanceTemplate->SetInternalFieldCount(1);

  // prototype methods
  node::SetPrototypeMethod(constructorTemplate, "parseStart", ParseStart);
  node::SetPrototypeMethod(constructorTemplate, "parseBuffer", ParseBuffer);
  node::SetPrototypeMethod(constructorTemplate, "parseEnd", ParseEnd);

  node::SetPrototypeMethod(constructorTemplate, "setStatementHandler", SetStatementHandler);
  node::SetPrototypeMethod(constructorTemplate, "setNamespaceHandler", SetNamespaceHandler);
  node::SetPrototypeMethod(constructorTemplate, "setMessageHandler", SetMessageHandler);

  // constructor function
  constructor = v8::Persistent<v8::Function>::New(constructorTemplate->GetFunction());
  exports->Set(v8::String::NewSymbol("Parser"), constructor);
}

v8::Handle<v8::Value> ParserWrapper::New(v8::Arguments const& args)
{
  v8::HandleScope scope;

  if (!args.IsConstructCall()) {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { args[0] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }

  v8::Local<v8::String> parserName;
  if (args.Length()) {
      parserName = args[0]->ToString();
  }
  ParserWrapper *wrapper;
  try {
    wrapper = new ParserWrapper(parserName);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }
  wrapper->Wrap(args.This());
  return args.This();
}

v8::Handle<v8::Value> ParserWrapper::SetStatementHandler(v8::Arguments const& args)
{
  if (args[0]->IsFunction()) {
    v8::HandleScope scope;
    ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());
    v8::Local<v8::Function> localHandler(v8::Local<v8::Function>::Cast(args[0]));
    if (!parserWrapper->statementHandler_.IsEmpty()) {
      parserWrapper->statementHandler_.Dispose();
    }
    parserWrapper->statementHandler_ = v8::Persistent<v8::Function>::New(localHandler);
  }
  return v8::Undefined();
}

v8::Handle<v8::Value> ParserWrapper::SetNamespaceHandler(v8::Arguments const& args)
{
  if (args[0]->IsFunction()) {
    v8::HandleScope scope;
    ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());
    v8::Local<v8::Function> localHandler(v8::Local<v8::Function>::Cast(args[0]));
    if (!parserWrapper->namespaceHandler_.IsEmpty()) {
      parserWrapper->namespaceHandler_.Dispose();
    }
    parserWrapper->namespaceHandler_ = v8::Persistent<v8::Function>::New(localHandler);
  }
  return v8::Undefined();
}

v8::Handle<v8::Value> ParserWrapper::SetMessageHandler(v8::Arguments const& args)
{
  if (args[0]->IsFunction()) {
    v8::HandleScope scope;
    ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());
    v8::Local<v8::Function> localHandler(v8::Local<v8::Function>::Cast(args[0]));
    if (!parserWrapper->messageHandler_.IsEmpty()) {
      parserWrapper->messageHandler_.Dispose();
    }
    parserWrapper->messageHandler_ = v8::Persistent<v8::Function>::New(localHandler);
  }
  return v8::Undefined();
}

v8::Handle<v8::Value> ParserWrapper::ParseStart(v8::Arguments const& args)
{
  ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());

  v8::HandleScope scope;

  if (!args.Length() || !args[0]->IsString()) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New("first argument must be string")));
    return v8::ThrowException(error);
  }

  v8::String::Utf8Value base(args[0]->ToString());
  
  try {
    parserWrapper->parser_.parseStart(*base);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> ParserWrapper::ParseBuffer(v8::Arguments const& args)
{
  v8::HandleScope scope;

  v8::Local<v8::Value> bufferHandle = args[0];
  if (!node::Buffer::HasInstance(bufferHandle)) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New("first argument must be buffer")));
    return ThrowException(error);
  }

  v8::Local<v8::Object> buffer = bufferHandle->ToObject();
  byte_t* bufferData = reinterpret_cast<byte_t*>(node::Buffer::Data(buffer));
  std::size_t bufferLength = node::Buffer::Length(buffer);

  ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());

  try {
    parserWrapper->parser_.parseBuffer(bufferData, bufferLength);
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

v8::Handle<v8::Value> ParserWrapper::ParseEnd(v8::Arguments const& args)
{
  ParserWrapper* parserWrapper = Unwrap<ParserWrapper>(args.This());

  try {
    parserWrapper->parser_.parseEnd();
  } catch (std::runtime_error const& e) {
    v8::Local<v8::Value> error(v8::Exception::Error(v8::String::New(e.what())));
    return v8::ThrowException(error);
  }

  return v8::Undefined();
}

ParserWrapper::ParserWrapper(v8::Handle<v8::Value> syntaxName)
  : parser_(*v8::String::AsciiValue(syntaxName))
{
  StatementWrapper::Initialize();

  parser_.setStatementHandler([this](Statement&& s) {
    if (!statementHandler_.IsEmpty()) {
      v8::HandleScope scope;
      const unsigned argc = 1;
      v8::Local<v8::Value> argv[argc] {
        v8::Local<v8::Value>::New(StatementWrapper::NewInstance(std::forward<Statement&&>(s)))
      };

      // calling into user code
      v8::TryCatch tryCatch;
      statementHandler_->Call(v8::Context::GetCurrent()->Global(), argc, argv);
      if (tryCatch.HasCaught()) {
        node::FatalException(tryCatch);
      }
    }
  });

  parser_.setNamespaceHandler([this](Namespace const& nspace) {
    if (!namespaceHandler_.IsEmpty()) {
      v8::HandleScope scope;
      const unsigned argc = 2;
      std::string namespaceURI(nspace.URI());
      v8::Local<v8::Value> argv[argc] {
        v8::Local<v8::Value>::New(v8::String::New(namespaceURI.data(), namespaceURI.length())),
        v8::Local<v8::Value>::New(v8::String::New(nspace.prefix().data(), nspace.prefix().length()))
      };

      // calling into user code
      v8::TryCatch tryCatch;
      namespaceHandler_->Call(v8::Context::GetCurrent()->Global(), argc, argv);
      if (tryCatch.HasCaught()) {
        node::FatalException(tryCatch);
      }
    }
  });

  parser_.setMessageHandler([this](Message const& message) {
    if (!messageHandler_.IsEmpty()) {
      v8::HandleScope scope;
      const unsigned argc = 2;
      v8::Local<v8::Value> argv[argc] {
        v8::Local<v8::Value>::New(v8::String::NewSymbol(message.type().data(), message.type().length())),
        v8::Local<v8::Value>::New(v8::String::New(message.text().data(), message.text().length()))
      };

      // calling into user code
      v8::TryCatch tryCatch;
      messageHandler_->Call(v8::Context::GetCurrent()->Global(), argc, argv);
      if (tryCatch.HasCaught()) {
        node::FatalException(tryCatch);
      }
    }
  });
}

ParserWrapper::~ParserWrapper()
{
  if (!statementHandler_.IsEmpty()) {
    statementHandler_.Dispose();
  }
  if (!namespaceHandler_.IsEmpty()) {
    namespaceHandler_.Dispose();
  }
  if (!messageHandler_.IsEmpty()) {
    messageHandler_.Dispose();
  }
}

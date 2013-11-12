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

#pragma once

#include <node.h>
#include "serializer.h"

class SerializerWrapper : node::ObjectWrap
{
public:
  static void Initialize(v8::Handle<v8::Object>&);
  static v8::Handle<v8::Value> New(v8::Arguments const&);

  static v8::Handle<v8::Value> SetDataHandler(v8::Arguments const&);
  static v8::Handle<v8::Value> SetEndHandler(v8::Arguments const&);

  static v8::Handle<v8::Value> SerializeStart(v8::Arguments const&);
  static v8::Handle<v8::Value> SerializeStatement(v8::Arguments const&);
  static v8::Handle<v8::Value> SerializeEnd(v8::Arguments const&);

private:
  SerializerWrapper(v8::Handle<v8::Value> syntaxName);
  ~SerializerWrapper();
  static v8::Persistent<v8::Function> constructor;
  Serializer serializer_;
  v8::Persistent<v8::Function> dataHandler_;
  v8::Persistent<v8::Function> endHandler_;
};

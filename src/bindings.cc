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

#include <v8.h>
#include <node.h>
#include "parser_wrapper.h"
#include "serializer_wrapper.h"

// Node module initializer
void InitModule(v8::Handle<v8::Object> exports) {
  ParserWrapper::Initialize(exports);
  SerializerWrapper::Initialize(exports);
}

NODE_MODULE(bindings, InitModule)

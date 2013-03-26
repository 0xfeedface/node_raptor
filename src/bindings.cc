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

#include <node.h>

#include "parser.h"
#include "serializer.h"

using namespace v8;
using namespace node;

void Initialize(Handle<Object> target) {
  NODE_SET_METHOD(target, "newParser", Parser::Initialize);
  NODE_SET_METHOD(target, "newSerializer", Serializer::Initialize);
}

NODE_MODULE(bindings, Initialize);

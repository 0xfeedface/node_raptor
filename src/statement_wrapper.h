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

#include <node.h>
#include "statement.h"

class StatementWrapper {
public:
  static void Initialize();
  static v8::Handle<v8::Object> NewInstance(Statement&&);
  static v8::Handle<v8::Value> ToString(v8::Arguments const&);
  static v8::Handle<v8::Value> Subject(v8::Local<v8::String> property, v8::AccessorInfo const&);
  static v8::Handle<v8::Value> Predicate(v8::Local<v8::String> property, v8::AccessorInfo const&);
  static v8::Handle<v8::Value> Object(v8::Local<v8::String> property, v8::AccessorInfo const&);
  static Statement StatementWithObject(v8::Handle<v8::Object> const& object);
  static raptor_term* newSubjectTermWithObject(v8::Handle<v8::Object> const&);
  static raptor_term* newPredicateTermWithObject(v8::Handle<v8::Object> const&);
  static raptor_term* newObjectTermWithObject(v8::Handle<v8::Object> const&);
private:
  static v8::Persistent<v8::ObjectTemplate> statementTemplate_;
  static v8::Persistent<v8::String> kSubjectSymbol;
  static v8::Persistent<v8::String> kPredicateSymbol;
  static v8::Persistent<v8::String> kObjectSymbol;
  static v8::Persistent<v8::String> kTypeSymbol;
  static v8::Persistent<v8::String> kValueSymbol;
  static v8::Persistent<v8::String> kLiteralSymbol;
  static v8::Persistent<v8::String> kTypedLiteralSymbol;
  static v8::Persistent<v8::String> kURISymbol;
  static v8::Persistent<v8::String> kBNodeSymbol;
  static v8::Persistent<v8::String> kDTypeSymbol;
  static v8::Persistent<v8::String> kLangSymbol;
  static StatementWrapper& Unwrap(v8::Handle<v8::Object>);
  static void TurnedWeak(v8::Persistent<v8::Value> value, void* data);
  explicit StatementWrapper(Statement&&);
  ~StatementWrapper();
  Statement statement_;
};

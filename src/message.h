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

#include <raptor.h>

class Message {
public:
  explicit Message(raptor_log_message const* message);
  std::string const& type() const { return type_; }
  std::string const& text() const { return text_; }

private:
  static std::string kMessageTypeNone;
  static std::string kMessageTypeDebug;
  static std::string kMessageTypeInfo;
  static std::string kMessageTypeWarning;
  static std::string kMessageTypeError;
  static std::string kMessageTypeFatal;

  std::string& type_;
  std::string  text_;
};

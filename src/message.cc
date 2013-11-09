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

#include <sstream>
#include "message.h"

std::string Message::kMessageTypeNone    = "";
std::string Message::kMessageTypeDebug   = "debug";
std::string Message::kMessageTypeInfo    = "info";
std::string Message::kMessageTypeWarning = "warning";
std::string Message::kMessageTypeError   = "error";
std::string Message::kMessageTypeFatal   = "fatal";

Message::Message(raptor_log_message const* message)
  : type_(kMessageTypeNone)
{
  switch (message->level) {
  case RAPTOR_LOG_LEVEL_NONE:
    // already set to none
    break;
  case RAPTOR_LOG_LEVEL_DEBUG:
    type_ = kMessageTypeDebug;
    break;
  case RAPTOR_LOG_LEVEL_INFO:
    type_ = kMessageTypeInfo;
    break;
  case RAPTOR_LOG_LEVEL_WARN:
    type_ = kMessageTypeWarning;
    break;
  case RAPTOR_LOG_LEVEL_ERROR:
    type_ = kMessageTypeError;
    break;
  case RAPTOR_LOG_LEVEL_FATAL:
    type_ = kMessageTypeFatal;
    break;
  }

  text_ = std::string(message->text);
}

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

#include <functional>

#include <raptor.h>
#if (RAPTOR_VERSION_MAJOR < 2)
#error "Raptor library version 2 or greater required"
#endif

#include "world.h"
#include "statement.h"
#include "namespace.h"


class Serializer {
public:
  typedef std::function<void(byte_t const*, std::size_t)> data_handler_t;
  typedef std::function<void()> end_handler_t;

  Serializer(std::string const& syntaxName);
  ~Serializer();

  Serializer(Serializer const&) = delete;
  Serializer& operator=(Serializer const&) = delete;

  static int WriteByteHandler(void* context, int const byte) {
    Serializer* serializer = static_cast<Serializer*>(context);
    if (serializer->dataHandler_) {
      serializer->dataHandler_(reinterpret_cast<byte_t const*>(&byte), 1);
    }
    return 0;
  }
  static int WriteBytesHandler(void* context, void const* data, std::size_t size, std::size_t nmemb) {
    Serializer* serializer = static_cast<Serializer*>(context);
    if (serializer->dataHandler_) {
      serializer->dataHandler_(static_cast<byte_t const*>(data), size * nmemb);
    }
    return 0;
  }
  static int WriteEndHandler(void* context) {
    Serializer* serializer = static_cast<Serializer*>(context);
    if (serializer->endHandler_) {
      serializer->endHandler_();
    }
    return 0;
  }

  void setDataHandler(data_handler_t const&);
  void setEndHandler(end_handler_t const&);

  void setNamespace(Namespace const&);
  void serializeStart(URI const&);
  void serializeStatement(Statement const&);
  void serializeEnd();

private:
  enum class SerializerState : unsigned {
    Init = 0,
    Serializing
  };
  SerializerState state_;

  data_handler_t dataHandler_ = nullptr;
  end_handler_t endHandler_   = nullptr;

  raptor_serializer* serializer_;
  raptor_iostream* stream_;
};

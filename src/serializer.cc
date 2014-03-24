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

#include <string>
#include <exception>
#include "serializer.h"
#include "uri.h"


Serializer::Serializer(std::string const& syntaxName)
  : state_(SerializerState::Init),
    serializer_(raptor_new_serializer(World::raptorWorld(), syntaxName.c_str()))
{
  static raptor_iostream_handler const handler = {
    2,
    NULL, NULL,
    WriteByteHandler,
    WriteBytesHandler,
    WriteEndHandler,
    NULL, NULL
  };
  stream_ = raptor_new_iostream_from_handler(World::raptorWorld(), this, &handler);
  if (!stream_) {
    throw std::runtime_error("could not create I/O stream");
  }
  state_ = SerializerState::Init;
}

Serializer::~Serializer()
{
  raptor_free_iostream(stream_);
  raptor_free_serializer(serializer_);
}

void Serializer::setDataHandler(data_handler_t const& handler)
{
  dataHandler_ = handler;
}

void Serializer::setEndHandler(end_handler_t const& handler)
{
  endHandler_ = handler;
}

void Serializer::setNamespace(Namespace const& nspace)
{
  raptor_serializer_set_namespace(serializer_,
                                  nspace.ID(),
                                  reinterpret_cast<byte_t const*>(nspace.prefix().c_str()));
}

void Serializer::serializeStart(URI const& baseURI)
{
  if (state_ != SerializerState::Init) {
    throw std::runtime_error("serialization already started");
  }
  raptor_serializer_start_to_iostream(serializer_, baseURI, stream_);
  state_ = SerializerState::Serializing;
}

void Serializer::serializeStatement(Statement const& statement)
{
  if (state_ != SerializerState::Serializing) {
    throw std::runtime_error("parsing not started or not yet ended");
  }
  raptor_serializer_serialize_statement(serializer_, statement);
}

void Serializer::serializeEnd()
{
  if (state_ != SerializerState::Serializing) {
    throw std::runtime_error("parsing not started or not yet ended");
  }
  raptor_serializer_serialize_end(serializer_);
  raptor_iostream_write_end(stream_);
  state_ = SerializerState::Init;
}

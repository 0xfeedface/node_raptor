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

#include <string>
#include <raptor.h>
#include "world.h"

class URI
{
public:
  explicit URI(byte_t const* s)
    : uri_(raptor_new_uri(World::raptorWorld(), s)) {}
  explicit URI(byte_t const* s, std::size_t length)
    : uri_(raptor_new_uri_from_counted_string(World::raptorWorld(), s, length)) {}
  explicit URI(char const* s)
    : uri_(raptor_new_uri(World::raptorWorld(), reinterpret_cast<byte_t const*>(s))) {}
  explicit URI(char const* s, std::size_t length)
    : uri_(raptor_new_uri_from_counted_string(World::raptorWorld(), reinterpret_cast<byte_t const*>(s), length)) {}
  explicit URI(std::string const& s)
    : URI(reinterpret_cast<byte_t const*>(s.c_str())) {}
  explicit URI(raptor_uri const* uri)
    : uri_(raptor_uri_copy(const_cast<raptor_uri*>(uri))) {}
  ~URI() { raptor_free_uri(uri_); }
  operator raptor_uri*() const { return uri_; }
  operator std::string() const;
private:
  // Adapt to raptor's bitwise const
  mutable raptor_uri* uri_;
};

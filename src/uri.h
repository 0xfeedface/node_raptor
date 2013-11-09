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
  explicit URI(std::string const& s)
    : uri_(raptor_new_uri(World::raptorWorld(), reinterpret_cast<const raptor_byte_t*>(s.c_str()))) {}
  explicit URI(raptor_uri const* uri)
    : uri_(raptor_uri_copy(const_cast<raptor_uri*>(uri))) {}
  ~URI() { raptor_free_uri(uri_); }
  operator raptor_uri*() { return uri_; }
  operator std::string() const;
private:
  raptor_uri* uri_;
};

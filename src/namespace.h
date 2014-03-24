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
#include "uri.h"

class Namespace {
public:
  explicit Namespace(raptor_namespace const* ns)
    : URI_(raptor_namespace_get_uri(ns)),
      prefix_(reinterpret_cast<const char*>(raptor_namespace_get_prefix(ns))) {}
  URI const& ID() const { return URI_; }
  std::string const& prefix() const { return prefix_; }
private:
  URI URI_;
  std::string prefix_;
};

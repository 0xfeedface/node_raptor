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

typedef unsigned char raptor_byte_t;

class World
{
public:
  static World& sharedWorld();
  static bool isParserName(std::string const& candidate);
  static raptor_world* raptorWorld();

  World(World const&) = delete;
  void operator=(World const&) = delete;
  ~World();
private:
  World();
  raptor_world* world_;
};

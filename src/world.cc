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

#include "world.h"

World& World::sharedWorld()
{
  static World instance_;
  return instance_;
}

raptor_world* World::raptorWorld()
{
  return sharedWorld().world_;
}

bool World::isParserName(std::string const& candidate)
{
  if (raptor_world_is_parser_name(World::raptorWorld(), candidate.c_str())) {
    return true;
  }

  return false;
}

World::World()
{
  world_ = raptor_new_world();
  raptor_world_open(world_);
}

World::~World()
{
  raptor_free_world(world_);
}

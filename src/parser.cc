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

#include <exception>

#include "parser.h"
#include "world.h"
#include "uri.h"
#include "statement.h"

Parser::Parser(std::string const& syntaxName)
    : syntaxName_(syntaxName),
      state_(ParserState::Init)
{
  if (!World::isParserName(syntaxName_)) {
    throw new std::runtime_error("unknown parser name or syntax not supported");
  }
  parser_ = raptor_new_parser(World::raptorWorld(), syntaxName_.c_str());
  raptor_parser_set_statement_handler(parser_, this, StatementCallback);
  raptor_parser_set_namespace_handler(parser_, this, NamespaceCallback);
  raptor_world_set_log_handler(World::raptorWorld(), this, MessageCallback);
  state_ = ParserState::Init;
}

Parser::~Parser()
{
  raptor_free_parser(parser_);
}

void Parser::setStatementHandler(statement_handler_t const& handler)
{
  statementHandler_ = handler;
}

void Parser::setNamespaceHandler(namespace_handler_t const& handler)
{
  namespaceHandler_ = handler;
}

void Parser::setMessageHandler(message_handler_t const& handler)
{
  messageHandler_ = handler;
}

void Parser::parseStart(std::string const& baseURI)
{
  if (state_ != ParserState::Init) {
    throw std::runtime_error("parsing already started");
  }
  raptor_parser_parse_start(parser_, URI(baseURI));
  state_ = ParserState::Parsing;
}

void Parser::parseBuffer(byte_t const* chunk, std::size_t chunkSize)
{
  if (state_ != ParserState::Parsing) {
    throw std::runtime_error("parsing not started or not yet ended");
  }
  raptor_parser_parse_chunk(parser_, chunk, chunkSize, false);
}

void Parser::parseEnd()
{
  if (state_ != ParserState::Parsing) {
    throw std::runtime_error("parsing not started or not yet ended");
  }
  raptor_parser_parse_chunk(parser_, NULL, 0, true);
  state_ = ParserState::Init;
}

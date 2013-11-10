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
#include <functional>

#include <raptor.h>
#if (RAPTOR_VERSION_MAJOR < 2)
#error "Raptor library version 2 or greater required"
#endif

#include "world.h"
#include "statement.h"
#include "namespace.h"
#include "message.h"

class Parser
{
public:
  typedef std::function<void(Statement&&)> statement_handler_t;
  typedef std::function<void(Namespace const&)> namespace_handler_t;
  typedef std::function<void(Message const&)> message_handler_t;

  Parser(std::string const& syntaxName);
  ~Parser();

  Parser(Parser const&) = delete;
  Parser& operator=(Parser const&) = delete;

  void setStatementHandler(statement_handler_t const&);
  void setNamespaceHandler(namespace_handler_t const&);
  void setMessageHandler(message_handler_t const&);

  void parseStart(std::string const&);
  void parseBuffer(byte_t const*, std::size_t);
  void parseEnd();

  static void StatementCallback(void* data, raptor_statement* statement) {
    Parser* parser(static_cast<Parser*>(data));
    if (parser->statementHandler_) {
      parser->statementHandler_(Statement(statement));
    }
  }
  static void NamespaceCallback(void* data, raptor_namespace* nspace) {
    Parser* parser(static_cast<Parser*>(data));
    if (parser->namespaceHandler_) {
      parser->namespaceHandler_(Namespace(nspace));
    }
  }
  static void MessageCallback(void* data, raptor_log_message* message) {
    Parser* parser(static_cast<Parser*>(data));
    if (parser->messageHandler_) {
      parser->messageHandler_(Message(message));
    }
  }

private:
  enum class ParserState : unsigned {
    Init = 0,
    Parsing
  };

  std::string syntaxName_;
  statement_handler_t statementHandler_ = nullptr;
  namespace_handler_t namespaceHandler_ = nullptr;
  message_handler_t messageHandler_ = nullptr;
  ParserState state_;

  std::string baseURI_;
  raptor_parser* parser_;
};

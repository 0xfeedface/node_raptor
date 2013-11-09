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
#include <raptor.h>
#include <cstdlib> // malloc

#include "statement.h"

std::string Statement::kURISymbol          = "uri";
std::string Statement::kBNodeSymbol        = "bnode";
std::string Statement::kLiteralSymbol      = "literal";
std::string Statement::kTypedLiteralSymbol = "typed-literal";
std::string Statement::kDTypeSymbol        = "dtype";
std::string Statement::kLangSymbol         = "lang";

Statement::Statement(raptor_statement* statement)
{
  statement_ = raptor_statement_copy(statement);
}

Statement::Statement(Statement&& other)
{
  statement_ = other.statement_;
  other.statement_ = nullptr;
}

Statement::~Statement()
{
  raptor_free_statement(statement_);
}

bool Statement::operator==(Statement const& other)
{
  if (raptor_statement_equals(statement_, other.statement_)) {
    return true;
  }
  return false;
}

Statement& Statement::operator=(Statement&& other)
{
  statement_ = other.statement_;
  other.statement_ = nullptr;
  return *this;
}

std::string Statement::toString() const
{
  void* statementString = NULL;
  std::size_t statementStringLength;
  raptor_iostream* iostream = raptor_new_iostream_to_string(statement_->world,
                                                            &statementString,
                                                            &statementStringLength,
                                                            malloc);
  if (!iostream) {
    throw std::runtime_error("could not create raptor iostream statement");
  }
    
  int error = raptor_statement_ntriples_write(statement_, iostream, 0);
  raptor_free_iostream(iostream);
    
  if (error) {
    if (statementString) {
        free(statementString);
        statementString = NULL;
    }
    throw std::runtime_error("could not serialize statement");
  }
    
  std::string result;
  if (statementString) {
      result.append(static_cast<char*>(statementString),
                    statementStringLength - 1 /* do not copy trailing line break */);
      free(statementString);
      statementString = NULL;
  }
  return result;
}

std::string Statement::subjectType() const
{
  if (statement_->subject->type == RAPTOR_TERM_TYPE_URI) {
    return kURISymbol;
  } else if (statement_->subject->type == RAPTOR_TERM_TYPE_BLANK) {
    return kBNodeSymbol;
  }
  return "unknown";
}

std::string Statement::subjectValue() const
{
  raptor_term_value* subject_term = &statement_->subject->value;
  raptor_uri* uri_value;
  raptor_term_blank_value* blank_value;
    
  std::string value;
  if (statement_->subject->type == RAPTOR_TERM_TYPE_URI) {
      uri_value = subject_term->uri;
      value.append(reinterpret_cast<char*>(raptor_uri_as_string(uri_value)));
  } else if (statement_->subject->type == RAPTOR_TERM_TYPE_BLANK) {
      blank_value = &subject_term->blank;
      value.append(reinterpret_cast<char*>(blank_value->string), blank_value->string_len);
  }

  return value;
}

std::string Statement::predicateType() const
{
  return kURISymbol;
}

std::string Statement::predicateValue() const
{
  raptor_term_value* predicate_term = &statement_->predicate->value;
  return std::string(reinterpret_cast<char*>(raptor_uri_as_string(predicate_term->uri)));
}

std::string Statement::objectType() const
{
  switch (statement_->object->type) {
  case RAPTOR_TERM_TYPE_URI:
    return kURISymbol;
    break;
  case RAPTOR_TERM_TYPE_LITERAL:
    if (statement_->object->value.literal.datatype) {
      return kTypedLiteralSymbol;
    }
    return kLiteralSymbol;
    break;
  case RAPTOR_TERM_TYPE_BLANK:
    return kBNodeSymbol;
    break;
  case RAPTOR_TERM_TYPE_UNKNOWN:
  default:
    return "unknown";
  }
}

std::string Statement::objectValue() const
{
  raptor_term_value* object_term = &statement_->object->value;
  raptor_uri* uri_value;
  raptor_term_literal_value* literal_value;
  raptor_term_blank_value* blank_value;
  
  std::string value;

  switch (statement_->object->type) {
  case RAPTOR_TERM_TYPE_URI:
    uri_value = object_term->uri;
    value.append(reinterpret_cast<char*>(raptor_uri_as_string(uri_value)));
    break;
  case RAPTOR_TERM_TYPE_LITERAL:
    literal_value = &object_term->literal;
    value.append(reinterpret_cast<char*>(literal_value->string), 
                                          literal_value->string_len);
    break;
  case RAPTOR_TERM_TYPE_BLANK:
    blank_value = &object_term->blank;
    value.append(reinterpret_cast<char*>(blank_value->string), 
                                          blank_value->string_len);
    break;
  case RAPTOR_TERM_TYPE_UNKNOWN:
  default:
    // do nothing
    break;
  }

  return value;
}

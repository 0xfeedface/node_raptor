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

class Statement {
public:
  Statement(raptor_statement*);
  Statement(Statement&&);
  ~Statement();
  bool operator==(Statement const&);
  Statement& operator=(Statement&&);
  operator raptor_statement*() const { return statement_; }

  std::string subjectType() const;
  std::string subjectValue() const;

  std::string predicateType() const;
  std::string predicateValue() const;

  std::string objectType() const;
  std::string objectValue() const;

  std::string objectDatatype() const {
    raptor_uri* datatype_uri = statement_->object->value.literal.datatype;
    unsigned char* datatype_uri_string = raptor_uri_to_string(datatype_uri);
    std::string value(reinterpret_cast<char*>(datatype_uri_string));
    if (datatype_uri_string) {
        raptor_free_memory(datatype_uri_string);
        datatype_uri_string = NULL;
    }
    return value;
  }

  std::string objectLanguage() const {
    return std::string(reinterpret_cast<char*>(statement_->object->value.literal.language), 
                                               statement_->object->value.literal.language_len);
  }

  bool objectHasDatatype() const {
    return (statement_->object->type == RAPTOR_TERM_TYPE_LITERAL &&
            statement_->object->value.literal.datatype);

  }

  bool objectHasLanguageTag() const {
    return (statement_->object->type == RAPTOR_TERM_TYPE_LITERAL &&
            statement_->object->value.literal.language);

  }

  std::string toString() const;
private:
  static std::string kURISymbol;
  static std::string kBNodeSymbol;
  static std::string kLiteralSymbol;
  static std::string kTypedLiteralSymbol;
  static std::string kDTypeSymbol;
  static std::string kLangSymbol;

  // Raptor being a C library uses bitwise const
  // so statement_ must be mutable to allow bitwise
  // (non-logical) changes.
  mutable raptor_statement* statement_;
};

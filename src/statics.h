#ifndef NODE_RAPTOR_SYMBOLS_H_
#define NODE_RAPTOR_SYMBOLS_H_

#include <v8.h>
#include <node.h>
#include <raptor.h>

using namespace v8;

static Persistent<String> subject_symbol  = NODE_PSYMBOL("subject");
static Persistent<String> pred_symbol     = NODE_PSYMBOL("predicate");
static Persistent<String> object_symbol   = NODE_PSYMBOL("object");
static Persistent<String> tostring_symbol = NODE_PSYMBOL("toString");
static Persistent<String> type_symbol     = NODE_PSYMBOL("type");
static Persistent<String> value_symbol    = NODE_PSYMBOL("value");
static Persistent<String> uri_symbol      = NODE_PSYMBOL("uri");
static Persistent<String> literal_symbol  = NODE_PSYMBOL("literal");
static Persistent<String> tliteral_symbol = NODE_PSYMBOL("typed-literal");
static Persistent<String> bnode_symbol    = NODE_PSYMBOL("bnode");
static Persistent<String> dtype_symbol    = NODE_PSYMBOL("datatype");
static Persistent<String> lang_symbol     = NODE_PSYMBOL("lang");
static Persistent<String> stmt_symbol     = NODE_PSYMBOL("statement");
static Persistent<String> nmspc_symbol    = NODE_PSYMBOL("namespace");
static Persistent<String> data_symbol     = NODE_PSYMBOL("data");
static Persistent<String> message_symbol  = NODE_PSYMBOL("message");
static Persistent<String> end_symbol      = NODE_PSYMBOL("end");
static Persistent<String> debug_symbol    = NODE_PSYMBOL("debug");
static Persistent<String> info_symbol     = NODE_PSYMBOL("info");
static Persistent<String> warn_symbol     = NODE_PSYMBOL("warning");
static Persistent<String> error_symbol    = NODE_PSYMBOL("error");
static Persistent<String> fatal_symbol    = NODE_PSYMBOL("fatal");

static raptor_world* world = raptor_new_world();

#endif
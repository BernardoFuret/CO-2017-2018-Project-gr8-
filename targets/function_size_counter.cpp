//#include <string>
#include <sstream>
//#include "targets/type_checker.h"
#include "targets/function_size_counter.h"
#include "ast/all.h"  // all.h is automatically generated

//** AUTOMATICALLY GENERATED BY mk-visitor-skel.pl

/**
 * Useful visitors:
 * - The ones that can go through a function body,
 * until finding a var declaration.
 */
void gr8::function_size_counter::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept( this, lvl );
  }
}

void gr8::function_size_counter::do_block_node(gr8::block_node *const node, int lvl) {
  // Visit:
  node->declarations()->accept( this, lvl + 2 );
  node->instructions()->accept( this, lvl + 2 );
}

void gr8::function_size_counter::do_function_definition_node(gr8::function_definition_node *const node, int lvl) {
  // Visit the function body:
  node->body()->accept( this, lvl + 2 );
}

void gr8::function_size_counter::do_sweeping_from_to_by_do_node(gr8::sweeping_from_to_by_do_node *const node, int lvl) {
  // Visit the block:
  node->block()->accept( this, lvl + 2 );
}

void gr8::function_size_counter::do_if_node(gr8::if_node *const node, int lvl) {
  // Visit the block:
  node->block()->accept( this, lvl + 2 );
}

void gr8::function_size_counter::do_if_else_node(gr8::if_else_node *const node, int lvl) {
  // Visit the then and else blocks:
  node->thenblock()->accept( this, lvl + 2 );
  node->elseblock()->accept( this, lvl + 2 );
}

void gr8::function_size_counter::do_variable_declaration_node(gr8::variable_declaration_node *const node, int lvl) {
  // Increment the function size:
  inc( node->type()->size() );
}


/** Empty visitors: */
void gr8::function_size_counter::do_null_node(gr8::null_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_address_node(gr8::address_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_again_node(gr8::again_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_cell_node(gr8::cell_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_evaluation_node(gr8::evaluation_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_function_invocation_node(gr8::function_invocation_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_input_node(gr8::input_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_objects_node(gr8::objects_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_plus_node(gr8::plus_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_post_node(gr8::post_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_return_node(gr8::return_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_stop_node(gr8::stop_node *const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_tweet_node(gr8::tweet_node *const node, int lvl) {
  // EMPTY
}

// ----------------------------------------------------------------------------
// CDK:
void gr8::function_size_counter::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_double_node(cdk::double_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_not_node(cdk::not_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_and_node(cdk::and_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_or_node(cdk::or_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void gr8::function_size_counter::do_integer_node(cdk::integer_node * const node, int lvl) {
  // EMPTY
}

void gr8::function_size_counter::do_string_node(cdk::string_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void gr8::function_size_counter::do_neg_node(cdk::neg_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------
void gr8::function_size_counter::do_add_node(cdk::add_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_sub_node(cdk::sub_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_mul_node(cdk::mul_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_div_node(cdk::div_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_mod_node(cdk::mod_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_lt_node(cdk::lt_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_le_node(cdk::le_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_ge_node(cdk::ge_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_gt_node(cdk::gt_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_ne_node(cdk::ne_node * const node, int lvl) {
  // EMPTY
}
void gr8::function_size_counter::do_eq_node(cdk::eq_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void gr8::function_size_counter::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  // EMPTY
}

void gr8::function_size_counter::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  // EMPTY
}

void gr8::function_size_counter::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  // EMPTY
}
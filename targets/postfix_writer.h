#ifndef __GR8_SEMANTICS_POSTFIX_WRITER_H__
#define __GR8_SEMANTICS_POSTFIX_WRITER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"
#include <set>

namespace gr8 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<gr8::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    std::shared_ptr<gr8::symbol> _context;
    int _level; // Context level, to help differentiating from parameter context. (hating this... but no time...)

    bool _is_calling;

    std::shared_ptr<gr8::symbol> _variable;
    bool _global_variable;

    basic_type * _global_type; // Info below.

    std::vector<std::string> _sweeping_again;
    std::vector<std::string> _sweeping_stop;

    std::set<std::string> _internal;
    std::set<std::string> _external { // RTS functions:
      "argc", "argv", "envp",
      "printi", "printd", "prints", "println",
      "readi", "readd"
    };
    std::set<std::string> _globals;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<gr8::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _context(nullptr), _level(-1), _global_variable(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    /** Context stuff: */
    // Set:
    void set_context( std::shared_ptr<gr8::symbol> context ) {
      _context = context;
      _level   = 0;
    }
    void clean_context() {
      _context = nullptr;
      _level   = -1;
    }
    void add_context_level() {
      _level++;
    }

    // Get:
    std::shared_ptr<gr8::symbol> get_context() {
      return _context;
    }
    bool parameters_context() {
      return !_level; // Terrible, terrible, terrible...
    }

    int parameters_offset() {
      return _context->parameters_offset();
    }
    int body_offset() {
      return _context->body_offset();
    }

    // Update context parameters offset:
    void update_context_parameters_offset( int i ) {
      _context->parameters_offset( _context->parameters_offset() + i );
    }

    // Update context body offset:
    void update_context_body_offset( int i ) {
      _context->body_offset( _context->body_offset() - i );
    }

    /** Call: */
    bool is_calling() {
      return _is_calling;
    }
    void enter_call() {
      _is_calling = true;
    }
    void leave_call() {
      _is_calling = false;
    }

    /** Variable: */
    void set_variable( std::shared_ptr<gr8::symbol> variable ) {
      _variable = variable;
    }
    void clean_variable() {
      _variable = nullptr;
      _global_variable = false;
    }
    // Global:
    void set_global_variable() {
      _global_variable = _variable ? true : false; // Being cautious...
    }
    bool variable_is_global() {
      return _global_variable;
    }

    /*bool is_global( std::shared_ptr<gr8::symbol> variable ) { // TODO
      return _globals.count( symbol->name() );...
    }*/

    // Get:
    std::shared_ptr<gr8::symbol> get_variable() {
      return _variable;
    }

    /** Global type: */
    /**
     * Brief explanation on what this does:
     * -- Mainly for the 'objects' operation (memory allocation).
     * -- The objects operation make space on the stack for something.
     * -- It's kinda like an array. In other words: a pointer.
     * -- It's points to something, depending on its context.
     * -- Possible usages on 'objects':
     * -- -> huge fake p (initially 5 objects)
     * -- -> assign 3 objects to p
     * -- In booth of theses cases, the pointer type be the same type
     * as what the leftvalue points (the leftvalue must be a pointer too).
     * -- So, since when we visit the right part of that declaration or
     * expression, we still haven't visited the left part (mainly in
     * assignment), how can we know what the 'objects' must point to?
     * -- This variable ('_global_type') will save the type of the 
     * gr8::variable_declaration_node or the type of the cdk::assignment_node,
     * since the type for thoses will be known and correct.
     * -- This way, when we call gr8::postfix_writer::do_objects_node, we can
     * know what type the 'objects' is assigning to.
     * -- This is particularly important because 'objects' needs to
     * allocate space in the stack; space equal to the size of the
     * subtype of the pointer.
     */
    void set_type( basic_type *t ) {
      _global_type = copy_type( t );
    }
    basic_type *get_type() {
      return _global_type;
    }
    void clean_type() {
      _global_type = nullptr;
    }

    /** Sweepings: */
    bool sweeping() {
      return !_sweeping_again.empty(); // && !_sweeping_stop.empty() Same thing.
    }
    void sweeping( std::string again_label, std::string stop_label ) {
      _sweeping_again.push_back( again_label );
      _sweeping_stop.push_back( stop_label );
    }
    std::string sweeping_again() {
      std::string again_label = _sweeping_again.back();
      _sweeping_again.pop_back();
      _sweeping_stop.pop_back();
      return again_label;
    }
    std::string sweeping_stop() {
      _sweeping_again.pop_back();
      std::string stop_label = _sweeping_stop.back();
      _sweeping_stop.pop_back();
      return stop_label;
    }
    void exit_sweeping() { // No time for better...
      if ( !_sweeping_again.empty() ) {
        _sweeping_again.pop_back();
      }
      if ( !_sweeping_stop.empty() ) {
        _sweeping_stop.pop_back();
      }
    }


    /** External/Internal stuff: */
    void add_internal_identifier( std::string name ) {
      _internal.insert( name );
    }
    void add_external_identifier( std::string name ) {
      _external.insert( name );
    }
    void declare_external_identifiers() {
      for ( auto& external_identifier: _external ) { // http://en.cppreference.com/w/cpp/container/set/set
        // Check if it is not defined internally:
        if ( _internal.count( external_identifier ) == 0 ) {
          _pf.EXTERN( external_identifier );
        }
      }
    }

  protected:
    void do_arithmetics( cdk::binary_expression_node * const node, int lvl );
    void do_comparison ( cdk::binary_expression_node * const node, int lvl );

    /**
     * Helper functions.
     */
  public:
    bool needs_cast( basic_type *child_node, basic_type *reference_node ) { // TODO: ( a, b ) => ( b, a ); ......
      return is_double( reference_node ) && is_int( child_node );
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

#endif

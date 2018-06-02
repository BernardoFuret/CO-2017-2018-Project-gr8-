#ifndef __GR8_BASIC_AST_VISITOR_H__
#define __GR8_BASIC_AST_VISITOR_H__

#include <string>
#include <memory>
#include <iostream>
#include <cdk/compiler.h>
#include "symbol.h"

/* do not edit -- include node forward declarations */
#define __NODE_DECLARATIONS_ONLY__
#include "ast/all.h"  // automatically generated
#undef __NODE_DECLARATIONS_ONLY__
/* do not edit -- end */

//!
//! Print nodes as XML elements to the output stream.
//!
class basic_ast_visitor {
protected:
  //! The owner compiler
  std::shared_ptr<cdk::compiler> _compiler;

private:

  // last symbol inserted in symbol table
  std::shared_ptr<gr8::symbol> _new_symbol;

protected:
  basic_ast_visitor(std::shared_ptr<cdk::compiler> compiler) :
      _compiler(compiler) {
  }

  bool debug() {
    return _compiler->debug();
  }

  std::ostream &os() {
    return *_compiler->ostream();
  }

public:
  virtual ~basic_ast_visitor() {
  }

public:
  std::shared_ptr<gr8::symbol> new_symbol() {
    return _new_symbol;
  }

  void set_new_symbol(std::shared_ptr<gr8::symbol> symbol) {
    _new_symbol = symbol;
  }

  void reset_new_symbol() {
    _new_symbol = nullptr;
  }

public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#define __PURE_VIRTUAL_DECLARATIONS_ONLY__
#include "ast/visitor_decls.h"       // automatically generated
#undef __PURE_VIRTUAL_DECLARATIONS_ONLY__
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  /**
   * Helper functions.
   */
public:
  // Type check functions:
  inline bool is_something( basic_type* to_compare, unsigned long int compare_to ) {
    return to_compare->name() == compare_to;
  }
  inline bool is_int    ( basic_type* t ) { return is_something( t, basic_type::TYPE_INT     ); }
  inline bool is_double ( basic_type* t ) { return is_something( t, basic_type::TYPE_DOUBLE  ); }
  inline bool is_string ( basic_type* t ) { return is_something( t, basic_type::TYPE_STRING  ); }
  inline bool is_pointer( basic_type* t ) { return is_something( t, basic_type::TYPE_POINTER ); }
  inline bool is_void   ( basic_type* t ) { return is_something( t, basic_type::TYPE_VOID    ); }
  inline bool is_unspec ( basic_type* t ) { return is_something( t, basic_type::TYPE_UNSPEC  ); }

  // Helper function to copy the type:
  inline basic_type* copy_type( basic_type* a_type ) {
    // Duplicate:
    basic_type* return_type = new basic_type( a_type->size(), a_type->name() );

    // Check for the subtype, to recursively copy it:
    if ( a_type->subtype() != nullptr ) {
      // Exists, so the return_type subtype must be a_type's subtype:
      return_type->_subtype = copy_type( a_type->subtype() );
    }

    // We are done, so return the copied type:
    return return_type;
  }

  // Helper function to check if two types are of the same type:
  inline bool type_equals( basic_type* l_type, basic_type* r_type ) {
    // Neither have a subtype:
    if ( !l_type->subtype() && !r_type->subtype() ) {
      // So just compare this level:
      return l_type->name() == r_type->name();
    }
    // One of them doesn't have a subtype, but the other does:
    else if ( !l_type->subtype() || !r_type->subtype() ) { // Might not be the best way to present it, but will work since if neither has a subtype, falls on the first if.
      // Therefore, they are not the same:
      return false;
    }

    // Else, both have a subtype; compare recursively:
    return type_equals( l_type->subtype(), r_type->subtype() );
  }

  // Helper function to see if two pointers are equal or one points to unspec while the other doesn't, while keeping the same depth:
  inline bool compatible_pointers( basic_type* l_type, basic_type* r_type  ) {
    // Check if they are not pointers:
    if ( !is_pointer( l_type ) && !is_pointer( r_type ) ) {
      return (
        ( is_unspec( l_type ) && !is_unspec( r_type ) )
        ||
        ( !is_unspec( l_type ) && is_unspec( r_type ) )
      );
    }
    // Check if one is pointer and the other is not: 
    else if (
      ( !is_pointer( l_type ) && is_pointer( r_type ) )
      ||
      ( is_pointer( l_type ) && !is_pointer( r_type ) )
    ) {
      return false;
    } else {
      // Recursion on subtype: (both are pointers here)
      return compatible_pointers( l_type->subtype(), r_type->subtype() );
    }
  }

  // Helper function to check if a left and right types are comaptible: (equal or double:=int)
  inline bool compatible_types( basic_type* l_type, basic_type* r_type ) {
    return (
      type_equals( l_type, r_type )
      ||
      ( is_double( l_type ) && is_int( r_type ) )
      ||
      compatible_pointers( l_type, r_type )
    );
  }

  // Helper function to check if a type is a pointer that points to unspec:
  inline bool points_to_unspec( basic_type* a_type ) {
    // First, check if it's still a pointer:
    if ( !is_pointer( a_type ) ) {
      return is_unspec( a_type );
    }

    // Else, it's a pointer:
    return points_to_unspec( a_type->subtype() );
  }

  // Types' sizes:
  struct SIZE {
  public:
    static const int _INT     = 4;
    static const int _DOUBLE  = 8;
    static const int _STRING  = 4;
    static const int _POINTER = 4;
    static const int _VOID    = 0;
  };
  /** End. */

};

#endif

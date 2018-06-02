#ifndef __GR8_FUNCTION_SIZE_COUNTER_H__
#define __GR8_FUNCTION_SIZE_COUNTER_H__

#include <string>
#include <iostream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace gr8 {

  //!
  //! Traverse syntax tree and to fetch the function size.
  //!
  class function_size_counter: public basic_ast_visitor {
    size_t _counter;

  public:
    function_size_counter( std::shared_ptr<cdk::compiler> compiler ) :
        basic_ast_visitor(compiler), _counter( 0 ) {
    }

  public:
    ~function_size_counter() {
      os().flush();
    }

  private:
    /** Method to increment. */
    void inc( size_t value ) {
      _counter += value;
    }

  public:
    /** Counter getter. */
    size_t counter() {
      return _counter;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // gr8

/*#define COUNT( compiler, node ) { \
  try { \
    gr8::function_size_counter __function_size_counter( compiler ); \
    (node)->accept( &__counter, 0 ); \
  } \
  catch ( const std::string &problem ) { \
    std::cerr << (node)->lineno() << ": " << problem << " << @function_size_counter" << std::endl; \
    return; \
  } \
  return __function_size_counter.counter(); \
}*/

/*#define COUNT_FUNCTION_SIZE \
  try { \
    gr8::function_size_counter __function_size_counter( this->_compiler ); \
    (node)->accept( &__function_size_counter, 0 ); \
  } \
  catch ( const std::string &problem ) { \
    std::cerr << (node)->lineno() << ": " << problem << " << @function_size_counter" << std::endl; \
    return; \
  }*/

#endif

// use n plus 1 for factorial || do printDate
#ifndef __GR8_FUNCTIONINVOCATIONNODE_H__
#define __GR8_FUNCTIONINVOCATIONNODE_H__

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing function invocation nodes.
   */
  class function_invocation_node: public cdk::expression_node {
    std::string _name; // Function name: factorial.
    cdk::sequence_node* _arguments; // function arguments: n plus 1.

  public:
    inline function_invocation_node(
      int lineno, std::string* name,
      cdk::sequence_node* arguments
    ) :
        cdk::expression_node(lineno),
        _name(*name), _arguments(arguments) {
    }

  public:
    inline std::string name() {
      return _name;
    }

    inline cdk::sequence_node* arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_invocation_node(this, level);
    }

  };

} // gr8

#endif

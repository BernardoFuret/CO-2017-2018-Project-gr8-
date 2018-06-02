// a?
#ifndef __GR8_ADDRESSNODE_H__
#define __GR8_ADDRESSNODE_H__

#include <cdk/ast/unary_expression_node.h>

namespace gr8 {

  /**
   * Class for describing address nodes.
   */
  class address_node: public cdk::expression_node {
    cdk::lvalue_node* _argument;

  public:
    inline address_node(int lineno, cdk::lvalue_node* argument) :
        cdk::expression_node(lineno), _argument(argument) {
    }

  public:
    inline cdk::lvalue_node* argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }

  };

} // gr8

#endif

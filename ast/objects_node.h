
#ifndef __GR8_OBJECTSNODE_H__
#define __GR8_OBJECTSNODE_H__

#include <cdk/ast/unary_expression_node.h>

namespace gr8 {

  /**
   * Class for describing objects nodes.
   */
  class objects_node: public cdk::unary_expression_node {

  public:
    inline objects_node(int lineno, cdk::expression_node *argument) :
        cdk::unary_expression_node(lineno, argument) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_objects_node(this, level);
    }

  };

} // gr8

#endif

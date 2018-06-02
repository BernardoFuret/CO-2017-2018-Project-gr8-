// cell a plus 1 at p (equivalente a p[a+1] em C)
#ifndef __GR8_CELLNODE_H__
#define __GR8_CELLNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace gr8 {

  /**
   * Class for describing cell nodes.
   */
  class cell_node: public cdk::lvalue_node {
    cdk::expression_node* _pointer; // p.
    cdk::expression_node* _position; // a plus 1.

  public:
    inline cell_node(int lineno, cdk::expression_node* pointer, cdk::expression_node* position) :
        cdk::lvalue_node(lineno), _pointer(pointer), _position(position) {
    }

  public:
    inline cdk::expression_node* pointer() {
      return _pointer;
    }

    inline cdk::expression_node* position() {
      return _position;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_cell_node(this, level);
    }

  };

} // gr8

#endif

// sweeping lvalue from expressao to expressao [ by expressao ] do bloco
#ifndef __GR8_SWEEPINGNODE_H__
#define __GR8_SWEEPINGNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing sweeping-cycle nodes.
   */
  class sweeping_from_to_by_do_node: public cdk::basic_node {
    cdk::lvalue_node *_lvalue;
    cdk::expression_node *_from_expression;
    cdk::expression_node *_to_expression;
    cdk::expression_node *_by_expression;
    cdk::basic_node *_block;

  public:
    inline sweeping_from_to_by_do_node( int lineno,
      cdk::lvalue_node *lvalue, cdk::expression_node *from_expression,
      cdk::expression_node *to_expression, cdk::expression_node *by_expression,
      cdk::basic_node *block ) :

        basic_node(lineno),
        _lvalue(lvalue), _from_expression(from_expression),
        _to_expression(to_expression), _by_expression(by_expression),
        _block(block) {
    }

  public:
    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }
    inline cdk::expression_node *from_expression() {
      return _from_expression;
    }
    inline cdk::expression_node *to_expression() {
      return _to_expression;
    }
    inline cdk::expression_node *by_expression() {
      return _by_expression;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweeping_from_to_by_do_node(this, level);
    }

  };

} // gr8

#endif

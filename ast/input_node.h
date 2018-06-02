
#ifndef __GR8_INPUTNODE_H__
#define __GR8_INPUTNODE_H__

#include <cdk/ast/expression_node.h>

namespace gr8 {

  /**
   * Class for describing input nodes.
   */
  class input_node: public cdk::expression_node {
    //cdk::lvalue_node *_argument;

  public:
    inline input_node(int lineno/*, cdk::lvalue_node *argument*/) :
        cdk::expression_node(lineno)/*, _argument(argument)*/ { // NOTES: gr8 doesn't receive anything; it's just "input", so... ???
    }

  public:

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_input_node(this, level);
    }

  };

} // gr8

#endif


#ifndef __GR8_STOPNODE_H__
#define __GR8_STOPNODE_H__

#include <cdk/ast/literal_node.h>

namespace gr8 {

  /**
   * Class for describing stop nodes.
   */
  class stop_node: public cdk::basic_node {
    int _argument;

  public:
    inline stop_node(int lineno, int argument) :
        cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline int argument() {
      return _argument;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stop_node(this, level);
    }

  };

} // gr8

#endif

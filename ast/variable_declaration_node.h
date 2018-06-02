//
#ifndef __GR8_VARIABLEDECLARATIONNODE_H__
#define __GR8_VARIABLEDECLARATIONNODE_H__

#include <cdk/ast/sequence_node.h>

namespace gr8 {

  /**
   * Class for describing variable declaration nodes.
   */
  class variable_declaration_node: public cdk::basic_node {
    bool _isPublic;
    bool _isUse;
    basic_type* _type;
    std::string _name;
    cdk::expression_node* _initially;

  public:
    inline variable_declaration_node(
      int lineno, bool isPublic, bool isUse,
      basic_type* type, std::string* name,
      cdk::expression_node* initially
    ) :
        cdk::basic_node(lineno), _isPublic(isPublic), _isUse(isUse),
        _type(type), _name(*name), _initially(initially) {
    }

  public:
    inline bool isUse() {
      return _isUse;
    }

    inline bool isPublic() {
      return _isPublic;
    }

    inline basic_type* type() {
      return _type;
    }

    inline std::string name() {
      return _name;
    }

    inline cdk::expression_node* initially() {
      return _initially;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // gr8

#endif

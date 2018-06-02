// (public )?huge fake (function|procedure) name uses huge fake p
#ifndef __GR8_FUNCTIONDECLARATIONNODE_H__
#define __GR8_FUNCTIONDECLARATIONNODE_H__

#include <cdk/ast/sequence_node.h>

namespace gr8 {

  /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::typed_node {
    bool _public; // is public / is not. 
    basic_type* _type; // Return type: huge fake.
    std::string _name; // Function name: name.
    cdk::sequence_node* _parameters; // function parameters: huge fake p.

  public:
    inline function_declaration_node(
      int lineno, bool publicBool,
      basic_type* type, std::string* name,
      cdk::sequence_node* parameters
    ) :
        cdk::typed_node(lineno), _public(publicBool),
        _type(type), _name(*name), _parameters(parameters) {
    }

  public:
    inline bool isPublic() {
      return _public;
    }

    inline basic_type* type() {
      return _type;
    }

    inline std::string name() {
      return _name;
    }

    inline void set_as_main() {
      _name = "_main";
    }

    inline cdk::sequence_node* parameters() {
      return _parameters;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // gr8

#endif

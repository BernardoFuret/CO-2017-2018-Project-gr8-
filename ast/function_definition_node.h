// define (public )?huge fake (function|procedure) name on huge fake p as
//   stuff
#ifndef __GR8_FUNCTIONDEFINITIONNODE_H__
#define __GR8_FUNCTIONDEFINITIONNODE_H__

#include <cdk/ast/basic_node.h>

namespace gr8 {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::basic_node {
    bool _public; // is public / is not. 
    basic_type* _type; // Return type: huge fake.
    std::string _name; // Function name: name.
    cdk::sequence_node* _parameters; // function parameters: huge fake p.
    block_node* _body; // function body: stuff.

  public:
    inline function_definition_node(
      int lineno, bool publicBool,
      basic_type* type, std::string* name,
      cdk::sequence_node* parameters,
      block_node* body
    ) :
        cdk::basic_node(lineno), _public(publicBool),
        _type(type), _name(*name), _parameters(parameters),
        _body(body) {
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

    inline block_node* body() {
      return _body;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // gr8

#endif

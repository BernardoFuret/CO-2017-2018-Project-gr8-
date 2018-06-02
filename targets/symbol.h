#ifndef __GR8_SEMANTICS_SYMBOL_H__
#define __GR8_SEMANTICS_SYMBOL_H__

#include <string>
#include <vector>
#include <cdk/basic_type.h>

namespace gr8 {

  class symbol {
    // Generic:
    basic_type *_type;
    std::string _name;
    bool _is_public;
    bool _is_callable;

    // Variable only:
    bool _is_use;
    int _offset;
    bool _global;

    // Function only:
    bool _is_defined;
    std::vector<basic_type*> _parameters;
    size_t _size;
    int _parameters_offset;
    int _body_offset;

    // Both, but not generic:
     // [function] parameters higher pointer. | [variable] arguments position on the stack. 


  public:
    // Variable constructor:
    symbol( basic_type *type, const std::string &name, bool is_public, bool use ) :
        _type(type), _name(name), _is_public(is_public), _is_callable(false), _is_use(use), _offset(0), _global(false) {
    }
    // Function constructor:
    symbol( basic_type *type, const std::string &name, bool is_public, bool is_defined, std::vector<basic_type*> parameters, size_t size ) :
        _type(type), _name(name), _is_public(is_public), _is_callable(true), _is_defined(is_defined),
        _parameters(parameters), _size(size), _parameters_offset(8), _body_offset(0) {
    }

    virtual ~symbol() {
      delete _type;
    }

    // Generic: 
    basic_type *type() const {
      return _type;
    }
    const std::string &name() const {
      return _name;
    }

    bool is_public() {
      return _is_public;
    }

    bool is_callable() {
      return _is_callable;
    }

    // Variable:
    bool is_use() {
      return _is_use;
    }
    int offset() {
      return _offset;
    }
    void offset( int i ) {
      _offset = i;
    }
    bool is_global() {
      return _global;
    }
    void global() {
      _global = true;
    }

    // Function:
    bool is_defined() {
      return _is_defined;
    }
    void defined() {
      _is_defined = true;
    }
    std::vector<basic_type*> parameters() {
      return _parameters;
    }
    size_t number_of_parameters() {
      return _parameters.size(); // ATTENTION HERE!
    }
    size_t size() {
      return _size;
    }
    void size( size_t s ) {
      _size = s; 
    }
    int parameters_offset() {
      return _parameters_offset;
    }
    void parameters_offset( int i ) {
      _parameters_offset = i;
    }
    int body_offset() {
      return _body_offset;
    }
    void body_offset( int i ) {
      _body_offset = i;
    }

  };

} // gr8

#endif

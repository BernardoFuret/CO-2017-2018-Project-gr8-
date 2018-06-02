#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated

/**
 * NOTES TO SELF:
 * Check all TODOs;
 * Check if I placed I2D in all places that should take an I2D (assignements; attributitions; comparisons; ARGS FOR FUNCTION CALLS!)
 */
//---------------------------------------------------------------------------
//------------GENERATED BY MK-VISITOR-SKEL.pl--------------------------------

void gr8::postfix_writer::do_sweeping_from_to_by_do_node(gr8::sweeping_from_to_by_do_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS; // https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_10
  // sweeping 'lvalue' from 'from' to 'to' by 'by' do 'block'
  // IMPORTANT NOTES: I MADE SOME LAST MINUTE CHANGES, SO SOME COMMENTS MAY BE WRONG, MAINLY WHEN IT COMES TO COMPARISONS!!

  // Labels:
  std::string //{
    label_step = mklbl( ++_lbl ), // increment/ decrement.
    label_test = mklbl( ++_lbl ),
    label_end  = mklbl( ++_lbl ),
    label_block  = mklbl( ++_lbl ),
    label_increment = mklbl( ++_lbl ), // if incrementing.
    label_decrement = mklbl( ++_lbl )  // if dercrementing.
  ;//}

  // For the 'again' and 'stop' instructions:
  //add_again( label_step ); // Jumps to the increment.
  //add_stop( label_end ); // Jumps to the sweeping end.
  sweeping( label_step, label_end );

  // Assign 'from' to the 'lvalue': (in gr8, it's literally an assignment, since you can't have stuff like 'sweeping small x from 1 to 3', given it's an lvalue)
  // Therefore, just do an assignement (from the parser: 'tASSIGN expression tTO leftvalue' here works like 'tASSIGN from_expression tTO lvalue' ).
  // Using the node is simpler because it'll do all the checks for me (e.g., I2D, etc.).
  cdk::assignment_node *lvalue_assignment = new cdk::assignment_node(
    node->lineno(), // Line number of the sweeping, since it happens at the sweeping header.
    node->lvalue(), // Where to be stored.
    node->from_expression() // What to store.
  );
  // Do:
  lvalue_assignment->accept( this, lvl + 2 );

  // So far, we have stored the value of 'from' on the 'lvalue'.

  // Enter the sweeping test:

  // Align: (I'll just align everytime before each label, just in case...)
  _pf.ALIGN();

  // Now, label the start of the check condition part:
  _pf.LABEL( label_test );

  // Check if we are incrememtning or decrementing:
  node->by_expression()->accept( this, lvl + 2 ); // this should leave its value on the stack. TODO: this will not work for variable cases of this arg, I think.
  // Compare with zero:
  _pf.INT( 0 );
  // Compare and jump if greater or equal: (greater or equal, because if the 'by' is 0 it doesn't matter either way)
  _pf.JGE( label_increment ); // So, go the increment route.

  // Pre-label align:
  _pf.ALIGN();

  // Label to point to the decrement route:
  _pf.LABEL( label_decrement );

  //{ // Condition:

    // Label for cases where 'by' is negative:
    // Compare if 'lvalue' is less than or equal to 'to':
    // Place 'lvalue' value on the stack:
    cdk::rvalue_node *rvalue = new cdk::rvalue_node( node->lineno(), node->lvalue() ); // Needs to be like this to fetch the value (else it'd be jsut the address)
    rvalue->accept( this, lvl + 2 );
    
    // Place 'to' value on the stack:
    node->to_expression()->accept( this, lvl + 2 );

    // Compare them: ('lvalue' < 'to')
    _pf.LE();

    // Get out of here if the result is zero: (meaning 'lvalue' >= 'to') 
    _pf.JZ( label_end ); // Jump to the end.

    // Else, do the block:
    _pf.JMP( label_block );

  //}

  // Pre-label align:
  _pf.ALIGN();

  // Label to point to the increment route:
  _pf.LABEL( label_increment );

  //{ // Condition:

    // Label for cases where 'by' is positive:
    // Compare if 'lvalue' is greater than or equal to 'to':
    // Place 'lvalue' value on the stack:
    rvalue = new cdk::rvalue_node( node->lineno(), node->lvalue() ); // Needs to be like this to fetch the value (else it'd be jsut the address)
    rvalue->accept( this, lvl + 2 );
    
    // Place 'to' value on the stack:
    node->to_expression()->accept( this, lvl + 2 );

    // Compare them: ('lvalue' >= 'to')
    _pf.GT();

    // Get out of here if the result is zero: (meaning 'lvalue' < 'to') 
    _pf.JZ( label_end ); // Jump to the end.

    // Else, do the block:
    _pf.JMP( label_block );

  //}

  // Pre-label align:
  _pf.ALIGN();

  // Label to point to the decrement route:
  _pf.LABEL( label_block );

  //{ // Begin sweeping block:

    // Just visit the block and do the stuff there:
    node->block()->accept( this, lvl + 2 );

  //}

  // Pre-label align:
  _pf.ALIGN();

  // Label to point the place where 'by' is updated:
  _pf.LABEL( label_step );

  // Assign a new value to 'lvalue':
  lvalue_assignment = new cdk::assignment_node(
    node->lineno(), // Line number of the sweeping, since it happens at the sweeping header.
    node->lvalue(), // Where to be stored.
    new cdk::add_node(
      node->lineno(), // Line number of the sweeping, since it happens at the sweeping header.
      new cdk::rvalue_node( // Need to create a wrapper to 'lvalue' to use it as an expression. Just following yacc: $$ = new cdk::rvalue_node (LINE, $1);
        node->lineno(), // Same.
        node->lvalue()  // The value.
      ),      // 'lvalue' plus >>
      node->by_expression() // >> plus 'by' ('by' can be negative, so all fine)
    ) // What to store.
  );
  // Do:
  lvalue_assignment->accept( this, lvl + 2 );

  // Iterate: (goes to the test)
  _pf.JMP( label_test );

  // Align:
  _pf.ALIGN();

  // Label to denote the end of the sweeping:
  _pf.LABEL( label_end );

  // Get out of the sweeping context:
  exit_sweeping();
}
void gr8::postfix_writer::do_stop_node(gr8::stop_node *const node, int lvl) {
  // Search if we are in a sweeping context:
  if ( !sweeping() ) {
    // We aren't, so throw error, because we have a 'stop' outside a sweeping:
    throw std::string( "Found 'stop' instruction outside of a sweeping!" );
  }

  // Number of sweepings to stop:
  int sweepings_to_stop = node->argument();
  std::cout << sweepings_to_stop << std::endl;
  while ( --sweepings_to_stop ) {
    // Pop sweeping end label:
    sweeping_stop();
    // One less sweeping to stop:
  std::cout << sweepings_to_stop << std::endl;
  }
  // Just jump onto the end of the desired sweeping:
  _pf.JMP( sweeping_stop() );
}
void gr8::postfix_writer::do_again_node(gr8::again_node *const node, int lvl) {
  // Search if we are in a sweeping context:
  if ( !sweeping() ) {
    // We aren't, so throw error, because we have an 'again' outside a sweeping:
    throw std::string( "Found 'again' instruction outside of a sweeping!" );
  }

  // Number of sweepings to stop:
  int sweepings_to_skip = node->argument();
  while ( --sweepings_to_skip ) {
    // Pop sweeping end label:
    sweeping_again();
    // One less sweeping to stop:
  }
  // Just jump onto the end of the desired sweeping:
  _pf.JMP( sweeping_again() );
}
void gr8::postfix_writer::do_return_node(gr8::return_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  
  // Save current context:
  std::shared_ptr<gr8::symbol> this_context = get_context();
  basic_type *this_context_type = this_context->type();

  // To store the return type: (default to void)
  basic_type *this_return_type = new basic_type( SIZE::_VOID, basic_type::TYPE_VOID );
  
  // Check if we are inside a function:
  if ( !this_context ) {
    // We don't, so throw error, because we have a 'return' outside a function:
    throw std::string( "Found 'return' instruction outside of a function!" );
  }

  // Check if it has an argument:
  if ( node->argument() != nullptr ) {
    // Visit it:
    node->argument()->accept( this, lvl + 2 );

    // Save its type:
    this_return_type = copy_type( node->argument()->type() );
  }

  // Compare its argument type against the function type:
  if ( !compatible_types( this_context_type, this_return_type ) ) {
    throw std::string( "Return type doesn't match with function type!" );
  }

  // Otherwise, all fine. But check if it needs cast:
  if ( needs_cast( this_return_type, this_context_type) ) {
    _pf.I2D();
  }

  // Actually return from the function:
  if ( is_double( this_context_type ) ) {
    // To eax:
    _pf.STFVAL64();
  } else if ( !is_void( this_context_type ) ) {
    // To eax:
    _pf.STFVAL32();
  }

  // Standard return:
  _pf.LEAVE();
  _pf.RET();

}
void gr8::postfix_writer::do_post_node(gr8::post_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Visit the argument to be printed:
  node->argument()->accept( this, lvl + 2 );

  // Check its type:
  if ( is_int( node->argument()->type() ) ) {
    // int:
    _pf.CALL( "printi" );
    _pf.TRASH( SIZE::_INT ); // delete the printed value
  } else if ( is_double( node->argument()->type() ) ) {
    // double:
    _pf.CALL( "printd" );
    _pf.TRASH( SIZE::_DOUBLE ); // delete the printed value
  } else if ( is_string( node->argument()->type() ) ) {
    // string:
    _pf.CALL( "prints" );
    _pf.TRASH( SIZE::_STRING ); // delete the printed value's address
  } else {
    // Other:
    std::cerr << "Invalid type for 'post'! (somehow)" << std::endl;
    exit( 1 );
  }

  // "post" prints a new line:
  _pf.CALL( "println" );
}
void gr8::postfix_writer::do_tweet_node(gr8::tweet_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Visit the argument to be printed:
  node->argument()->accept( this, lvl + 2 );

  // Check its type:
  if ( is_int( node->argument()->type() ) ) {
    // int:
    _pf.CALL( "printi" );
    _pf.TRASH( SIZE::_INT ); // delete the printed value
  } else if ( is_double( node->argument()->type() ) ) {
    // double:
    _pf.CALL( "printd" );
    _pf.TRASH( SIZE::_DOUBLE ); // delete the printed value
  } else if ( is_string( node->argument()->type() ) ) {
    // string:
    _pf.CALL( "prints" );
    _pf.TRASH( SIZE::_STRING ); // delete the printed value's address
  } else {
    // Other:
    std::cerr << "Invalid type for 'tweet'! (somehow)" << std::endl;
    exit( 1 );
  }
}
void gr8::postfix_writer::do_input_node(gr8::input_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  // Try to find a context for the input:
  std::shared_ptr<gr8::symbol> variable = get_variable();
  //std::shared_ptr<gr8::symbol> context  = get_context();

  basic_type *input_context_type;
  // First, check the variable:
  if ( variable ) {
    input_context_type = copy_type( variable->type() );
  }
  // Then the function:
  /*else if ( context ) {
    input_context_type = copy_type( context->type() ); // TODO: this doesn't make sense. However, I need to account for "use input for f".
  }*/
  // Otherwise, default to int: (for 'post' and 'tweet')
  else {
    input_context_type = new basic_type( SIZE::_INT, basic_type::TYPE_INT );
  }

  // Read:
  if ( is_double( input_context_type ) ) {
    // Read double:
    _pf.CALL( "readd" );
    // Store it "somewhere":
    _pf.LDFVAL64();
  } else if ( is_int( input_context_type ) ) {
    // Read int: TODO: Can I read a pointer address as int to be used later?
    // Store it "somewhere":
    _pf.LDFVAL32();
  } else {
    // Cannot read other types:
    throw std::string( "Invalid type for 'input'! (shouldn't happen...)" );
  }
}
void gr8::postfix_writer::do_plus_node(gr8::plus_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  
  // Visit the argument:
  node->argument()->accept( this, lvl + 2 );
}
void gr8::postfix_writer::do_objects_node(gr8::objects_node *const node, int lvl) { // TODO
  ASSERT_SAFE_EXPRESSIONS; // Makes space for X things of what it points to. // https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_9 (m alloc init)

  // Due to the type checker, it points to anything. So check its context:
  basic_type *context_type = get_type();

  // Being safe:
  if ( !context_type ) {
    throw std::string( "Found 'objects' being misused!" ); // Can't simply have '5 objects'.
  }

  // Check if the context type is correct:
  if ( !is_pointer( context_type ) && !context_type->subtype() && !is_unspec( context_type->subtype() ) ) {
    throw std::string( "Variable must be a pointer with a specified subtype to use 'objects'!" );
  }

  // Coerce this node subtype into the context subtype:
  node->type()->_subtype = copy_type( context_type->subtype() );

  // Now we can focus. (similar logic to cell, but the reverse)
  // Visit the argument: (this should leave its value on the stack) (number of positions we want to reserve)
  node->argument()->accept( this, lvl + 2 );

  // Add the size of this node's subtype to the stack: (size of each position that we want to reserve)
  _pf.INT( node->type()->subtype()->size() );

  // Multiply them, to get the number of addresses we need to save:
  _pf.MUL();

  // Make the above space:
  _pf.ALLOC(); //care should be taken when local variables exist.

  //_pf.DECR(  node->type()->subtype()->size()  );
  // Found that we need to adjust the stack pointer: (Pushes the value of the stack pointer)
  _pf.SP(); // After some tries, this works, so...

  // Clean context type:
  clean_type();
}
void gr8::postfix_writer::do_address_node(gr8::address_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Visit:
  node->argument()->accept( this, lvl + 2 ); // lvalue being accepted here (an identifier, for instance) will push the address into the stack.
}
void gr8::postfix_writer::do_cell_node(gr8::cell_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Visit the pointer: (place to count from where things will be placed)
  node->pointer()->accept( this, lvl + 2 );
  
  // Visit the position index: (leaves on the stack how many "blocks" we will go into, on the above address)
  node->position()->accept( this, lvl + 2 );

  // Add size (an int) of the type to the stack: (see https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_7  vec + ix)
  _pf.INT( node->type()->size() );

  // Multiply the size of it with the index: (address leaps into the array)
  _pf.MUL(); // Will leave the stack with the number of memory positions we need to walk.

  // Add them to the initial pointer value: (leave the position where things can get placed in)
  _pf.ADD();
}
void gr8::postfix_writer::do_block_node(gr8::block_node *const node, int lvl) { // DONE
  // It's a block so we need to create scope:
  _symtab.push();

  // Visit child nodes:
  node->declarations()->accept( this, lvl + 2 );
  node->instructions()->accept( this, lvl + 2 );

  // Exit scope:
  _symtab.pop();
}
void gr8::postfix_writer::do_null_node(gr8::null_node *const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Represented by the literal integer 0: 
  _pf.INT( 0 ); // https://en.wikipedia.org/wiki/Null_pointer#C++
}

void gr8::postfix_writer::do_variable_declaration_node(gr8::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Fetch the variable:
  std::shared_ptr<gr8::symbol> variable = _symtab.find_local( node->name() ); // Can't override declarations, so, due to type checker, it will exist locally.

  // Fetch the context into a var:
  std::shared_ptr<gr8::symbol> context = get_context();

  // Set global left value size: (for info: check postfix_writer.h)
  set_type( node->type() );

  // Set variable context:
  set_variable( variable );

  // Check if the declaration belongs to a function:
  if ( context ) {
    
    // It does, so check if we are on the parameters context:
    if ( parameters_context() ) {
      // We are, so set this variable offset:
      variable->offset( context->parameters_offset() );
  /*std::cout << "\targument for: " << context->name() << std::endl;
  std::cout << "\targument name: " << variable->name() << std::endl;
  std::cout << "\targument offset: " << variable->offset() << std::endl;*/
      // Update the context parameters offset:
      update_context_parameters_offset( variable->type()->size() );
    }

    // Else, we are inside a function body:
    else {
      // Set the offset for this variable inside the function:
      variable->offset( context->body_offset() - variable->type()->size() );
      // Update the context body offset:
      update_context_body_offset( variable->offset() );
      // Check possible initial value:
      if ( node->initially() != nullptr ) {

        // There is one, so visit it:
        node->initially()->accept( this, lvl + 2 );

        // In case the 'initially' has an int but the var is a double:
        if ( needs_cast( node->initially()->type(), node->type() ) ) {
          _pf.I2D();
        }

        // Load this variable's address:
        _pf.LOCAL( variable->offset() );
        //std::cout << "LOCAL " << variable->offset() << std::endl;

        // Add value to the register:
        if ( is_double( node->type() ) ) {
          // If double:
          _pf.STDOUBLE();
        } else {
          //std::cout << "STINT " << std::endl;
          // If int;
          _pf.STINT();
        }

      }
  /*std::cout << "\tfvar for: " << context->name() << std::endl;
  std::cout << "\tfvar name: " << variable->name() << std::endl;
  std::cout << "\tfvar offset: " << variable->offset() << std::endl;*/
    } // Close "we are in a function body" (args vs. body context).

  }

  // Else, we are on the global (most outer) scope:
  else {
    // Set as global: (needed because it doesn't belong to a function context)
    set_global_variable();
    variable->global();

    // Check possible initalization:
    if ( node->initially() != nullptr ) {
      // There is, so visit it: (needs to be like this because of the strings...)
      node->initially()->accept( this, lvl + 2 ); // SINT, SDOUBLE etc...

    } else {
      // No initial value, so:
      _pf.BSS();
      // Mandatory align:
      _pf.ALIGN();
      // Label:
      _pf.LABEL( variable->name() );
      // Make space for it:
      _pf.SALLOC( variable->type()->size() );
    }

  }

  /*std::cout << "\tvariable!:" << std::endl;
  std::cout << "\tvariable name: " << variable->name() << std::endl;
  std::cout << "\tvariable public: " << (variable->is_public() ? "true" : "false") << std::endl;
  std::cout << "\tvariable is use: " << variable->is_use() << std::endl;
  std::cout << "\tvariable offset: " << variable->offset() << std::endl;*/
  
  // Check if it is an external variable: TODO: check this (works for all cases?).
  if ( node->isUse() ) {
    add_external_identifier( node->name() );
    //std::cout << "\tvariable extern: " << "yes" << std::endl;
  }

  // Clean variable context:
  clean_variable();

}

void gr8::postfix_writer::do_function_declaration_node(gr8::function_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Add as an external identifier:
  add_external_identifier( node->name() );
}
void gr8::postfix_writer::do_function_definition_node(gr8::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Get this function symbol:
  std::shared_ptr<gr8::symbol> symbol = _symtab.find( node->name() ); // TODO: Need to check if it exists?
  
  // Add as an internal identifier, so we can now we don't need to declare it as external later.
  add_internal_identifier( symbol->name() );

  // Create a pseudo-scope, for the args: (I can't think of a better way to do this.)
  _symtab.push();

 /* std::cout << "function DEF!:" << std::endl;
  std::cout << "function name: " << symbol->name() << std::endl;
  std::cout << "function size: " << symbol->size() << std::endl;
  std::cout << "function public: " << (symbol->is_public() ? "true" : "false") << std::endl;
  std::cout << "function n of param: " << symbol->number_of_parameters() << std::endl;
  std::cout << "function type size: " << symbol->type()->size() << std::endl;*/

  // Define segment:
  _pf.TEXT();
  // Align:
  _pf.ALIGN();
  // Check if it can be used by other modules:
  if ( symbol->is_public() ) {
    _pf.GLOBAL( symbol->name(), _pf.FUNC() );
  }
  // Label:
  _pf.LABEL( symbol->name() );
  // Make space:
  _pf.ENTER( symbol->size() );

  // Set the context:
  set_context( symbol );

  // Visit the parameters:
  node->parameters()->accept( this, lvl + 2 );

  // Set the context to be the function body:
  add_context_level();

  // Visit the body:
  node->body()->accept( this, lvl + 2 );

  // Clean the context:
  clean_context();

  // Default return, in case the function doesn't have the 'return' instruction:
  _pf.LEAVE();
  _pf.RET();

  // Leave function args scope:
  _symtab.pop();

  // If we are in the _main function:
  if ( symbol->name() == "_main" ) {
    // Import external identifiers:
    declare_external_identifiers();
  }

  //std::cout << "function parameters offset: " << symbol->parameters_offset() << std::endl;
  //std::cout << "function body offset: " << symbol->body_offset() << std::endl;
}
void gr8::postfix_writer::do_function_invocation_node(gr8::function_invocation_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS; // https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_12 ( f() @ main)

  // Get this function symbol:
  std::shared_ptr<gr8::symbol> symbol = _symtab.find( node->name() ); // TODO: Need to check if it exists?

  /*std::cout << "CALLING function!:" << std::endl;
  std::cout << "function name: " << symbol->name() << std::endl;
  std::cout << "function size: " << symbol->size() << std::endl;
  std::cout << "function public: " << (symbol->is_public() ? "true" : "false") << std::endl;
  std::cout << "function n of param: " << symbol->number_of_parameters() << std::endl;
  std::cout << "function type size: " << symbol->type()->size() << std::endl;
  std::cout << "function will trash: " << symbol->parameters_offset() - 8 << std::endl;*/
  
  // Need to place the args on the stack in backwards:
  enter_call();

  // Visit:
  node->arguments()->accept( this, lvl + 2 );

  // Remove function call scope: (for args)
  leave_call();
  
  // Make the function call:
  _pf.CALL( symbol->name() );

  // When done, remove the function arguments from the stack:
  _pf.TRASH( symbol->parameters_offset() - 8 ); // SP + FP

  // Push the result:
  if ( is_double( node->type() ) ) {
    // Double:
    _pf.LDFVAL64();
  } else if ( !is_void( node->type() ) ) {
    // Int, etc.:
    _pf.LDFVAL32();
  }
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void gr8::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // TODO
}
void gr8::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // TODO
}
void gr8::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<gr8::symbol> variable = get_variable();

  // Check if we are dealing with a global var:
  if ( variable_is_global() ) {
    // We are, so:
    _pf.DATA();
    // Mandatory align:
    _pf.ALIGN();
    // Check if it can be used by other modules:
    if ( variable->is_public() ) {
      _pf.GLOBAL( variable->name(), _pf.OBJ() );
    }
    // Label:
    _pf.LABEL( get_variable()->name() );
    // Declare a static double precision (64-bit) floating point value:
    _pf.SDOUBLE( node->value() );
  } else {
    // We aren't, so just push it into the stack:
    _pf.DOUBLE( node->value() ); // push a double.
  }
}
void gr8::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Visit the argument: (leave the result on the stack)
  node->argument()->accept( this, lvl + 2 );

  // Push a 0:
  _pf.INT( 0 );
  // Compare them: (if we had a zero before, then EQ will say true (1); that's what we want, because 'not 0' is 1. And vice-versa.)
  _pf.EQ();
}
void gr8::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Define label:
  std::string lbl_end = mklbl( ++_lbl );
  
  // Visit the left node: (leaves the value 0 or not 0)
  node->left()->accept( this, lvl + 2 );

  // AND: if false (0), no further check needed:
  _pf.JZ( lbl_end );

  // (Else) Visit the right node: (leaves 0 or not 0)
  node->right()->accept( this, lvl );

  // Mandatory alignement: 
  _pf.ALIGN();

  // Insert label:
  _pf.LABEL( lbl_end );
}
void gr8::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) { // DONE
  // CHECK
  ASSERT_SAFE_EXPRESSIONS;

  // Define label:
  std::string lbl_end = mklbl( ++_lbl );
  
  // Visit the left node: (leaves the value 0 or not 0)
  node->left()->accept( this, lvl + 2 );

  // OR: if true (not 0), no further check needed:
  _pf.JNZ( lbl_end );

  // (Else) Visit the right node: (leaves 0 or not 0)
  node->right()->accept( this, lvl );

  // Mandatory alignement: 
  _pf.ALIGN();

  // Insert label:
  _pf.LABEL( lbl_end );
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) { // DONE
  // Check if it is a function call:
  if ( !is_calling() ) {
    // Default behaviour:
    for ( size_t i = 0; i < node->size(); i++ ) {
      node->node( i )->accept( this, lvl );
    }
  } else {
    // For function calls, place the arguments backwards:
    for ( size_t i = node->size(); i > 0; i-- ) { // size_t should never be less than 0 here.
      node->node( i - 1 )->accept( this, lvl );
    }
  }
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<gr8::symbol> variable = get_variable();

  // Check if we are dealing with a global var:
  if ( variable_is_global() ) {
    // We are, so:
    _pf.DATA();
    // Mandatory align:
    _pf.ALIGN();
    // Check if it can be used by other modules:
    if ( variable->is_public() ) {
      _pf.GLOBAL( variable->name(), _pf.OBJ() );
    }
    // Label:
    _pf.LABEL( variable->name() );
    // Declare a static 32-bit integer value:
    _pf.SINT( node->value() );
  } else {
    //std::cout << "INT " << node->value() << std::endl;
    // We aren't, so just push it into the stack:
    _pf.INT( node->value() ); // push an integer
  }
}

void gr8::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) { // 
  ASSERT_SAFE_EXPRESSIONS;

  // Get the variable:
  std::shared_ptr<gr8::symbol> variable = get_variable();

  // Label:
  int string_label = ++_lbl;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL( mklbl( string_label ) ); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  // Check if it's a global var: (check https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation)
  if ( variable_is_global() ) {
    // It is, so:
    _pf.DATA();
    // Mandatory align:
    _pf.ALIGN();
    // Check if it can be used by other modules:
    if ( variable->is_public() ) {
      _pf.GLOBAL( variable->name(), _pf.OBJ() );
    }
    // Label:
    _pf.LABEL( variable->name() );
    // Declare the address associated with name:
    _pf.SADDR( mklbl( string_label ) );
  } else {
    /* leave the address on the stack */
    _pf.TEXT(); // return to the TEXT segment
    _pf.ADDR( mklbl( string_label ) ); // the string to be printed
  }
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Visit the argument:
  node->argument()->accept( this, lvl );

  // Check node's type:
  if ( is_int( node->type() ) ) {
    // Is int:
    _pf.NEG();

  } else {
    // Is double: (because of type_checker)
    _pf.DNEG();

  }
}

//---------------------------------------------------------------------------
void gr8::postfix_writer::do_arithmetics( cdk::binary_expression_node * const node, int lvl ) { // DONE
  // Visit the left node: (value)
  node->left()->accept( this, lvl + 2 );

  // Check if the left node needs to be casted:
  if ( needs_cast( node->left()->type(), node->type() ) ) {
    // Cast it:
    _pf.I2D();
  }

  // Visit the right node: (value)
  node->right()->accept( this, lvl + 2 );

  // Check if the right node needs to be casted:
  if ( needs_cast( node->right()->type(), node->type() ) ) {
    // Cast it:
    _pf.I2D();
  }
}
void gr8::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_arithmetics( node, lvl );

  // Do the operation:
  if ( is_double( node->type() ) ) {
    // double:
    _pf.DADD();
  } else {
    // int || pointer:
    _pf.ADD();
  }
}
void gr8::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_arithmetics( node, lvl );

  // Do the operation:
  if ( is_double( node->type() ) ) {
    // double:
    _pf.DSUB();
  } else {
    // int || pointer:
    _pf.SUB();
  }
}
void gr8::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_arithmetics( node, lvl );

  // Do the operation:
  if ( is_double( node->type() ) ) {
    // double:
    _pf.DMUL();
  } else {
    // int:
    _pf.MUL();
  }
}
void gr8::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_arithmetics( node, lvl );

  // Do the operation:
  if ( is_double( node->type() ) ) {
    // double:
    _pf.DDIV();
  } else {
    // int:
    _pf.DIV();
  }
}
void gr8::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) { // DONE
  ASSERT_SAFE_EXPRESSIONS;

  // Can only be int, so just visit:
  node->left ()->accept( this, lvl );
  node->right()->accept( this, lvl );
  
  // Do the operation:
  _pf.MOD();
}

void gr8::postfix_writer::do_comparison( cdk::binary_expression_node * const node, int lvl ) { // DONE
  // Visit the left node:
  node->left()->accept( this, lvl + 2 );

  // Create flag to check if there is a double:
  bool flag_double = false;

  // Check if the left node needs to be cast:
  if ( needs_cast( node->left()->type(), node->right()->type() ) ) {
    // Cast it:
    _pf.I2D();
    flag_double = true;
  }

  // Visit the right node:
  node->right()->accept( this, lvl + 2 );

  // Check if the right node needs to be cast:
  if ( needs_cast( node->right()->type(), node->left()->type() ) ) {
    // Cast it:
    _pf.I2D();
    flag_double = true;
  }

  // If there's a double, we need to properly compare them:
  if ( flag_double ) {
    _pf.DCMP(); // Compare two doubles.
    _pf.INT( 0 ); // Push a 0. This is needed to compare with the above result, using the proper comparison operator.
  }
}
void gr8::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_comparison( node, lvl );

  // Do the operation:
  _pf.LT();
}
void gr8::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  // unused.
  /*ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();*/
}
void gr8::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  // unused.
  /*ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();*/
}
void gr8::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_comparison( node, lvl );

  // Do the operation:
  _pf.GT();
}
void gr8::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  // unused.
  /*ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();*/
}
void gr8::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // Standard check:
  do_comparison( node, lvl );

  // Do the operation:
  _pf.EQ();
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Get this identifier symbol: (variable or function)
  std::shared_ptr<gr8::symbol> symbol = _symtab.find( node->name() );

  // Check if it is global:
  if ( symbol->is_global() ) { // if ( is_global( symbol ) ) { TODO: think about this (too much work for now and no time).
    // Global, so load its name address:
    _pf.ADDR( node->name() );
  } else {
    // Local, so load its offset on the function address:
    _pf.LOCAL( symbol->offset() ); // https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_7
  }
}

void gr8::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Visit:
  node->lvalue()->accept(this, lvl);

  // Operation by type:
  if ( is_double( node->type() ) ) {
    // Load double:
    _pf.LDDOUBLE();
  } else {
    // Load int:
    _pf.LDINT();
  }
}

void gr8::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) { // TODO
  ASSERT_SAFE_EXPRESSIONS; // https://www.l2f.inesc-id.pt/~david/w/pt/Code_Generation/Exercise_7 (ix--, bt in gr8 there's no return in assignment)
  /*node->rvalue()->accept(this, lvl); // determine the new value
  //_pf.DUP32();
  if (new_symbol() == nullptr) {
    node->lvalue()->accept(this, lvl); // where to store the value
  } else {
    _pf.DATA(); // variables are all global and live in DATA
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(new_symbol()->name()); // name variable location
    reset_new_symbol();
    _pf.SINT(0); // initialize it to 0 (zero)
    _pf.TEXT(); // return to the TEXT segment
    node->lvalue()->accept(this, lvl);  //DAVID: bah!
  }
  _pf.STINT(); // store the value at address*/

  // Set global left value size: (for info: check postfix_writer.h)
  set_type( node->type() );
  
  // Visit the right value: (value (this is an expression!))
  node->rvalue()->accept( this, lvl + 2 ); // determine the new value

  // In case we are assigning an int to a double:
  if ( needs_cast( node->rvalue()->type(), node->type() ) ) {
    _pf.I2D();
  }

  // Visit the left value: (address where to place the value) (identifier node or cell array)
  node->lvalue()->accept( this, lvl + 2 ); // where to store the value

  // Store according to its type:
  if ( is_double( node->type() ) ) {
    // Double:
    _pf.STDOUBLE();
  } else {
    // Int
    _pf.STINT();
  }

  // Just to be safe:
  clean_type();

}

//---------------------------------------------------------------------------

/*void gr8::postfix_writer::do_program_node(gr8::program_node * const node, int lvl) {
  // Note that Simple didn't have functions. Thus, it didn't need
  // a function node. However, it had to start in the main function.
  // program_node (representing the whole program) doubled as a
  // main function node.

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  _pf.ENTER(0);  // Simple didn't implement local variables

  node->statements()->accept(this, lvl);

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();

  // these are just a few library function imports
  _pf.EXTERN("readi");
  _pf.EXTERN("printi");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
}*/

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_evaluation_node(gr8::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Visit it:
  node->argument()->accept( this, lvl ); // determine the value

  // Save the type into a var:
  basic_type *node_type = node->argument()->type();

  // Check types:
  if ( is_int( node_type ) ) {

    // (int)
    _pf.TRASH( SIZE::_INT ); // delete the evaluated value

  } else if ( is_double( node_type ) ) {

    // (double)
    _pf.TRASH( SIZE::_DOUBLE );

  }
  else if ( is_string( node_type ) ) {

    // (string)
    _pf.TRASH( SIZE::_STRING ); // delete the evaluated value's address

  } else if ( is_pointer( node_type ) ) {

    // (pointer)
    _pf.TRASH( SIZE::_POINTER );

  } else if ( is_void( node_type ) ) {

    // (void)
    // Do nothing, because it puts nothing on the stack.

  } else {

    std::cerr << node->lineno() << " >> @evaluation THIS SHOULD NOT HAPPEN!" << std::endl;
    exit(1);

  }
}

/*void gr8::postfix_writer::do_print_node(gr8::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->type()->name() == basic_type::TYPE_INT) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->type()->name() == basic_type::TYPE_STRING) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  _pf.CALL("println"); // print a newline
}*/

//---------------------------------------------------------------------------

/*void gr8::postfix_writer::do_read_node(gr8::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.CALL("readi");
  _pf.LDFVAL32();
  node->argument()->accept(this, lvl);
  _pf.STINT();
}*/

//---------------------------------------------------------------------------

/*void gr8::postfix_writer::do_while_node(gr8::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}*/

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_if_node(gr8::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void gr8::postfix_writer::do_if_else_node(gr8::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

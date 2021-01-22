# parser
Math expressions parser implementation

Builds a binary tree based on the expression
Can do different operations with the trees like arithmetic operations or derivative calculating
Use _parseData::_node* tree = parseExpression(std::string s); to build a tree
Use _parseData::_node* derivative = tree->derivative(); to take derivative
You can evaluate the function at the point x by using (*tree)(x) or tree->operator()(x)

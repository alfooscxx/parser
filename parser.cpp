// * source for plot builder
// *

#include "parser.h"

using namespace _parseData;

// * algebraic operations implementation

_nodeOperator* binary_operation(_node* first, _node* second, _nodeOperator::_operation operation)
{
	_nodeOperator* result = new _nodeOperator(first, second, operation);
	return result;
}

// * implementation of calling tree nodes as the function they contain

const double _nodeOperator::operator()(const double x) const noexcept
{
	switch (operation)
	{
	case _nodeOperator::_operation::ADDITION:
		return((*firstOperand)(x) + (*secondOperand)(x));
		break;
	case _nodeOperator::_operation::SUBTRACTION:
		return((*firstOperand)(x) - (*secondOperand)(x));
		break;
	case _nodeOperator::_operation::MULTIPLICATION:
		return((*firstOperand)(x) * (*secondOperand)(x));
		break;
	case _nodeOperator::_operation::DIVISION:
		return((*firstOperand)(x) / (*secondOperand)(x));
		break;
	case _nodeOperator::_operation::POWER:
		return(pow((*firstOperand)(x), (*secondOperand)(x)));
		break;
	default:
		break;
	}
}

const double _nodeFunction::operator()(const double x) const
{
	switch (this->function)
	{
	case _nodeFunction::_function::COS:
		return cosf((*argument)(x));
		break;
	case _nodeFunction::_function::CTAN:
		return 1 / tanf((*argument)(x));
		break;
	case _nodeFunction::_function::EXP:
		return expf((*argument)(x));
		break;
	case _nodeFunction::_function::LOG:
		return logf((*argument)(x));
		break;
	case _nodeFunction::_function::SIN:
		return sinf((*argument)(x));
		break;
	case _nodeFunction::_function::TAN:
		return tanf((*argument)(x));
		break;
	default:
		break;
	}
}

// * Destructors

_nodeFunction::~_nodeFunction()
{
	delete this->argument;
}

_nodeOperator::~_nodeOperator()
{
	delete this->firstOperand;
	delete this->secondOperand;
}

size_t findOperator(const _substring& _expression, const _nodeOperator::_operation operation)
{
	char _operation;
	switch (operation)
	{
	case _nodeOperator::_operation::ADDITION:
		_operation = '+';
		break;
	case _nodeOperator::_operation::DIVISION:
		_operation = '/';
		break;
	case _nodeOperator::_operation::MULTIPLICATION:
		_operation = '*';
		break;
	case _nodeOperator::_operation::POWER:
		_operation = '^';
		break;
	case _nodeOperator::_operation::SUBTRACTION:
		_operation = '-';
		break;
	}
	int parCounter = 0;
	int exprBegin;
	for (exprBegin = _expression.begin; exprBegin < _expression.end && _expression[exprBegin] == ' '; ++exprBegin)
		;
	if (_expression[exprBegin] == '(')
		++parCounter;
	else if (_expression[exprBegin] == ')')
		--parCounter;
	for (size_t i = exprBegin + 1; i <= _expression.end; ++i)
	{
		if (_expression[i] == '(')
			++parCounter;
		else if (_expression[i] == ')')
			--parCounter;
		if ((parCounter == 0) && (_expression[i] == _operation))
			return i;
	}
	return 0;
}

_nodeexpr parseSubexpression(const _substring& substring)
{
	const size_t end = substring.end;
	_substring resultString(substring); // resultString.begin = substring.begin
	size_t& subEnd = resultString.end;
	size_t subBegin;
	for (subBegin = substring.begin; subBegin <= end && substring[subBegin] == ' '; ++subBegin)
		;
	switch (substring[subBegin])
	{
	case 'x':
	{
		for (subEnd = subBegin + 1; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
			;
		--subEnd;
		_node* result = new _node();
		return _nodeexpr(resultString, result);
	}
	case '(':
	{
		size_t parPosition;
		int parCounter = 0;
		for (parPosition = subBegin; parCounter != 1 || substring[parPosition] != ')'; ++parPosition)
		{
			if (parPosition == end)
				throw std::exception();
			if (substring[parPosition] == '(')
				++parCounter;
			else if (substring[parPosition] == ')')
				--parCounter;
		}
		for (subEnd = parPosition + 1; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
			;
		--subEnd;
		_substring inside(substring.expression, subBegin + 1, parPosition - 1);
		_node* parsedNode = parseExpression(inside);
		return _nodeexpr(resultString, parsedNode);
	}
	case '-':
	{
		subEnd = subBegin;
		_nodeConst* result = new _nodeConst(-1);
		return _nodeexpr(resultString, result);
	}
	}
	if (isWord(substring, subBegin, "sqrt"))
	{
		_substring squareArg = substring;
		squareArg.begin = subBegin + 4;
		_nodeexpr squareArgFunc = parseSubexpression(squareArg);
		subEnd = squareArgFunc.expression.end;
		_nodeConst* power = new _nodeConst(0.5);
		_nodeOperator* sqrt = binary_operation(squareArgFunc.node, power, _nodeOperator::_operation::POWER);
		return _nodeexpr(resultString, sqrt);
	}
	else
	{
		for (int i = 5; i < 11; ++i)
		{
			if (isWord(substring, subBegin, Operations[i]))
			{
				_substring arg = substring;
				arg.begin = subBegin + (Operations[i]).size();
				_nodeexpr argFunc = parseSubexpression(arg);
				resultString.end = argFunc.expression.end;
				_nodeFunction* func = new _nodeFunction(argFunc.node, static_cast<_nodeFunction::_function>(i - 5));
				return _nodeexpr(resultString, func);
			}
		}
	}
	// probably expression is a number
	std::string string_number = "";
	size_t i;
	for (i = subBegin; i <= end && (substring[i] == 46 || (substring[i] >= 48 && substring[i] <= 57)); ++i)
	{
		string_number.push_back(substring[i]);
	}
	if (string_number.length() == 0)
		throw std::exception();
	for (subEnd = i; subEnd <= end && substring[subEnd] == ' '; ++subEnd)
		;
	--subEnd;
	double number = std::stod(string_number);
	_nodeConst* constNumber = new _nodeConst(number);
	return _nodeexpr(resultString, constNumber);
}

bool isWord(_substring string, size_t wordBegin, std::string word)
{
	for (size_t i = 0; i < word.length(); ++i)
	{
		if (wordBegin + i > string.end)
			return false;
		if (string[wordBegin + i] != word[i])
			return false;
	}
	return true;
}

_node* parseExpression(const _substring& expr)
{
	const size_t begin = expr.begin;
	const size_t end = expr.end;
	_nodeexpr firstExpr = parseSubexpression(expr); // first math subexpression
	if (firstExpr.expression.end == end) // if the first expr is the whole string
		return firstExpr.node;
	size_t operatorPlace;
	for (int i = 0; i < 4; ++i)
	{
		const _nodeOperator::_operation operation = static_cast<_nodeOperator::_operation>(i);
		operatorPlace = findOperator(expr, operation);
		if (operatorPlace && operatorPlace != end)
		{
			_node* left = parseExpression(_substring(expr.expression, begin, operatorPlace - 1));
			_node* right = parseExpression(_substring(expr.expression, operatorPlace + 1, end));
			return (binary_operation(left, right, operation));
		}
	}
	if (expr[firstExpr.expression.end + 1] != '^')
	{
		// can throw an exception if the expression cannot be parsed
		// returns a product of the first subexpr and the rest of the expression
		_node* restFunction = parseExpression(_substring(expr.expression,
			firstExpr.expression.end + 1,
			end));
		return binary_operation(firstExpr.node, restFunction, _nodeOperator::_operation::MULTIPLICATION);
	}
	operatorPlace = findOperator(expr, _nodeOperator::_operation::POWER);
	if (operatorPlace && operatorPlace != end)
	{
		_node* left = parseExpression(_substring(expr.expression, begin, operatorPlace - 1));
		_node* right = parseExpression(_substring(expr.expression, operatorPlace + 1, end));
		return (binary_operation(left, right, _nodeOperator::_operation::POWER));
	}
}

std::function<double(double)> getFunction(std::string expr)
{
	_node* tree = parseExpression(_substring(expr));
	return std::function<double(double)>([=](const double x)
	{
		return (*tree)(x);
	});
}

std::function<double(double)> getKDerivative(std::string expr, unsigned k = 1)
{
	_node** derivatives = new _node*[k];
	derivatives[0] = parseExpression(expr)->derivative();
	for (int i = 1; i < k; ++i)
	{
		derivatives[i] = (derivatives[i - 1])->derivative();
	}
	return std::function<double(double)>([=](const double x)
	{
		return (*derivatives[k - 1])(x);
	});
}

int main()
{
	std::string expr;
	std::getline(std::cin, expr);
	_node* tree = parseExpression(_substring(expr));
	_node* derivative = tree->derivative();
	tree->printBT("", false);
	derivative->printBT("", false);
	std::function<double(double)> func = getFunction(expr);
	std::function<double(double)> der = getKDerivative(expr, 1);
	double x;
	do
	{
		std::cin >> x;
		std::cout << func(x) << std::endl;
		std::cout << der(x) << std::endl;
	} while (x != 0);
	return 0;
}
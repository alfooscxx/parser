// * plot builder source header
// *

#ifndef _PLOT_SOURCE_H_
#define _PLOT_SOURCE_H_

// * includes
#include <string> //? std::string
#include <functional> //? std::function
#include <cmath> //? std math functions
#include <iostream>

// * details of parser implementation

//* DEBUG INFORMATION

// * TREE PRINTING

static const std::string Operations[] =
{
	" + ",
	" - ",
	" * ",
	" / ",
	" ^ ",
	"sin",
	"cos",
	"tan",
	"ctg",
	"exp",
	"log",
	"sqrt"
};

namespace _parseData
{
	struct _substring
	{
		std::string expression;
		size_t begin;
		size_t end;
		_substring()
		{}
		_substring(const std::string& _expression)
			: expression(_expression), begin(0), end(expression.length() - 1)
		{}
		_substring(const std::string& _expression, const size_t _begin, const size_t _end)
			: expression(_expression), begin(_begin), end(_end)
		{}
		char operator[](const size_t i)
		{
			return expression[i];
		}
		const char operator[](const size_t i) const
		{
			return expression[i];
		}
	};
	// * binary tree containing the internal structure of the expression
	// * parent class containing identical function as itself
	class _node
	{
	public:
		_node() {}
		// * copy constructor
		_node(const _node& _node)
		{}

		virtual ~_node() {}

		virtual _node* clone() const
		{
			return new _node(*this);
		}

		virtual const double operator()(const double x) const { return x; }

		virtual _node* derivative() const;

		virtual _node* simplify() { return this; }

		virtual void printBT(const std::string& prefix, bool isLeft)
		{
			std::cout << prefix;

			std::cout << (isLeft ? "|---" : " ---");

			// print the value of the node
			std::cout << " x " << std::endl;
		}
	};
	// * child class containing constant function
	class _nodeConst : public _node
	{
	private:
		double const_value;
	public:
		_nodeConst(double _value)
			: const_value(_value)
		{}
		// * copy constructor
		_nodeConst(const _nodeConst& _nodeConst)
		{
			const_value = _nodeConst.const_value;
		}

		virtual ~_nodeConst() {}

		virtual _nodeConst* clone() const override
		{
			return new _nodeConst(*this);
		}

		virtual const double operator()(const double x) const noexcept { return const_value; }

		virtual _node* derivative() const;

		virtual _node* simplify() { return this; }

		virtual void printBT(const std::string& prefix, bool isLeft)
		{
			std::cout << prefix;

			std::cout << (isLeft ? "|---" : " ---");

			// print the value of the node
			std::cout << " " << const_value << std::endl;
		}
	};
	// * child class containing different algebraic functions
	class _nodeFunction : public _node
	{
	private:
		_node * argument;
	public:
		enum class _function
		{
			SIN,
			COS,
			TAN,
			CTAN,
			EXP,
			LOG,
		};

		_function function;

		_nodeFunction()
		{}
		_nodeFunction(_node* _argument, _function FUNCTION_TYPE)
			: argument(_argument)
		{
			function = FUNCTION_TYPE;
		}
		_nodeFunction(const _nodeFunction& _nodeFunction)
		{
			argument = _nodeFunction.argument;
			function = _nodeFunction.function;
		}

		virtual ~_nodeFunction();

		virtual _nodeFunction* clone() const override
		{
			_nodeFunction* result = new _nodeFunction;
			result->argument = argument->clone();
			result->function = function;
			return result;
		}

		virtual const double operator()(const double x) const;

		virtual _node* derivative() const;

		virtual _node* simplify();

		virtual void printBT(const std::string& prefix, bool isLeft)
		{
			std::cout << prefix;

			std::cout << (isLeft ? "|---" : " ---");

			// print the value of the node
			std::cout << Operations[static_cast<int>(function) + 5] << std::endl;

			// enter the next tree level - left and right branch
			argument->printBT(prefix + (isLeft ? "|   " : "    "), false);
		}
	};
	// * child class containing binary ariphmetic operations
	class _nodeOperator : public _node
	{
	private:
		_node * firstOperand;
		_node* secondOperand;
	public:
		enum class _operation
		{
			ADDITION,
			SUBTRACTION,
			MULTIPLICATION,
			DIVISION,
			POWER,
		};

		_operation operation;

		_nodeOperator()
		{}
		_nodeOperator(_node* _first, _node* _second)
			: firstOperand(_first), secondOperand(_second)
		{}
		_nodeOperator(_node* _first, _node* _second, _operation OPERATION_TYPE)
			: firstOperand(_first), secondOperand(_second)
		{
			operation = OPERATION_TYPE;
		}
		_nodeOperator(const _nodeOperator& _nodeOperator)
		{
			firstOperand = _nodeOperator.firstOperand;
			secondOperand = _nodeOperator.secondOperand;
			operation = _nodeOperator.operation;
		}

		virtual ~_nodeOperator();

		virtual _nodeOperator* clone() const override
		{
			_nodeOperator* result = new _nodeOperator;
			result->firstOperand = firstOperand->clone();
			result->secondOperand = secondOperand->clone();
			result->operation = operation;
			return result;
		}

		virtual const double operator()(const double x) const noexcept;

		virtual _node* derivative() const;

		virtual _node* simplify();

		virtual void printBT(const std::string& prefix, bool isLeft)
		{
			std::cout << prefix;

			std::cout << (isLeft ? "|---" : " ---");

			// print the value of the node
			std::cout << Operations[static_cast<int>(operation)] << std::endl;

			// enter the next tree level - left and right branch
			firstOperand->printBT(prefix + (isLeft ? "|   " : "    "), true);
			secondOperand->printBT(prefix + (isLeft ? "|   " : "    "), false);
		}
	};
	struct _nodeexpr
	{
		_parseData::_substring expression;
		_parseData::_node* node;
		_nodeexpr()
		{}
		_nodeexpr(const _parseData::_substring& _expression, _parseData::_node* _node)
			: expression(_expression), node(_node)
		{}
	};
};

// * some functions declarations
size_t findOperator(const _parseData::_substring& _expression, const _parseData::_nodeOperator::_operation operation);
_parseData::_node* parseExpression(const _parseData::_substring& expr);
_parseData::_nodeexpr parseSubexpression(const _parseData::_substring& substring);
_parseData::_nodeOperator* binary_operation(_parseData::_node* first, _parseData::_node* second, _parseData::_nodeOperator::_operation operation);
bool isWord(_parseData::_substring string, size_t wordBegin, std::string word);
std::function<double(double)> getFunction(std::string expr);
std::function<double(double)> getKDerivative(std::string expr, unsigned k);

// * ifndef _PLOT_SOURCE_H_
#endif
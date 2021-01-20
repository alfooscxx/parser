// * plot builder source header
// *

#ifndef _PLOT_SOURCE_H_
#define _PLOT_SOURCE_H_

// * includes
#include <utility> //? std::pair
#include <vector> //? std::vector
#include <string> //? std::string
#include <functional> //? std::function
#include <cmath> //? std math functions

// * type aliasing
struct _plotData
{
	using coordinates = std::pair<float, float>;
	using plot = std::vector<coordinates>;
	using function = std::function<float(float)>;
};

// * details of parser implementation
struct _parseData
{
	enum class _exprNode
	{
		ADDITION,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION,
		POWER,
		IDENTICAL,
		CONSTANT,
		SUPERPOSITION,
		//even the sky,
		//Allah himself...
	};
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
	class exprNode
	{
		private:
			_parseData::_exprNode operation;
			exprNode* first;
			exprNode* second;
		public:
			exprNode(_parseData::_exprNode _operation, exprNode* _first, exprNode* _second)
				: operation(_operation), first(_first), second(_second)
			{}
			exprNode(_parseData::exprNode _operation)
				: operation(_operation), first(nullptr), second(nullptr)
			{}
			exprNode()
			{}

			// * basic operations

			friend exprNode operator+(exprNode& _first, exprNode& _second);
			friend exprNode operator-(exprNode& _first, exprNode& _second);
			friend exprNode operator*(exprNode& _first, exprNode& _second);
			friend exprNode operator/(exprNode& _first, exprNode& _second);

			// * other algebraic operations

			friend exprNode power(exprNode& _first, exprNode& _second);
			friend exprNode superposition(exprNode& _first, exprNode& _second);
	};
	struct _nodeexpr
	{
		_parseData::_substring expression;
		_parseData::exprNode node;
		_nodeexpr()
		{}
		_nodeexpr(const _parseData::_substring& _expression, const _parseData::exprNode _node)
			: expression(_expression), node(_node)
		{}
	};
};



// * functors
class lambdaAddition
{
public:
	lambdaAddition(const _plotData::function& _left, const _plotData::function& _right);
	float operator()(float x) const noexcept;
	_plotData::function left;
	_plotData::function right;
};

//
class lambdaSubtraction
{
public:
	lambdaSubtraction(const _plotData::function& _left, const _plotData::function& _right);
	float operator()(float x) const noexcept;
	_plotData::function left;
	_plotData::function right;
};

//
class lambdaMultiplication
{
public:
	lambdaMultiplication(const _plotData::function& _left, const _plotData::function& _right);
	float operator()(float x) const noexcept;
	_plotData::function left;
	_plotData::function right;
};

//
class lambdaDivision
{
public:
	lambdaDivision(const _plotData::function& _left, const _plotData::function& _right);
	float operator()(float x) const;
	_plotData::function left;
	_plotData::function right;
};

//
class lambdaPower
{
public:
	lambdaPower(const _plotData::function& _left, const _plotData::function& _right);
	float operator()(float x) const noexcept;
	_plotData::function left;
	_plotData::function right;
};

//
class lambdaSuper
{
public:
	lambdaSuper(const _plotData::function& _func, const _plotData::function& _arg);
	float operator()(float x) const noexcept;
	_plotData::function func;
	_plotData::function arg;
};

//
class lambdaConstant
{
public:
	lambdaConstant(float x);
	float constant;
	float operator()(float x) const noexcept;
};

//
class lambdaId
{
public:
	lambdaId();
	float operator()(float x) const noexcept;
};

// * some functions declarations
size_t findOperator(const _parseData::_substring& _expression, const _parseData::_exprNode operation);
_plotData::function parseExpression(const _parseData::_substring& expr);
_parseData::_nodeexpr parseSubexpression(const _parseData::_substring& substring);
bool isWord(_parseData::_substring string, size_t wordBegin, std::string word);

// * ifndef _PLOT_SOURCE_H_
#endif
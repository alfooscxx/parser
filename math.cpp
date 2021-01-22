#include "parser.h"

_parseData::_node* _parseData::_node::derivative() const
{
	_parseData::_nodeConst* result = new _parseData::_nodeConst(1);
	return result;
}

_parseData::_node* _parseData::_nodeConst::derivative() const
{
	_parseData::_nodeConst* result = new _parseData::_nodeConst(0);
	return result;
}

_parseData::_node* _parseData::_nodeOperator::derivative() const
{
	_node* first;
	_node* second;
	_node* third;
	switch (this->operation)
	{
	case _nodeOperation::ADDITION:
		// CXX17 [[fallthrough]];
	case _nodeOperation::SUBTRACTION:
		return binary_operation(this->firstOperand->derivative(), this->secondOperand->derivative(), this->operation);
	case _nodeOperation::MULTIPLICATION:
		first = binary_operation(this->firstOperand->derivative(), this->secondOperand, _nodeOperation::MULTIPLICATION);
		second = binary_operation(this->firstOperand, this->secondOperand->derivative(), _nodeOperation::MULTIPLICATION);
		return binary_operation(first, second, _nodeOperation::ADDITION);
	case _nodeOperation::DIVISION:
		first = binary_operation(this->firstOperand->derivative(), this->secondOperand, _nodeOperation::MULTIPLICATION);
		second = binary_operation(this->firstOperand, this->secondOperand->derivative(), _nodeOperation::MULTIPLICATION);
		third = binary_operation(this->secondOperand, this->secondOperand, _nodeOperation::MULTIPLICATION);
		return binary_operation(binary_operation(first, second, _nodeOperation::SUBTRACTION), third, _nodeOperation::DIVISION);
	case _nodeOperation::POWER:
		first = new _nodeFunction(this->firstOperand, _nodeOperation::LOG);
		second = binary_operation(first, this->secondOperand, _nodeOperation::MULTIPLICATION);
		// TODO: fix extra memory usage
		third = new _nodeFunction(second, _nodeOperation::EXP);
		return third->derivative();
	default:
		throw "bad operation";
	}
}

_parseData::_node* _parseData::_nodeFunction::derivative() const
{
	_node* internalDerivative = this->argument->derivative();
	_node* result;
	_nodeFunction* func1;
	_nodeFunction* func2;
	_nodeConst* koeff;
	switch (this->operation)
	{
	case _nodeOperation::COS:
		koeff = new _nodeConst(-1);
		func1 = new _nodeFunction(this->argument, _nodeOperation::SIN);
		result = binary_operation(koeff, func1, _nodeOperation::MULTIPLICATION);
		break;
	case _nodeOperation::SIN:
		result = new _nodeFunction(this->argument, _nodeOperation::COS);
		break;
	case _nodeOperation::EXP:
		result = new _nodeFunction(this->argument, _nodeOperation::EXP);
		break;
	case _nodeOperation::LOG:
		koeff = new _nodeConst(1);
		result = binary_operation(koeff, this->argument, _nodeOperation::DIVISION);
		break;
	case _nodeOperation::TAN:
		koeff = new _nodeConst(1);
		func1 = new _nodeFunction(this->argument, _nodeOperation::COS);
		func2 = new _nodeFunction(this->argument, _nodeOperation::COS);
		result = binary_operation(koeff, binary_operation(func1, func2, _nodeOperation::MULTIPLICATION), _nodeOperation::DIVISION);
		break;
	case _nodeOperation::CTAN:
		koeff = new _nodeConst(-1);
		func1 = new _nodeFunction(this->argument, _nodeOperation::SIN);
		func2 = new _nodeFunction(this->argument, _nodeOperation::SIN);
		result = binary_operation(koeff, binary_operation(func1, func2, _nodeOperation::MULTIPLICATION), _nodeOperation::DIVISION);
		break;
	default:
		throw "bad operation";
	}
	return binary_operation(result, internalDerivative, _nodeOperation::MULTIPLICATION);
}
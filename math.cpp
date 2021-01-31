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
	case _nodeOperator::_operation::ADDITION:
		// fallthrough
	case _nodeOperator::_operation::SUBTRACTION:
		return binary_operation(this->firstOperand->derivative(), this->secondOperand->derivative(), this->operation);
	case _nodeOperator::_operation::MULTIPLICATION:
		first = binary_operation(this->firstOperand->derivative(), this->secondOperand, _nodeOperator::_operation::MULTIPLICATION);
		second = binary_operation(this->firstOperand, this->secondOperand->derivative(), _nodeOperator::_operation::MULTIPLICATION);
		return binary_operation(first, second, _nodeOperator::_operation::ADDITION);
	case _nodeOperator::_operation::DIVISION:
		first = binary_operation(this->firstOperand->derivative(), this->secondOperand, _nodeOperator::_operation::MULTIPLICATION);
		second = binary_operation(this->firstOperand, this->secondOperand->derivative(), _nodeOperator::_operation::MULTIPLICATION);
		third = binary_operation(this->secondOperand, this->secondOperand, _nodeOperator::_operation::MULTIPLICATION);
		return binary_operation(binary_operation(first, second, _nodeOperator::_operation::SUBTRACTION), third, _nodeOperator::_operation::DIVISION);
	case _nodeOperator::_operation::POWER:
		first = new _nodeFunction(this->firstOperand, _nodeFunction::_function::LOG);
		second = binary_operation(first, this->secondOperand, _nodeOperator::_operation::MULTIPLICATION);
		// TODO: fix extra memory usage
		third = new _nodeFunction(second, _nodeFunction::_function::EXP);
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
	switch (this->function)
	{
	case _nodeFunction::_function::COS:
		koeff = new _nodeConst(-1);
		func1 = new _nodeFunction(this->argument, _nodeFunction::_function::SIN);
		result = binary_operation(koeff, func1, _nodeOperator::_operation::MULTIPLICATION);
		break;
	case _nodeFunction::_function::SIN:
		result = new _nodeFunction(this->argument, _nodeFunction::_function::COS);
		break;
	case _nodeFunction::_function::EXP:
		result = new _nodeFunction(this->argument, _nodeFunction::_function::EXP);
		break;
	case _nodeFunction::_function::LOG:
		koeff = new _nodeConst(1);
		result = binary_operation(koeff, this->argument, _nodeOperator::_operation::DIVISION);
		break;
	case _nodeFunction::_function::TAN:
		koeff = new _nodeConst(1);
		func1 = new _nodeFunction(this->argument, _nodeFunction::_function::COS);
		func2 = new _nodeFunction(this->argument, _nodeFunction::_function::COS);
		result = binary_operation(koeff, binary_operation(func1, func2, _nodeOperator::_operation::MULTIPLICATION), _nodeOperator::_operation::DIVISION);
		break;
	case _nodeFunction::_function::CTAN:
		koeff = new _nodeConst(-1);
		func1 = new _nodeFunction(this->argument, _nodeFunction::_function::SIN);
		func2 = new _nodeFunction(this->argument, _nodeFunction::_function::SIN);
		result = binary_operation(koeff, binary_operation(func1, func2, _nodeOperator::_operation::MULTIPLICATION), _nodeOperator::_operation::DIVISION);
		break;
	default:
		throw "bad operation";
	}
	return binary_operation(result, internalDerivative, _nodeOperator::_operation::MULTIPLICATION);
}
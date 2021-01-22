#include "plot_source.h"

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
	_node* result;
	_nodeOperator* first;
	_nodeOperator* second;
	switch (this->operation)
	{
	case _nodeOperation::ADDITION:
		return binary_operation(this->firstOperand->derivative(), this->secondOperand->derivative(), _nodeOperation::ADDITION);
	case _nodeOperation::SUBTRACTION:
		return binary_operation(this->firstOperand->derivative(), this->secondOperand->derivative(), _nodeOperation::SUBTRACTION);
	case _nodeOperation::MULTIPLICATION:
		first = binary_operation(this->firstOperand, this->secondOperand->derivative(), _nodeOperation::MULTIPLICATION);
		second = binary_operation(this->firstOperand->derivative(), this->secondOperand, _nodeOperation::MULTIPLICATION);
		result = binary_operation(first, second, _nodeOperation::ADDITION);
	default:
		result = new _node;
		break;
	}
	return result;
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
	default:
		result = new _node;
		break;
	}
	return binary_operation(result, internalDerivative, _nodeOperation::MULTIPLICATION);
}
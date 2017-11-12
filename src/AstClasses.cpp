#include <bits/stdc++.h>
#include "AstDefs.h"
using namespace std;
using namespace llvm;
#define TBS print_tabs()
int tabs = 0;
const int width = 4;
std::map<string, int> var_table;
std::map<string, std::vector<int>> array_table;
static LLVMContext Context;
static Module *ModuleOb = new Module("Flat-B Compiler jit", Context);
static IRBuilder<> Builder(Context);
static map<string, AllocaInst *> NamedValues;
Function *fooFunc;

Function *createFunc(IRBuilder <> &Builder, string Name) {
	FunctionType *functype = FunctionType::get(Builder.getInt32Ty(), false);
	Function *foofunc = Function::Create(
		functype, Function::ExternalLinkage, Name, ModuleOb);
	return foofunc;
}

BasicBlock *createBB(Function *foofunc, string Name) {
	return BasicBlock::Create(Context, Name, foofunc);
}

GlobalVariable *createGlob(IRBuilder <> &Builder, string Name) {
	ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
	GlobalVariable *gvar = ModuleOb->getNamedGlobal(Name);
	gvar->setLinkage(GlobalValue::CommonLinkage);
	gvar->setAlignment(4);
	return gvar;
}

void BasicBuildLLVM(Module * TheModule)
{
    fooFunc = createFunc(Builder,"main");
    BasicBlock  *entry = createBB(fooFunc,"entry");
    Builder.SetInsertPoint(entry);
}

void print_tabs() {
	for (int i = 0; i < tabs; i++)
		for (int j = 0; j < width; j++)
			cout<<" ";
}

int evaluate(int lhs, string operand, int rhs) {
	if (operand.compare("+") == 0)
		return (lhs + rhs);
	if (operand.compare("-") == 0)
		return (lhs - rhs);
	if (operand.compare("*") == 0)
		return (lhs * rhs);
	if (operand.compare("/") == 0)
		return (lhs / rhs);
	if (operand.compare("%") == 0)
		return (lhs % rhs);
	if (operand.compare(">") == 0)
		return (lhs > rhs);
	if (operand.compare("<") == 0)
		return (lhs < rhs);
	if (operand.compare("==") == 0)
		return (lhs == rhs);
	if (operand.compare("!=") == 0)
		return (lhs != rhs);
	if (operand.compare("&&") == 0)
		return (lhs && rhs);
	if (operand.compare("||") == 0)
		return (lhs || rhs);
}

Variable::Variable(string identifier) {
	this->type = string("variable");
	this->identifier = identifier; 
}

Variable::Variable(string identifier, int len) {
	this->type = string("array");
	this->identifier = identifier;
	this->length = len;
}

FieldDeclaration::FieldDeclaration(string datatype, class Variables *variables) {
	this->datatype = datatype; 
	this->variables = variables->getVariables();
}

Assignment::Assignment(class Location *loc, class Expression *expr) {
	this->stmt_type = string("assignment");
	this->loc = loc;
	this->expr = expr;
}

Expression::Expression(class Expression *lhs, string operand, class Expression *rhs) {
	this->stmt_type = string("binary_expression");
	this->lhs = lhs;
	this->rhs = rhs;
	this->operand = operand;
}

Expression::Expression(class Location *loc) {
	this->stmt_type = string("unary_expression");
	this->loc = loc;
}

Location::Location(int value) {
	this->type = string("number");
	this->value = value;
}

Location::Location(string identifier) {
	this->type = string("variable");
	this->identifier = identifier;
}

Location::Location(string identifier, class Expression *index) {
	this->type = string("array");
	this->identifier = identifier;
	this->index = index;
}

Block::Block(class Statements *statements) {
	this->block_statements = statements->getStatements();
}

WhileStatement::WhileStatement(class Expression *cond, class Block *blk) {
	this->stmt_type = string("while");
	this->condition = cond;
	this->while_block = blk;
}

ForStatement::ForStatement(string id, class Location *start, class Location *end, class Block *blk) {
	this->stmt_type = string("for");
	this->type = string("no step");
	this->loop_variable = id;
	this->v_start = start;
	this->v_end = end;
	this->for_block = blk;
}

ForStatement::ForStatement(string id, class Location *start, class Location *end, class Location *step, class Block *blk) {
	this->stmt_type = string("for");
	this->type = string("step");
	this->loop_variable = id;
	this->v_start = start;
	this->v_step = step;
	this->v_end = end;
	this->for_block = blk;
}

IfElseStatement::IfElseStatement(class Expression *expr, class Block *blk) {
	this->stmt_type = string("if");
	this->type = string("if");
	this->condition = expr;
	this->if_block = blk;
}

IfElseStatement::IfElseStatement(class Expression *expr, class Block *iblk, class Block *eblk) {
	this->stmt_type = string("ifelse");
	this->type = string("ifelse");
	this->condition = expr;
	this->if_block = iblk;
	this->else_block = eblk; 
}

GotoStatement::GotoStatement(string id) {
	this->stmt_type = string("goto");
	this->type = string("goto");
	this->identifier = id;
}

GotoStatement::GotoStatement(string id, class Expression *cond) {
	this->stmt_type = string("gotoif");
	this->type = string("gotoif");
	this->identifier = id;
	this->condition = cond;
}

Print::Print(class Location *value) {
	this->stmt_type = string("print");
	this->type = string("loc");
	this->value = value;
}

Print::Print(string text) {
	this->stmt_type = string("print");
	this->type = string("str");
	this->text = text;
}

Print::Print(string text, class Location *value) {
	this->stmt_type = string("print");
	this->type = string("locstr");
	this->text = text;
	this->value = value;
}

ReadLine::ReadLine(class Location *value) {
	this->stmt_type = string("read");
	this->value = value;
}

Main::Main(class FieldDeclarations *decls, class Statements *stmts) {
	this->field_declarations = decls;
	this->statements = stmts;
}

int Variable::getLength() {
	return this->length;
}

string Variable::getType() {
	return this->type;
}

string Variable::getIdentifier() {
	return this->identifier;
}

void Variables::store(class Variable *variable) {
	this->variables.push_back(variable);
}

std::vector<class Variable*> Variables::getVariables() {
	return this->variables;
}

string FieldDeclaration::getDataType() {
	return this->datatype;
}

std::vector<class Variable *> FieldDeclaration::getVariables() {
	return this->variables;
}

void FieldDeclarations::store(class FieldDeclaration *declaration) {
	return this->field_declarations.push_back(declaration);
}

std::vector<class FieldDeclaration *> FieldDeclarations::getFieldDecls() {
	return this->field_declarations;
}	

string Statement::getType() {
	return this->stmt_type;
}

void Statement::setType(string type) {
	this->stmt_type = type;
}

string Statement::getLabel() {
	return this->label;
}

void Statement::setLabel(string label) {
	this->label = label;
}

std::vector<class Statement *> Block::getBlkStatements() {
	return this->block_statements;
}

void Statements::store(class Statement *statement) {
	this->statements.push_back(statement);
}

std::vector<class Statement *> Statements::getStatements() {
	return this->statements;
}

string Location::getType() {
	return this->type; 
}

string Location::getIdentifier() {
	return this->identifier;
}

class Expression *Location::getIndex() {
	return this->index;
}

int Location::getValue() {
	return this->value;
}

string Expression::getType() {
	return this->stmt_type;
}

void Main::interpret() {
	TBS;
	cout<<"<Main>"<<endl;
		tabs++;
		field_declarations->interpret();
		statements->interpret();
		tabs--;
	TBS;
	cout<<"</Main>"<<endl;
}

void Main::traverse() {
	TBS;
	cout<<"<Main>"<<endl;
		tabs++;
		field_declarations->traverse();
		statements->traverse();
		tabs--;
	TBS;
	cout<<"</Main>"<<endl;
}

void FieldDeclarations::interpret() {
	TBS;
	cout<<"<Declarations>"<<endl;
		tabs++;
		for (int i = 0; i < field_declarations.size(); ++i)
			field_declarations[i]->interpret();
		tabs--;
	TBS;
	cout<<"</Declarations>"<<endl;
}

void FieldDeclarations::traverse() {
	TBS;
	cout<<"<Declarations>"<<endl;
		tabs++;
		for (int i = 0; i < field_declarations.size(); ++i)
			field_declarations[i]->traverse();
		tabs--;
	TBS;
	cout<<"</Declarations>"<<endl;
}

void FieldDeclaration::interpret() {
	TBS;
	cout<<"<Declaration>"<<endl;
		tabs++;
		for (int i = 0; i < variables.size(); i++)
			variables[i]->interpret();
		tabs--;
	TBS;
	cout<<"</Declaration>"<<endl;
}

void FieldDeclaration::traverse() {
	TBS;
	cout<<"<Declaration>"<<endl;
		tabs++;
		TBS;
		cout<<"DataType: "<<endl;
		for (int i = 0; i < variables.size(); i++)
			variables[i]->traverse();
		tabs--;
	TBS;
	cout<<"</Declaration>"<<endl;
}

void Variable::interpret() {
	TBS;
	if (type.compare("array") == 0) {
		cout<<"<Array>"<<endl;
		array_table[identifier] = std::vector<int> (length, 0);
	}
	else {
		cout<<"<Variable>"<<endl;
		var_table.insert(pair <string, int> (identifier, 0));
	}
}

void Variable::traverse() {
	TBS;
	cout<<"<Variable ";
	cout<<"declaration: "<<identifier;
	if (type.compare("Array") == 0)
		cout<<", length: "<<length;
	cout<<" />"<<endl;
}

void Location::traverse() {
	TBS;
	cout<<"<Location ";
	if (type.compare("number") == 0) {
		TBS;
		cout<<"number: "<<value;
	}
	else if (type.compare("variable") == 0) {
		TBS;
		cout<<"variable: "<<identifier;
	}
	else if (type.compare("array") == 0)  {
		TBS;
		cout<<"identifier: "<<identifier;
		TBS;
		cout<<"index: ";
		tabs++;
		index->traverse();
		tabs--;
	}
	TBS;
	cout<<" />"<<endl;
}

int Location::interpret() {
	TBS;
	cout<<"<Location/>"<<endl;
	if (type.compare("number") == 0) {
		return value;
	}
	else if (type.compare("variable") == 0) {
		return var_table[identifier];
	}
	else if (type.compare("array") == 0) {
		return array_table[identifier][index->interpret()];
	}
}

void Block::traverse() {
	TBS;
	cout<<"<Block>"<<endl;
		tabs++;
		for (int i = 0; i < block_statements.size(); i++)
			block_statements[i]->traverse();
		tabs--;
	TBS;
	cout<<"</Block>"<<endl;
}

void Block::interpret() {
	TBS;
	cout<<"<Block>"<<endl;
		tabs++;
		for (int i = 0; i < block_statements.size(); i++)
			block_statements[i]->interpret();
		tabs--;
	TBS;
	cout<<"</Block>"<<endl;
}

void Statements::traverse() {
	TBS;
	cout<<"<Statements>"<<endl;
		tabs++;
		for (int i = 0; i < statements.size(); i++) {
			TBS;
			cout<<"<Label: "<<statements[i]->getLabel()<<">"<<endl;
			statements[i]->traverse();
		}
		tabs--;
	TBS;
	cout<<"</Statements>"<<endl;
}

void Statements::interpret() {
	TBS;
	cout<<"<Statements>"<<endl;
		tabs++;
		for (int i = 0; i < statements.size(); i++)
			statements[i]->interpret();
		tabs--;
	TBS;
	cout<<"</Statements>"<<endl;
}

void Expression::traverse() {
	if (stmt_type.compare("unary_expression") == 0) {
		TBS;
		cout<<"<Value>"<<endl;
		tabs++;
		loc->traverse();
		tabs--;
		TBS;
		cout<<"</Value>"<<endl;
	}
	else { 
		TBS;
		cout<<"<Binary Expression Operator: "<<operand<<" >"<<endl;
		tabs++;
		lhs->traverse();
		rhs->traverse();
		tabs--;
		TBS;
		cout<<"</Binary Expression>"<<endl;
	}
}

int Expression::interpret() {
	int value;
	if (stmt_type.compare("unary_expression") == 0) {
		TBS;
		cout<<"<Value>"<<endl;
		tabs++;
		value = loc->interpret();
		tabs--;
		TBS;
		cout<<"</Value>"<<endl;
	}
	else { 
		TBS;
		cout<<"<Binary Expression Operator: "<<operand<<" >"<<endl;
		tabs++;
		value = evaluate(lhs->interpret(), operand, rhs->interpret());
		tabs--;
		TBS;
		cout<<"</Binary Expression>"<<endl;
	}
	return value;
}

void Assignment::traverse() {
	TBS;
	cout<<"<Assignment Expression>"<<endl;
		tabs++;
		loc->traverse();
		expr->traverse();
		tabs--;
	TBS;
	cout<<"</Assignment Expression>"<<endl;
}

int Assignment::interpret() {
	TBS;
	cout<<"<Assignment Expression>"<<endl;
		tabs++;	
		if (loc->getType().compare("variable") == 0) {
			var_table[loc->getIdentifier()] = expr->interpret();
		}
		if (loc->getType().compare("array") == 0) { 
			array_table[string(loc->getIdentifier())][loc->getIndex()->interpret()] = 5;
		}
		tabs--;
	TBS;
	cout<<"</Assignment Expression>"<<endl;
	return 0;
}

void WhileStatement::traverse() {
	TBS;
	cout<<"<While Block>"<<endl;
		tabs++;
		condition->traverse();
		while_block->traverse();
		tabs--;
	TBS;
	cout<<"</While Block>"<<endl;
}

int WhileStatement::interpret() {
	TBS;
	cout<<"<While Block>"<<endl;
		tabs++;
		while (condition->interpret())
			while_block->interpret();
		tabs--;
	TBS;
	cout<<"</While Block>"<<endl;
	return 0;
}

void ForStatement::traverse() {
	TBS;
	cout<<"<For Block>"<<endl;
	TBS;cout<<"<loop variable: "<<loop_variable<<">"<<endl;
	TBS;cout<<"<start: ";
	v_start->traverse();
	TBS;cout<<"/>"<<endl;
	TBS;cout<<"<end: ";
	v_end->traverse();
	TBS;cout<<"/>"<<endl;
	if (type.compare("step") == 0) {
		TBS;
		cout<<"<step: ";
		v_step->traverse();
		cout<<"/>"<<endl;
	}
		tabs++;
			for_block->traverse();
		tabs--;
	TBS;
	cout<<"</For Block>"<<endl;
}

int ForStatement::interpret() {
	TBS;
	cout<<"<For Block>"<<endl;
	tabs++;
	if (type.compare("step") == 0) {
		var_table.insert(pair <string, int> (loop_variable, 0));
		for (int i = v_start->interpret(); i < v_end->interpret(); i+v_step->interpret()) {
			var_table[loop_variable] = i;
			for_block->interpret();			
		}
	}
	else {
		var_table.insert(pair <string, int> (loop_variable, 0));
		for (int i = v_start->interpret(); i < v_end->interpret(); i++) {
			var_table[loop_variable] = i;
			for_block->interpret();	
		}
	}
	tabs--;
	TBS;
	cout<<"</For Block>"<<endl;
	return 0;
}

void IfElseStatement::traverse() {
	TBS;
	cout<<"<If Block>"<<endl;
		tabs++;
		if_block->traverse();
		tabs--;
	TBS;
	cout<<"</If Block>"<<endl;
	if (type.compare("ifelse") == 0) {
		TBS;
		cout<<"<Else Block>"<<endl;
			tabs++;
			else_block->traverse();
			tabs--;
		TBS;
		cout<<"</Else Block>"<<endl;
	}
}

int IfElseStatement::interpret() {
	TBS;
	cout<<"<If Block>"<<endl;
	tabs++;	
	if (type.compare("ifelse") == 0) {
		if (condition->interpret())
			if_block->interpret();
		else
			else_block->interpret();
	}
	else {
		if (condition->interpret())
			if_block->interpret();
	}
	tabs--;
	TBS;
	cout<<"<If Block>"<<endl;
	return 0;
}

void GotoStatement::traverse() {
	TBS;
	cout<<"<Goto label: "<<identifier<<endl;
	if (type.compare("gotoif") == 0) {
		tabs++;
		condition->traverse();
		tabs--;
	}
	TBS;
	cout<<" />"<<endl;
}

void Print::traverse() {
	TBS;
	cout<<"<Print ";
	if (type.compare("str") == 0) {
		cout<<"string: "<<text;
	}
	else if (type.compare("loc") == 0) {
		tabs ++;
		value->traverse();
		tabs--;
	}
	else {
		cout<<"string: "<<text;
		tabs++;
		value->traverse();
		tabs--;
	}
	TBS;
	cout<<" />";
}

int Print::interpret() {
	TBS;
	cout<<"<print>"<<endl;
	if (type.compare("loc") == 0)
		cout<<value->interpret();
	else if (type.compare("locstr") == 0)
		cout<<text<<value->interpret();
	else
		cout<<text;
	TBS;
	cout<<"</print>"<<endl;
	return 0;
}

void ReadLine::traverse() {
	TBS;
	cout<<"<Read ";
		tabs++;
		value->traverse();
		tabs--;
	cout<<" />";
}

Value *Variable::codegen() {
	Value *V = ConstantInt::get(Context, APInt(32, 0));
	return V;
}

Value *FieldDeclaration::codegen() {
	for (int i = 0; i < variables.size(); i++) {
		class Variable *variable = variables[i];
		if (variable->getType().compare("array") == 0) {
			llvm::Type *type;
			type = Type::getInt32Ty(Context);
			ArrayType* arrType = ArrayType::get(type, variable->getLength());
			PointerType* PointerTy_1 = PointerType::get(ArrayType::get(type, variable->getLength()), 0);
			GlobalVariable* gv = new GlobalVariable(*ModuleOb, arrType, false, GlobalValue::ExternalLinkage, 0, variable->getIdentifier());
			gv->setInitializer(ConstantAggregateZero::get(arrType));
		}
		else {
			ModuleOb->getOrInsertGlobal(variable->getIdentifier(), Builder.getInt32Ty());
			GlobalVariable *gvar = ModuleOb->getNamedGlobal(variable->getIdentifier());
			gvar->setLinkage(GlobalValue::CommonLinkage);
			gvar->setAlignment(4);
			ConstantInt* const_int_val = ConstantInt::get(Context, APInt(32,0));
			gvar->setInitializer(const_int_val);
		}
	}
	Value *v = ConstantInt::get(Context, APInt(32, 1));
	return v;
}

Value *FieldDeclarations::codegen() {
	for (int i = 0; i < field_declarations.size(); i++)
		field_declarations[i]->codegen();
	Value *v = ConstantInt::get(Context, APInt(32, 1));
	return v;
}

Value *Expression::codegen() {
	if (this->getType().compare("unary_expression") == 0) {
		Value *v = loc->codegen();
		if(loc->getType().compare("number") != 0)	
			Value *val = Builder.CreateLoad(v);
		return v;
	}
	if (this->getType().compare("binary_expression") == 0) {
		Value *left = lhs->codegen();
		Value *right = rhs->codegen();
		if (left == 0) 
			return reportError::ErrorV("Error in left operand of " + operand);
		if (right == 0)
			return reportError::ErrorV("Error in right operand of " + operand);
		Value *v;
		if (operand.compare("+") == 0)
			v = Builder.CreateAdd(left, right, "addtmp");
		else if (operand.compare("-") == 0)
			v = Builder.CreateSub(left, right, "subtmp");
		else if (operand.compare("*") == 0)
			v = Builder.CreateMul(left, right, "multmp");
		else if (operand.compare("/") == 0)
			v = Builder.CreateUDiv(left, right, "divtmp");
		else if (operand.compare("%") == 0)
			v = Builder.CreateURem(left, right, "modtmp");
		else if (operand.compare("<") == 0)
			v = Builder.CreateICmpULT(left, right, "ltcomparetmp");
		else if (operand.compare(">") == 0)
			v = Builder.CreateICmpUGT(left, right, "gtcomparetmp");
		else if (operand.compare("==") == 0)
			v = Builder.CreateICmpEQ(left, right, "equalcomparetmp");
		else if (operand.compare("!=") == 0)
			v = Builder.CreateICmpNE(left, right, "notequalcomparetmp");
		return v;
	}
}

Value *Statements::codegen() {
	Value *v = ConstantInt::get(Context, APInt(32, 1));
	for (int i = 0; i < statements.size(); i++)
		v = statements[i]->codegen();
	return v;
}

Value *Main::codegen() {
	Value *v = ConstantInt::get(Context, APInt(32, 0));
	BasicBuildLLVM(ModuleOb);
	field_declarations->codegen();
	statements->codegen();
	cout<<"------- Code Generation --------"<<endl;
	Builder.CreateRet(Builder.getInt32(0));
	ModuleOb->dump();
	return v;
}

Value *Block::codegen() {
	Value *v = ConstantInt::get(Context, APInt(32, 0));
	for (int i = 0; i < block_statements.size(); i++)
		block_statements[i]->codegen();
	return v;
}

Value *Location::codegen() {
	if (this->getType().compare("number") == 0) {
		Value *v = ConstantInt::get(Context, APInt(32, value));
		return v;
	}
	else {
		Value* v = ModuleOb->getNamedGlobal(identifier);
		if (v == 0)
			return reportError::ErrorV("Unknown Variable name "+identifier);
		if (type.compare("array") != 0) {
			return v;
		}
		else {
			Value *index = this->index->codegen();
			cout<<"here"<<endl;
			if (this->index->getType().compare("unary_expression") == 0)
				index = Builder.CreateLoad(index);
			if (index == 0) 
				return reportError::ErrorV("Invalid Index");
			vector <Value *> array_index;
			array_index.push_back(Builder.getInt32(0));
			array_index.push_back(index);
			v = Builder.CreateGEP(v, array_index, identifier+"_Index");
			return v;
		}
	}
}

Value *Assignment::codegen() {
	Value *cur = ModuleOb->getGlobalVariable(loc->getIdentifier());
	if (cur == 0)
	 	return reportError::ErrorV("Unknown Variable Name "+loc->getIdentifier());
	Value *rhs = expr->codegen();
	if (rhs == 0)
	 	return reportError::ErrorV("Error in right hand side of Assignment");
	Value *v = Builder.CreateStore(rhs, lhs);
	return v;
}

Value *IfElseStatement::codegen() {
	Value *cond = condition->codegen();
	if (cond == 0)
		return reportError::ErrorV("Invalid Expression in the IF");
	Function* TheFunction = Builder.GetInsertBlock()->getParent();
	BasicBlock *ifBlock = BasicBlock::Create(Context, "if", TheFunction);
	BasicBlock *elseBlock = BasicBlock::Create(Context, "else");
	BasicBlock *nextBlock = BasicBlock::Create(Context, "ifcont");
	Builder.CreateCondBr(cond, ifBlock, elseBlock);
	Builder.SetInsertPoint(ifBlock);
	Value* ifval  = if_block->codegen();
	if (ifval == 0)
		return 0;

	Builder.CreateBr(nextBlock);
	ifBlock = Builder.GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(elseBlock);
	Builder.SetInsertPoint(elseBlock);
	Value* elseval;
	if (type.compare("ifelse") == 0) {
		elseval = else_block->codegen();
		if (elseval == 0)
			return 0;
	}
	Builder.CreateBr(nextBlock);
	elseBlock = Builder.GetInsertBlock();
	TheFunction->getBasicBlockList().push_back(nextBlock);
	Builder.SetInsertPoint(nextBlock);

	PHINode *phi = Builder.CreatePHI(Type::getInt32Ty(Context), 2, "iftmp");
	phi->addIncoming(ifval, ifBlock);
	phi->addIncoming(elseval, elseBlock);
	Value *V = ConstantInt::get(Context, APInt(32,0));
	return V;
}

// Value *ForStatement::codegen() {

// }

// Value *WhileStatement::codegen() {

// }

// Value *ReadLine::codegen() {

// }

// Value *Print::codegen() {

// }

#include "ast.h"

using namespace std;
using namespace llvm;

Value* AssignmentExpressionNode::Codegen(CodegenContext* ctx) {
  auto lhs = left->Codegen(ctx);
  // TODO(cjihrig): Right now only unary expressions are supported.
  return lhs;
};

Value* BlockStatementNode::Codegen(CodegenContext* ctx) {
  return nullptr;
};

Value* BooleanLiteralNode::Codegen(CodegenContext* ctx) {
  return ConstantInt::getBool(*ctx->llvm_context, value);
};

Value* EmptyStatementNode::Codegen(CodegenContext* ctx) {
  return nullptr;
};

Value* FunctionDeclarationNode::Codegen(CodegenContext* ctx) {
  // TODO(cjihrig): Function params and return type are not correct.
  auto return_type = Type::getInt32Ty(*ctx->llvm_context);
  vector<Type*> params(0, Type::getVoidTy(*ctx->llvm_context));
  auto function_type = FunctionType::get(return_type, params, false);
  auto the_function = Function::Create(
    function_type,
    Function::ExternalLinkage,
    id->name,
    ctx->module.get()
  );
  auto bb = BasicBlock::Create(*ctx->llvm_context, "entrypoint", the_function);

  ctx->builder->SetInsertPoint(bb);

  for (int i = 0; i < body->size(); ++i) {
    body->at(i)->Codegen(ctx);
  }

  return the_function;
};

Value* IdentifierNode::Codegen(CodegenContext* ctx) {
  // TODO(cjihrig): Check that the symbol actually exists.
  return ctx->symbol_table[name];
};

Value* IfStatementNode::Codegen(CodegenContext* ctx) {
  // Create the test expression.
  auto lhs = ConstantInt::get(Type::getInt32Ty(*ctx->llvm_context), 1, false);
  auto rhs = ConstantInt::get(Type::getInt32Ty(*ctx->llvm_context), 0, false);
  auto test = ctx->builder->CreateICmpNE(lhs, rhs, "test");

  // Create the basic blocks for the consequent, alternate, and merge as
  // necessary.
  auto fn = ctx->builder->GetInsertBlock()->getParent();
  auto consequent_bb = BasicBlock::Create(*ctx->llvm_context, "consequent", fn);
  auto merge_bb = BasicBlock::Create(*ctx->llvm_context, "merge", fn);

  // Create the branch based on the test.
  ctx->builder->CreateCondBr(test, consequent_bb, merge_bb);

  // Create the consequent.
  ctx->builder->SetInsertPoint(consequent_bb);
  // TODO(cjihrig): Codegen the consequent here.
  ctx->builder->CreateBr(merge_bb);

  // Create the merge block.
  // TODO(cjihrig): The Phi node is not correct.
  ctx->builder->SetInsertPoint(merge_bb);
  auto phi = ctx->builder->CreatePHI(
    Type::getInt32Ty(*ctx->llvm_context), 1, "result"
  );
  phi->addIncoming(rhs, consequent_bb);
  return phi;
};

Value* ProgramNode::Codegen(CodegenContext* ctx) {
  for (int i = 0; i < body->size(); ++i) {
    body->at(i)->Codegen(ctx);
  }

  // TODO(cjihrig): The return value should come from user code.
  printf("********************************\n");
  printf("GENERATED CODE:\n");
  ctx->module->print(llvm::errs(), nullptr);
  verifyModule(*ctx->module, &llvm::errs());
  printf("********************************\n");

  return nullptr;
};

Value* ReturnStatementNode::Codegen(CodegenContext* ctx) {
  // TODO(cjihrig): Figure out void return value.
  // ctx->builder->CreateRetVoid();
  auto return_value = ConstantInt::get(
    Type::getInt32Ty(*ctx->llvm_context), 43, false
  );
  return ctx->builder->CreateRet(return_value);
};

Value* StringLiteralNode::Codegen(CodegenContext* ctx) {
  return nullptr;
};

Value* VariableDeclaratorNode::Codegen(CodegenContext* ctx) {
  auto identifier = ctx->builder->CreateAlloca(
    Type::getDoubleTy(*ctx->llvm_context), nullptr, id->name
  );
  auto rhs = init->Codegen(ctx);

  ctx->builder->CreateStore(rhs, identifier);
  ctx->symbol_table[id->name] = identifier;

  return nullptr;
};

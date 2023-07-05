#ifndef __SHINOBI_AST_NODES_H__
#define __SHINOBI_AST_NODES_H__

#include <map>
#include <vector>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

class CodegenContext {
  public:
    CodegenContext(std::string filename) {
      llvm_context = std::make_unique<llvm::LLVMContext>();
      // TODO(cjihrig): Make module name the file name?
      module = std::make_unique<llvm::Module>(filename, *llvm_context);
      builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);
      symbol_table.clear();
    };

    std::unique_ptr<llvm::LLVMContext> llvm_context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    // TODO(cjihrig): Refactor symbol table into something better.
    std::map<std::string, llvm::Value*> symbol_table;
};

class IdentifierNode;

class ASTNode {
  public:
    ASTNode(int line, int column) : _line(line), _col(column) {}
    virtual ~ASTNode() = default;
    virtual llvm::Value* Codegen(CodegenContext* ctx) = 0;

  // protected:
    int _line;
    int _col;
};

class AssignmentExpressionNode : public ASTNode {
  public:
    AssignmentExpressionNode(
      int op,
      std::unique_ptr<ASTNode> left,
      std::unique_ptr<ASTNode> right,
      int line,
      int column
    ) :
      ASTNode(line, column),
      op(op),
      left(std::move(left)),
      right(std::move(right)) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    int op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

class BlockStatementNode : public ASTNode {
  public:
    BlockStatementNode(int line, int column) : ASTNode(line, column) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;
};

class BooleanLiteralNode : public ASTNode {
  public:
    BooleanLiteralNode(
      bool value,
      int line,
      int column
    ) : ASTNode(line, column), value(value) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    bool value;
};

class EmptyStatementNode : public ASTNode {
  public:
    EmptyStatementNode(int line, int column) : ASTNode(line, column) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;
};

class FunctionDeclarationNode : public ASTNode {
  public:
    FunctionDeclarationNode(
      std::unique_ptr<IdentifierNode> id,
      std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> body,
      int line,
      int column
    ) : ASTNode(line, column), id(std::move(id)), body(std::move(body)) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> body;
};

class IdentifierNode : public ASTNode {
  public:
    IdentifierNode(
      std::string name,
      int line,
      int column
    ) : ASTNode(line, column), name(name) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::string name;
};

class IfStatementNode : public ASTNode {
  public:
    IfStatementNode(
      std::unique_ptr<ASTNode> test,
      std::unique_ptr<ASTNode> consequent,
      std::unique_ptr<ASTNode> alternate,
      int line,
      int column
    ) :
      ASTNode(line, column),
      test(std::move(test)),
      consequent(std::move(consequent)),
      alternate(std::move(alternate)) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::unique_ptr<ASTNode> test;
    std::unique_ptr<ASTNode> consequent;
    std::unique_ptr<ASTNode> alternate;
};

class ProgramNode : public ASTNode {
  public:
    ProgramNode(
      std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> body,
      int line,
      int column
    ) : ASTNode(line, column), body(std::move(body)) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>> body;
};

class ReturnStatementNode : public ASTNode {
  public:
    ReturnStatementNode(int line, int column) : ASTNode(line, column) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;
};

class StringLiteralNode : public ASTNode {
  public:
    StringLiteralNode(
      std::string value,
      int line,
      int column
    ) : ASTNode(line, column), value(value) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::string value;
};

class VariableDeclaratorNode : public ASTNode {
  public:
    VariableDeclaratorNode(
      std::unique_ptr<IdentifierNode> id,
      std::unique_ptr<ASTNode> init,
      int line,
      int column
    ) : ASTNode(line, column), id(std::move(id)), init(std::move(init)) {};
    llvm::Value* Codegen(CodegenContext* ctx) override;

    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<ASTNode> init;
};

#endif /* __SHINOBI_AST_NODES_H__ */

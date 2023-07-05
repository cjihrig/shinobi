#include <stdio.h>
#include <filesystem>

#include "ast.h"
#include "lexer.h"
#include "parser.h"

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

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h" // This is in a new location in new versions.
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace std;
using namespace llvm;

bool WriteObjectFile(CodegenContext* ctx);

int main(void) {
  filesystem::path input_path = "test/fixtures/function.x";
  error_code err;
  string absolute_path = filesystem::absolute(input_path, err);

  if (err) {
    llvm::errs() << "Could not get absolute path: " << err.message();
    return 1;
  }

  auto lexer = std::make_unique<Lexer>(absolute_path);

  if (lexer->Open() != 0) {
    return 1;
  }

  // auto lexer = std::make_unique<Lexer>("assignment.x");
  // auto lexer = std::make_unique<Lexer>("const.x");
  // auto lexer = std::make_unique<Lexer>("empty.x");
  // auto lexer = std::make_unique<Lexer>("test/fixtures/function.x");
  // auto lexer = std::make_unique<Lexer>("if.x");
  // auto lexer = std::make_unique<Lexer>("line-comment.x");
  // Lexer* lexer = new Lexer("string.x");
  // Lexer* lexer = new Lexer("if-with-statement.x");
  // Lexer* lexer = new Lexer("foo.x");
  // Lexer* lexer = new Lexer("two-semicolons.x");
  // Lexer* lexer = new Lexer("semicolon.x");
  // Parser* parser = new Parser(lexer);

  auto parser = std::make_unique<Parser>(std::move(lexer));
  std::unique_ptr<ProgramNode> ast = parser->Parse();

  CodegenContext* ctx = new CodegenContext("function.x");

  ast->Codegen(ctx);

  WriteObjectFile(ctx);

  // printf("************************************\n");

  // std::unique_ptr<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
  // std::unique_ptr<llvm::Module> TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);
  // std::unique_ptr<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

  // std::vector<llvm::Type*> void_formal(0, llvm::Type::getVoidTy(*TheContext));
  // llvm::FunctionType* void_function = llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), void_formal, false);
  // llvm::Function* main_function = llvm::Function::Create(void_function, llvm::Function::ExternalLinkage, "main", TheModule.get());
  // auto bb = llvm::BasicBlock::Create(*TheContext, "entrypoint", main_function);

  // Builder->SetInsertPoint(bb);

  // // auto f = TheModule->getFunction("main");
  // // f->print(llvm::errs());
  // // main_function->print(llvm::errs());

  // auto variable = Builder->CreateAlloca(llvm::Type::getDoubleTy(*TheContext), nullptr, "foobar");
  // auto literal = llvm::ConstantFP::get(*TheContext, llvm::APFloat(3.14159));
  // auto store = Builder->CreateStore(literal, variable);

  // main_function->print(llvm::errs());
  // printf("************************************\n");

  // printf("ROOT IS %d %d\n", ast->_line, ast->_col);

  // ifstream input_file;

  // input_file.open("foo.x");

  // if (!input_file.is_open()) {
  //   cout << "Could not open file!" << endl;
  //   return -1;
  // }

  // cout << "Opened file!" << endl;
  // input_file.close();

  return 0;
}

bool WriteObjectFile(CodegenContext* ctx) {
  // Initialize the target registry etc.
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  auto triple = sys::getDefaultTargetTriple();
  ctx->module->setTargetTriple(triple);
  std::string error;
  auto target = TargetRegistry::lookupTarget(triple, error);

  if (!target) {
    errs() << error;
    return false;
  }

  auto cpu = "generic";
  auto features = "";

  TargetOptions target_options;
  auto relocation_model = std::optional<Reloc::Model>();
  auto target_machine = target->createTargetMachine(
    triple, cpu, features, target_options, relocation_model
  );

  ctx->module->setDataLayout(target_machine->createDataLayout());

  auto filename = "output.o";
  std::error_code err;
  raw_fd_ostream dest(filename, err, sys::fs::OF_None);

  if (err) {
    errs() << "Could not open file: " << err.message();
    return false;
  }

  legacy::PassManager pass;

  if (target_machine->addPassesToEmitFile(
    pass, dest, nullptr, CGFT_ObjectFile
  )) {
    errs() << "The target machine can't emit a file of this type.";
    return false;
  }

  pass.run(*ctx->module);
  dest.flush();

  outs() << "Wrote " << filename << "\n";
  return true;
}

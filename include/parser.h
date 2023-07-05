#ifndef __SHINOBI_PARSER_H__
#define __SHINOBI_PARSER_H__

#include <vector>

#include "ast.h"
#include "lexer.h"

class Parser {
  public:
    Parser(std::unique_ptr<Lexer> lexer);
    std::unique_ptr<ProgramNode> Parse();

  private:
    bool Expect(int token, int expected);
    std::unique_ptr<AssignmentExpressionNode> ParseAssignmentExpression();
    std::unique_ptr<BlockStatementNode> ParseBlockStatement();
    std::unique_ptr<VariableDeclaratorNode> ParseConstStatement();
    std::unique_ptr<EmptyStatementNode> ParseEmptyStatement();
    std::unique_ptr<ASTNode> ParseExpression();
    std::unique_ptr<FunctionDeclarationNode> ParseFunctionDeclaration();
    std::unique_ptr<IdentifierNode> ParseIdentifier();
    std::unique_ptr<IfStatementNode> ParseIfStatement();
    std::unique_ptr<VariableDeclaratorNode> ParseLetStatement();
    std::unique_ptr<ASTNode> ParsePrimaryExpression();
    std::unique_ptr<ReturnStatementNode> ParseReturnStatement();
    std::unique_ptr<ASTNode> ParseSourceElement();
    std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>>
        ParseSourceElementList();
    std::unique_ptr<ASTNode> ParseStatement();
    std::unique_ptr<std::vector<std::unique_ptr<ASTNode>>>
        ParseStatementList();
    std::unique_ptr<Lexer> _lexer;
};

#endif /* __SHINOBI_PARSER_H__ */

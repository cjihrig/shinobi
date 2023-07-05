#include "parser.h"

using namespace std;

#define EXPECT_OR_RETURN(actual, expected)                                    \
  do {                                                                        \
    if (Expect((actual), (expected)) == false) {                              \
      return nullptr;                                                         \
    }                                                                         \
  } while (0)

Parser::Parser(unique_ptr<Lexer> lexer) {
  _lexer = std::move(lexer);
}

unique_ptr<ProgramNode> Parser::Parse() {
  _lexer->NextToken();
  auto nodes = ParseSourceElementList();
  EXPECT_OR_RETURN(_lexer->Token(), TokenEOF);
  return make_unique<ProgramNode>(std::move(nodes), 0, 0);
}

bool Parser::Expect(int token, int expected) {
  if (token != expected) {
    printf("SYNTAX ERROR: expected %c (%d) but got %c (%d)\n", expected, expected, token, token);
    return false;
  }

  return true;
}

unique_ptr<AssignmentExpressionNode> Parser::ParseAssignmentExpression() {
  // TODO(cjihrig): ParsePrimaryExpression() is not the correct production type.
  int line = _lexer->Line();
  int column = _lexer->Column();
  auto left = ParsePrimaryExpression();
  auto right = nullptr; // TODO(cjihrig): Fix this.
  auto node = make_unique<AssignmentExpressionNode>(
    '=', std::move(left), std::move(right), line, column
  );
  return node;
}

unique_ptr<BlockStatementNode> Parser::ParseBlockStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), '{');
  auto node = make_unique<BlockStatementNode>(
    _lexer->Line(), _lexer->Column()
  );
  _lexer->NextToken();
  ParseStatementList();
  EXPECT_OR_RETURN(_lexer->Token(), '}');
  _lexer->NextToken();
  return node;
}

unique_ptr<VariableDeclaratorNode> Parser::ParseConstStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenConst);
  int line = _lexer->Line();
  int column = _lexer->Column();
  _lexer->NextToken();
  auto id = ParseIdentifier();
  EXPECT_OR_RETURN(_lexer->Token(), '=');
  _lexer->NextToken();
  auto init = ParseAssignmentExpression();
  EXPECT_OR_RETURN(_lexer->Token(), ';');
  _lexer->NextToken();
  auto node = make_unique<VariableDeclaratorNode>(
    std::move(id), std::move(init), line, column
  );
  return node;
}

unique_ptr<EmptyStatementNode> Parser::ParseEmptyStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), ';');
  auto node = make_unique<EmptyStatementNode>(
    _lexer->Line(), _lexer->Column()
  );
  _lexer->NextToken();
  return node;
}

unique_ptr<ASTNode> Parser::ParseExpression() {
  auto r = ParsePrimaryExpression();
  return r;
}

unique_ptr<FunctionDeclarationNode> Parser::ParseFunctionDeclaration() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenFunction);
  int line = _lexer->Line();
  int column = _lexer->Column();
  _lexer->NextToken();
  auto id = ParseIdentifier();
  EXPECT_OR_RETURN(_lexer->Token(), '(');
  // TODO(cjihrig): Function arguments.
  _lexer->NextToken();
  EXPECT_OR_RETURN(_lexer->Token(), ')');
  _lexer->NextToken();
  EXPECT_OR_RETURN(_lexer->Token(), ':');
  _lexer->NextToken();

  // TODO(cjihrig): Return type.
  _lexer->NextToken();

  EXPECT_OR_RETURN(_lexer->Token(), '{');
  _lexer->NextToken();
  auto nodes = ParseSourceElementList();
  EXPECT_OR_RETURN(_lexer->Token(), '}');
  _lexer->NextToken();
  auto node = make_unique<FunctionDeclarationNode>(
    std::move(id), std::move(nodes), line, column
  );
  return node;
}

unique_ptr<IdentifierNode> Parser::ParseIdentifier() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenIdentifier);
  auto node = make_unique<IdentifierNode>(
    _lexer->Lexeme(), _lexer->Line(), _lexer->Column()
  );
  _lexer->NextToken();
  return node;
}

unique_ptr<IfStatementNode> Parser::ParseIfStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenIf);
  int line = _lexer->Line();
  int column = _lexer->Column();
  EXPECT_OR_RETURN(_lexer->NextToken(), '(');
  _lexer->NextToken();
  auto test = ParseExpression();
  EXPECT_OR_RETURN(_lexer->Token(), ')');
  _lexer->NextToken();
  auto consequent = ParseBlockStatement();
  _lexer->NextToken();
  // TODO(cjihrig): Support alternate (else).

  auto node = make_unique<IfStatementNode>(
    std::move(test), std::move(consequent), nullptr, line, column
  );

  return node;
}

unique_ptr<VariableDeclaratorNode> Parser::ParseLetStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenLet);
  int line = _lexer->Line();
  int column = _lexer->Column();
  _lexer->NextToken();
  auto id = ParseIdentifier();
  EXPECT_OR_RETURN(_lexer->Token(), '=');
  _lexer->NextToken();
  auto init = ParseAssignmentExpression();
  EXPECT_OR_RETURN(_lexer->Token(), ';');
  _lexer->NextToken();
  auto node = make_unique<VariableDeclaratorNode>(
    std::move(id), std::move(init), line, column
  );
  return node;
}

unique_ptr<ASTNode> Parser::ParsePrimaryExpression() {
  int token = _lexer->Token();

  if (token == TokenIdentifier) {
    return ParseIdentifier();
  } else if (token == TokenTrue) {
    auto node = make_unique<BooleanLiteralNode>(
      true, _lexer->Line(), _lexer->Column()
    );

    _lexer->NextToken();
    return node;
  } else if (token == TokenFalse) {
    auto node = make_unique<BooleanLiteralNode>(
      false, _lexer->Line(), _lexer->Column()
    );

    _lexer->NextToken();
    return node;
  } else if (token == TokenNumericLiteral) {
  } else if (token == TokenStringLiteral) {
    auto node = make_unique<StringLiteralNode>(
      _lexer->Lexeme(), _lexer->Line(), _lexer->Column()
    );

    _lexer->NextToken();
    return node;
  } else {
    return nullptr;
  }

  _lexer->NextToken();
  return nullptr;
}

unique_ptr<ReturnStatementNode> Parser::ParseReturnStatement() {
  EXPECT_OR_RETURN(_lexer->Token(), TokenReturn);
  int line = _lexer->Line();
  int column = _lexer->Column();
  _lexer->NextToken();
  EXPECT_OR_RETURN(_lexer->Token(), ';');
  _lexer->NextToken();
  auto node = make_unique<ReturnStatementNode>(line, column);
  return node;
}

unique_ptr<ASTNode> Parser::ParseSourceElement() {
  if (_lexer->Token() == TokenFunction) {
    return ParseFunctionDeclaration();
  } else {
    return ParseStatement();
  }
}

unique_ptr<vector<unique_ptr<ASTNode>>>
    Parser::ParseSourceElementList() {
  auto nodes = make_unique<vector<unique_ptr<ASTNode>>>();

  while (auto source_element = ParseSourceElement()) {
    nodes->push_back(std::move(source_element));
  }

  return nodes;
}

unique_ptr<ASTNode> Parser::ParseStatement() {
  int token = _lexer->Token();

  if (token == TokenIf) {
    return ParseIfStatement();
  } else if (token == '{') {
    return ParseBlockStatement();
  } else if (token == TokenConst) {
    return ParseConstStatement();
  } else if (token == TokenLet) {
    return ParseLetStatement();
  } else if (token == TokenReturn) {
    return ParseReturnStatement();
  } else if (token == ';') {
    return ParseEmptyStatement();
  } else {
    return nullptr;
  }
}

unique_ptr<vector<unique_ptr<ASTNode>>>
    Parser::ParseStatementList() {
  auto nodes = make_unique<vector<unique_ptr<ASTNode>>>();

  while (auto statement = ParseStatement()) {
    nodes->push_back(std::move(statement));
  }

  return nodes;
}

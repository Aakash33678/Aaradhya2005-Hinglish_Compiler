#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cctype>
#include <regex>
#include <set>
#include <stdexcept>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  Token types
// ─────────────────────────────────────────────────────────────
enum class TokenType
{
    VAL,
    PRT,
    AGAR,
    NHI_TO,
    BHEJO,
    JABTAK, //  while loop  ("jab tak" = "as long as")
    KAAM,   //  function def ("kaam"    = "work / function")
    ID,
    NUMBER,
    STRING,
    OP,
    COMPARE,
    ASSIGN,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMI,
    COMMA, //  comma for parameter / argument lists
    END
};

// ─────────────────────────────────────────────────────────────
//  Token structure
// ─────────────────────────────────────────────────────────────
struct Token
{
    TokenType type;
    string value;
    int line;
    int column;

    Token(TokenType t, string v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// ─────────────────────────────────────────────────────────────
//  Return exception — used to propagate bhejo out of a function
// ─────────────────────────────────────────────────────────────
struct ReturnException
{
    int value;
    explicit ReturnException(int v) : value(v) {}
};

// ─────────────────────────────────────────────────────────────
//  AST Node
// ─────────────────────────────────────────────────────────────
struct ASTNode
{
    string nodeType;
    vector<shared_ptr<ASTNode>> children;
    string value;
    int indentLevel = 0;

    ASTNode(string type, string val = "") : nodeType(type), value(val) {}

    void addChild(shared_ptr<ASTNode> child)
    {
        child->indentLevel = indentLevel + 1;
        children.push_back(child);
    }

    void print() const
    {
        cout << string(indentLevel * 2, ' ') << "└─ " << nodeType;
        if (!value.empty())
            cout << " (" << value << ")";
        cout << endl;
        for (const auto &child : children)
            child->print();
    }

    // JSON output for parse-tree visualisation
    void printJSON(ostream &out, int indent = 0) const
    {
        string ind(indent, ' ');
        out << ind << "{\n";
        out << ind << "  \"type\": \"" << nodeType << "\"";
        if (!value.empty())
            out << ",\n"
                << ind << "  \"value\": \"" << value << "\"";
        if (!children.empty())
        {
            out << ",\n"
                << ind << "  \"children\": [\n";
            for (size_t i = 0; i < children.size(); ++i)
            {
                children[i]->printJSON(out, indent + 4);
                if (i + 1 < children.size())
                    out << ",\n";
            }
            out << "\n"
                << ind << "  ]";
        }
        out << "\n"
            << ind << "}";
    }

    // ── Intermediate code (Three-Address Code) ──────────────
    string generateIntermediateCode(vector<string> &code, int &tempCount)
    {
        if (nodeType == "Program")
        {
            for (const auto &child : children)
                child->generateIntermediateCode(code, tempCount);
        }
        else if (nodeType == "Declaration")
        {
            if (!children.empty())
            {
                string temp = children[0]->generateIntermediateCode(code, tempCount);
                code.push_back(value + " = " + temp);
            }
        }
        //  assignment to existing variable
        else if (nodeType == "Assignment")
        {
            if (!children.empty())
            {
                string temp = children[0]->generateIntermediateCode(code, tempCount);
                code.push_back(value + " = " + temp);
            }
        }
        else if (nodeType == "BinaryExpr")
        {
            string L = children[0]->generateIntermediateCode(code, tempCount);
            string R = children[1]->generateIntermediateCode(code, tempCount);
            string res = "T" + to_string(++tempCount);
            code.push_back(res + " = " + L + " " + value + " " + R);
            return res;
        }
        else if (nodeType == "Identifier")
        {
            return value;
        }
        else if (nodeType == "NumberLiteral")
        {
            return value;
        }
        else if (nodeType == "Return")
        {
            if (!children.empty())
            {
                string rv = children[0]->generateIntermediateCode(code, tempCount);
                code.push_back("return " + rv);
            }
        }
        else if (nodeType == "Print")
        {
            if (!children.empty())
            {
                string pv = children[0]->generateIntermediateCode(code, tempCount);
                code.push_back("print " + pv);
            }
        }
        else if (nodeType == "IfElse")
        {
            string cond = children[0]->generateIntermediateCode(code, tempCount);
            string lElse = "L" + to_string(++tempCount);
            string lEnd = "L" + to_string(++tempCount);
            code.push_back("ifnot " + cond + " goto " + lElse);
            children[1]->generateIntermediateCode(code, tempCount);
            code.push_back("goto " + lEnd);
            code.push_back(lElse + ":");
            if (children.size() > 2)
                children[2]->generateIntermediateCode(code, tempCount);
            code.push_back(lEnd + ":");
        }
        //  while loop
        else if (nodeType == "WhileLoop")
        {
            string lStart = "L" + to_string(++tempCount);
            string lEnd = "L" + to_string(++tempCount);
            code.push_back(lStart + ":");
            string cond = children[0]->generateIntermediateCode(code, tempCount);
            code.push_back("ifnot " + cond + " goto " + lEnd);
            children[1]->generateIntermediateCode(code, tempCount);
            code.push_back("goto " + lStart);
            code.push_back(lEnd + ":");
        }
        //  function definition
        else if (nodeType == "FuncDef")
        {
            code.push_back("func " + value + ":");
            // children[0] = ParamList, children[1] = Block
            children[1]->generateIntermediateCode(code, tempCount);
            code.push_back("endfunc " + value);
        }
        //  function call (expression — returns a temp)
        else if (nodeType == "FuncCall")
        {
            for (size_t i = 0; i < children.size(); ++i)
            {
                string arg = children[i]->generateIntermediateCode(code, tempCount);
                code.push_back("param " + arg);
            }
            string res = "T" + to_string(++tempCount);
            code.push_back(res + " = call " + value + ", " + to_string(children.size()));
            return res;
        }
        else if (nodeType == "Block")
        {
            for (const auto &child : children)
                child->generateIntermediateCode(code, tempCount);
        }
        return "";
    }

    // ── Assembly code generation ─────────────────────────────
    string getRegister(int idx) const
    {
        static vector<string> regs = {"eax", "ebx", "ecx", "edx"};
        return regs[idx % regs.size()];
    }

    string generateAssembly(vector<string> &asmCode, int &regCount) const
    {
        if (nodeType == "NumberLiteral" || nodeType == "Identifier")
        {
            string reg = getRegister(regCount++);
            asmCode.push_back("mov " + reg + ", " + value);
            return reg;
        }
        if (nodeType == "BinaryExpr")
        {
            string lReg = children[0]->generateAssembly(asmCode, regCount);
            string rReg = children[1]->generateAssembly(asmCode, regCount);
            if (value == "+")
                asmCode.push_back("add " + lReg + ", " + rReg);
            else if (value == "-")
                asmCode.push_back("sub " + lReg + ", " + rReg);
            else if (value == "*")
                asmCode.push_back("imul " + lReg + ", " + rReg);
            else if (value == "/")
            {
                asmCode.push_back("cdq");
                asmCode.push_back("idiv " + rReg);
            }
            return lReg;
        }
        if (nodeType == "Declaration" || nodeType == "Assignment")
        {
            if (!children.empty())
            {
                int rc = 0;
                string r = children[0]->generateAssembly(asmCode, rc);
                asmCode.push_back("mov " + value + ", " + r);
            }
            else
            {
                asmCode.push_back("mov " + value + ", 0");
            }
            return "";
        }
        if (nodeType == "Program")
        {
            for (const auto &child : children)
                child->generateAssembly(asmCode, regCount);
            return "";
        }
        if (nodeType == "Return")
        {
            if (!children.empty())
            {
                int rc = 0;
                string r = children[0]->generateAssembly(asmCode, rc);
                asmCode.push_back("mov eax, " + r);
                asmCode.push_back("ret");
            }
            return "";
        }
        //  while loop assembly
        if (nodeType == "WhileLoop")
        {
            static int wlCount = 0;
            int id = wlCount++;
            string lStart = ".Lwhile_start" + to_string(id);
            string lEnd = ".Lwhile_end" + to_string(id);
            asmCode.push_back(lStart + ":");
            int rc = 0;
            string condReg = children[0]->generateAssembly(asmCode, rc);
            asmCode.push_back("test " + condReg + ", " + condReg);
            asmCode.push_back("jz " + lEnd);
            children[1]->generateAssembly(asmCode, regCount);
            asmCode.push_back("jmp " + lStart);
            asmCode.push_back(lEnd + ":");
            return "";
        }
        //  function definition assembly
        if (nodeType == "FuncDef")
        {
            asmCode.push_back(value + ":");
            asmCode.push_back("push ebp");
            asmCode.push_back("mov ebp, esp");
            // children[1] = function body Block
            children[1]->generateAssembly(asmCode, regCount);
            asmCode.push_back("pop ebp");
            asmCode.push_back("ret");
            return "";
        }
        //  function call assembly (cdecl: push args right-to-left)
        if (nodeType == "FuncCall")
        {
            for (int i = (int)children.size() - 1; i >= 0; --i)
            {
                int rc = 0;
                string argReg = children[i]->generateAssembly(asmCode, rc);
                asmCode.push_back("push " + argReg);
            }
            asmCode.push_back("call " + value);
            if (!children.empty())
                asmCode.push_back("add esp, " + to_string(children.size() * 4));
            return "eax"; // return value lives in eax
        }
        if (nodeType == "Block")
        {
            for (const auto &child : children)
                child->generateAssembly(asmCode, regCount);
            return "";
        }
        return "";
    }
};

// ─────────────────────────────────────────────────────────────
//  Interpreter
// ─────────────────────────────────────────────────────────────
class Interpreter
{
    map<string, int> variables;
    map<string, shared_ptr<ASTNode>> functions;

public:
    void execute(shared_ptr<ASTNode> node)
    {
        if (!node)
            return;

        if (node->nodeType == "Program")
        {
            // First pass: register all function definitions so they can be
            // called even before their textual position in the source.
            for (auto &child : node->children)
                if (child->nodeType == "FuncDef")
                    functions[child->value] = child;

            // Second pass: execute all non-function statements in order.
            for (auto &child : node->children)
                if (child->nodeType != "FuncDef")
                    execute(child);
        }
        else if (node->nodeType == "Declaration")
        {
            int val = node->children.empty() ? 0 : evaluate(node->children[0]);
            variables[node->value] = val;
        }
        //  assignment to an already-declared variable
        else if (node->nodeType == "Assignment")
        {
            variables[node->value] = evaluate(node->children[0]);
        }
        else if (node->nodeType == "Print")
        {
            if (!node->children.empty())
            {
                auto &child = node->children[0];
                if (child->nodeType == "StringLiteral")
                {
                    string s = child->value;
                    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
                        s = s.substr(1, s.size() - 2);
                    cout << s << endl;
                }
                else
                {
                    cout << evaluate(child) << endl;
                }
            }
        }
        else if (node->nodeType == "IfElse")
        {
            if (evaluate(node->children[0]))
                execute(node->children[1]);
            else if (node->children.size() > 2)
                execute(node->children[2]);
        }
        else if (node->nodeType == "Block")
        {
            for (auto &child : node->children)
                execute(child);
        }
        else if (node->nodeType == "Return")
        {
            int val = evaluate(node->children[0]);
            throw ReturnException(val); // propagate up the call stack
        }
        //  while loop
        else if (node->nodeType == "WhileLoop")
        {
            while (evaluate(node->children[0]))
                execute(node->children[1]);
        }
        //  function call as a standalone statement (return value discarded)
        else if (node->nodeType == "FuncCall")
        {
            callFunction(node);
        }
        // FuncDef nodes are handled in the first pass above; ignore here.
        else if (node->nodeType == "FuncDef")
        {
            functions[node->value] = node;
        }
    }

    int evaluate(shared_ptr<ASTNode> node)
    {
        if (node->nodeType == "NumberLiteral")
            return stoi(node->value);

        if (node->nodeType == "Identifier")
        {
            if (!variables.count(node->value))
            {
                cerr << "Runtime error: undefined variable '" << node->value << "'" << endl;
                return 0;
            }
            return variables[node->value];
        }

        if (node->nodeType == "BinaryExpr")
        {
            int L = evaluate(node->children[0]);
            int R = evaluate(node->children[1]);
            if (node->value == "+")
                return L + R;
            if (node->value == "-")
                return L - R;
            if (node->value == "*")
                return L * R;
            if (node->value == "/")
                return R != 0 ? L / R : 0;
            if (node->value == "==")
                return L == R;
            if (node->value == "!=")
                return L != R;
            if (node->value == "<")
                return L < R;
            if (node->value == "<=")
                return L <= R;
            if (node->value == ">")
                return L > R;
            if (node->value == ">=")
                return L >= R;
        }

        //  function call used inside an expression
        if (node->nodeType == "FuncCall")
            return callFunction(node);

        return 0;
    }

private:
    //  call a user-defined function and return its value
    int callFunction(shared_ptr<ASTNode> callNode)
    {
        const string &funcName = callNode->value;
        if (!functions.count(funcName))
        {
            cerr << "Runtime error: undefined function '" << funcName << "'" << endl;
            return 0;
        }

        auto funcDef = functions[funcName];
        // funcDef->children[0] = ParamList
        // funcDef->children[1] = Block (body)
        auto &paramList = funcDef->children[0];

        // Save the caller's variable scope
        map<string, int> savedVars = variables;

        // Bind call arguments to parameter names
        for (size_t i = 0; i < paramList->children.size(); ++i)
        {
            string paramName = paramList->children[i]->value;
            int argVal = (i < callNode->children.size())
                             ? evaluate(callNode->children[i])
                             : 0;
            variables[paramName] = argVal;
        }

        int returnVal = 0;
        try
        {
            execute(funcDef->children[1]); // execute the body
        }
        catch (ReturnException &ret)
        {
            returnVal = ret.value;
        }

        // Restore caller's scope
        variables = savedVars;
        return returnVal;
    }
};

// ─────────────────────────────────────────────────────────────
//  Compiler class
// ─────────────────────────────────────────────────────────────
class HinglishCompiler
{
private:
    vector<Token> tokens;
    size_t currentTokenIndex = 0;
    map<string, string> symbolTable;
    vector<string> errors;
    vector<string> intermediateCode;
    shared_ptr<ASTNode> ast;

    const set<string> reservedKeywords =
        {"val", "prt", "agar", "nhi-to", "bhejo", "jabtak", "kaam"};

public:
    void compile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error: Could not open file '" << filename << "'" << endl;
            return;
        }

        string sourceCode((istreambuf_iterator<char>(file)),
                          istreambuf_iterator<char>());
        file.close();

        cout << "=== Source Code ===" << endl;
        cout << sourceCode << endl
             << endl;

        // Phase 1: Lexical Analysis
        cout << "=== Lexical Analysis (Tokenization) ===" << endl;
        tokenize(sourceCode);
        printTokens();

        if (!errors.empty())
        {
            printErrors();
            return;
        }

        // Phase 2: Syntax Analysis
        cout << "\n=== Syntax Analysis (Parsing) ===" << endl;
        ast = parseProgram();

        if (!errors.empty())
        {
            printErrors();
            return;
        }

        cout << "\nParse Tree (JSON):" << endl;
        if (ast)
            ast->printJSON(cout, 0);
        cout << endl;

        // Phase 3: Semantic Analysis
        cout << "\n=== Semantic Analysis ===" << endl;
        semanticAnalysis(ast.get());

        if (!errors.empty())
        {
            printErrors();
            return;
        }

        cout << "\nSymbol Table:" << endl;
        for (const auto &entry : symbolTable)
            cout << entry.first << ": " << entry.second << endl;

        // Phase 4: Intermediate Code Generation
        cout << "\n=== Intermediate Code Generation ===" << endl;
        int tempCount = 0;
        ast->generateIntermediateCode(intermediateCode, tempCount);

        cout << "\nIntermediate Code (Three-Address Code):" << endl;
        for (size_t i = 0; i < intermediateCode.size(); ++i)
            cout << i << ": " << intermediateCode[i] << endl;

        // Phase 5: Assembly Code Generation
        cout << "\n=== Assembly Code Generation ===" << endl;
        vector<string> asmCode;
        int regCount = 0;
        if (ast)
            ast->generateAssembly(asmCode, regCount);

        cout << "\nAssembly Code:" << endl;
        for (size_t i = 0; i < asmCode.size(); ++i)
            cout << i << ": " << asmCode[i] << endl;

        cout << "\nCompilation successful!" << endl;
        cout << "\n=== Output of Input Code ===" << endl;
        Interpreter interpreter;
        interpreter.execute(ast);
    }

private:
    // ── Lexer ─────────────────────────────────────────────────
    void tokenize(const string &source)
    {
        // IMPORTANT: keywords must appear BEFORE the general ID pattern
        // so they are matched first.
        vector<pair<string, TokenType>> tokenSpecs = {
            // Keywords
            {"jabtak", TokenType::JABTAK},
            {"kaam", TokenType::KAAM},
            {"val", TokenType::VAL},
            {"prt", TokenType::PRT},
            {"agar", TokenType::AGAR},
            {"nhi-to", TokenType::NHI_TO},
            {"bhejo", TokenType::BHEJO},
            // Comparison operators (multi-char first)
            {"==", TokenType::COMPARE},
            {"!=", TokenType::COMPARE},
            {"<=", TokenType::COMPARE},
            {">=", TokenType::COMPARE},
            {"<", TokenType::COMPARE},
            {">", TokenType::COMPARE},
            // Assignment
            {"=", TokenType::ASSIGN},
            // Arithmetic operators
            {"[+*/\\-]", TokenType::OP},
            // Punctuation
            {"\\(", TokenType::LPAREN},
            {"\\)", TokenType::RPAREN},
            {"\\{", TokenType::LBRACE},
            {"\\}", TokenType::RBRACE},
            {";", TokenType::SEMI},
            {",", TokenType::COMMA},
            // Literals
            {"\"[^\"]*\"", TokenType::STRING},
            {"[0-9]+", TokenType::NUMBER},
            // Identifiers (must be last)
            {"[a-zA-Z_][a-zA-Z0-9_]*", TokenType::ID},
        };

        size_t pos = 0;
        int line = 1;
        int lineStart = 0;

        while (pos < source.length())
        {
            // Skip whitespace
            while (pos < source.length() && isspace(source[pos]))
            {
                if (source[pos] == '\n')
                {
                    line++;
                    lineStart = pos + 1;
                }
                pos++;
            }
            if (pos >= source.length())
                break;

            bool matched = false;
            for (const auto &spec : tokenSpecs)
            {
                regex re("^" + spec.first);
                smatch m;
                string rem = source.substr(pos);

                if (regex_search(rem, m, re))
                {
                    string val = m.str();
                    int col = pos - lineStart;
                    tokens.emplace_back(spec.second, val, line, col);
                    pos += val.length();
                    matched = true;
                    break;
                }
            }

            if (!matched)
            {
                int col = pos - lineStart;
                errors.push_back("Illegal character '" + string(1, source[pos]) +
                                 "' at line " + to_string(line) +
                                 ", column " + to_string(col));
                pos++;
            }
        }
        tokens.emplace_back(TokenType::END, "", line, 0);
    }

    // ── Parser ────────────────────────────────────────────────
    shared_ptr<ASTNode> parseProgram()
    {
        auto program = make_shared<ASTNode>("Program");
        while (currentToken().type != TokenType::END)
        {
            auto stmt = parseStatement();
            if (stmt)
                program->addChild(stmt);
        }
        return program;
    }

    shared_ptr<ASTNode> parseStatement()
    {
        if (currentToken().type == TokenType::VAL)
            return parseDeclaration();

        if (currentToken().type == TokenType::PRT)
            return parsePrint();

        if (currentToken().type == TokenType::AGAR)
            return parseIfElse();

        if (currentToken().type == TokenType::BHEJO)
            return parseReturn();

        if (currentToken().type == TokenType::JABTAK)
            return parseWhileLoop();

        if (currentToken().type == TokenType::KAAM)
            return parseFuncDef();

        // Identifier: either a function call (f(args);) or an assignment (x = expr;)
        if (currentToken().type == TokenType::ID)
        {
            string name = currentToken().value;
            advance(); // consume the identifier

            if (currentToken().type == TokenType::LPAREN)
            {
                // Function call statement
                auto callNode = parseFuncCallExpr(name);
                if (!callNode)
                    return nullptr;
                if (currentToken().type != TokenType::SEMI)
                {
                    errors.push_back("Expected ';' after function call at line " +
                                     to_string(currentToken().line));
                    return nullptr;
                }
                advance(); // skip ';'
                return callNode;
            }

            if (currentToken().type == TokenType::ASSIGN)
            {
                advance(); // skip '='
                auto expr = parseExpression();
                if (!expr)
                {
                    errors.push_back("Invalid expression in assignment at line " +
                                     to_string(currentToken().line));
                    return nullptr;
                }
                if (currentToken().type != TokenType::SEMI)
                {
                    errors.push_back("Expected ';' after assignment at line " +
                                     to_string(currentToken().line));
                    return nullptr;
                }
                advance(); // skip ';'
                auto assignNode = make_shared<ASTNode>("Assignment", name);
                assignNode->addChild(expr);
                return assignNode;
            }

            errors.push_back("Unexpected token '" + currentToken().value +
                             "' after identifier '" + name + "' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }

        errors.push_back("Unexpected statement token '" + currentToken().value +
                         "' at line " + to_string(currentToken().line) +
                         ", column " + to_string(currentToken().column));
        advance();
        return nullptr;
    }

    shared_ptr<ASTNode> parseWhileLoop()
    {
        advance(); // skip 'jabtak'

        if (currentToken().type != TokenType::LPAREN)
        {
            errors.push_back("Expected '(' after 'jabtak' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        advance(); // skip '('

        auto cond = parseExpression();
        if (!cond)
        {
            errors.push_back("Invalid condition in jabtak statement");
            return nullptr;
        }

        if (currentToken().type != TokenType::RPAREN)
        {
            errors.push_back("Expected ')' after jabtak condition");
            return nullptr;
        }
        advance(); // skip ')'

        if (currentToken().type != TokenType::LBRACE)
        {
            errors.push_back("Expected '{' after jabtak condition");
            return nullptr;
        }
        advance(); // skip '{'

        auto body = make_shared<ASTNode>("Block");
        while (currentToken().type != TokenType::RBRACE &&
               currentToken().type != TokenType::END)
        {
            auto stmt = parseStatement();
            if (stmt)
                body->addChild(stmt);
        }

        if (currentToken().type != TokenType::RBRACE)
        {
            errors.push_back("Expected '}' at end of jabtak block");
            return nullptr;
        }
        advance(); // skip '}'

        auto whileNode = make_shared<ASTNode>("WhileLoop");
        whileNode->addChild(cond);
        whileNode->addChild(body);
        return whileNode;
    }

    //  kaam funcName(param1, param2) { body }
    shared_ptr<ASTNode> parseFuncDef()
    {
        advance(); // skip 'kaam'

        if (currentToken().type != TokenType::ID)
        {
            errors.push_back("Expected function name after 'kaam' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        string funcName = currentToken().value;
        if (reservedKeywords.count(funcName))
        {
            errors.push_back("Cannot use reserved keyword '" + funcName +
                             "' as a function name");
            return nullptr;
        }
        advance(); // skip function name

        if (currentToken().type != TokenType::LPAREN)
        {
            errors.push_back("Expected '(' after function name '" + funcName + "'");
            return nullptr;
        }
        advance(); // skip '('

        auto paramList = make_shared<ASTNode>("ParamList");
        while (currentToken().type != TokenType::RPAREN &&
               currentToken().type != TokenType::END)
        {
            if (currentToken().type != TokenType::ID)
            {
                errors.push_back("Expected parameter name in kaam '" + funcName + "'");
                return nullptr;
            }
            string paramName = currentToken().value;
            if (reservedKeywords.count(paramName))
            {
                errors.push_back("Cannot use reserved keyword '" + paramName +
                                 "' as a parameter name");
                return nullptr;
            }
            paramList->addChild(make_shared<ASTNode>("Param", paramName));
            advance(); // skip param name
            if (currentToken().type == TokenType::COMMA)
                advance(); // skip ','
        }

        if (currentToken().type != TokenType::RPAREN)
        {
            errors.push_back("Expected ')' after parameters of kaam '" + funcName + "'");
            return nullptr;
        }
        advance(); // skip ')'

        if (currentToken().type != TokenType::LBRACE)
        {
            errors.push_back("Expected '{' for body of kaam '" + funcName + "'");
            return nullptr;
        }
        advance(); // skip '{'

        auto body = make_shared<ASTNode>("Block");
        while (currentToken().type != TokenType::RBRACE &&
               currentToken().type != TokenType::END)
        {
            auto stmt = parseStatement();
            if (stmt)
                body->addChild(stmt);
        }

        if (currentToken().type != TokenType::RBRACE)
        {
            errors.push_back("Expected '}' at end of kaam '" + funcName + "'");
            return nullptr;
        }
        advance(); // skip '}'

        auto funcDef = make_shared<ASTNode>("FuncDef", funcName);
        funcDef->addChild(paramList);
        funcDef->addChild(body);
        return funcDef;
    }

    //  parse a function-call expression; name has already been consumed,
    //      currentToken() must be '(' when this is called.
    shared_ptr<ASTNode> parseFuncCallExpr(const string &funcName)
    {
        advance(); // skip '('

        auto callNode = make_shared<ASTNode>("FuncCall", funcName);

        while (currentToken().type != TokenType::RPAREN &&
               currentToken().type != TokenType::END)
        {
            auto arg = parseExpression();
            if (arg)
                callNode->addChild(arg);
            if (currentToken().type == TokenType::COMMA)
                advance(); // skip ','
        }

        if (currentToken().type != TokenType::RPAREN)
        {
            errors.push_back("Expected ')' in call to '" + funcName + "'");
            return nullptr;
        }
        advance(); // skip ')'

        return callNode;
    }

    shared_ptr<ASTNode> parseReturn()
    {
        advance(); // skip 'bhejo'

        auto expr = parseExpression();
        if (!expr)
        {
            errors.push_back("Invalid expression in bhejo statement");
            return nullptr;
        }
        if (currentToken().type != TokenType::SEMI)
        {
            errors.push_back("Expected ';' after bhejo statement");
            return nullptr;
        }
        advance(); // skip ';'

        auto retNode = make_shared<ASTNode>("Return");
        retNode->addChild(expr);
        return retNode;
    }

    shared_ptr<ASTNode> parsePrint()
    {
        advance(); // skip 'prt'

        if (currentToken().type != TokenType::LPAREN)
        {
            errors.push_back("Expected '(' after 'prt'");
            return nullptr;
        }
        advance(); // skip '('

        shared_ptr<ASTNode> expr = nullptr;
        if (currentToken().type == TokenType::STRING)
        {
            expr = make_shared<ASTNode>("StringLiteral", currentToken().value);
            advance();
        }
        else
        {
            expr = parseExpression();
        }

        if (currentToken().type != TokenType::RPAREN)
        {
            errors.push_back("Expected ')' after prt argument");
            return nullptr;
        }
        advance(); // skip ')'

        if (currentToken().type != TokenType::SEMI)
        {
            errors.push_back("Expected ';' after prt statement");
            return nullptr;
        }
        advance(); // skip ';'

        auto prtNode = make_shared<ASTNode>("Print");
        if (expr)
            prtNode->addChild(expr);
        return prtNode;
    }

    shared_ptr<ASTNode> parseIfElse()
    {
        advance(); // skip 'agar'

        if (currentToken().type != TokenType::LPAREN)
        {
            errors.push_back("Expected '(' after 'agar'");
            return nullptr;
        }
        advance(); // skip '('

        auto condition = parseExpression();
        if (!condition)
        {
            errors.push_back("Invalid condition in agar statement");
            return nullptr;
        }

        if (currentToken().type != TokenType::RPAREN)
        {
            errors.push_back("Expected ')' after agar condition");
            return nullptr;
        }
        advance(); // skip ')'

        if (currentToken().type != TokenType::LBRACE)
        {
            errors.push_back("Expected '{' after agar condition");
            return nullptr;
        }
        advance(); // skip '{'

        auto ifBlock = make_shared<ASTNode>("Block");
        while (currentToken().type != TokenType::RBRACE &&
               currentToken().type != TokenType::END)
        {
            auto stmt = parseStatement();
            if (stmt)
                ifBlock->addChild(stmt);
        }
        if (currentToken().type != TokenType::RBRACE)
        {
            errors.push_back("Expected '}' at end of agar block");
            return nullptr;
        }
        advance(); // skip '}'

        // Optional nhi-to block
        shared_ptr<ASTNode> elseBlock = nullptr;
        if (currentToken().type == TokenType::NHI_TO)
        {
            advance(); // skip 'nhi-to'
            if (currentToken().type != TokenType::LBRACE)
            {
                errors.push_back("Expected '{' after nhi-to");
                return nullptr;
            }
            advance(); // skip '{'
            elseBlock = make_shared<ASTNode>("Block");
            while (currentToken().type != TokenType::RBRACE &&
                   currentToken().type != TokenType::END)
            {
                auto stmt = parseStatement();
                if (stmt)
                    elseBlock->addChild(stmt);
            }
            if (currentToken().type != TokenType::RBRACE)
            {
                errors.push_back("Expected '}' at end of nhi-to block");
                return nullptr;
            }
            advance(); // skip '}'
        }

        auto ifElseNode = make_shared<ASTNode>("IfElse");
        ifElseNode->addChild(condition);
        ifElseNode->addChild(ifBlock);
        if (elseBlock)
            ifElseNode->addChild(elseBlock);

        return ifElseNode;
    }

    shared_ptr<ASTNode> parseDeclaration()
    {
        advance(); // skip 'val'

        if (currentToken().type == TokenType::VAL ||
            currentToken().type == TokenType::PRT ||
            currentToken().type == TokenType::AGAR ||
            currentToken().type == TokenType::NHI_TO ||
            currentToken().type == TokenType::BHEJO ||
            currentToken().type == TokenType::JABTAK ||
            currentToken().type == TokenType::KAAM)
        {
            errors.push_back("Cannot use reserved keyword '" + currentToken().value +
                             "' as an identifier after 'val' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        if (currentToken().type != TokenType::ID)
        {
            errors.push_back("Expected identifier after 'val' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        string varName = currentToken().value;
        if (reservedKeywords.count(varName))
        {
            errors.push_back("Cannot use reserved keyword '" + varName +
                             "' as an identifier after 'val' at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        advance(); // skip ID

        shared_ptr<ASTNode> expr = nullptr;
        if (currentToken().type == TokenType::ASSIGN)
        {
            advance(); // skip '='
            expr = parseExpression();
            if (!expr)
            {
                errors.push_back("Invalid expression in declaration at line " +
                                 to_string(currentToken().line));
                return nullptr;
            }
        }
        if (currentToken().type != TokenType::SEMI)
        {
            errors.push_back("Expected ';' at end of declaration at line " +
                             to_string(currentToken().line));
            return nullptr;
        }
        advance(); // skip ';'

        auto decl = make_shared<ASTNode>("Declaration", varName);
        if (expr)
            decl->addChild(expr);
        return decl;
    }

    // Expression parser (Pratt / precedence-climbing)
    shared_ptr<ASTNode> parseExpression(int minPrec = 0)
    {
        auto left = parsePrimary();
        if (!left)
            return nullptr;

        while (true)
        {
            string op = currentToken().value;
            int prec = getPrecedence(op);
            if ((currentToken().type == TokenType::OP ||
                 currentToken().type == TokenType::COMPARE) &&
                prec >= minPrec)
            {
                advance();
                auto right = parseExpression(prec + 1);
                if (!right)
                    return nullptr;
                auto bin = make_shared<ASTNode>("BinaryExpr", op);
                bin->addChild(left);
                bin->addChild(right);
                left = bin;
            }
            else
            {
                break;
            }
        }
        return left;
    }

    shared_ptr<ASTNode> parsePrimary()
    {
        if (currentToken().type == TokenType::ID)
        {
            string name = currentToken().value;
            advance(); // consume identifier

            // If followed by '(' it is a function-call expression
            if (currentToken().type == TokenType::LPAREN)
                return parseFuncCallExpr(name);

            return make_shared<ASTNode>("Identifier", name);
        }
        if (currentToken().type == TokenType::NUMBER)
        {
            auto node = make_shared<ASTNode>("NumberLiteral", currentToken().value);
            advance();
            return node;
        }
        if (currentToken().type == TokenType::LPAREN)
        {
            advance(); // skip '('
            auto expr = parseExpression();
            if (currentToken().type != TokenType::RPAREN)
            {
                errors.push_back("Expected ')' in grouped expression");
                return nullptr;
            }
            advance(); // skip ')'
            return expr;
        }

        errors.push_back("Expected identifier, number, or '(' in expression at line " +
                         to_string(currentToken().line));
        return nullptr;
    }

    int getPrecedence(const string &op)
    {
        if (op == "==" || op == "!=" || op == "<" ||
            op == ">" || op == "<=" || op == ">=")
            return 0;
        if (op == "+" || op == "-")
            return 1;
        if (op == "*" || op == "/")
            return 2;
        return -1;
    }

    Token &currentToken() { return tokens[currentTokenIndex]; }
    void advance()
    {
        if (currentTokenIndex < tokens.size() - 1)
            ++currentTokenIndex;
    }

    // ── Token printer ─────────────────────────────────────────
    void printTokens() const
    {
        for (const auto &token : tokens)
        {
            cout << "Line " << token.line << ", Column " << token.column << ": ";
            switch (token.type)
            {
            case TokenType::VAL:
                cout << "Keyword(val)";
                break;
            case TokenType::PRT:
                cout << "Keyword(prt)";
                break;
            case TokenType::AGAR:
                cout << "Keyword(agar)";
                break;
            case TokenType::NHI_TO:
                cout << "Keyword(nhi-to)";
                break;
            case TokenType::BHEJO:
                cout << "Keyword(bhejo)";
                break;
            case TokenType::JABTAK:
                cout << "Keyword(jabtak)";
                break;
            case TokenType::KAAM:
                cout << "Keyword(kaam)";
                break;
            case TokenType::ID:
                cout << "ID";
                break;
            case TokenType::NUMBER:
                cout << "NUMBER";
                break;
            case TokenType::STRING:
                cout << "STRING";
                break;
            case TokenType::OP:
                cout << "OP";
                break;
            case TokenType::COMPARE:
                cout << "COMPARE";
                break;
            case TokenType::ASSIGN:
                cout << "ASSIGN";
                break;
            case TokenType::LPAREN:
                cout << "LPAREN";
                break;
            case TokenType::RPAREN:
                cout << "RPAREN";
                break;
            case TokenType::LBRACE:
                cout << "LBRACE";
                break;
            case TokenType::RBRACE:
                cout << "RBRACE";
                break;
            case TokenType::SEMI:
                cout << "SEMI";
                break;
            case TokenType::COMMA:
                cout << "COMMA";
                break;
            case TokenType::END:
                cout << "END";
                break;
            }
            cout << " = " << token.value << endl;
        }
    }

    // ── Semantic analysis ─────────────────────────────────────
    void semanticAnalysis(ASTNode *node)
    {
        if (!node)
            return;

        if (node->nodeType == "Program")
        {
            // Register function names first so forward calls are valid
            for (const auto &child : node->children)
                if (child->nodeType == "FuncDef")
                    symbolTable[child->value] = "function";

            for (const auto &child : node->children)
                semanticAnalysis(child.get());
        }
        else if (node->nodeType == "Declaration")
        {
            if (symbolTable.count(node->value))
                errors.push_back("Variable '" + node->value + "' already declared.");
            else
                symbolTable[node->value] = "variable";

            if (!node->children.empty())
                semanticAnalysis(node->children[0].get());
        }
        //  assignment — variable must already exist
        else if (node->nodeType == "Assignment")
        {
            if (!symbolTable.count(node->value))
                errors.push_back("Assignment to undeclared variable '" + node->value + "'.");

            if (!node->children.empty())
                semanticAnalysis(node->children[0].get());
        }
        else if (node->nodeType == "Identifier")
        {
            if (!symbolTable.count(node->value))
                errors.push_back("Undeclared identifier '" + node->value + "'.");
        }
        else if (node->nodeType == "BinaryExpr")
        {
            semanticAnalysis(node->children[0].get());
            semanticAnalysis(node->children[1].get());
        }
        else if (node->nodeType == "Return")
        {
            if (!node->children.empty())
                semanticAnalysis(node->children[0].get());
        }
        //  while loop
        else if (node->nodeType == "WhileLoop")
        {
            semanticAnalysis(node->children[0].get()); // condition
            semanticAnalysis(node->children[1].get()); // body
        }
        //  function definition
        else if (node->nodeType == "FuncDef")
        {
            // Function name already registered in Program pass; just analyse body.
            auto &paramList = node->children[0];
            vector<string> paramNames;
            for (auto &param : paramList->children)
            {
                symbolTable[param->value] = "variable";
                paramNames.push_back(param->value);
            }
            semanticAnalysis(node->children[1].get()); // body
            // Remove params from global symbol table after checking body
            for (const auto &p : paramNames)
                symbolTable.erase(p);
        }
        //  function call
        else if (node->nodeType == "FuncCall")
        {
            if (!symbolTable.count(node->value))
                errors.push_back("Call to undefined function '" + node->value + "'.");

            for (const auto &child : node->children)
                semanticAnalysis(child.get());
        }
        else
        {
            // Generic fallback: recurse into children
            for (const auto &child : node->children)
                semanticAnalysis(child.get());
        }
    }

    void printErrors() const
    {
        cout << "\nCompilation errors:" << endl;
        for (const auto &error : errors)
            cout << "  " << error << endl;
    }
};

// ─────────────────────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────────────────────
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <filename.li>" << endl;
        return 1;
    }

    HinglishCompiler compiler;
    compiler.compile(argv[1]);

    return 0;
}
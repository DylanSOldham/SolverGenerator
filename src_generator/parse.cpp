#include "parse.h"

std::vector<Parameter> parse_parameters(std::vector<Token> tokens);
std::shared_ptr<Expression> parse_expression(std::vector<Token>& tokens);

Symbol parse_symbol(std::vector<Token>& tokens)
{   
    Symbol symbol = tokens[0].symbol.value();

    TokenType end_token;
    switch (tokens[1].type)
    {
        case TokenType::LBRACKET:
            end_token = TokenType::RBRACKET;
            symbol.type = SymbolType::STATE;
        break;
        case TokenType::LPAREN:
            end_token = TokenType::RPAREN;
            symbol.type = SymbolType::FUNCTION;
        break;
        default:
            tokens.erase(tokens.begin());
            symbol.type = SymbolType::FUNCTION;
            return symbol;
    }

    auto end_it = std::find_if(tokens.begin() + 1, tokens.end(), [&](Token token){ return token.type == end_token; });
    if (end_it == tokens.end()) {
        tokens.clear();
        symbol.type = SymbolType::UNRESOLVED;
        return symbol;
    }

    std::vector<Token> parameters_tokens = std::vector<Token>(tokens.begin() + 2, end_it);
    std::vector<Parameter> parameters = parse_parameters(parameters_tokens);

    symbol.parameters = parameters;
    tokens.erase(tokens.begin(), end_it + 1);
    return symbol;
}

enum Priority
{
    PRIORITY_ALL = 0,
    PRIORITY_ADD = 1,
    PRIORITY_MUL = 2,
    PRIORITY_EXP = 3,
    PRIORITY_PAREN = 4,
};

std::vector<Token> pop_subexpr_tokens(std::vector<Token>& tokens, Priority priority)
{
    int paren_level = 0;
    int bracket_level = 0;
    auto it = tokens.begin();
    for (; it != tokens.end(); ++it) 
    {
        if (it->type == TokenType::LPAREN) paren_level += 1;
        if (it->type == TokenType::RPAREN) paren_level -= 1;
        if (paren_level > 0) continue;
        if (paren_level < 0) break;

        if (it->type == TokenType::LBRACKET) bracket_level += 1;
        if (it->type == TokenType::RBRACKET) bracket_level -= 1;
        if (bracket_level > 0) continue;
        if (bracket_level < 0) break;

        if (priority >= Priority::PRIORITY_ADD && (it->type == TokenType::ADD || it->type == TokenType::SUBTRACT))
        {
            // Special exception allowing negate expressions to be alone in exponents
            if (priority == Priority::PRIORITY_EXP && it == tokens.begin() + 1 && it->type == TokenType::SUBTRACT)
                continue;

            break;
        }

        if (priority >= Priority::PRIORITY_MUL && (it->type == TokenType::MULTIPLY || it->type == TokenType::DIVIDE)) 
            break;

        if (priority >= Priority::PRIORITY_EXP && it->type == TokenType::EXPONENT) 
            break;
    }

    auto subexpr_tokens = std::vector<Token>(tokens.begin(), it);
    tokens.erase(tokens.begin(), it);
    return subexpr_tokens;
}

std::shared_ptr<Expression> parse_unary_expression(std::vector<Token>& tokens)
{
    if (tokens[1].type == TokenType::LPAREN)
    {
        tokens.erase(tokens.begin());
        auto subexpr_tokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_PAREN);
        return parse_expression(subexpr_tokens);
    }

    tokens.erase(tokens.begin());
    auto subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ADD);
    return parse_expression(subexprTokens);
}

std::shared_ptr<Expression> parse_expression(std::vector<Token>& tokens)
{
    std::shared_ptr<Expression> expression = nullptr;
    std::vector<Token> subexprTokens;
                
    std::cerr << tokens.size() << " ";
    for (auto t : tokens)
        std::cerr << t.to_string() << " ";
    std::cerr << "\n";

    while (tokens.size() > 0)
    {
        switch(tokens[0].type) 
        {
            case TokenType::LPAREN:
                {
                tokens.erase(tokens.begin());
                auto subexpr_tokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ALL);
                tokens.erase(tokens.begin());
                expression = parse_expression(subexpr_tokens);
                }
                continue;
            case TokenType::SYMBOL:
                expression = std::make_shared<SymbolExpression>(parse_symbol(tokens));
                continue;
            case TokenType::CONSTANT:
                expression = std::make_shared<ConstantExpression>(tokens[0].value.value());
                tokens.erase(tokens.begin());
                continue;
            case TokenType::ADD:
                tokens.erase(tokens.begin());
                subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ADD);
                expression = std::make_shared<AddExpression>(expression, parse_expression(subexprTokens));
                continue;
            case TokenType::SUBTRACT:
                tokens.erase(tokens.begin());
                subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ADD);
                expression = std::make_shared<SubtractExpression>(expression, parse_expression(subexprTokens));
                continue;
            case TokenType::MULTIPLY:
                tokens.erase(tokens.begin());
                subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_MUL);
                expression = std::make_shared<MultiplyExpression>(expression, parse_expression(subexprTokens));
                continue;
            case TokenType::DIVIDE:
                tokens.erase(tokens.begin());
                subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_MUL);
                expression = std::make_shared<DivideExpression>(expression, parse_expression(subexprTokens));
                continue;
            case TokenType::EXPONENT:
                tokens.erase(tokens.begin());
                subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_EXP);
                expression = std::make_unique<ExponentExpression>(expression, parse_expression(subexprTokens));
                continue;
            case TokenType::NEGATE:
                {
                auto unary_expr = parse_unary_expression(tokens);
                if (!unary_expr)
                {
                    std::cerr << "Error: Negate doesn't have a valid expression.\n";
                }
                expression = std::make_shared<NegateExpression>(unary_expr);
                }
                continue;
            case TokenType::SQRT:
                {
                auto unary_expr = parse_unary_expression(tokens);
                if (!unary_expr)
                {
                    std::cerr << "Error: Sqrt doesn't have a valid expression.\n";
                }
                expression = std::make_shared<SqrtExpression>(unary_expr);
                }
                continue;
            case TokenType::EXP:
                {
                auto unary_expr = parse_unary_expression(tokens);
                if (!unary_expr)
                {
                    std::cerr << "Error: Sqrt doesn't have a valid expression.\n";
                }
                expression = std::make_shared<ExpExpression>(unary_expr);
                }
                continue;
            default:
                std::cerr << "Unable to parse expression starting with token " << tokens[0].to_string() << "\n";
                return nullptr;
        }
    }

    return expression;
}

std::vector<Parameter> parse_parameters(std::vector<Token> tokens)
{
    std::vector<Parameter> parameters;

    while (1)
    {
        auto it = std::find_if(tokens.begin(), tokens.end(), [](Token token){ return token.type == TokenType::COMMA; });

        std::vector<Token> parameter_tokens = std::vector<Token>(tokens.begin(), it);

        Parameter parameter;
        if (parameter_tokens.size() == 1 && parameter_tokens[0].type == TokenType::SYMBOL)
        {
            parameter.type = ParameterType::VARIABLE;
            parameter.symbol = parameter_tokens[0].symbol.value().name;
            parameters.push_back(parameter);
        }
        else
        {
            parameter.type = ParameterType::EXPRESSION;
            parameter.expression = parse_expression(parameter_tokens);
            parameters.push_back(parameter);
        }

        if (it == tokens.end()) break;

        tokens.erase(tokens.begin(), it + 1);
    }

    return parameters;
}

void parse_state_definition(SystemDeclarations& system, std::vector<Token> tokens)
{
    if (tokens.size() < 2 || !tokens[1].symbol.has_value()) 
    {
        std::cerr << "Error: Derivative is missing symbol.\n";
        return;
    }

    tokens.erase(tokens.begin()); // Remove the derivative token
    Symbol symbol = parse_symbol(tokens);
    tokens.erase(tokens.begin()); // Remove the = token

    std::shared_ptr<Expression> rhs = parse_expression(tokens);
    if (!rhs)
    {
        std::cerr << "Error: Malformed rhs expression for derivative of " << symbol.name << "\n";
        return;
    }

    system.state_variables.push_back(StateVariable(symbol, std::move(rhs)));
}

void parse_initial_value(SystemDeclarations& system, std::vector<Token> tokens)
{
    tokens.erase(tokens.begin()); // Remove "INITIAL"
    Symbol symbol = parse_symbol(tokens); // Grab and remove the symbol
    tokens.erase(tokens.begin()); // Remove "="

    std::shared_ptr<Expression> expression = parse_expression(tokens);
    if (!expression)
    {
        std::cerr << "Error: Malformed expression for initial value of " << symbol.name << "\n";
        return;
    }
    system.initial_states.push_back(InitialState { symbol, expression });
}

void parse_expression_declaration(SystemDeclarations& system, std::vector<Token> tokens)
{
    Symbol symbol = parse_symbol(tokens);

    tokens.erase(tokens.begin());
    std::shared_ptr<Expression> expression = parse_expression(tokens);
    if (!expression)
    {
        std::cerr << "Malformed expression: " << symbol.name << "\n";
        return;
    }

    if (symbol.type == SymbolType::FUNCTION)
    {
        auto function = system.find_function_definition(symbol);
        if (function == nullptr)
        {
            system.function_definitions.push_back(Function(symbol));
            function = system.find_function_definition(symbol);
        }

        function->definitions.push_back( FunctionDefinition { symbol.parameters, expression });
        return;
    }
}

void parse_symbol_declaration(SystemDeclarations& system, std::vector<Token> tokens) 
{
    Symbol symbol = tokens[0].symbol.value();

    if (tokens[2].type == TokenType::LIST)
    {
       system.state_lists.emplace(symbol.name, tokens[2].list_size);
       return;
    }

    parse_expression_declaration(system, tokens);
    return;
}

void parse_declaration(SystemDeclarations& system, std::string line)
{
    std::vector<Token> tokens = tokenize(line);

    if (tokens.size() == 0) return;
    
    switch (tokens[0].type)
    {
        case TokenType::DERIVATIVE:
            parse_state_definition(system, tokens);
        break;
        case TokenType::SYMBOL:
            parse_symbol_declaration(system, tokens);
            break;
        case TokenType::INITIAL:
            parse_initial_value(system, tokens);
            break;
        break;
    }
}

void read_system(SystemDeclarations& system, std::ifstream& stream)
{
    std::string line;

    while (getline(stream, line))
    {
        parse_declaration(system, line);
    }
}
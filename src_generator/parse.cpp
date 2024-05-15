#include "parse.h"

std::vector<ListIndex> parse_indices(std::vector<Token> tokens);

Symbol parse_symbol(std::vector<Token>& tokens)
{
    Symbol symbol = tokens[0].symbol.value();
    std::vector<ListIndex> indices;

    if (tokens[1].type == TokenType::LPAREN)
    {
        auto end_it = std::find_if(tokens.begin() + 1, tokens.end(), [](Token token){ return token.type == TokenType::RPAREN; });
        if (end_it == tokens.end()) return symbol;

        std::vector<Token> indices_tokens = std::vector<Token>(tokens.begin() + 2, end_it);

        std::vector<ListIndex> indices = parse_indices(indices_tokens);
        symbol.indices = indices;

        tokens.erase(tokens.begin(), end_it + 1);
        return symbol;
    }

    tokens.erase(tokens.begin());
    return symbol;
}

std::shared_ptr<Expression> parse_parentheses(std::vector<Token>& tokens)
{
    size_t paren_level = 0;
    auto it = tokens.begin();
    for (; it != tokens.end(); ++it)
    {
        if (paren_level == 0 && it->type == TokenType::RPAREN) break;
        if (it->type == TokenType::LPAREN) paren_level += 1;
        if (it->type == TokenType::RPAREN) paren_level -= 1;
    }

    auto subexprTokens = std::vector<Token>(tokens.begin() + 1, it - 1);
    tokens.erase(tokens.begin(), it);
    return parse_expression(subexprTokens);
}

enum Priority
{
    PRIORITY_ADD = 1,
    PRIORITY_MUL = 2,
    PRIORITY_EXP = 3
};

std::vector<Token> pop_subexpr_tokens(std::vector<Token>& tokens, Priority priority)
{
    // 3 = stop on exp
    // 2 = stop on mul/div
    // 1 = stop on add/sub
    // 0 = stop on nothing 

    size_t paren_level = 0;
    auto it = tokens.begin();
    for (; it != tokens.end(); ++it) 
    {
        if (it->type == TokenType::LPAREN) paren_level += 1;
        if (it->type == TokenType::RPAREN) paren_level -= 1;
        if (paren_level > 0) continue;

        if (priority >= Priority::PRIORITY_ADD && (it->type == TokenType::ADD || it->type == TokenType::SUBTRACT)) 
            break;

        if (priority >= Priority::PRIORITY_MUL && (it->type == TokenType::MULTIPLY || it->type == TokenType::DIVIDE)) 
            break;

        if (priority >= Priority::PRIORITY_EXP && it->type == TokenType::EXPONENT) 
            break;
    }

    auto subexpr_tokens = std::vector<Token>(tokens.begin(), it);

    for (auto t : subexpr_tokens)
        std::cerr << t.to_string() << " ";
    std::cerr << std::endl;

    tokens.erase(tokens.begin(), it);
    return subexpr_tokens;
}

std::shared_ptr<Expression> parse_unary_expression(std::vector<Token>& tokens)
{
    if (tokens[1].type == TokenType::LPAREN)
    {
        tokens.erase(tokens.begin());
        return parse_parentheses(tokens);
    }

    tokens.erase(tokens.begin());
    auto subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ADD);
    return parse_expression(subexprTokens);
}

std::shared_ptr<Expression> parse_expression(std::vector<Token>& tokens)
{
    std::shared_ptr<Expression> expression = nullptr;
    std::vector<Token> subexprTokens;

    while (tokens.size() > 0)
    {
        switch(tokens[0].type) 
        {
            case TokenType::LPAREN:
                expression = parse_parentheses(tokens);
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
                if (expression) 
                {
                    tokens.erase(tokens.begin());
                    subexprTokens = pop_subexpr_tokens(tokens, Priority::PRIORITY_ADD);
                    expression = std::make_shared<SubtractExpression>(expression, parse_expression(subexprTokens));
                } else
                {
                    expression = std::make_shared<NegateExpression>(parse_unary_expression(tokens));
                }
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
            default:
                return nullptr;
        }
    }

    return expression;
}

std::vector<ListIndex> parse_indices(std::vector<Token> tokens)
{
    std::vector<ListIndex> indices;

    while (1)
    {
        auto it = std::find_if(tokens.begin(), tokens.end(), [](Token token){ return token.type == TokenType::COMMA; });

        std::vector<Token> index_tokens = std::vector<Token>(tokens.begin(), it);

        ListIndex index;
        if (index_tokens.size() == 1 && index_tokens[0].type == TokenType::SYMBOL)
        {
            index.type = IndexType::VARIABLE;
            index.list_symbol = index_tokens[0].symbol.value().symbol;
            indices.push_back(index);
        }
        else
        {
            index.type = IndexType::EXPRESSION;
            index.expression = parse_expression(index_tokens);
            indices.push_back(index);
        }

        if (it == tokens.end()) break;

        tokens.erase(tokens.begin(), it + 1);
    }

    return indices;
}

void parse_state_definition(System& system, std::vector<Token> tokens)
{
    if (!tokens[0].symbol.has_value()) 
    {
        std::cerr << "Error: Derivative is missing symbol.\n";
        return;
    }

    Symbol symbol = tokens[0].symbol.value();
    tokens.erase(tokens.begin(), tokens.begin() + 2);

    std::shared_ptr<Expression> rhs = parse_expression(tokens);
    if (!rhs)
    {
        std::cerr << "Error: Malformed rhs expression for derivative of " << symbol.symbol << "\n";
    }

    system.state_variables.push_back(StateVariable(symbol, std::move(rhs)));
}

void parse_initial_value(System& system, std::vector<Token> tokens)
{
    tokens.erase(tokens.begin()); // Remove "INITIAL"
    Symbol symbol = parse_symbol(tokens); // Grab and remove the symbol
    tokens.erase(tokens.begin()); // Remove "="

    std::shared_ptr<Expression> expression = parse_expression(tokens);
    if (!expression)
    {
        std::cerr << "Error: Malformed expression for initial value of " << symbol.symbol << "\n";
        return;
    }
    system.initial_states.push_back(InitialState { symbol, parse_expression(tokens) });
}

void parse_expression_declaration(System& system, std::vector<Token> tokens)
{
    Symbol symbol = parse_symbol(tokens);
    tokens.erase(tokens.begin());
    std::shared_ptr<Expression> expression = parse_expression(tokens);
    if (!expression)
    {
        std::cerr << "Malformed expression: " << symbol.symbol << "\n";
        return;
    }
    system.expression_definitions[symbol.symbol] = parse_expression(tokens);
}

void parse_symbol_declaration(System& system, std::vector<Token> tokens) 
{
    Symbol symbol = tokens[0].symbol.value();

    if (tokens[2].type == TokenType::LIST)
    {
       system.state_lists.emplace(symbol.symbol, tokens[2].list_size);
       return;
    }
        
    parse_expression_declaration(system, tokens);
    return;
}

void parse_declaration(System& system, std::string line)
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

void read_system(System& system, std::ifstream& stream)
{
    std::string line;

    while (getline(stream, line))
    {
        parse_declaration(system, line);
    }
}
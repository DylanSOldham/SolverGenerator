#include "parse.h"

std::vector<ListIndex> parse_indices(std::vector<Token> tokens);

std::shared_ptr<Expression> parse_expression(std::vector<Token> tokens)
{
    std::shared_ptr<Expression> expression = nullptr;
    std::vector<Token> subexprTokens;

    while (tokens.size() > 0)
    {
        switch(tokens[0].type) 
        {
            case TokenType::SYMBOL:
                if (tokens[1].type == TokenType::LPAREN)
                {
                    auto end_it = std::find_if(tokens.begin() + 1, tokens.end(), [](Token token){ return token.type == TokenType::RPAREN; });

                    if (end_it == tokens.end()) 
                    {
                        std::cerr << "Missing matching right parentheses.\n";
                        return nullptr;
                    }

                    std::vector<Token> indices_tokens = std::vector<Token>(tokens.begin() + 2, end_it);

                    std::vector<ListIndex> indices = parse_indices(indices_tokens);

                    expression = std::make_shared<SymbolExpression>(Symbol(tokens[0].symbol.value().symbol, indices));
                    tokens.erase(tokens.begin(), end_it + 1);
                    continue;
                }
                expression = std::make_shared<SymbolExpression>(tokens[0].symbol.value());
                tokens.erase(tokens.begin());
                continue;
            case TokenType::CONSTANT:
                expression = std::make_shared<ConstantExpression>(tokens[0].value.value());
                tokens.erase(tokens.begin());
                continue;
            case TokenType::ADD:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                return std::make_shared<AddExpression>(expression, parse_expression(subexprTokens));
            case TokenType::SUBTRACT:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());   
                if (expression) 
                {
                    return std::make_shared<SubtractExpression>(expression, parse_expression(subexprTokens));
                }
                {
                    auto it = tokens.begin() + 1;
                    size_t paren_level = 0;

                    // Explanation: https://www.youtube.com/watch?v=CoEPkF2ti8M
                    for (; it != tokens.end(); ++it) 
                    {
                        if (it->type == TokenType::LPAREN) paren_level += 1;
                        if (it->type == TokenType::RPAREN) paren_level -= 1;
                        if (paren_level == 0 && (it->type == TokenType::ADD || it->type == TokenType::SUBTRACT)) 
                            break;
                    }
                    subexprTokens = std::vector<Token>(tokens.begin() + 1, it);
                    expression = std::make_shared<NegateExpression>(parse_expression(subexprTokens));
                    tokens.erase(tokens.begin(), it);
                }
                continue;
            case TokenType::MULTIPLY:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                return std::make_unique<MultiplyExpression>(std::move(expression), parse_expression(subexprTokens));
            case TokenType::DIVIDE:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                return std::make_unique<DivideExpression>(std::move(expression), parse_expression(subexprTokens));
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
    system.state_variables.push_back(StateVariable(symbol, std::move(rhs)));
}

void parse_initial_value(System& system, std::vector<Token> tokens)
{
    auto end_it = tokens.begin();

    std::optional<Symbol> underlying_symbol = tokens[0].symbol.value().uninitial();
    if (!underlying_symbol)
    {
        std::cerr << "Error: Tried to use non-initial variable to initialize variable." << std::endl;
        return;
    }
    
    // Parse indices if they are present
    if (tokens[1].type == TokenType::LPAREN)
    {
        end_it = std::find_if(tokens.begin() + 1, tokens.end(), [](Token token){ return token.type == TokenType::RPAREN; });
        if (end_it == tokens.end()) return;

        std::vector<Token> indices_tokens = std::vector<Token>(tokens.begin() + 2, end_it);

        std::vector<ListIndex> indices = parse_indices(indices_tokens);
        underlying_symbol.value().indices = indices;
    }

    tokens.erase(tokens.begin(), end_it + 2);
    system.initial_states.push_back(InitialState { underlying_symbol.value(), parse_expression(tokens) });
}

void parse_symbol_declaration(System& system, std::vector<Token> tokens) 
{
    Symbol symbol = tokens[0].symbol.value();

    if (symbol.is_initial()) 
    {
        return parse_initial_value(system, tokens);
    }

    if (tokens[2].type == TokenType::LIST)
    {
        system.state_lists.emplace(symbol.symbol, tokens[2].list_size);
    }
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
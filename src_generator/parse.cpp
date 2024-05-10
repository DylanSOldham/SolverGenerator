#include "parse.h"

std::unique_ptr<Expression> parse_expression(std::vector<Token> tokens)
{
    std::unique_ptr<Expression> expression = nullptr;
    std::vector<Token> subexprTokens;

    while (tokens.size() > 0)
    {
        switch(tokens[0].type) 
        {
            case TokenType::SYMBOL:
                expression = std::make_unique<SymbolExpression>(tokens[0].symbol.value());
                tokens.erase(tokens.begin());
                continue;
            case TokenType::CONSTANT:
                expression = std::make_unique<ConstantExpression>(tokens[0].value.value());
                tokens.erase(tokens.begin());
                continue;
            case TokenType::ADD:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());
                return std::make_unique<AddExpression>(std::move(expression), parse_expression(subexprTokens));
            case TokenType::SUBTRACT:
                subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());   
                if (expression) 
                {
                    return std::make_unique<SubtractExpression>(std::move(expression), parse_expression(subexprTokens));
                }
                {
                    auto it = tokens.begin() + 1;
                    for (; it != tokens.end(); ++it) 
                    {
                        if (it->type == TokenType::ADD || it->type == TokenType::SUBTRACT) 
                            break;
                    }
                    subexprTokens = std::vector<Token>(tokens.begin() + 1, it);
                    expression = std::make_unique<NegateExpression>(parse_expression(subexprTokens));
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

void parse_state_definition(System& system, std::vector<Token> tokens)
{
    if (!tokens[0].symbol.has_value()) std::cerr << "Error: Derivative is missing symbol.\n";

    Symbol symbol = tokens[0].symbol.value();
    tokens.erase(tokens.begin(), tokens.begin() + 2);

    std::unique_ptr<Expression> rhs = parse_expression(tokens);
    system.state_variables.push_back(StateVariable(symbol, std::move(rhs)));
}

void parse_initial_value(System& system, std::vector<Token> tokens)
{
    std::optional<Symbol> underlying_symbol = tokens[0].symbol.value().uninitial();
    if (!underlying_symbol)
    {
        std::cerr << "Error: Tried to use non-initial variable to initialize variable." << std::endl;
        return;
    }

    tokens.erase(tokens.begin(), tokens.begin() + 2);
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
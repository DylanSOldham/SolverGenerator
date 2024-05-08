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
                if (expression) 
                {
                    subexprTokens = std::vector<Token>(tokens.begin() + 1, tokens.end());   
                    return std::make_unique<SubtractExpression>(std::move(expression), parse_expression(subexprTokens));
                }
                for (auto it = tokens.begin() + 1; it != tokens.end(); ++it) 
                {
                    if (it->type == TokenType::ADD || it->type == TokenType::SUBTRACT) 
                    {
                        subexprTokens = std::vector<Token>(tokens.begin() + 1, it);
                        tokens.erase(tokens.begin(), it);
                        break;
                    }
                }
                expression = std::make_unique<NegateExpression>(parse_expression(subexprTokens));
                continue;
            default:
                continue;
        }
    }

    return expression;
}

void parse_dependent_definition(System& system, std::vector<Token> tokens)
{
    if (!tokens[0].symbol.has_value()) std::cerr << "Error: Derivative is missing symbol.\n";

    Symbol symbol = tokens[0].symbol.value();
    tokens.erase(tokens.begin(), tokens.begin() + 2);

    std::unique_ptr<Expression> expr = parse_expression(tokens);

    system.dependent_variables.push_back(DependentVariable(symbol, std::move(expr)));
}

void parse_initial_value(System& system, std::vector<Token> tokens)
{
    for (size_t i = 0; i < system.dependent_variables.size(); ++i)
    {
        if (system.dependent_variables[i].symbol == tokens[0].symbol.value()) 
        {
            system.dependent_variables[i].initial_value = tokens[2].value.value();
        }
    }
}

void parse_declaration(System& system, std::string line)
{
    std::vector<Token> tokens = tokenize(line);

    if (tokens.size() == 0) return;
    
    switch (tokens[0].type)
    {
        case TokenType::DERIVATIVE:
            parse_dependent_definition(system, tokens);
        break;
        case TokenType::SYMBOL:
            parse_initial_value(system, tokens);
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
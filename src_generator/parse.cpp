#include "parse.h"

std::unique_ptr<Expression> parse_expression(System& system, std::vector<Token> tokens)
{
    switch (tokens[0].type) {
        case TokenType::CONSTANT:
            break;
        case TokenType::SYMBOL:
            if (tokens.size() > 1) { 
                auto symexpr = std::make_unique<SymbolExpression>(tokens[0].symbol.value());

                switch (tokens[1].type) {
                    case TokenType::ADD:
                        tokens.erase(tokens.begin(), tokens.begin() + 2);
                        return std::make_unique<AddExpression>(
                            std::move(symexpr),
                            parse_expression(system, tokens)
                        );
                        break;
                    default:
                    break;
                }
            }
            return std::make_unique<SymbolExpression>(tokens[0].symbol.value());
        break;
        case TokenType::SUBTRACT:
            tokens.erase(tokens.begin());
            return std::make_unique<NegateExpression>(parse_expression(system, tokens));
        break;
        default:
        break;
    }

    return nullptr;
}

void parse_dependent_definition(System& system, std::vector<Token> tokens)
{
    if (!tokens[0].symbol.has_value()) std::cerr << "Error: Derivative is missing symbol.\n";

    Symbol symbol = tokens[0].symbol.value();
    tokens.erase(tokens.begin(), tokens.begin() + 2);

    std::unique_ptr<Expression> expr = parse_expression(system, tokens);

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
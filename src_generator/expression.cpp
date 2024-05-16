#include "expression.h"
#include "parse.h"

std::string SymbolExpression::generate(System& system)
{
    std::stringstream str;

    SymbolType type = system.resolve_symbol_type(symbol);
    if (type == SymbolType::STATE) {
        if (symbol.is_list()) {
            auto& list_index = symbol.indices[0];
            switch(list_index.type)
            {
                case IndexType::VARIABLE:
                    {
                    if (system.list_bindings.count(list_index.list_symbol.value()))
                    {
                        size_t i = system.list_bindings[list_index.list_symbol.value()];
                        str << "values[INDEX_" << symbol.to_string() << "_START + " << i << "]";
                    }
                    else
                    {
                        std::cerr << "Error: Tried to use unbound index variable in definition.\n";
                        return "";
                    }
                    }
                break;
                case IndexType::EXPRESSION:
                    {
                    str << "values[INDEX_" << symbol.to_string() << "_START + (size_t)" << list_index.expression->generate(system) << "]";
                    }
                break;
                default:
                break;
            }
        }
        else 
        {
            str << "values[INDEX_" << symbol.to_string() << "]";
        }  

        return str.str();
    }

    if (type == SymbolType::LIST_INDEX)
    {
        if (!system.list_bindings.count(symbol.to_string()))
        {
            std::cerr << "Error: Used an unbound list index." << std::endl;
        }

        return symbol.to_string();
    }

    if (type == SymbolType::COMPOUND)
    {
        auto expr = system.expression_definitions[symbol.symbol];
        if (!expr)
        {
            std::cerr << "Error: Expression " << symbol.symbol << " is undefined.\n";
        }
        return symbol.symbol + "()";
    }

    std::cerr << "Use of undefined symbol " << symbol.to_string() << ". Make sure this is intentional.\n";

    return symbol.to_string();
}

bool SymbolExpression::has_state_dependencies(System& system)
{
    // Probably better as a map, having this ordered is nice for generation though
    for (StateVariable& dep : system.state_variables)
    {
        if (dep.symbol == symbol) return true;
    }

    return false;
}

std::string AddExpression::generate(System& system)
{
    std::stringstream code;
    code << "((" << lhs->generate(system) << ") + (" << rhs->generate(system) << "))";
    return code.str();
}

std::string SubtractExpression::generate(System& system)
{
    std::stringstream code;
    code << "((" << lhs->generate(system) << ") - (" << rhs->generate(system) << "))";
    return code.str();
}

std::string MultiplyExpression::generate(System& system)
{
    std::stringstream code;
    code << "((" << lhs->generate(system) << ") * (" << rhs->generate(system) << "))";
    return code.str();
}

std::string DivideExpression::generate(System& system)
{
    std::stringstream code;
    code << "((" << lhs->generate(system) << ") / (" << rhs->generate(system) << "))";
    return code.str();
}

std::string ExponentExpression::generate(System& system)
{
    if (!base) {
        std::cerr << "Error: Exponent expression is missing base." << std::endl;
        return "";
    }

    if (!exp) {
        std::cerr << "Error: Exponent expression is missing exponent." << std::endl;
        return "";
    }

    std::stringstream code;
    code << "std::pow(" << base->generate(system) << ", " << exp->generate(system) << ")";
    return code.str();
}

std::string SqrtExpression::generate(System& system)
{
    std::stringstream code;
    code << "std::sqrt(" << base->generate(system) << ")";
    return code.str();
}
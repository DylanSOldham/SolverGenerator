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
                case IndexType::NUMBER:
                    {
                    size_t i = list_index.index_start;
                    str << "values[INDEX_" << symbol.to_string() << "_START + " << i << "]";
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

    std::cerr << "Use of undefined symbol " << symbol.to_string() << ". Make sure this is intentional.\n";

    return symbol.to_string();
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
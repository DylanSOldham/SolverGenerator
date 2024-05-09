#include "expression.h"
#include "parse.h"

std::string SymbolExpression::generate(System& system)
{
    std::stringstream str;

    SymbolType type = system.resolve_symbol_type(symbol);
    if (type == SymbolType::STATE) {
        if (symbol.is_list()) {
            size_t list_index = system.list_bindings[symbol.indices[0].symbol];
            str << "values[INDEX_" << symbol.to_string() << "_" << list_index << "]";
        }
        else 
        {
            str << "values[INDEX_" << symbol.to_string() << "]";
        }  

        return str.str();
    }

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
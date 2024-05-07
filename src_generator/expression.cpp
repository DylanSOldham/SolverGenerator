#include "expression.h"
#include "parse.h"

std::string SymbolExpression::generate(System& system)
{
    SymbolType type = system.resolve_symbol_type(symbol);
    if (type == SymbolType::DEPENDENT) {
        return "values[INDEX_" + symbol.to_string() + "]";
    }

    return symbol.to_string();
}

std::string AddExpression::generate(System& system)
{
    std::stringstream code;
    code << "((" << lhs->generate(system) << ") + (" << rhs->generate(system) << "))";
    return code.str();
}
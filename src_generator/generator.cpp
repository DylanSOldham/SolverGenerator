#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "expression.h"
#include "parse.h"

std::string generate_index_list(System &system, Symbol state_symbol)
{
    if (state_symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Skipping index generation for a non variable list index." << std::endl;
        return "";
    }

    auto &list_symbol = state_symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate indices for a list which doesn't exist.\n";
        return "";
    }

    std::stringstream str;

    size_t list_size = system.state_lists[list_symbol];
    str << "#define INDEX_" << state_symbol.to_string() << "_START " << system.max_index << "\n";
    str << "#define INDEX_" << state_symbol.to_string() << "_SIZE " << list_size << "\n";

    system.max_index += list_size;

    return str.str();
}

std::string generate_setter_list(System &system, InitialState &initial_state)
{
    if (initial_state.symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Error: Trying to generate indices for a non variable list index." << std::endl;
        return "";
    }

    auto &list_symbol = initial_state.symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate setters for a list which doesn't exist.\n";
        return "";
    }

    size_t list_size = system.state_lists[list_symbol];
    system.list_bindings.clear();
    system.list_bindings[list_symbol] = list_size;

    std::stringstream str;

    str << "\n    for (size_t " << list_symbol << " = 1; "
        << list_symbol << " <= " << list_size << "; "
        << "++" << list_symbol << ")\n"
        << "    {\n"

        << "        values[INDEX_" << initial_state.symbol.to_string() << "_START + (" << list_symbol << " - 1)] = "
        << initial_state.rhs->generate(system) << ";\n"

        << "    }\n";

    return str.str();
}

std::string generate_derivative_list(System &system, StateVariable &state_variable)
{
    if (state_variable.symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Trying to generate indices for a non variable list index." << std::endl;
        return "";
    }

    auto &list_symbol = state_variable.symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate derivatives for a list which doesn't exist.\n";
        return "";
    }

    size_t list_size = system.state_lists[list_symbol];
    system.list_bindings.clear();
    system.list_bindings[list_symbol] = list_size;

    std::stringstream str;

    str << "\n    for (size_t " << list_symbol << " = 1; "
        << list_symbol << " <= " << list_size << "; "
        << "++" << list_symbol << ")\n"
        << "    {\n"

        << "        derivatives[INDEX_" << state_variable.symbol.to_string() << "_START + (" << list_symbol << " - 1)] = "
        << state_variable.rhs->generate(system) << ";\n"

        << "    }\n";

    return str.str();
}

std::string generate_csv_list(System &system, Symbol state_symbol)
{
    if (state_symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Skipping index generation for a non variable list index." << std::endl;
        return "";
    }

    auto &list_symbol = state_symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate csv labels for a list which doesn't exist.\n";
        return "";
    }

    std::stringstream str;

    size_t list_size = system.state_lists[list_symbol];
    for (size_t i = 0; i < list_size; ++i)
    {
        str << ", " << state_symbol.to_string() << "(" << i + 1 << ")";
    }

    return str.str();
}

std::string generate_expression_functions(System &system)
{
    std::stringstream str;

    auto &bound_expressions = system.expression_definitions;

    for (auto &p : bound_expressions)
    {
        str << "\ndouble " << p.first << "();";
    }

    for (auto &p : bound_expressions)
    {
        auto symbol = p.first;
        std::shared_ptr<Expression> expression = p.second;

        if (!expression)
        {
            std::cerr << "Error: " << symbol << " is missing definition.\n";
            continue;
        }

        str << "\n\ndouble " << symbol << "()\n"
            << "{\n"
            << "    return " << expression->generate(system) << ";\n"
            << "}";
    }

    return str.str();
}

std::string generate_state_indices(System &system)
{
    std::stringstream str;
    auto &state_variables = system.state_variables;

    for (size_t i = 0; i < state_variables.size(); ++i)
    {
        if (state_variables[i].symbol.is_list())
        {
            str << generate_index_list(system, state_variables[i].symbol);
        }
        else
        {
            str << "#define INDEX_" << state_variables[i].symbol.to_string() << " " << system.max_index++ << "\n";
        }
    }
    str << "\n";

    return str.str();
}

std::string generate_initial_state_setter(System &system)
{
    auto &initial_states = system.initial_states;

    std::stringstream str;

    str << "\n\nvoid get_initial_state(N_Vector state) {"
        << "\n    double* values = N_VGetArrayPointer(state);\n";
    for (size_t i = 0; i < initial_states.size(); ++i)
    {
        if (initial_states[i].symbol.is_list())
        {
            if (initial_states[i].symbol.indices[0].type == IndexType::EXPRESSION)
                continue;

            str << generate_setter_list(system, initial_states[i]);
        }
        else
        {
            if (!initial_states[i].rhs)
            {
                std::cerr << "Error: Initial state of " << initial_states[i].symbol.symbol << " is missing definition.\n";
                continue;
            }

            system.list_bindings.clear();
            str << "    values[INDEX_" << initial_states[i].symbol.to_string() << "] = " << initial_states[i].rhs->generate(system) << ";\n";
        }
    }

    // Second pass to define manual overrides to the list
    for (size_t i = 0; i < initial_states.size(); ++i)
    {
        if (initial_states[i].symbol.is_list() && initial_states[i].symbol.indices[0].type == IndexType::EXPRESSION)
        {
            system.list_bindings.clear();
            str << "    values[INDEX_" << initial_states[i].symbol.to_string() << "_START + (size_t)"
                << "(" << initial_states[i].symbol.indices[0].expression->generate(system) << " - 1)" << "] = "
                << initial_states[i].rhs->generate(system) << ";\n";
        }
    }

    str << "}\n\n";

    return str.str();
}

std::string generate_derivative_definitions(System &system)
{
    auto &deps = system.state_variables;

    std::stringstream str;

    for (size_t i = 0; i < deps.size(); ++i)
    {
        if (deps[i].symbol.is_list())
        {
            if (deps[i].symbol.indices[0].type == IndexType::EXPRESSION)
                continue;

            str << generate_derivative_list(system, deps[i]);
        }
        else
        {
            system.list_bindings.clear();
            str << "    derivatives[INDEX_" << deps[i].symbol.to_string() << "] = " << deps[i].rhs->generate(system) << ";\n";
        }
    }
    str << "\n";

    // Second pass to define manual overrides to the list
    for (size_t i = 0; i < deps.size(); ++i)
    {
        if (deps[i].symbol.is_list() && deps[i].symbol.indices[0].type == IndexType::EXPRESSION)
        {
            system.list_bindings.clear();
            str << "    derivatives[INDEX_" << deps[i].symbol.to_string() << "_START + (size_t)" << deps[i].symbol.indices[0].index_start - 1
                << "] = " << deps[i].rhs->generate(system) << ";\n";
        }
    }

    return str.str();
}

std::string generate_state_csv_label_getter(System &system)
{
    std::stringstream str;

    auto &deps = system.state_variables;

    str << "std::string get_state_csv_label() {\n";

    str << "    return \"t (seconds)";
    for (size_t i = 0; i < deps.size(); ++i)
    {
        if (deps[i].symbol.is_list())
        {
            str << generate_csv_list(system, deps[i].symbol);
        }
        else
        {
            str << ", " << deps[i].symbol.to_string();
        }
    }
    str << "\\n\";\n";

    str << "}\n\n";

    return str.str();
}

std::string generate_system(System &system)
{
    std::stringstream str;

    auto &deps = system.state_variables;

    str << "int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {\n"
        << "    double* values = N_VGetArrayPointer(y);\n"
        << "    double* derivatives = N_VGetArrayPointer(ydot);\n"
        << generate_derivative_definitions(system)
        << "    return 0;\n"
        << "}";

    return str.str();
}

int main(int argc, char **argv)
{
    std::string filename = (argc < 2) ? "system.txt" : argv[1];
    std::ifstream system_src_file(filename, std::ios::in);
    System system;
    read_system(system, system_src_file);
    system_src_file.close();

    std::ofstream outmodule("generated/system.h", std::ios::out);
    outmodule << "#include <cmath>\n"
              << "#include <nvector/nvector_serial.h>\n\n"
              << generate_state_indices(system)
              << "#define NUM_DEPS " << system.max_index << "\n"
              << generate_expression_functions(system)
              << generate_initial_state_setter(system)
              << generate_state_csv_label_getter(system)
              << generate_system(system);

    return 0;
}
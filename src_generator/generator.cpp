#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "expression.h"
#include "parse.h"

std::string generate_index_list(System& system, Symbol state_symbol)
{
    if (state_symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Skipping index generation for a non variable list index." << std::endl; 
        return "";
    }

    auto& list_symbol = state_symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate indices for a list which doesn't exist.\n";
        return "";
    }
        
    std::stringstream str;

    size_t list_size = system.state_lists[list_symbol];
    for (size_t i = 0; i < list_size; ++i) 
    {
        str << "#define INDEX_" << state_symbol.to_string() << "_" << i + 1 << " " << system.max_index++ << "\n";
    }

    return str.str();
}

std::string generate_setter_list(System& system, InitialState& initial_state)
{
    if (initial_state.symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Trying to generate indices for a non variable list index." << std::endl; 
        return "";
    }

    auto& list_symbol = initial_state.symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate setters for a list which doesn't exist.\n";
        return "";
    }

    std::stringstream str;

    size_t list_size = system.state_lists[list_symbol];
    for (size_t i = 0; i < list_size; ++i) 
    {
        system.list_bindings.clear();
        system.list_bindings[list_symbol] = i + 1;
        
        str << "    values[INDEX_" << initial_state.symbol.to_string() << "_" << i + 1 << "] = " << initial_state.rhs->generate(system) << ";\n";
    }

    return str.str();
}

std::string generate_derivative_list(System& system, StateVariable& state_variable)
{
    if (state_variable.symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Trying to generate indices for a non variable list index." << std::endl; 
        return "";
    }

    auto& list_symbol = state_variable.symbol.indices[0].list_symbol.value();
    if (!system.state_lists.count(list_symbol))
    {
        std::cerr << "Error: Tried to generate derivatives for a list which doesn't exist.\n";
        return "";
    }

    std::stringstream str;

    size_t list_size = system.state_lists[list_symbol];
    for (size_t i = 0; i < list_size; ++i) 
    {
        system.list_bindings.clear();
        system.list_bindings[list_symbol] = i + 1;

        str << "    derivatives[INDEX_" << state_variable.symbol.to_string() << "_" << i + 1 << "] = " << state_variable.rhs->generate(system) << ";\n";
    }

    return str.str();
}

std::string generate_csv_list(System& system, Symbol state_symbol)
{
    if (state_symbol.indices[0].type != IndexType::VARIABLE)
    {
        std::cerr << "Warning: Skipping index generation for a non variable list index." << std::endl; 
        return "";
    }

    auto& list_symbol = state_symbol.indices[0].list_symbol.value();
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

std::string generate_state_indices(System& system)
{
    std::stringstream str;
    auto& state_variables = system.state_variables;

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

std::string generate_initial_state_setter(System& system) 
{
    auto& initial_states = system.initial_states;

    std::stringstream str;

    str << "void get_initial_state(N_Vector state) {\n"
        << "    double* values = N_VGetArrayPointer(state);\n";
    for (size_t i = 0; i < initial_states.size(); ++i)
    {
        if (initial_states[i].symbol.is_list())
        {
            if (initial_states[i].symbol.indices[0].type == IndexType::NUMBER)
                continue;

            str << generate_setter_list(system, initial_states[i]);
        }
        else
        {
            system.list_bindings.clear();
            str << "    values[INDEX_" << initial_states[i].symbol.to_string() << "] = " << initial_states[i].rhs->generate(system) << ";\n";
        }
    }

    str << "\n";

    // Second pass to define manual overrides to the list
    for (size_t i = 0; i < initial_states.size(); ++i)
    {
        if (initial_states[i].symbol.is_list() && initial_states[i].symbol.indices[0].type == IndexType::NUMBER)
        {
            system.list_bindings.clear();
            str << "    values[INDEX_" << initial_states[i].symbol.to_string() << "_" << initial_states[i].symbol.indices[0].index_start 
                << "] = " << initial_states[i].rhs->generate(system) << ";\n";
        }
    }

    str << "}\n\n";

    return str.str();
}

std::string generate_derivative_definitions(System& system)
{
    auto& deps = system.state_variables;

    std::stringstream str;

    for (size_t i = 0; i < deps.size(); ++i)
    {
        if (deps[i].symbol.is_list())
        {
            if (deps[i].symbol.indices[0].type == IndexType::NUMBER)
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
        if (deps[i].symbol.is_list() && deps[i].symbol.indices[0].type == IndexType::NUMBER)
        {
            system.list_bindings.clear();
            str << "    derivatives[INDEX_" << deps[i].symbol.to_string() << "_" << deps[i].symbol.indices[0].index_start 
                << "] = " << deps[i].rhs->generate(system) << ";\n";
        }
    }

    return str.str();
}

std::string generate_state_csv_label_getter(System& system)
{
    std::stringstream str;

    auto& deps = system.state_variables;

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

std::string generate_system(System& system)
{
    std::stringstream str;

    auto& deps = system.state_variables;

    str << "int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {\n"
        << "    double* values = N_VGetArrayPointer(y);\n"
        << "    double* derivatives = N_VGetArrayPointer(ydot);\n\n"
        << generate_derivative_definitions(system)
        << "    return 0;\n"
        << "}";

    return str.str();
}

int main()
{
  std::ifstream system_src_file("system.txt", std::ios::in);
  System system;
  read_system(system, system_src_file);
  system_src_file.close();

  std::ofstream outmodule("generated/system.h", std::ios::out);
  outmodule << "#include <nvector/nvector_serial.h>\n\n"
            << generate_state_indices(system)
            << "#define NUM_DEPS " << system.max_index << "\n\n"
            << generate_initial_state_setter(system)
            << generate_state_csv_label_getter(system)
            << generate_system(system);

  return 0;
}
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

#include "expression.h"
#include "parse.h"

std::string generate_state_indices(std::vector<DependentVariable>& deps)
{
    std::stringstream str;

    for (size_t i = 0; i < deps.size(); ++i)
    {
        str << "#define INDEX_" << deps[i].symbol.to_string() << " " << i << "\n";
    }
    str << "\n";

    return str.str();
}

std::string generate_initial_state_setter(std::vector<DependentVariable>& deps) 
{
    std::stringstream str;

    str << "void set_initial_state(N_Vector state) {\n"
        << "    double* values = N_VGetArrayPointer(state);\n";
    for (size_t i = 0; i < deps.size(); ++i)
    {
        str << "    values[INDEX_" << deps[i].symbol.to_string() << "] = " << deps[i].initial_value << ";\n";
    }
    str << "}\n\n";

    return str.str();
}

std::string generate_derivative_definitions(System& system)
{
    auto& deps = system.dependent_variables;

    std::stringstream str;

    for (size_t i = 0; i < deps.size(); ++i)
    {
        str << "    derivatives[INDEX_" << deps[i].symbol.to_string() << "] = " << deps[i].rhs->generate(system) << ";\n";
    }
    str << "\n";

    return str.str();
}

std::string generate_state_csv_label_getter(System& system)
{
    std::stringstream str;

    auto& deps = system.dependent_variables;

    str << "std::string get_state_csv_label() {\n";

    str << "    return \"t (seconds)";
    for (size_t i = 0; i < deps.size(); ++i)
    {
        str << ", " << deps[i].symbol.to_string();
    }
    str << "\\n\";\n";

    str << "}\n\n";

    return str.str();
}

int main()
{
  std::ifstream system_src_file("system.txt", std::ios::in);
  System system;
  parse_system(system, system_src_file);
  system_src_file.close();

  for (size_t i = 0; i < system.dependent_variables.size(); ++i)
  {
      std::cout << "Got dependent variable " << system.dependent_variables[i].symbol.to_string() 
                << " with initial value " << system.dependent_variables[i].initial_value
                << " with definition " << system.dependent_variables[i].rhs->generate(system)
                << "\n";
  }

  std::ofstream outmodule("modules/system.h", std::ios::out);
  outmodule << "#define NUM_DEPS " << system.dependent_variables.size() << "\n\n"
            << generate_state_indices(system.dependent_variables)
            << generate_initial_state_setter(system.dependent_variables)
            << generate_state_csv_label_getter(system)
            << "int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {\n"
            << "    double* values = N_VGetArrayPointer(y);\n"
            << "    double* derivatives = N_VGetArrayPointer(ydot);\n\n"
            << generate_derivative_definitions(system)
            << "    return 0;\n"
            << "}";

  return 0;
}
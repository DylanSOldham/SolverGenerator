#include <cmath>
#include <nvector/nvector_serial.h>

#define INDEX_C_START 0
#define INDEX_C_SIZE 2

#define NUM_DEPS 2


double lattice_parameter()
{
    return ((1.7) * (std::pow(10, -(23))));
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 2; ++n)
    {
        values[INDEX_C_START + (n - 1)] = n;
    }

    values[INDEX_C_START + (size_t)(1 - 1)] = 1;
}

std::string get_state_csv_label() {
    return "t (seconds), C(1), C(2)\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= 2; ++n)
    {
        derivatives[INDEX_C_START + (n - 1)] = values[INDEX_C_START + (size_t)((n) - (1))];
    }

    derivatives[INDEX_C_START + (size_t)0] = ((1) - (((2) * (lattice_parameter()))));
    return 0;
}
#include <cmath>
#include <nvector/nvector_serial.h>

#define INDEX_C_START 0
#define INDEX_C_SIZE 10
#define INDEX_G 10

#define NUM_DEPS 11

double T();

double T()
{
    return 1;
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 10; ++n)
    {
        values[INDEX_C_START + (n - 1)] = ((T()) + (n));
    }
    values[INDEX_G] = -(5);
}

std::string get_state_csv_label() {
    return "t (seconds), C[1], C[2], C[3], C[4], C[5], C[6], C[7], C[8], C[9], C[10], G\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= 10; ++n)
    {
        derivatives[INDEX_C_START + (n - 1)] = ((-(values[INDEX_G])) + (n));
    }
    derivatives[INDEX_G] = values[INDEX_C_START + (size_t)(1 - 1)];

    derivatives[INDEX_C_START + (size_t)(1 - 1)] = -(values[INDEX_G]);
    return 0;
}
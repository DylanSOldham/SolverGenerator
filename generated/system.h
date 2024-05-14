#include <nvector/nvector_serial.h>

#define INDEX_C_START 0
#define INDEX_C_SIZE 5
#define INDEX_G 5

#define NUM_DEPS 6

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 5; ++n)
    {
        values[INDEX_C_START + (n - 1)] = n;
    }
    values[INDEX_G] = 1.000000;

    values[INDEX_C_START + 1] = 10.000000;
}

std::string get_state_csv_label() {
    return "t (seconds), C(1), C(2), C(3), C(4), C(5), G\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= 5; ++n)
    {
        derivatives[INDEX_C_START + (n - 1)] = ((-(values[INDEX_G])) + (((n) / (2.000000))));
    }
    derivatives[INDEX_G] = values[INDEX_C_START + 4];

    derivatives[INDEX_C_START + 0] = -(((2.000000) * (values[INDEX_G])));
    return 0;
}
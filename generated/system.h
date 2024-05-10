#include <nvector/nvector_serial.h>

#define INDEX_C_0 0
#define INDEX_C_1 1
#define INDEX_C_2 2
#define INDEX_C_3 3
#define INDEX_G 4

#define NUM_DEPS 5

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_C_0] = 0;
    values[INDEX_C_1] = 1;
    values[INDEX_C_2] = 2;
    values[INDEX_C_3] = 3;
    values[INDEX_G] = 10.000000;
}

std::string get_state_csv_label() {
    return "t (seconds), C(1), C(2), C(3), C(4), G\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    derivatives[INDEX_C_0] = -(values[INDEX_G]);
    derivatives[INDEX_C_1] = -(values[INDEX_G]);
    derivatives[INDEX_C_2] = -(values[INDEX_G]);
    derivatives[INDEX_C_3] = -(values[INDEX_G]);
    derivatives[INDEX_G] = values[INDEX_C_3];

    return 0;
}
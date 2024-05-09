#include <nvector/nvector_serial.h>

#define INDEX_C_0 0
#define INDEX_C_1 1
#define INDEX_C_2 2

#define NUM_DEPS 3

void set_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_C_0] = 20;
    values[INDEX_C_1] = 20;
    values[INDEX_C_2] = 20;
}

std::string get_state_csv_label() {
    return "t (seconds), C(1), C(2), C(3)\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    derivatives[INDEX_C_0] = -(values[INDEX_C_0]);
    derivatives[INDEX_C_1] = -(values[INDEX_C_1]);
    derivatives[INDEX_C_2] = -(values[INDEX_C_2]);

    return 0;
}
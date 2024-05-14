#include <nvector/nvector_serial.h>

#define INDEX_C_1 0
#define INDEX_C_2 1
#define INDEX_C_3 2
#define INDEX_C_4 3
#define INDEX_C_5 4
#define INDEX_G 5

#define NUM_DEPS 6

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_C_1] = 0.000000;
    values[INDEX_C_2] = 0.000000;
    values[INDEX_C_3] = 0.000000;
    values[INDEX_C_4] = 0.000000;
    values[INDEX_C_5] = 0.000000;
    values[INDEX_G] = 0.000000;
}

std::string get_state_csv_label() {
    return "t (seconds), C(1), C(2), C(3), C(4), C(5), G\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    derivatives[INDEX_C_1] = ((-(values[INDEX_G])) + (((1) / (2.000000))));
    derivatives[INDEX_C_2] = ((-(values[INDEX_G])) + (((2) / (2.000000))));
    derivatives[INDEX_C_3] = ((-(values[INDEX_G])) + (((3) / (2.000000))));
    derivatives[INDEX_C_4] = ((-(values[INDEX_G])) + (((4) / (2.000000))));
    derivatives[INDEX_C_5] = ((-(values[INDEX_G])) + (((5) / (2.000000))));
    derivatives[INDEX_G] = values[INDEX_C_4];

    return 0;
}
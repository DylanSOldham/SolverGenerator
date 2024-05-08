#include <nvector/nvector_serial.h>

#define NUM_DEPS 3

#define INDEX_C 0
#define INDEX_G 1
#define INDEX_A 2

void set_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_C] = 20;
    values[INDEX_G] = 10;
    values[INDEX_A] = 1;
}

std::string get_state_csv_label() {
    return "t (seconds), C, G, A\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    derivatives[INDEX_C] = values[INDEX_G];
    derivatives[INDEX_G] = ((values[INDEX_C]) - (((values[INDEX_G]) + (values[INDEX_C]))));
    derivatives[INDEX_A] = ((-(values[INDEX_G])) - (((values[INDEX_A]) + (2.000000))));

    return 0;
}
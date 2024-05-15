#include <cmath>
#include <nvector/nvector_serial.h>


#define NUM_DEPS 0



double atomic_volume()
{
    return std::pow(lattice_param(), ((3) / (4)));
}


double burgers_vector()
{
    return ((lattice_param()) / (std::sqrt(2)));
}


double grain_size()
{
    return ((4) * (std::pow(10, -(3))));
}


double i_binding()
{
    return 0.6;
}


double i_dislocation_bias()
{
    return 0.8;
}


double i_dislocation_bias_param()
{
    return 1.1;
}


double i_formation()
{
    return 4.1;
}


double i_loop_bias()
{
    return 63;
}


double i_migration()
{
    return 0.45;
}


double lattice_param()
{
    return lattice_param();
}


double recombination_radius()
{
    return ((0.7) * (std::pow(10, -(7))));
}


double v_binding()
{
    return 0.5;
}


double v_dislocation_bias()
{
    return 0.65;
}


double v_dislocation_bias_param()
{
    return 1;
}


double v_formation()
{
    return 1.7;
}


double v_loop_bias()
{
    return 33;
}


double v_migration()
{
    return 1.35;
}
void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_i_diffusion] = ((1) * (std::pow(10, -(3))));
    values[INDEX_v_diffusion] = 0.6;
    values[INDEX_dislocation_density] = ((10) * (std::pow(10, -(12))));

}

std::string get_state_csv_label() {
    return "t (seconds)\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    return 0;
}
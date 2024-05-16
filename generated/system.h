#include <cmath>
#include <nvector/nvector_serial.h>

#define INDEX_Rho 0

#define NUM_DEPS 1

double atomic_volume();
double burgers_vector();
double dislocation_density_0();
double dislocation_density_evolution();
double dislocation_gain();
double flux();
double grain_size();
double i_bi();
double i_binding();
double i_diffusion_0();
double i_dislocation_bias();
double i_dislocation_bias_param();
double i_formation();
double i_loop_bias();
double i_migration();
double i_quad();
double i_tri();
double lattice_param();
double recombination();
double recombination_radius();
double temperature();
double v_bi();
double v_binding();
double v_diffusion_0();
double v_dislocation_bias();
double v_dislocation_bias_param();
double v_formation();
double v_loop_bias();
double v_migration();
double v_quad();
double v_tri();

double atomic_volume()
{
    return ((std::pow(lattice_param(), 3)) / (4));
}

double burgers_vector()
{
    return ((lattice_param()) / (std::sqrt(2)));
}

double dislocation_density_0()
{
    return ((10) * (std::pow(10, -(12))));
}

double dislocation_density_evolution()
{
    return 300;
}

double dislocation_gain()
{
    return 0;
}

double flux()
{
    return ((2.9) * (std::pow(10, -(7))));
}

double grain_size()
{
    return ((4) * (std::pow(10, -(3))));
}

double i_bi()
{
    return 5;
}

double i_binding()
{
    return 0.6;
}

double i_diffusion_0()
{
    return ((1) * (std::pow(10, -(3))));
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

double i_quad()
{
    return 6;
}

double i_tri()
{
    return 2;
}

double lattice_param()
{
    return ((3.6) * (std::pow(10, -(8))));
}

double recombination()
{
    return 3;
}

double recombination_radius()
{
    return ((0.7) * (std::pow(10, -(7))));
}

double temperature()
{
    return ((330) + (273.15));
}

double v_bi()
{
    return 6;
}

double v_binding()
{
    return 0.5;
}

double v_diffusion_0()
{
    return 0.6;
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

double v_quad()
{
    return 2;
}

double v_tri()
{
    return 3;
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);
    values[INDEX_Rho] = dislocation_density_0();
}

std::string get_state_csv_label() {
    return "t (seconds), Rho\n";
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);
    derivatives[INDEX_Rho] = ((dislocation_gain()) - (((((dislocation_density_evolution()) * (std::pow(burgers_vector(), 2)))) * (std::pow(values[INDEX_Rho], ((3) / (2)))))));

    return 0;
}
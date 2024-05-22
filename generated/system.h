#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>

#define INDEX_Ci_START 0
#define INDEX_Ci_SIZE 50
#define INDEX_Cv_START 50
#define INDEX_Cv_SIZE 50
#define INDEX_Rho 100

#define NUM_DEPS 101

double i_migration = 0.45;
double v_migration = 1.35;
double i_diffusion_0 = ((1) * (std::pow(10, -(3))));
double v_diffusion_0 = 0.6;
double i_formation = 4.1;
double v_formation = 1.7;
double i_binding = 0.6;
double v_binding = 0.5;
double recombination_radius = ((0.7) * (std::pow(10, -(7))));
double i_loop_bias = 63;
double i_dislocation_bias = 0.8;
double i_dislocation_bias_param = 1.1;
double v_loop_bias = 33;
double v_dislocation_bias = 0.65;
double v_dislocation_bias_param = 1;
double dislocation_density_0 = ((10) * (std::pow(10, -(12))));
double grain_size = ((4) * (std::pow(10, -(3))));
double lattice_param = ((3.6) * (std::pow(10, -(8))));
double burgers_vector = ((lattice_param) / (std::sqrt(2)));
double atomic_volume = ((std::pow(lattice_param, 3)) / (4));
double flux = ((2.9) * (std::pow(10, -(7))));
double temperature = ((330) + (273.15));
double recombination = 3;
double i_bi = 5;
double i_tri = 2;
double i_quad = 6;
double v_bi = 6;
double v_tri = 3;
double v_quad = 2;
double dislocation_density_evolution = 300;
double dislocation_gain = 0;
double i_gain(double n);
double v_gain(double n);

double i_gain(double n)
{
	if (n == (1)) {
		 return ((((flux) * (recombination))) * (((((((1) - (i_bi))) - (i_tri))) - (i_quad))));
	}
	if (n == (2)) {
		 return ((((flux) * (recombination))) * (i_bi));
	}
	if (n == (3)) {
		 return ((((flux) * (recombination))) * (i_tri));
	}
	if (n == (4)) {
		 return ((((flux) * (recombination))) * (i_quad));
	}
	return 0;
}

double v_gain(double n)
{
	if (n == (1)) {
		 return ((((flux) * (recombination))) * (((((((1) - (v_bi))) - (v_tri))) - (v_quad))));
	}
	if (n == (2)) {
		 return ((((flux) * (recombination))) * (v_bi));
	}
	if (n == (3)) {
		 return ((((flux) * (recombination))) * (v_tri));
	}
	if (n == (4)) {
		 return ((((flux) * (recombination))) * (v_quad));
	}
	return 0;
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 50; ++n)
    {
        values[INDEX_Ci_START + (n - 1)] = ((1) * (std::pow(10, -(6))));
    }

    for (size_t n = 1; n <= 50; ++n)
    {
        values[INDEX_Cv_START + (n - 1)] = ((1) * (std::pow(10, -(6))));
    }
    values[INDEX_Rho] = dislocation_density_0;
}

std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t i = 0; i < 50; ++i)
	{
		str << ", Ci[" << i + 1 << "]";
	}

	for (size_t i = 0; i < 50; ++i)
	{
		str << ", Cv[" << i + 1 << "]";
	}

	str << ", Rho";
	return str.str();
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= 50; ++n)
    {
        derivatives[INDEX_Ci_START + (n - 1)] = i_gain((n));
    }

    for (size_t n = 1; n <= 50; ++n)
    {
        derivatives[INDEX_Cv_START + (n - 1)] = v_gain((n));
    }
    derivatives[INDEX_Rho] = ((dislocation_gain) - (((((dislocation_density_evolution) * (std::pow(burgers_vector, 2)))) * (std::pow(values[INDEX_Rho], ((3) / (2)))))));

    derivatives[INDEX_Ci_START + (size_t)(1 - 1)] = i_gain((1));
    derivatives[INDEX_Cv_START + (size_t)(1 - 1)] = v_gain((1));
    return 0;
}
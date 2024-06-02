#include <iostream>

#include <cvodes/cvodes.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_spgmr.h>
#include <sunmatrix/sunmatrix_dense.h> 

#include "../generated/system.h"

void handleError(int sunerr)
{
    if (sunerr) std::cout << SUNGetErrMsg(sunerr) << "\n";
}

int p_solve(sunrealtype t, N_Vector y, N_Vector fy, N_Vector r, N_Vector z, sunrealtype gamma, sunrealtype delta, int lr, void *user_data)
{
    return 0;
}

int main()
{
    SUNContext sun_context;
    N_Vector state;
    SUNLinearSolver linear_solver;
    void* cvodes_memory_block;
    size_t state_size = STATE_SIZE;

    handleError( SUNContext_Create(SUN_COMM_NULL, &sun_context) );

    state = N_VNew_Serial(state_size, sun_context);
    get_initial_state(state);
    
    cvodes_memory_block = CVodeCreate(CV_BDF, sun_context);
    handleError( CVodeInit(cvodes_memory_block, derivative, 0, state) );
    handleError( CVodeSStolerances(cvodes_memory_block, 1e-6, 1e9) );
    linear_solver = SUNLinSol_SPGMR(state, SUN_PREC_NONE, 0, sun_context);
    CVodeSetMaxNumSteps(cvodes_memory_block, 50000);
    CVodeSetMinStep(cvodes_memory_block, 1e-30);
    CVodeSetMaxStep(cvodes_memory_block, 1e5);
    CVodeSetInitStep(cvodes_memory_block, 1e-10);
    handleError( CVodeSetLinearSolver(cvodes_memory_block, linear_solver, NULL) );
    handleError( CVodeSetPreconditioner(cvodes_memory_block, NULL, p_solve) );

    double tout = 1e8;
    double sample_interval = 1e6;

    std::cout << get_state_csv_label() << std::endl;
    for (double t = 0; t <= tout;)
    {
        int sunerr = CVode(cvodes_memory_block, t + sample_interval, state, &t, CV_NORMAL);
        if (sunerr) break;
        std::cout << t;
        std::cout << get_csv_line(state) << "\n";
    }

    N_VDestroy_Serial(state);
    SUNLinSolFree(linear_solver);
    CVodeFree(&cvodes_memory_block);
    SUNContext_Free(&sun_context);

    return 0;
}
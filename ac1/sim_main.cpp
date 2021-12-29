// DESCRIPTION: Verilator: Verilog example module
//
// This file ONLY is placed under the Creative Commons Public Domain, for
// any use, without warranty, 2017 by Wilson Snyder.
// SPDX-License-Identifier: CC0-1.0
//======================================================================

// For std::unique_ptr
#include <memory>

#include <iostream>
#include <fstream>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }

int main(int argc, char** argv, char** env) {
    // This is a more complicated example, please also see the simpler examples/make_hello_c.

    // Prevent unused variable warnings
    if (false && argc && argv && env) {}

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");

    // Construct a VerilatedContext to hold simulation time, etc.
    // Multiple modules (made later below with Vtop) may share the same
    // context to share time, or modules may have different contexts if
    // they should be independent from each other.

    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    // Verilator must compute traced signals
    contextp->traceEverOn(true);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
    // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

    //load first input
    std::ifstream in_file("day1_input.txt");
    vluint64_t stim_data;
    vluint64_t last_data;

    if (in_file >> stim_data) {
        std::cout<<"stim data "<<stim_data<<std::endl;
    } else  {
        // Final model cleanup
        std::cout<<"fail"<<std::endl;
        top->final();
        return -1;
    }

    // Set Vtop's input signals
    top->I_RSTN = !0;
    top->I_CLK = 0;

    // Simulate until $finish
    while (!contextp->gotFinish()) {

        // 1 timeprecision period passes...
        contextp->timeInc(1);

        // Toggle a fast (time/2 period) clock
        top->I_CLK = !top->I_CLK;

        // Toggle control signals on an edge that doesn't correspond
        // to where the controls are sampled; in this example we do
        // this only on a negedge of I_CLK, because we know
        // reset is not sampled there.
        if (!top->I_CLK) {

            top->I_RSTN = (contextp->time() < 10) ? 0 : 1;

            if (top->I_RSTN == 1) {
                top->I_DATA=stim_data;

                if (in_file >> stim_data) {
                    std::cout<<"stim data "<<stim_data<<std::endl;
                    last_data = stim_data;
                } else {
                    std::cout<<"file done, clear pipeline "<<last_data<<std::endl;
                    top->I_DATA = last_data;
                    VL_PRINTF("[%" VL_PRI64 "d] I_CLK=%x rstn=%x count=%" VL_PRI64 "d \n",
                    contextp->time(), top->I_CLK, top->I_RSTN, top->O_COUNT);
                    contextp->timeInc(1);
                    top->I_CLK ^= 1;
                    top->eval();
                    VL_PRINTF("[%" VL_PRI64 "d] I_CLK=%x rstn=%x count=%" VL_PRI64 "d \n",
                    contextp->time(), top->I_CLK, top->I_RSTN, top->O_COUNT);
                    contextp->timeInc(1);
                    top->I_CLK ^= 1;
                    top->eval();
                    VL_PRINTF("[%" VL_PRI64 "d] I_CLK=%x rstn=%x count=%" VL_PRI64 "d \n",
                    contextp->time(), top->I_CLK, top->I_RSTN, top->O_COUNT);
                    contextp->timeInc(1);
                    top->I_CLK ^= 1;
                    top->eval();
                    contextp->timeInc(1);
                    top->I_CLK ^= 1;
                    top->eval();
                    contextp->timeInc(1);
                    top->I_CLK ^= 1;
                    top->eval();
                    // Read outputs
                    VL_PRINTF("[%" VL_PRI64 "d] I_CLK=%x rstn=%x count=%" VL_PRI64 "d \n",
                    contextp->time(), top->I_CLK, top->I_RSTN, top->O_COUNT);
                    return 0;
                }
            }




        }

        // Evaluate model
        // (If you have multiple models being simulated in the same
        // timestep then instead of eval(), call eval_step() on each, then
        // eval_end_step() on each. See the manual.)
        top->eval();

        // Read outputs
        VL_PRINTF("[%" VL_PRI64 "d] I_CLK=%x rstn=%x count=%" VL_PRI64 "d \n",
                  contextp->time(), top->I_CLK, top->I_RSTN, top->O_COUNT);
    }

    // Final model cleanup
    top->final();

    // Coverage analysis (calling write only after the test is known to pass)
#if VM_COVERAGE
    Verilated::mkdir("logs");
    contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Return good completion status
    // Don't use exit() or destructor won't get called
    return 0;
}

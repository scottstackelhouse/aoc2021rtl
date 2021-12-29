# test_my_dut.py (extended)

import cocotb
from cocotb.triggers import Timer
from cocotb.triggers import FallingEdge, RisingEdge

import numpy as np


async def generate_clock(dut, p=10, u="ns"):
    """Generate clock pulses. p is period, u is the time unit string
    default is p=10, u='ns'  """

    half_0 = np.floor(p/2)
    half_1 = p - half_0
    while True:
        dut.I_CLK.value = 0
        await Timer(half_0, units=u)
        dut.I_CLK.value = 1
        await Timer(half_1, units=u)

async def reset(dut, c, r, cycles=2):
    """reset the dut
    c is the clock signal
    r is the reset signal"""

    #wait for neg edge, assert reset for c cycles
    await FallingEdge(c)
    for _ in range(cycles):
        r.value = 0
        await FallingEdge(c)
    r.value = 1

async def mon(dut, c, s, m="watching: "):
    """ monitor a signal s on clock c with message m """
    dut._log.info("starting monitor")
    while True:
        await RisingEdge(c)
        dut._log.info("%s %s", m, s.value.integer)


async def stim_agent(dut, fpath=None):
    assert (fpath != None), "error, did not set fpath"
    f = open(fpath,'r')
    # read in stimulus
    d = f.readlines()

    done = False
    while (not done):
        await RisingEdge(dut.I_CLK)
        if (dut.I_RSTN.value == 0):
            dut._log.info("stim_agent: still in reset")
            continue #still in reset, loop again
  
        # drive all bu the last
        for i in range(len(d)):
            # dut._log.info("driving %d, %s", i, d[i])
            await FallingEdge(dut.I_CLK)
            dut.I_DATA.value = int(d[i])
            if (i == (len(d)-1)):
                dut.I_LAST.value = 1
            else:
                dut.I_LAST.value = 0
        
        f.close()
        done = True
    
    while True:
        await FallingEdge(dut.I_CLK)
        dut.I_LAST.value = 0
        dut.I_DATA.value = 0

        

        
async def monitor_agent(dut):
    done = False

    while (not done):
        await RisingEdge(dut.I_CLK)
        if (dut.I_RSTN.value == 0):
            dut._log.info("monitor_agent: still in reset")
            continue
        
        if (dut.O_LAST.value == 1):
            done = True
            ret_val = dut.O_COUNT.value.integer

    return ret_val


@cocotb.test()
async def test1(dut):
    """main test"""

    await cocotb.start(generate_clock(dut))  # run the clock "in the background"

    # start monitor tasks in the background
    #await cocotb.start(mon(dut, dut.I_CLK, dut.I_RSTN, "reset is: "))
    #await cocotb.start(mon(dut, dut.I_CLK, dut.O_COUNT, "O_COUNT is: "))
    #await cocotb.start(mon(dut, dut.I_CLK, dut.O_LAST, "O_LAST is: "))

    # start the agents in the background
    stim_agent_task = await cocotb.start(stim_agent(dut, "../ac1/day1_input.txt"))
    monitor_agent_task = await(cocotb.start(monitor_agent(dut)))

    # await Timer(50, units="ns")  # wait a bit
    await reset(dut, dut.I_CLK, dut.I_RSTN);

    final = await monitor_agent_task;

    dut._log.info("Final count is %d", final)
    assert final == 1584, "final value is not correct"


   
    
module controllerTop(

);


/* Make sure that the pipeline clock is derived from the gpuClock (synchronous rising edges)
 * but only with certain clock pulses gated/bubbled, resulting in "missing" pulses
 * 
 * Example: 
 *                    _   _   _   _   _
 * gpuClock         _| |_| |_| |_| |_| |_
 *                    _               _
 * pipeline clock   _| |_____________| |_
 *
 * In this example, the pipeline took 4 clock cycles to oomplete
 *
 */

endmodule
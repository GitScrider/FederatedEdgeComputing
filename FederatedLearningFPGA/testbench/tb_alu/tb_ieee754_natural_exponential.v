`timescale 1ns/1ns

module tb_ieee754_divider;

    parameter ITER = 10;

    reg clock;
    reg reset;
    reg [31:0] A; 
    wire [31:0] result; 
    wire done;

    ieee754_natural_exponential #(ITER) uut (
        .clock(clock),
        .reset(reset),
        .A(A),
        .result(result),
        .done(done)
    );

    always #10 clock = ~clock;

    initial begin
        
        clock = 0;
        reset = 1;
        A = 32'h40400000; // 3.0

        #25;
        reset = 0;

        wait(done);
        $display("Time: %t | A: %h | Result: %h",$time, A, result);
        $finish;

    end

endmodule
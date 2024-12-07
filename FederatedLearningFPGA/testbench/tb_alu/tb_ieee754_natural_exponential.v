`timescale 1ns/1ns

module tb_ieee754_divider;

    parameter ITER = 4;

    reg clock;
    reg [31:0] A; 
    wire [31:0] result; 

    ieee754_natural_exponential #(ITER) uut (
        .clock(clock),
        .A(A),
        .result(result)
    );

    always #10 clock = ~clock;

    initial begin
        
        clock = 0;

        A = 32'h40400000; // 3.0

        #1600;
        $display("Time: %t | A: %h | Result: %h",$time, A, result);

        $finish;

    end

endmodule
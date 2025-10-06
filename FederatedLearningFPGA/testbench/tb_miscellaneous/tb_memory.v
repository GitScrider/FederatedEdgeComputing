`timescale 1ns / 1ps

module memory_tb;

    parameter WORDS = 16; 

    reg clock;
    reg reset;
    reg write_enable;
    reg [31:0] write_data;
    reg [$clog2(WORDS)-1:0] address;
    wire [31:0] read_data;

    memory_parametrized #(WORDS) uut (
        .clock(clock),
        .reset(reset),
        .write_enable(write_enable),
        .write_data(write_data),
        .address(address),
        .read_data(read_data)
    );

    initial begin
        clock = 0;
        forever #5 clock = ~clock;
    end


    initial begin

        reset = 1;
        write_enable = 0;
        write_data = 32'b0;
        address = 0;

        #10 reset = 0;

        #10 write_enable = 1; address = 3; write_data = 32'hDEADBEEF; 
        #10 write_enable = 1; address = 5; write_data = 32'hCAFEBABE; 

        #10 write_enable = 0; address = 3; 
        #10 address = 5; 

        #20 $finish;
    end

    initial begin
        $monitor("Time: %0t | Reset: %b | Write_Enable: %b | Address: %0d | Write_Data: %h | Read_Data: %h",
                 $time, reset, write_enable, address, write_data, read_data);
    end

endmodule

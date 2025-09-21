`timescale 1ns / 1ps

module tb_memory_32bit;

    // Test parameters
    parameter MEMORY_DEPTH = 16;  // Small memory for testing
    parameter ADDR_WIDTH = $clog2(MEMORY_DEPTH);

    // Test signals
    reg clock;
    reg reset;
    reg write_enable;
    reg [ADDR_WIDTH-1:0] address;
    reg [31:0] write_data;
    wire [31:0] read_data;

    // Instantiate the memory module
    memory_32bit #(
        .MEMORY_DEPTH(MEMORY_DEPTH)
    ) uut (
        .clock(clock),
        .reset(reset),
        .write_enable(write_enable),
        .address(address),
        .write_data(write_data),
        .read_data(read_data)
    );

    // Clock generation
    initial begin
        clock = 0;
        forever #5 clock = ~clock;  // 100MHz clock (10ns period)
    end

    // Test stimulus
    initial begin
        // Initialize signals
        reset = 1;
        write_enable = 0;
        address = 0;
        write_data = 32'h00000000;

        // Test 1: Reset functionality
        #20;
        reset = 0;
        #10;

        // Test 2: Write operations
        write_enable = 1;
        address = 0; write_data = 32'hAABBCCDD;
        #10;
        address = 1; write_data = 32'h11223344;
        #10;
        address = 2; write_data = 32'hDEADBEEF;
        #10;
        address = 3; write_data = 32'hCAFEBABE;
        #10;

        // Test 3: Read operations
        write_enable = 0;
        address = 0;
        #10;
        address = 1;
        #10;
        address = 2;
        #10;
        address = 3;
        #10;

        // Test 4: Write to different addresses
        write_enable = 1;
        address = 15; write_data = 32'hFEDCBA98;
        #10;
        address = 8; write_data = 32'h87654321;
        #10;

        // Test 5: Read from newly written addresses
        write_enable = 0;
        address = 15;
        #10;
        address = 8;
        #10;

        // Test 6: Reset during operation
        reset = 1;
        #10;
        reset = 0;
        #10;

        // Test 7: Read after reset (should be all zeros)
        address = 0;
        #10;
        address = 1;
        #10;
        address = 15;
        #10;

        // End simulation
        #20 $finish;
    end

    // Monitor and display results
    initial begin
        $monitor("Time: %0t | Reset: %b | Write_Enable: %b | Address: %0d | Write_Data: %h | Read_Data: %h",
                 $time, reset, write_enable, address, write_data, read_data);
    end

    // Additional verification
    initial begin
        $display("=== 32-bit Memory Testbench Started ===");
        $display("Memory Depth: %0d words", MEMORY_DEPTH);
        $display("Address Width: %0d bits", ADDR_WIDTH);
        $display("=====================================");
    end

endmodule 
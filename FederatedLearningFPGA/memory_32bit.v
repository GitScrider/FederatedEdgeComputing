module memory_32bit #(
    parameter MEMORY_DEPTH = 256,  // Default 256 words, can be changed
    parameter ADDR_WIDTH = $clog2(MEMORY_DEPTH)
)(
    input wire clock,                    // Clock signal
    input wire reset,                    // Reset signal (active high)
    input wire write_enable,             // Write enable signal
    input wire [ADDR_WIDTH-1:0] address, // Memory address
    input wire [31:0] write_data,        // Data to write (32-bit)
    output reg [31:0] read_data          // Data read from memory (32-bit)
);

    // Memory array declaration
    reg [31:0] memory_array [0:MEMORY_DEPTH-1];
    
    // Loop variable for initialization
    integer i;

    // Memory initialization
    initial begin
        for (i = 0; i < MEMORY_DEPTH; i = i + 1) begin
            memory_array[i] = 32'h00000000;  // Initialize all memory locations to 0
        end
    end

    // Write operation - synchronous
    always @(posedge clock) begin
        if (reset) begin
            // Reset all memory locations to 0
            for (i = 0; i < MEMORY_DEPTH; i = i + 1) begin
                memory_array[i] <= 32'h00000000;
            end
        end else if (write_enable) begin
            // Write data to specified address
            memory_array[address] <= write_data;
        end
    end

    // Read operation - asynchronous (combinational)
    always @(*) begin
        read_data = memory_array[address];
    end

endmodule 
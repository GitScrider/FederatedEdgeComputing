module shiftreg_to_7seg (
    input [15:0] shift,      // Número binário de 4 bits
    output reg [6:0] seg     // Sinais para o display de 7 segmentos
);

// Segmentos: [gfedcba]
always @(*) begin
    case (shift)
        16'b0000000000000001: seg = 7'b1000000; // 0
        16'b0000000000000010: seg = 7'b1111001; // 1
        16'b0000000000000100: seg = 7'b0100100; // 2
        16'b0000000000001000: seg = 7'b0110000; // 3
        16'b0000000000010000: seg = 7'b0011001; // 4
        16'b0000000000100000: seg = 7'b0010010; // 5
        16'b0000000001000000: seg = 7'b0000010; // 6
		  16'b0000000010000000: seg = 7'b1111000; // 7
        16'b0000000100000000: seg = 7'b0000000; // 8
        16'b0000001000000000: seg = 7'b0010000; // 9
        16'b0000010000000000: seg = 7'b0001000; // A
        16'b0000100000000000: seg = 7'b0000011; // b
        16'b0001000000000000: seg = 7'b1000110; // C
        16'b0010000000000000: seg = 7'b0100001; // d
        16'b0100000000000000: seg = 7'b0000110; // E
        16'b1000000000000000: seg = 7'b0001110; // F
        default: seg = 7'b1111111; // Apagar o display
    endcase
end

endmodule


module bin_to_7seg (
    input [3:0] binary,      // Número binário de 4 bits
    output reg [6:0] seg     // Sinais para o display de 7 segmentos
);

// Segmentos: [gfedcba]
always @(*) begin
    case (binary)
        4'b0000: seg = 7'b1000000; // 0
        4'b0001: seg = 7'b1111001; // 1
        4'b0010: seg = 7'b0100100; // 2
        4'b0011: seg = 7'b0110000; // 3
        4'b0100: seg = 7'b0011001; // 4
        4'b0101: seg = 7'b0010010; // 5
        4'b0110: seg = 7'b0000010; // 6
        4'b0111: seg = 7'b1111000; // 7
        4'b1000: seg = 7'b0000000; // 8
        4'b1001: seg = 7'b0010000; // 9
        4'b1010: seg = 7'b0001000; // A
        4'b1011: seg = 7'b0000011; // b
        4'b1100: seg = 7'b1000110; // C
        4'b1101: seg = 7'b0100001; // d
        4'b1110: seg = 7'b0000110; // E
        4'b1111: seg = 7'b0001110; // F
        default: seg = 7'b1111111; // Apagar o display
    endcase
end

endmodule


module clock_counter (
    input wire clk,                   
    input wire ffbp,                    
    output reg [15:0] out              
);

initial begin
	out <= 16'b0000000000000001;
end

always @(posedge clk) begin
    if (ffbp) begin
        // Deslocamento para a direita
        out <= {1'b0, out[15:1]};
    end else begin
		  // Deslocamento para a esquerda
        out <= {out[14:0], 1'b0};
    end
end

endmodule

module NeuralNetworkFPGA (
    input CLOCK_50,          
    input [17:0] SW,
	 input KEY0,
    output [6:0] HEX0,       
	 output [6:0] HEX7
	 
	 );

wire [3:0] binary;
wire [6:0] seg;

// clock counter
wire ffbp;
wire clock;
wire [15:0]outcounter;

assign binary = SW[3:0];  

assign ffbp = SW[17];
assign clock = KEY0;  
  

clock_counter layercounter (
	.clk(clock),
	.ffbp(ffbp),
	.out(outcounter)
	);

shiftreg_to_7seg s2s(
	.shift(outcounter),
	.seg(HEX7)
);
	
    

// Instanciar o módulo bin_to_7seg
bin_to_7seg b2s (
    .binary(binary),
    .seg(HEX0)
);

endmodule
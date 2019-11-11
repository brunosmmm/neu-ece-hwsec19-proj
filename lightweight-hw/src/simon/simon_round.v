`include "simon_common.vh"

module simon_round
  #(
    parameter integer SIMON_MAX_WORD_WIDTH = 64,
    parameter integer REGISTER_WIDTH = 64
    )
   (
    input                                 mode,
    input                                 enc_dec,
    input [SIMON_MAX_WORD_WIDTH-1:0]      block1_in,
    input [SIMON_MAX_WORD_WIDTH-1:0]      block2_in,
    input [SIMON_MAX_WORD_WIDTH-1:0]      key,
    input                                 i_valid,
    output reg [SIMON_MAX_WORD_WIDTH-1:0] block1_out,
    output reg [SIMON_MAX_WORD_WIDTH-1:0] block2_out,
    output reg                            o_valid,
    output reg                            i_ready,
    input                                 ck,
    input                                 nrst
    );

   reg [`SIMON_64_128_WORD_SIZE-1:0] rl1_64in;
   reg [`SIMON_64_128_WORD_SIZE-1:0] rl2_64in;
   reg [`SIMON_64_128_WORD_SIZE-1:0] rl8_64in;
   reg [`SIMON_128_128_WORD_SIZE-1:0] rl1_128in;
   reg [`SIMON_128_128_WORD_SIZE-1:0] rl2_128in;
   reg [`SIMON_128_128_WORD_SIZE-1:0] rl8_128in;

   wire [`SIMON_64_128_WORD_SIZE-1:0] rl1_64out;
   wire [`SIMON_64_128_WORD_SIZE-1:0] rl2_64out;
   wire [`SIMON_64_128_WORD_SIZE-1:0] rl8_64out;
   wire [`SIMON_128_128_WORD_SIZE-1:0] rl1_128out;
   wire [`SIMON_128_128_WORD_SIZE-1:0] rl2_128out;
   wire [`SIMON_128_128_WORD_SIZE-1:0] rl8_128out;

   rotate_unit
     #(
       .amount(1),
       .width(`SIMON_128_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_1_128
       (
        .i_value(rl1_128in),
        .o_value(rl1_128out)
        );

   rotate_unit
     #(
       .amount(2),
       .width(`SIMON_128_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_2_128
       (
        .i_value(rl2_128in),
        .o_value(rl2_128out)
        );

   rotate_unit
     #(
       .amount(8),
       .width(`SIMON_128_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_8_128
       (
        .i_value(rl8_128in),
        .o_value(rl8_128out)
        );

   rotate_unit
     #(
       .amount(1),
       .width(`SIMON_64_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_1_64
       (
        .i_value(rl1_64in),
        .o_value(rl1_64out)
        );

   rotate_unit
     #(
       .amount(2),
       .width(`SIMON_64_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_2_64
       (
        .i_value(rl2_64in),
        .o_value(rl2_64out)
        );

   rotate_unit
     #(
       .amount(8),
       .width(`SIMON_64_128_WORD_SIZE),
       .rl(`ROTATE_MODE_LEFT)
       ) rotate_left_8_64
       (
        .i_value(rl8_64in),
        .o_value(rl8_64out)
        );

   wire [SIMON_MAX_WORD_WIDTH-1:0]     out;
   wire [SIMON_MAX_WORD_WIDTH-1:0]     xin, yin;
   reg [SIMON_MAX_WORD_WIDTH-1:0]      tmp;
   reg                                 busy;
   reg                                 enc_dec_mode, simon_mode;

   assign xin = enc_dec ? block1_in : block2_in;
   assign yin = enc_dec ? block2_in : block1_in;
   assign out = (simon_mode == `SIMON_MODE_64_128) ? {32'b0, (rl1_64out & rl8_64out) ^ rl2_64out} ^ tmp:
                (rl1_128out & rl8_128out) ^ rl2_128out ^ tmp;

   // can do in two cycles?
   always @(posedge ck) begin
      if (!nrst) begin
         enc_dec_mode <= '0;
         simon_mode <= '0;
         block1_out <= '0;
         block2_out <= '0;
         o_valid <= '0;
         i_ready <= '1;
         busy <= '0;
         tmp <= '0;
         rl1_64in <= 0;
         rl2_64in <= 0;
         rl8_64in <= 0;
      end
      else begin
         if (i_valid && !busy) begin
            i_ready <= '0;
            busy <= '1;
            enc_dec_mode <= enc_dec;
            simon_mode <= mode;
            // precalculate some stuff
            tmp <= key ^ yin;
            if (mode == `SIMON_MODE_64_128) begin
               rl1_64in <= xin[31:0];
               rl2_64in <= xin[31:0];
               rl8_64in <= xin[31:0];
            end
            else begin
               rl1_128in <= xin;
               rl2_128in <= xin;
               rl8_128in <= xin;
            end
            if (enc_dec) begin
               block2_out <= block1_in;
            end
            else begin
               block1_out <= block2_in;
            end
         end // if (i_valid && !busy)
         else begin
            if (busy) begin
               o_valid <= '1;
               i_ready <= '1;
               busy <= '0;
               if (enc_dec_mode) begin
                  block1_out <= out;
               end
               else begin
                  block2_out <= out;
               end
            end
            else begin
               o_valid <= '0;
            end
         end // else: !if(i_valid && !busy)
      end // else: !if(!nrst)
   end // always @ (posedge clk)

endmodule

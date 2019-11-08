`include "simon_common.vh"

module simon_round
  #(
    parameter integer SIMON_MAX_WORD_WIDTH = 64,
    parameter integer REGISTER_WIDTH = 64
    )
   (
    input                                  mode,
    input [SIMON_MAX_WORD_WIDTH-1:0]      block_in,
    input                                  i_valid,
    output reg [SIMON_MAX_WORD_WIDTH-1:0] block_out,
    output reg                             o_valid,
    output reg                             i_ready,
    input ck,
    input nrst
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

   endmodule

`include "simon_common.vh"

`define KEXP_STATE_IDLE 0
`define KEXP_STATE_LOAD 1
`define KEXP_STATE_EXP  2
`define KEXP_STATE_DONE 3

module simon_kexp
  #(
    parameter integer SIMON_MAX_ROUNDS = 68,
    parameter integer SIMON_MAX_WORD_WIDTH = 64,
    parameter integer REGISTER_WIDTH = 64,
    parameter integer SIMON_KEY_WIDTH = 128
    )
  (
   input                             mode,
   input [SIMON_KEY_WIDTH-1:0]       key,
   input                             k_valid,
   output                            k_ready,
   output [SIMON_MAX_WORD_WIDTH-1:0] exp [0:SIMON_MAX_ROUNDS-1],
   output                            exp_valid,
   input                             ck,
   input                             nrst
   );

   assign exp = xKey;

   localparam [63:0] Z_64_128 = 64'b0011001101101001111110001000010100011001001011000000111011110101;
   localparam [63:0] Z_128_128 = 64'b0011110000101100111001010001001000000111101001100011010111011011;

   // registers for expanded key
   reg [SIMON_MAX_WORD_WIDTH-1:0] xKey [0:SIMON_MAX_ROUNDS-1];
   reg [REGISTER_WIDTH-1:0]       flags;

   // rotation units
   reg [`SIMON_64_128_WORD_SIZE-1:0] rr3_64in;
   reg [`SIMON_64_128_WORD_SIZE-1:0] rr1_64in;
   reg [`SIMON_128_128_WORD_SIZE-1:0] rr3_128in;
   reg [`SIMON_128_128_WORD_SIZE-1:0] rr1_128in;

   wire [`SIMON_64_128_WORD_SIZE-1:0]  rr3_64out;
   wire [`SIMON_64_128_WORD_SIZE-1:0]  rr1_64out;
   wire [`SIMON_128_128_WORD_SIZE-1:0] rr3_128out;
   wire [`SIMON_128_128_WORD_SIZE-1:0] rr1_128out;

   rotate_unit
     #(
       .amount(3),
       .width(`SIMON_64_128_WORD_SIZE),
       .rl(`ROTATE_MODE_RIGHT)
       ) rotate_right_3_64
       (
        .i_value(rr3_64in),
        .o_value(rr3_64out)
        );

   rotate_unit
     #(
       .amount(1),
       .width(`SIMON_64_128_WORD_SIZE),
       .rl(`ROTATE_MODE_RIGHT)
       ) rotate_right_1_64
       (
        .i_value(rr1_64in),
        .o_value(rr1_64out)
        );

   rotate_unit
     #(
       .amount(3),
       .width(`SIMON_128_128_WORD_SIZE),
       .rl(`ROTATE_MODE_RIGHT)
       ) rotate_right_3_128
       (
        .i_value(rr3_128in),
        .o_value(rr3_128out)
        );

   rotate_unit
     #(
       .amount(1),
       .width(`SIMON_128_128_WORD_SIZE),
       .rl(`ROTATE_MODE_RIGHT)
       ) rotate_right_1_128
       (
        .i_value(rr1_128in),
        .o_value(rr1_128out)
        );


   // internal flags and logic
   wire                               kexp_start;
   reg [1:0]                          kexp_state;
   reg                                kexp_phase;
   reg [16:0]                         kexp_pending;
   reg                                cur_mode; // current mode
   assign exp_valid = (kexp_state == `KEXP_STATE_DONE);
   assign k_ready = (kexp_state == `KEXP_STATE_IDLE);

   // key expander logic
   always @(posedge ck) begin
      if (!nrst) begin
         kexp_state <= `KEXP_STATE_IDLE;
         kexp_phase <= 0;
         rr3_64in <= 0;
         rr1_64in <= 0;
         rr3_128in <= 0;
         rr1_128in <= 0;
      end
      else begin
         case (kexp_state)
           `KEXP_STATE_IDLE: begin
              if (k_valid) begin
                 cur_mode <= mode;
                 kexp_state <= `KEXP_STATE_EXP;
                 if (mode == `SIMON_MODE_64_128) begin
                    xKey[0] <= {32'b0, key[31:0]};
                    xKey[1] <= {32'b0, key[63:32]};
                    xKey[2] <= {32'b0, key[95:64]};
                    xKey[3] <= {32'b0, key[127:96]};
                    kexp_pending <= `SIMON_64_128_ROUNDS-`SIMON_64_128_KEY_WORDS;
                    rr3_64in <= key[127:96];
                 end
                 else begin
                    xKey[0] <= key[63:0];
                    xKey[1] <= key[127:64];
                    kexp_pending <= `SIMON_128_128_ROUNDS-`SIMON_128_128_KEY_WORDS;
                    rr3_128in <= key[127:64];
                 end // else: !if(mode == `SIMON_MODE_64_128)
              end
           end // case: KEXP_STATE_IDLE
           `KEXP_STATE_EXP: begin
              if (kexp_pending == 'b0) begin
                 kexp_state <= `KEXP_STATE_DONE;
              end
              else begin
                 if (kexp_phase) begin
                    // next round's shifted word
                    if (mode == `SIMON_MODE_64_128) begin
                       rr1_64in <= rr3_64out ^ xKey[`SIMON_64_128_ROUNDS-kexp_pending-3][31:0];
                       rr3_64in <= xKey[`SIMON_64_128_ROUNDS-kexp_pending-1][31:0];
                    end
                    else begin
                       rr1_128in <= rr3_128out ^ xKey[`SIMON_128_128_ROUNDS-kexp_pending-3];
                       rr3_128in <= xKey[`SIMON_128_128_ROUNDS-kexp_pending-1];
                    end
                    kexp_phase <= '1;
                 end // if (kexp_phase)
                 else begin
                    if (mode == `SIMON_MODE_64_128) begin
                       xKey[`SIMON_64_128_ROUNDS-kexp_pending] <= {32'b0,
                          ~xKey[`SIMON_64_128_ROUNDS-kexp_pending-`SIMON_64_128_KEY_WORDS][31:0] ^ rr1_64out ^
                          {31'b0, Z_64_128[(`SIMON_64_128_ROUNDS-kexp_pending-`SIMON_64_128_KEY_WORDS)%62]} ^ 32'd3};
                    end
                    else begin
                       xKey[`SIMON_128_128_ROUNDS-kexp_pending] <=
                          ~xKey[`SIMON_128_128_ROUNDS-kexp_pending-`SIMON_128_128_KEY_WORDS] ^ rr1_128out ^
                          {63'b0,Z_128_128[(`SIMON_128_128_ROUNDS-kexp_pending-`SIMON_128_128_KEY_WORDS)%62]} ^ 64'd3;
                    end
                    kexp_phase <= '0;
                    kexp_pending <= kexp_pending - 1;
                 end // else: !if(kexp_phase)
              end // else: !if(!kexp_pending)
           end // case: `KEXP_STATE_EXP
         endcase // case (kexp_state)
      end
   end

endmodule

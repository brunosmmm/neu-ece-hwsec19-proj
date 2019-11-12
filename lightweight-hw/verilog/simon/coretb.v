`include "simon_common.vh"
`define RESET_CYCLES 10
`define SIMULATION_CYCLES 10000
`define SIM_STATE_START 0
`define SIM_STATE_WAIT_READY 1
`define SIM_STATE_WAIT_DONE 2
`define SIM_STATE_ENCRYPT 3

module coretb();

   localparam [`SIMON_KEY_WIDTH-1:0] TEST_KEY = 128'h56AB09BBA4F930110042AA2AFF020180;
   localparam [63:0] TEST_BLOCK = 64'h0706050403020100;

   wire [`SIMON_KEY_WIDTH-1:0] simon_key;
   wire simon_mode;
   assign simon_mode = `SIMON_MODE_64_128;
   assign simon_key = TEST_KEY;

   reg  key_valid;
   wire kexp_ready, exp_valid;
   /* verilator lint_off UNUSED */
   wire [63:0] expanded [0:`SIMON_128_128_ROUNDS-1];
   genvar      i;
   generate
      for (i = 0; i < `SIMON_128_128_ROUNDS; i = i + 1) begin : expand
         wire [63:0] expanded_;
         assign expanded_ = expanded[i];
      end
   endgenerate

   /* verilator lint_on UNUSED */

   reg [2:0]   sim_state;
   reg [31:0]  cycles;
   reg         ck;
   reg         nrst;

   wire [63:0] round_64_1;
   wire [63:0] round_64_2;
   assign round_64_1 = {32'b0, TEST_BLOCK[31:0]};
   assign round_64_2 = {32'b0, TEST_BLOCK[63:32]};
   reg [63:0]  round_64_x;
   reg [63:0]  round_64_y;
   reg         block_valid;
   wire        round_valid;
   wire [63:0] block1_out;
   wire [63:0] block2_out;
   reg [$clog2(`SIMON_64_128_ROUNDS)-1:0] rounds_pending;
   reg [63:0]                             round_key;

   simon_kexp key_expander
     (
      .mode (simon_mode),
      .key (simon_key),
      .k_valid (key_valid),
      .k_ready (kexp_ready),
      .expanded(expanded),
      .exp_valid (exp_valid),
      .ck (ck),
      .nrst (nrst)
      );

   simon_round round
     (
      .mode (simon_mode),
      .enc_dec(1'b1),
      .block1_in(round_64_x),
      .block2_in(round_64_y),
      .key (round_key),
      .i_valid (block_valid),
      .o_valid (round_valid),
      .block1_out (block1_out),
      .block2_out (block2_out),
      .ck (ck),
      .nrst (nrst)
      );

   initial begin
      $display("Starting...");
      $dumpfile("simon_kexp.vcd");
      $dumpvars(0, coretb);
      ck = 0;
      nrst = 0;
      reset_counter = 'b0;
      cycles = 0;
   end

   always begin
      ck = #1 ~ck;
   end

   // reset generator
   reg [31:0] reset_counter;
   always @(posedge ck) begin
      cycles <= cycles + 1;
      if (cycles == `SIMULATION_CYCLES) begin
         $finish;
      end
      if (!nrst) begin
         reset_counter <= reset_counter + 1;
         if (reset_counter == `RESET_CYCLES) begin
            nrst <= 1;
         end
      end
   end


   always @(posedge ck) begin
      if (!nrst) begin
         sim_state <= `SIM_STATE_START;
         key_valid <= 0;
         rounds_pending <= 0;
         round_key <= 0;
         block_valid <= 0;
         round_64_x <= 0;
         round_64_y <= 0;
      end
      else begin
         case (sim_state)
           `SIM_STATE_START:
             sim_state <= `SIM_STATE_WAIT_READY;
           `SIM_STATE_WAIT_READY: begin
              if (kexp_ready) begin
                 key_valid <= 1;
                 sim_state <= `SIM_STATE_WAIT_DONE;
              end
           end
           `SIM_STATE_WAIT_DONE: begin
              key_valid <= 0;
              if (exp_valid) begin
                 $display("key expansion done");
                 round_64_x <= round_64_1;
                 round_64_y <= round_64_2;
                 sim_state <= `SIM_STATE_ENCRYPT;
                 rounds_pending <= `SIMON_64_128_ROUNDS;
                 block_valid <= 1;
                 round_key <= expanded[0];
              end
           end
           `SIM_STATE_ENCRYPT: begin
              if (rounds_pending == 0 && round_valid) begin
                 $display("done encrypting");
                 $finish;
              end
              else begin
                 if (round_valid) begin
                    round_64_x <= block1_out;
                    round_64_y <= block2_out;
                    block_valid <= 1;
                    round_key <= expanded[`SIMON_64_128_ROUNDS-rounds_pending+1];
                    rounds_pending <= rounds_pending - 1;
                 end
                 else begin
                    block_valid <= 0;
                 end
              end
           end
         endcase
      end
   end



endmodule

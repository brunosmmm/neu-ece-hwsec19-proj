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
   wire        rst;
   assign rst = ~nrst;

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

   SimonKeyExpander key_expander
     (
      .io_mode (simon_mode),
      .io_key (simon_key),
      .io_kValid (key_valid),
      .io_kReady (kexp_ready),
      .io_expanded_0 (expanded[0]),
      .io_expanded_1 (expanded[1]),
      .io_expanded_2 (expanded[2]),
      .io_expanded_3 (expanded[3]),
      .io_expanded_4 (expanded[4]),
      .io_expanded_5 (expanded[5]),
      .io_expanded_6 (expanded[6]),
      .io_expanded_7 (expanded[7]),
      .io_expanded_8 (expanded[8]),
      .io_expanded_9 (expanded[9]),
      .io_expanded_10 (expanded[10]),
      .io_expanded_11 (expanded[11]),
      .io_expanded_12 (expanded[12]),
      .io_expanded_13 (expanded[13]),
      .io_expanded_14 (expanded[14]),
      .io_expanded_15 (expanded[15]),
      .io_expanded_16 (expanded[16]),
      .io_expanded_17 (expanded[17]),
      .io_expanded_18 (expanded[18]),
      .io_expanded_19 (expanded[19]),
      .io_expanded_20 (expanded[20]),
      .io_expanded_21 (expanded[21]),
      .io_expanded_22 (expanded[22]),
      .io_expanded_23 (expanded[23]),
      .io_expanded_24 (expanded[24]),
      .io_expanded_25 (expanded[25]),
      .io_expanded_26 (expanded[26]),
      .io_expanded_27 (expanded[27]),
      .io_expanded_28 (expanded[28]),
      .io_expanded_29 (expanded[29]),
      .io_expanded_30 (expanded[30]),
      .io_expanded_31 (expanded[31]),
      .io_expanded_32 (expanded[32]),
      .io_expanded_33 (expanded[33]),
      .io_expanded_34 (expanded[34]),
      .io_expanded_35 (expanded[35]),
      .io_expanded_36 (expanded[36]),
      .io_expanded_37 (expanded[37]),
      .io_expanded_38 (expanded[38]),
      .io_expanded_39 (expanded[39]),
      .io_expanded_40 (expanded[40]),
      .io_expanded_41 (expanded[41]),
      .io_expanded_42 (expanded[42]),
      .io_expanded_43 (expanded[43]),
      .io_expanded_44 (expanded[44]),
      .io_expanded_45 (expanded[45]),
      .io_expanded_46 (expanded[46]),
      .io_expanded_47 (expanded[47]),
      .io_expanded_48 (expanded[48]),
      .io_expanded_49 (expanded[49]),
      .io_expanded_50 (expanded[50]),
      .io_expanded_51 (expanded[51]),
      .io_expanded_52 (expanded[52]),
      .io_expanded_53 (expanded[53]),
      .io_expanded_54 (expanded[54]),
      .io_expanded_55 (expanded[55]),
      .io_expanded_56 (expanded[56]),
      .io_expanded_57 (expanded[57]),
      .io_expanded_58 (expanded[58]),
      .io_expanded_59 (expanded[59]),
      .io_expanded_60 (expanded[60]),
      .io_expanded_61 (expanded[61]),
      .io_expanded_62 (expanded[62]),
      .io_expanded_63 (expanded[63]),
      .io_expanded_64 (expanded[64]),
      .io_expanded_65 (expanded[65]),
      .io_expanded_66 (expanded[66]),
      .io_expanded_67 (expanded[67]),
      .io_expValid (exp_valid),
      .clock (ck),
      .reset (rst)
      );

   SimonRound round
     (
      .io_mode (simon_mode),
      .io_encDec(1'b1),
      .io_block1In(round_64_x),
      .io_block2In(round_64_y),
      .io_roundKey (round_key),
      .io_iValid (block_valid),
      .io_oValid (round_valid),
      .io_block1Out (block1_out),
      .io_block2Out (block2_out),
      .clock (ck),
      .reset (rst)
      );

   initial begin
      $display("Starting...");
      $dumpfile("simon_kexp_chisel.vcd");
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
              if (rounds_pending == 1 && round_valid) begin
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

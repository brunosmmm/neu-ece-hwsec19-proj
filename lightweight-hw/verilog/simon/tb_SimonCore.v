`include "simon_common.vh"
`define RESET_CYCLES 10
`define SIMULATION_CYCLES 100000
`define SIM_STATE_START 0
`define SIM_STATE_LOAD_KEY 1
`define SIM_STATE_WAIT_KEY 2
`define SIM_STATE_ENCRYPT 3
`define SIM_STATE_WAIT 4
`define SIM_STATE_DECRYPT 5

module tb_SimonCore();

   localparam [`SIMON_KEY_WIDTH-1:0] TEST_KEY = 128'h56AB09BBA4F930110042AA2AFF020180;
   localparam [63:0] TEST_BLOCK = 64'h0706050403020100;

   wire [`SIMON_KEY_WIDTH-1:0] simon_key;
   wire simon_mode;
   assign simon_mode = `SIMON_MODE_64_128;
   assign simon_key = TEST_KEY;

   reg  key_valid;
   wire kexp_ready, exp_valid;

   reg [3:0]   sim_state;
   reg [31:0]  cycles;
   reg         ck;
   reg         nrst;
   wire        rst;
   assign rst = ~nrst;

   wire [63:0] round_64_1;
   wire [63:0] round_64_2;
   assign round_64_1 = {32'b0, TEST_BLOCK[31:0]};
   assign round_64_2 = {32'b0, TEST_BLOCK[63:32]};
   reg [$clog2(`SIMON_64_128_ROUNDS)-1:0] rounds_pending;
   reg [15:0]                             wait_counter;

   reg [63:0]  data_reg_1;
   reg [63:0]  data_reg_2;

   wire [63:0] data_out_1;
   wire [63:0] data_out_2;
   wire        data_ready;
   reg         round_start;
   wire        round_done;
   wire        key_done;
   reg         enc_dec;
   reg         single;
   SimonCore core
     (
      .clock (ck),
      .reset(rst),

      .io_keyL (simon_key[63:0]),
      .io_keyH (simon_key[127:64]),
      .io_kValid (key_valid),
      .io_kExpDone (key_done),
      .io_sMode(simon_mode),
      .io_data1In (data_reg_1),
      .io_data2In (data_reg_2),
      .io_data1Out (data_out_1),
      .io_data2Out (data_out_2),
      .io_dInReady (data_ready),
      .io_dInValid (round_start),
      .io_dOutValid (round_done),
      .io_dEncDec (enc_dec),
      .io_rSingle (single)
      );


   initial begin
      $display("Starting...");
      $dumpfile("SimonCore.vcd");
      $dumpvars(0, tb_SimonCore);
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
         round_start <= 0;
         rounds_pending <= 0;
         data_reg_1 <= 0;
         data_reg_2 <= 0;
         single <= 0;
         enc_dec <= 0;
         wait_counter <= 0;
      end
      else begin
         case (sim_state)
           `SIM_STATE_START:
             sim_state <= `SIM_STATE_LOAD_KEY;
           `SIM_STATE_LOAD_KEY: begin
              key_valid <= 1;
              sim_state <= `SIM_STATE_WAIT_KEY;
           end
           `SIM_STATE_WAIT_KEY: begin
              key_valid <= 0;
              if (key_done) begin
                 $display("key expansion done");
                 data_reg_1 <= round_64_1;
                 data_reg_2 <= round_64_2;
                 single <= 1;
                 enc_dec <= 1;
                 round_start <= 1;
                 sim_state <= `SIM_STATE_ENCRYPT;
                 rounds_pending <= `SIMON_64_128_ROUNDS;
              end
           end
           `SIM_STATE_ENCRYPT: begin
              if (rounds_pending == 0 && round_done) begin
                 $display("done encrypting");
                 enc_dec <= 0;
                 round_start <= 0;
                 wait_counter <= 10;
                 rounds_pending <= `SIMON_64_128_ROUNDS;
                 sim_state <= `SIM_STATE_WAIT;
              end
              else begin
                 if (round_done) begin
                    data_reg_1 <= data_out_1;
                    data_reg_2 <= data_out_2;
                    if (round_start) begin
                       round_start <= 0;
                    end
                    else begin
                       round_start <= 1;
                       rounds_pending <= rounds_pending - 1;
                    end
                 end
                 else begin
                    round_start <= 0;
                 end
              end
           end
           `SIM_STATE_WAIT: begin
              if (wait_counter > 0) begin
                 wait_counter <= wait_counter - 1;
              end
              else begin
                 round_start <= 1;
                 sim_state <= `SIM_STATE_DECRYPT;
              end
           end
           `SIM_STATE_DECRYPT: begin
              if (rounds_pending == 0 && round_done) begin
                 $display("done decrypting");
                 $finish;
              end
              else begin
                 if (round_done) begin
                    data_reg_1 <= data_out_1;
                    data_reg_2 <= data_out_2;
                    if (round_start) begin
                       round_start <= 0;
                    end
                    else begin
                       round_start <= 1;
                       rounds_pending <= rounds_pending - 1;
                    end
                 end
                 else begin
                    round_start <= 0;
                 end
              end
            end
         endcase
      end
   end



endmodule

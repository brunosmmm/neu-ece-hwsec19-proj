`include "simon_common.vh"

`define SIMON_MMIO_REG_SCONF 6'h00
`define SIMON_MMIO_REG_KEY1 6'h08
`define SIMON_MMIO_REG_KEY2 6'h10
`define SIMON_MMIO_REG_DATA1 6'h18
`define SIMON_MMIO_REG_DATA2 6'h20

`define SIMON_MMIO_REG_SCONF_MASK 4'hF
`define SIMON_MMIO_REG_SCONF_MODE 0
`define SIMON_MMIO_REG_SCONF_ENC_DEC 1
`define SIMON_MMIO_REG_SCONF_SINGLE 2
`define SIMON_MMIO_REG_SCONF_READY 4
`define SIMON_MMIO_REG_SCONF_BUSY 5

module simon_mmio
  (
   input [5:0] address,
   input [63:0] i_data,
   output reg [63:0] o_data,
   input rd,
   input wr,

   input clk,
   input nrst
   );

   reg [3:0] sconf;
   reg [127:0] key;
   reg [63:0]  data_1;
   reg [63:0]  data_2;

   reg         k_valid;
   wire        k_ready;
   wire        exp_valid;
   wire [63:0] expanded [0:`SIMON_128_128_ROUNDS-1];
   wire [63:0] data_1_out;
   wire [63:0] data_2_out;
   reg [63:0]  round_key;
   reg         d_valid;
   wire        r_valid;
   wire        r_ready;

   // internal control signals
   reg         k_busy;
   reg         r_busy;
   reg         r_start;
   reg         ready;
   wire        busy;
   wire [63:0] sconf_out;
   assign sconf_out = {58'b0, busy, ready, sconf};
   assign busy = k_busy || r_busy || !k_ready || !r_ready;

   reg [$clog2(`SIMON_128_128_ROUNDS)-1:0] pending_rounds;
   reg [$clog2(`SIMON_128_128_ROUNDS)-1:0] round_counter;

   simon_kexp key_expander
     (
      .mode (sconf[`SIMON_MMIO_REG_SCONF_MODE]),
      .key (key),
      .k_valid (k_valid),
      .k_ready (k_ready),
      .expanded (expanded),
      .exp_valid (exp_valid),
      .ck (clk),
      .nrst (nrst)
      );

   simon_round round
     (
      .mode (sconf[`SIMON_MMIO_REG_SCONF_MODE]),
      .enc_dec(sconf[`SIMON_MMIO_REG_SCONF_ENC_DEC]),
      .block1_in(data_1),
      .block2_in(data_2),
      .key (round_key),
      .i_valid (d_valid),
      .i_ready (r_ready),
      .o_valid (r_valid),
      .block1_out (data_1_out),
      .block2_out (data_2_out),
      .ck (clk),
      .nrst (nrst)
      );

   always @(posedge clk) begin
      if (!nrst) begin
         key <= 0;
         data_1 <= 0;
         data_2 <= 0;
         sconf <= 0;
         k_valid <= 0;
         k_busy <= 0;
         ready <= 0;
         d_valid <= 0;
         round_key <= 0;
      end
      else begin
         if (!k_busy && wr) begin
            case (address)
              `SIMON_MMIO_REG_SCONF: begin
                 sconf <= i_data[3:0] & `SIMON_MMIO_REG_SCONF_MASK;
              end
              `SIMON_MMIO_REG_KEY1: begin
                 key[63:0] <= i_data;
                 ready <= 0;
              end
              `SIMON_MMIO_REG_KEY2: begin
                 key[127:64] <= i_data;
                 k_valid <= 1;
                 k_busy <= 1;
                 ready <= 0;
              end
              `SIMON_MMIO_REG_DATA1: begin
                 data_1 <= i_data;
                 if (sconf[`SIMON_MMIO_REG_SCONF_MODE] == `SIMON_MODE_64_128) begin
                    // start
                    r_start <= 1;
                 end
              end
              `SIMON_MMIO_REG_DATA2: begin
                 data_2 <= i_data;
                 if (sconf[`SIMON_MMIO_REG_SCONF_MODE] == `SIMON_MODE_128_128) begin
                    // start
                    r_start <= 1;
                 end
              end
              default: begin
              end
            endcase
         end
         else begin
            if (k_busy && exp_valid) begin
               k_busy <= 0;
               ready <= 1;
            end
         end // else: !if(!k_busy && wr)
      end
   end

   always @(posedge clk) begin
      if (!nrst) begin
         o_data <= 0;
      end
      else begin
         if (rd) begin
            case (address)
              `SIMON_MMIO_REG_SCONF: begin
                 o_data <= sconf_out;
              end
              `SIMON_MMIO_REG_DATA1: begin
                 o_data <= data_1;
              end
              `SIMON_MMIO_REG_DATA2: begin
                 o_data <= data_2;
              end
              default:  begin
                 o_data <= 64'b0;
              end
            endcase
         end
      end
   end

   // control key expander
   always @(posedge clk) begin
      if (!nrst) begin
         r_busy <= 0;
         pending_rounds <= 0;
         round_counter <= 0;
      end
      else begin
         if (ready && r_start) begin
            if (r_valid) begin
               if (pending_rounds == 0) begin
                  data_1 <= data_1_out;
                  data_2 <= data_2_out;
                  r_busy <= 0;
                  d_valid <= 0;
                  if (sconf[`SIMON_MMIO_REG_SCONF_MODE] == `SIMON_MODE_64_128) begin
                     if (round_counter == `SIMON_64_128_ROUNDS - 1)  begin
                        round_counter <= 0;
                     end
                     else begin
                        round_counter <= round_counter + 1;
                     end
                  end
                  else begin
                     if (round_counter == `SIMON_128_128_ROUNDS -1) begin
                        round_counter <= 0;
                     end
                     else begin
                        round_counter <= round_counter + 1;
                     end
                  end
               end
               else begin
                  pending_rounds <= pending_rounds - 1;
                  d_valid <= 1;
               end
            end
            else begin
               d_valid <= 1;
               r_busy <= 1;
               if (sconf[`SIMON_MMIO_REG_SCONF_ENC_DEC]) begin
                  round_key <= expanded[round_counter];
               end
               else begin
                  if (sconf[`SIMON_MMIO_REG_SCONF_MODE] == `SIMON_MODE_64_128) begin
                     round_key <= expanded[`SIMON_64_128_ROUNDS-round_counter];
                  end
                  else begin
                     round_key <= expanded[`SIMON_128_128_ROUNDS-round_counter];
                  end
               end
               if (sconf[`SIMON_MMIO_REG_SCONF_SINGLE]) begin
                  pending_rounds <= 0;
               end
               else begin
                  if (sconf[`SIMON_MMIO_REG_SCONF_MODE] == `SIMON_MODE_64_128) begin
                     pending_rounds <= `SIMON_64_128_ROUNDS;
                  end
                  else begin
                     pending_rounds <= `SIMON_128_128_ROUNDS;
                  end
               end
            end
         end // if (ready && r_start)
      end // else: !if(!nrst)
   end

endmodule

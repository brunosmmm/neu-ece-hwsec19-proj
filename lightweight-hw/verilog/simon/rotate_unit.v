`include "simon_common.vh"

module rotate_unit
  #(
    parameter integer amount = 1,
    parameter integer width = 32,
    parameter rl = `ROTATE_MODE_LEFT
    )
   (
    input [width-1:0] i_value,
    output [width-1:0] o_value
    );

   wire                right_left;
   assign right_left = rl;
   assign o_value = (right_left == `ROTATE_MODE_LEFT) ? {i_value[width-amount-1:0], i_value[width-1:width-amount]} :
                    {i_value[amount-1:0], i_value[width-1:amount]} ;

endmodule

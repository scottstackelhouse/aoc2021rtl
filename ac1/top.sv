
module top #(
    //ADDR_WIDTH=32,
    DATA_WIDTH=32
) (

    //apb responder_if
    input  logic I_CLK,
    input  logic I_RSTN,
    input  logic I_LAST,
    input  logic [DATA_WIDTH-1:0] I_DATA,
    output logic [DATA_WIDTH-1:0] O_COUNT,
    output logic O_LAST
);

logic [DATA_WIDTH-1:0] count;
logic [DATA_WIDTH-1:0] current_data;
logic [DATA_WIDTH-1:0] previous_data;
logic new_is_greater_than;
logic last_p1;
logic last_p2;


//pipe data through a set of registers for temp storage
//we reset to all ones to not get a false tick on the first
//sample
always_ff @(posedge I_CLK) begin : pipeline_reg_p
    if (I_RSTN == 0) begin
        current_data <= 2**DATA_WIDTH-1;
        previous_data <= 2**DATA_WIDTH-1;
    end else begin
        previous_data <= current_data;
        current_data <= I_DATA;
    end
end

//pipeline last signal to indicate completion of processing
always_ff @(posedge I_CLK ) begin
    if (I_RSTN == 0) begin
        last_p1 <= 0;
        last_p2 <= 0;
    end else begin
        last_p1 <= I_LAST;
        last_p2 <= last_p1;
    end
end

//assign last pipeline output 
assign O_LAST = last_p2;

//test if current data is > previous data
assign new_is_greater_than = current_data > previous_data;

//count if current_data is greater than previous_data
always_ff @(posedge I_CLK) begin : counter_p
    if (I_RSTN == 0) begin
        count <= 'd0;
    end else if (new_is_greater_than) begin
        count <= count + 1'b1;
    end
end

assign O_COUNT = count;

 // Print some stuff as an example
   initial begin
      if ($test$plusargs("trace") != 0) begin
         $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
         $dumpfile("logs/vlt_dump.vcd");
         $dumpvars();
      end
      $display("[%0t] Model running...\n", $time);
   end


endmodule;

module fsm_to_test (
	input clk, 
	input resetn,
	input [3:1] r,
	output [3:1] g
);

  typedef enum logic [1:0] {
	A = 2'b00,
	B = 2'b01,
	C = 2'b10,
	D = 2'b11
} state_t;

state_t present_state, next_state;

always @(*) begin
  	next_state = A;
	case(present_state)
      A: next_state = state_t'(
		(!r[1] & !r[2] & !r[3]) ? A :
		(!r[1] & !r[2] & r[3]) ? D:
		(!r[1] & r[2]) ? C:
        B);
		
      B: next_state = state_t'(
        (r[1]) ? B : A);
		
      C: next_state = state_t'(
        (r[2]) ? C : A);
		
      D: next_state = state_t'(
        (r[3]) ? D : A);
	endcase
end

always @ (posedge clk) begin 
  	if (!resetn) present_state <= A;
	else present_state <= next_state;
end


  assign g[1] = (present_state == B) ? 1 : 0;
  assign g[2] = (present_state == C) ? 1 : 0;
  assign g[3] = (present_state == D) ? 1 : 0;

endmodule
/*
 * Copyright (C) 2012 Tiberiu S. Petre
 * 
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

/* 
 * Adder example
 *
 *   - implementation of a simple adder using method ports
 *
 */

#include <iostream>
#include "method_ports.h"
using namespace std;

// Stimulus for the adder
struct adder_input {
	int a,b;
	adder_input() : a(0),b(0) {}
};

// The adder
struct adder {
	// Input/output method ports
	in_method_port<adder,adder_input> in_mp;
	out_method_port<adder,int> out_mp;
	// Implementation of the in-method port
	void in_transfer(adder_input &input) {
		int c;
		c= input.a + input.b;
		out_mp.transfer(c);
	}
	// Constructor
	adder() : in_mp(),out_mp() {
		in_mp.register_transfer(this,&adder::in_transfer);
	}
};

// Testbench
struct adder_tb {
	out_method_port<adder_tb,adder_input> out_mp;
	in_method_port<adder_tb,int> in_mp;
	
	// Drive stimulus on the adder's input port
	void run() {
		adder_input data;
		data.a=2;data.b=3;
		out_mp.transfer(data);
	}
	
	// Receive the result from the adder and display it
	void out_transfer(int &result) {
		cout << "Result is " << result << endl;
	}
	
	// Constructor
	adder_tb() : out_mp(), in_mp() {
		in_mp.register_transfer(this,&adder_tb::out_transfer);
	}

};

int main() {
	// Instantiate adder and testbench
	adder dut;
	adder_tb tb;
	// Connect the testbench to the adder
	tb.out_mp.bind(&dut.in_mp);
	dut.out_mp.bind(&tb.in_mp);
	// Run a simple test
	tb.run();

	return 0;
}


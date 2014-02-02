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
 * Producer-Consumer example
 *
 *   - the most simple scenario possible - a producer connected to a consumer
 *
 */

#include <iostream>
#include "method_ports.h"

using namespace std;

struct producer {
    // Out-method port
    out_method_port<producer,int> out_mp;
    
    // Send something through the out method port
    void run() {
	int x=777;
        out_mp.transfer(x);
    }

    // Constructor
    producer():out_mp("prod.out_mp") {} 
};

struct consumer {
    // In-method port
    in_method_port<consumer,int> in_mp;
    
    // Implementation of the in-method port
    // (receive something through the in-method port and do something with it)
    void transfer_fp(int &x) {
        cout << "X=" << x <<endl;
    }

    // Constructor
    consumer():in_mp("cons.in_mp") {
	// Bind the in-method port to its implementation
        in_mp.register_transfer(this,&consumer::transfer_fp);
    }
};

int main() {
    // Declare instances of producer and consumer
    producer prod;
    consumer cons;
    // Connect prod to cons
    // (bind the in and out method ports from the two instances)
    prod.out_mp(&cons.in_mp);
    // Call prod's run() method to send something to cons
    prod.run();

    return 0;
}


###############
# Introduction

MPL(Method Ports Library) is a software library written in C++ which provides support for simple transaction level modeling. You can use it to model any type of transaction-based system in an elegant and intuitive manner.

##############
# Example

struct producer {
    out_method_port<producer, int> out_mp; // Connection to consumer
    void run() {
        int x=22;
        out_mp.transfer(x); // send something to the consumer
    }
};

struct consumer {
    in_method_port<consumer, in> in_mp; // Connection to producer
    // Called automatically every time the consumer receives something from the producer
    void transfer(int& x) {
        // Do something with x ...
        cout<< "x=" << x << endl;
    }
    consumer():in_mp() {
        // Bind in-port to its implementation
        in_mp.register_transfer(this, &consumer::transfer);
    }
};

int main() {
    producer prod;
    consumer cons;
   
    // Connect them
    prod.out_mp(&cons.in_mp);
   
    // Run a test
    prod.run(); // This will output "x=22"
  
    return 0;
}


##############
# Instalation

No instalation is required. Just include the method_ports.h file in your application and you're ready to go.
Also, please have a look over the example *.cpp files for usage guidelines.

#############
# Copyright

This library is subject to the terms and conditions of the MIT License. This means that you can do whatever you want with it free of charge as long as you give me a little credit by mentioning me as a humble contributor to your application ;)


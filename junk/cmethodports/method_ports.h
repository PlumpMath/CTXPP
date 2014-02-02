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
 * Method Ports Library
 *
 *   - a library which offers support for simple Transaction Level Modeling
 *   - see example code for usage guidelines
 *
 */

#ifndef _METHOD_PORTS
#define _METHOD_PORTS

#include <iostream>
#include <string>

using namespace std;

// Auxiliary abstract classes
template <typename PAYLOAD>
struct method_port_base { virtual void transfer(PAYLOAD &p) = 0;};

template <typename PAYLOAD>
struct in_method_port_base:method_port_base<PAYLOAD> { };

template <typename PAYLOAD>
struct out_method_port_base:method_port_base<PAYLOAD> { };

///////////////////////////////////////////////////////
// in_method_port - to be instantiated in a consumer //
///////////////////////////////////////////////////////
template <typename MODULE, typename PAYLOAD>
struct in_method_port : in_method_port_base<PAYLOAD> {
    
    // Bind the in-method port to its implementation
    void register_transfer(MODULE* mod,void(MODULE::*t_fp)(PAYLOAD&)) {
	if(mod==NULL) 
		cout << "Warning : [in_method_port : " <<
			name << 
			".register_transfer(...)] - registering a NULL consumer. Transfers won't work." << endl;
	if(t_fp==NULL) 
		cout << "Warning : [in_method_port : " <<
			name << 
			".register_transfer(...)] - registering a NULL implementation. Transfers won't work." << endl;

        transfer_fp = t_fp;
        target = mod;
    }
    // Constructors
    in_method_port() {
        transfer_fp = NULL;
        target = NULL;
	name = "";
    }
    in_method_port(const char* nm) {
        transfer_fp = NULL;
        target = NULL;
	name=nm;
    }
	private:
    void (MODULE::*transfer_fp)(PAYLOAD&);
    MODULE* target;
    string name;
    // Called automatically when the in-port receives a payload
    virtual void transfer(PAYLOAD& p) {
	if(target==NULL || transfer_fp==NULL) {
		cout << "Warning : [in_method_port : " <<
			name << 
			".transfer(...)]" << endl <<
			"    In-method port is not connected properly."<< endl
			<<"    Transfer did not take place."<< endl <<
			"    Make sure you called register_transfer(...) with the right arguments." << endl;
		return;
	}
        (target->*transfer_fp)(p);
    }

};

////////////////////////////////////////////////////////
// out_method_port - to be instantiated in a producer //
////////////////////////////////////////////////////////
template <typename MODULE, typename PAYLOAD>
struct out_method_port:out_method_port_base<PAYLOAD> {
    // Binds the out-method port to an in-method port 
    void bind(in_method_port_base<PAYLOAD>* o) {
	if(o==NULL)
		cout << "Warning : [out_method_port : " <<
			name << 
			".bind(...)] - binding to a NULL port. Transfers won't work." << endl;
        other = o;
    }
    // Overloaded operator which does the same thing as bind
    void operator ()(in_method_port_base<PAYLOAD>* o) {
	bind(o);
    }
    // Call this method to transmit a payload to a consumer
    virtual void transfer(PAYLOAD &p) {
	if(other==NULL) {
		cout << "Warning : [out_method_port : " <<
			name << 
			".tarnsfer(...)]" << endl <<
			"    Out-method port is not connected properly."<< endl
			<<"    Transfer did not take place."<< endl <<
			"    Make sure you called bind(...) with the right argument." << endl;
		return;
	}
        other->transfer(p);
    }
    // Constructors
    out_method_port() {
	other = NULL;
	name = "";
    }
    out_method_port(const char* nm) {
	other = NULL;
	name = nm;
    }
	private:
    in_method_port_base<PAYLOAD>* other;
    string name;
};

#endif


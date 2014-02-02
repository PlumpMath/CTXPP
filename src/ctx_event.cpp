#include "ctxpp.h"


ctx_event::ctx_event(const char* n):
	components()
{
	ker = ctx_kernel::get();
	listeners=0;
	state=CTX_EVENT_OFF;
	name = n;
}

void ctx_event::consume() {
	listeners = (listeners>0)?listeners-1:0;
	if(listeners==0)
		state=CTX_EVENT_OFF;	
}

void ctx_event::add_listener() {
	listeners++;
}

void ctx_event::emit() {
	state=CTX_EVENT_ON;
	call_async_callbacks();
	ker->emitted_events.push_back(this);
	// TODO : add temporal expression logic
}

void ctx_event::register_component(ctx_component* c) {
	components.push_back(c);
}

void ctx_event::call_sync_callbacks() {
	std::vector<ctx_component*>::iterator c;
	for(c=components.begin();c!=components.end();++c)
		(*c)->call_sync_callbacks(this);
}

void ctx_event::call_async_callbacks() {
	std::vector<ctx_component*>::iterator c;
	for(c=components.begin();c!=components.end();++c)
		(*c)->call_async_callbacks(this);
}


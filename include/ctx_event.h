#ifndef CTX_EVENT_H
#define CTX_EVENT_H

typedef enum {
	CTX_EVENT_ON,
	CTX_EVENT_OFF
} ctx_event_state;

struct ctx_event {
	
	ctx_event(const char*);
	~ctx_event();
	
	ctx_kernel* ker;
	int listeners;
	std::string name;
	ctx_event_state state;
	
	std::vector<ctx_component*> components;

	virtual void consume();
	virtual void emit();
	virtual void register_component(ctx_component*);
	virtual void add_listener();
	
	virtual void call_sync_callbacks();
	virtual void call_async_callbacks();

};

#endif
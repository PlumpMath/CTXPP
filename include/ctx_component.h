#ifndef CTX_COMPONENT_H
#define CTX_COMPONENT_H

struct ctx_component {
	ctx_kernel* ker;
	ctx_component* parent;
	std::string name;
	ctx_component();
	ctx_component(const char*,ctx_component*);
	~ctx_component();
		
	virtual void call_sync_callbacks(ctx_event*);
	virtual void call_async_callbacks(ctx_event*);
	
	// Phases
	virtual void build_phase();
	virtual void connect_phase();
	virtual void run_phase();
	virtual void finalize_phase();
	
	// Messages
	virtual void message(std::string msg_id, std::string msg, ctx_verb verb);
	virtual void warning(std::string wrn_id, std::string wrn_msg);
	virtual void error(std::string err_id, std::string err_msg);
	virtual void enable_message(std::string);
	virtual void enable_warning(std::string);
	virtual void enable_error(std::string);
	virtual void disable_message(std::string);
	virtual void disable_warning(std::string);
	virtual void disable_error(std::string);
	void set_verbosity(ctx_verb);
private:
	ctx_verb verbosity;
	std::vector<std::string> disabled_messages;
	std::vector<std::string> disabled_errors;
	std::vector<std::string> disabled_warnings;
};

#define CTX_COMP_SETUP(NAME)\
	typedef NAME this_type;\
	typedef void(NAME::*EVENT_CB)();\
	std::multimap<ctx_event*,EVENT_CB> sync_callbacks;\
	std::multimap<ctx_event*,EVENT_CB> async_callbacks;\
	virtual void call_async_callbacks(ctx_event* e) {\
		EVENT_CB cb;\
		std::pair<std::multimap<ctx_event*,EVENT_CB>::iterator, std::multimap<ctx_event*,EVENT_CB>::iterator > range;\
		std::multimap<ctx_event*,EVENT_CB>::iterator it;\
		range = async_callbacks.equal_range(e);\
		for(it=range.first;it!=range.second;++it){\
			cb=it->second;\
			(this->*cb)();\
		}\
	}\
	virtual void call_sync_callbacks(ctx_event* e) {\
		EVENT_CB cb;\
		std::pair<std::multimap<ctx_event*,EVENT_CB>::iterator, std::multimap<ctx_event*,EVENT_CB>::iterator > range;\
		std::multimap<ctx_event*,EVENT_CB>::iterator it;\
		range = sync_callbacks.equal_range(e);\
		for(it=range.first;it!=range.second;++it){\
			cb=it->second;\
			(this->*cb)();\
		}\
	}\
	virtual void on_event_sync(ctx_event* ev,EVENT_CB cb) {\
		sync_callbacks.insert(std::pair<ctx_event*,EVENT_CB>(ev,cb));\
	}\
	virtual void on_event_async(ctx_event* ev,EVENT_CB cb) {\
		async_callbacks.insert(std::pair<ctx_event*,EVENT_CB>(ev,cb));\
	}

#endif
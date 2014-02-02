#include "ctxpp.h"

ctx_component::ctx_component() {}

ctx_component::~ctx_component() {}

ctx_component::ctx_component(const char* n, ctx_component* p) {
	ker = ctx_kernel::get();
	name=n;
	parent=p;
}

void ctx_component::message(std::string msg_id, std::string msg, ctx_verb verb) {
	std::vector<std::string>::iterator it;
	for(it = disabled_messages.begin();it!=disabled_messages.end();++it)
		if((*it)==msg_id)
			return;
	if(verb<=verbosity)
		std::cout<<"MESSAGE @"<<ker->time<<" ["<<msg_id<<"]:"<<msg<<std::endl;
}

void ctx_component::warning(std::string wrn_id, std::string wrn_msg) {
	std::vector<std::string>::iterator it;
	for(it = disabled_warnings.begin();it!=disabled_warnings.end();++it)
		if((*it)==wrn_id)
			return;
	std::cout<<"WARNING @"<<ker->time<<" ["<<wrn_id<<"]:"<<wrn_msg<<std::endl;	
}

void ctx_component::error(std::string err_id, std::string err_msg) {
	std::vector<std::string>::iterator it;
	for(it = disabled_errors.begin();it!=disabled_errors.end();++it)
		if((*it)==err_id)
			return;
	std::cout<<"ERROR @"<<ker->time<<" ["<<err_id<<"]:"<<err_msg<<std::endl;
	ker->state = CTX_KERNEL_FINISHED;
}

void ctx_component::enable_message(std::string id) {
	std::vector<std::string>::iterator it;
	for(it = disabled_messages.begin();it!=disabled_messages.end();++it)
		if((*it)==id)
			break;
	disabled_messages.erase(it);
}
void ctx_component::enable_warning(std::string id) { 
	std::vector<std::string>::iterator it;
	for(it = disabled_warnings.begin();it!=disabled_warnings.end();++it)
		if((*it)==id)
			break;
	disabled_warnings.erase(it);
}
void ctx_component::enable_error(std::string id) {
	std::vector<std::string>::iterator it;
	for(it = disabled_errors.begin();it!=disabled_errors.end();++it)
		if((*it)==id)
			break;
	disabled_errors.erase(it);
}
void ctx_component::disable_message(std::string id) {
	std::string ids = id;
	disabled_messages.push_back(ids);
}
void ctx_component::disable_warning(std::string id) {
	std::string ids = id;
	disabled_warnings.push_back(ids);
}
void ctx_component::disable_error(std::string id) {
	std::string ids = id;
	disabled_errors.push_back(ids);
}

void ctx_component::set_verbosity(ctx_verb v) {
	verbosity = v;
}

void ctx_component::call_sync_callbacks(ctx_event* e) { }
void ctx_component::call_async_callbacks(ctx_event* e) { }


void ctx_component::build_phase() { } 
void ctx_component::connect_phase() { } 
void ctx_component::run_phase() { } 
void ctx_component::finalize_phase() { }
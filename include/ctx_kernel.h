#ifndef CTX_KERNEL_H
#define CTX_KERNEL_H

typedef enum {
	CTX_KERNEL_RUNNING,
	CTX_KERNEL_PAUSED,
	CTX_KERNEL_FINISHED
} ctx_kernel_state;


struct ctx_kernel {
	
	friend class ctx_task_base;
	friend class ctx_event;
	friend class ctx_component;

	double time;
	long int delta;
	double time_unit;
	double time_scale;
	ctx_task_base* current_task;
	static ctx_kernel* m_inst;
	ctx_component* root;
	ctx_kernel_state state;
	

	ctx_kernel();
	~ctx_kernel();

	static ctx_kernel* get();

	// Simulation logic
	virtual void evaluate();
	virtual void update();
	virtual void advance_time(double);
	virtual void advance_delta(long int);
	virtual void advance_waiting_tasks();
	virtual void run(ctx_component*,double,long int);
	virtual void register_task(ctx_task_base*);
	virtual void unregister_task(ctx_task_base* task);
	
	virtual void register_component(ctx_component* c);
	
	virtual ctx_task_base* get_current_task();
	
	// Component related methods
	virtual void initialize_components();
	virtual void build_components(ctx_component*);
	virtual void connect_components(ctx_component*);
	virtual void run_components(ctx_component*);
	virtual void schedule_resume_task(ctx_task_base*);
	virtual void schedule_start_task(ctx_task_base*);
	void get_root_components(std::vector<ctx_component*>&);
	void get_leaf_components(std::vector<ctx_component*>&);
	void get_child_components(ctx_component*,std::vector<ctx_component*>&);
	
	bool tasks_running();
	
	// Getters and setters
	void set_root(ctx_component*);
	virtual void set_tasks_were_killed();

	private:
	std::vector<ctx_component*> components;
// 	std::vector<ctx_task_base*> tasks;
	// Task management lists
	std::list<ctx_task_base*> running_tasks;
	std::list<ctx_task_base*> pending_tasks;
	std::list<ctx_task_base*> finished_tasks;
	std::vector<ctx_task_base*> new_tasks;
	std::list<ctx_task_base*> tasks_consuming_time;
	// Kernel state
	//std::map<ctx_task_base*,double> times_to_wait;
	std::map<ctx_task_base*,ctx_event*> events_to_wait;
	std::vector<ctx_event*> emitted_events;
	std::vector<ctx_event*> events_to_fire;
	std::map<ctx_task_base*,ctx_type> context_switches;
	std::map<ctx_task_base*,ctx_task_base*> tasks_to_join;
	std::map<ctx_task_base*,std::vector<ctx_task_base*>*> tasks_to_fork;
	std::vector<ctx_signal*> signals;
	
	// For simulation time management
	std::set<double> times_to_wait;
	double dq;
	double delta_t;
	
	bool with_real_time;
	bool tasks_were_killed;
	
	

};

#endif
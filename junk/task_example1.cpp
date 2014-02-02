
// Implement task
CS_TASK(drive_packet)

	// Declare fields(optional)
	
	// Declare return type(default is int)
	CS_RETURNS(double*)
	
	// Implement constructor(optional)
	
	// Add other methods(optional)

CS_BEGIN_BODY
	
	// Do some stuff here
	
	// Wait abstract time
	CS_DELAY(104);
	
	// Wait standard time units
	CS_DELAY_STD(25,SIM_S);

	// Wait events
	CS_WAIT(ev);

	// Emit events
	ev.emit();
	
	// Join single task
	ctx_task* task1 = new Task1(arg1,arg2,arg3,...);
	CS_JOIN(task1);

	// Fork-join other tasks
	task_list.add(task1);
	task_list.add(task2);
	task_list.add(task3);
	// Wait for all to finish 
	// then resume 
	CS_ALL_OF(task_list);
	// Wait for one to finish
	// then kill all others
	// and resume
	CS_FIRST_OF(task_list);
	// Wait for one to finish
	// and resume
	// (don't kill the others)
	CS_ANY_OF(task_list);
	// Don't wait for any one to finish
	// Start them all and resume
	fork(task_list);
	start(Task1::get_instance());
	
	// Stop current task
	CS_STOP;

	// Kill another task
	some_other_task.kill();

	// Stop simulation
	CS_FINISH;

	// Make an empty context switch
	CS_YIELD;

	// Return a value
	CS_RETURN_VALUE(x);

	/* Examples */

	// Pull an item from a get port
	ctx_task* pull_task = some_get_port.get_pull_task(&item);
	CS_JOIN(pull_task);
	cout<<item;
	
CS_END_BODY



provider fixsed 
{
	/* 
	 * This pair of probes can be used to measure the time it takes a lua script to 
	 * process a message. 
	 */
	probe script__call(char* SenderCompID, char* TargetCompID, char* MsgType);
	probe script__return(char* SenderCompID, char* TargetCompID, char* MsgType);
};

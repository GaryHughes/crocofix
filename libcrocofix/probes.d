provider crocofix 
{
	probe session__message__read(char* SenderCompID, char* TargetCompID, char* MsgType);
	probe session__message__write(char* SenderCompID, char* TargetCompID, char* MsgType);
};

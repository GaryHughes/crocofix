provider crocofix 
{
	probe session__message__read(char* MsgType);
	probe session__message__write(char* MsgType);
};

/* 
    This is dummy for non macOS builds so we can avoid conditionally creating it in CMakeLists.txt 
    and so we can avoid ifdefing the usage of these macros in code 
*/

#define	CROCOFIX_SESSION_MESSAGE_READ(arg0) do {} while (0)
#define	CROCOFIX_SESSION_MESSAGE_READ_ENABLED() (0)
#define	CROCOFIX_SESSION_MESSAGE_WRITE(arg0) do {} while (0)
#define	CROCOFIX_SESSION_MESSAGE_WRITE_ENABLED() (0)

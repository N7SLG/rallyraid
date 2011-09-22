
#ifndef ERROR_H
#define ERROR_H


#ifdef __cplusplus
//extern "C" {
#endif

/* generate a fatal error, debug trap or a message. */
void switchPrintDestination();
void PrintError(int num, const char *msg, ...);
void PrintMessage(int num, const char *msg, ...);

#ifdef __cplusplus
//}
#endif

#endif // ERROR_H

#include <backend/api/RESTfulAPI.hpp>

/* 
void signalHandler(int signal)
{
    switch (signal) {
        case SIGABRT: std::cerr << "Received SIGABRT: Abnormal termination triggered." << std::endl; break;
        case SIGSEGV: std::cerr << "Received SIGSEGV: Segmentation fault occurred." << std::endl; break;
        case SIGINT: std::cerr << "Received SIGINT: Program interrupted (Ctrl+C)." << std::endl; break;
        case SIGILL: std::cerr << "Received SIGILL: Illegal instruction." << std::endl; break;
        case SIGFPE: std::cerr << "Received SIGFPE: Erroneous arithmetic operation." << std::endl; break;
        case SIGTERM: std::cerr << "Received SIGTERM: Termination request." << std::endl; break;
        case SIGHUP: std::cerr << "Received SIGHUP: Hangup." << std::endl; break;
        case SIGQUIT: std::cerr << "Received SIGQUIT: Quit." << std::endl; break;
        case SIGTRAP: std::cerr << "Received SIGTRAP: Trace/breakpoint trap." << std::endl; break;
        case SIGKILL: std::cerr << "Received SIGKILL: Killed." << std::endl; break;
        case SIGBUS: std::cerr << "Received SIGBUS: Bus error." << std::endl; break;
        case SIGSYS: std::cerr << "Received SIGSYS: Bad system call." << std::endl; break;
        case SIGPIPE: std::cerr << "Received SIGPIPE: Broken pipe." << std::endl; break;
        case SIGALRM: std::cerr << "Received SIGALRM: Alarm clock." << std::endl; break;
        case SIGURG: std::cerr << "Urgent data is available at a socket." << std::endl; break;
        case SIGSTOP: std::cerr << "Stop, unblockable." << std::endl; break;
        case SIGTSTP: std::cerr << "Keyboard stop." << std::endl; break;
        case SIGCONT: std::cerr << "Continue." << std::endl; break;
        case SIGCHLD: std::cerr << "Child terminated or stopped." << std::endl; break;
        case SIGTTIN: std::cerr << "Background read from control terminal." << std::endl; break;
        case SIGTTOU: std::cerr << "Background write to control terminal." << std::endl; break;
#ifdef __linux__
        case SIGPOLL: std::cerr << "Pollable event occurred (System V)." << std::endl; break;
#endif
        case SIGXCPU: std::cerr << "CPU time limit exceeded." << std::endl; break;
        case SIGXFSZ: std::cerr << "File size limit exceeded." << std::endl; break;
        case SIGVTALRM: std::cerr << "Virtual timer expired." << std::endl; break;
        case SIGPROF: std::cerr << "Profiling timer expired." << std::endl; break;
        case SIGUSR1: std::cerr << "User-defined signal 1." << std::endl; break;
        case SIGUSR2: std::cerr << "User-defined signal 2." << std::endl; break;
        default: std::cerr << "Received signal: " << signal << std::endl; break;
    }
    // Optionally exit the program after handling the signal

    std::exit(signal);
}
 */

int main(int argc, char ** argv) 
{
    RESTfulAPI::instance();

    return EXIT_SUCCESS;
}
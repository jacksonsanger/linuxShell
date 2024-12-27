# Mini Linux Shell

A simplified Linux shell implemented in C, supporting core shell features, command execution, input/output redirection, and background process management.

## Features

- **Command Execution**: Executes built-in commands (`cd`, `exit`) and system commands (`ls`, `ps`) using `fork()` and `execvp()`.
- **Custom Prompt**: Displays the current working directory in the prompt, dynamically updated using `getcwd()`.
- **Input/Output Redirection**: Supports `>` and `<` for redirecting output and input streams, implemented using `freopen()` to remap file descriptors.
- **Background Processes**: Handles background execution of commands via `&` and reaps zombie processes using `SIGCHLD` signal handlers.

## Implementation Details

- **Tokenization**: User input is parsed into individual tokens using `strtok()`. The resulting token array is formatted to be compatible with `execvp()`, ensuring proper command execution.
- **Redirection Handling**: Custom helper functions identify and process redirection tokens (`>` and `<`), removing them from the command array and remapping streams before execution.
- **Background Execution**: Commands ending with `&` are executed in the background, allowing the shell to continue processing other user inputs. Zombie processes are cleaned up using signal handlers.
- **Error Handling**: Implements robust error checks for invalid commands, directory changes (`chdir()`), and redirection operations.

## Usage

1. Run the shell to interact with the command line.
2. Use standard Linux commands or built-in commands:
   - `cd <directory>`: Change the working directory.
   - `exit`: Exit the shell.
3. Redirect input and output as needed:
   - `ls > output.txt`: Redirects the output of `ls` to `output.txt`.
   - `cat < input.txt`: Reads input from `input.txt`.
4. Execute commands in the background:
   - `sleep 10 &`: Runs the `sleep` command in the background.

## Key Highlights

- **System Calls**: Utilizes `fork()`, `execvp()`, and `waitpid()` to manage process creation and execution.
- **Signal Handling**: Implements a `SIGCHLD` signal handler to manage background process lifecycle and prevent zombies.
- **Interactive Prompt**: Dynamically updates the shell prompt to reflect the current working directory.
- **Error Messages**: Provides meaningful feedback for invalid commands, failed redirections, or inaccessible directories.

## Future Enhancements

Potential improvements include:
- Adding support for interactive background processes (e.g., `vim`).
- Implementing additional built-in commands (e.g., `jobs`, `fg`, `bg`).
- Enhancing user feedback for background process completion.

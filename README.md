# MumShell
A home-made shell for VE482 project 1.

## Supported Functions (150/150)

*More detailed specifications can be found [here](https://github.com/tc-imba/VE482-FA2018-public/blob/master/p1/specifications/p1_specifications.pdf).*

1. Write a working read/parse/execute loop and an `exit` command; [5]
2. Handle single commands without arguments (e.g. `ls`); [5]
3. Support commands with arguments (e.g. `apt-get update` or `pkgin update`); [5]
4. File I/O redirection: [5+5+5+2]
    1. Output redirection by overwriting a ﬁle (e.g. `echo 123 > 1.txt`);
    2. Output redirection by appending to a ﬁle (e.g. `echo 465 >> 1.txt`);
    3. Input redirection (e.g. `cat < 1.txt`);
    4. Combine 4.1 and 4.2 with 4.3;
5. Support for bash style redirection syntax (e.g. `cat < 1.txt 2.txt > 3.txt 4.txt`); [8]

6. Pipes: [5+5+5+10]
    1. Basic pipe support (e.g. `echo 123 | grep 1`);
    2. Run all ‘stages’ of piped process in parallel. (e.g. `yes ve482 | grep 482`);
    3. Extend 6.2 to support requirements 4. and 5. (e.g. `cat < 1.txt 2.txt | grep 1 > 3.txt`);
    4. Extend 6.3 to support arbitrarily deep “cascade pipes” (e.g. `echo 123 | grep 1 | grep 1 | grep 1`)

*Note: the sub-processes must be reaped in order to be awarded the marks.*

7. Support CTRL-D (similar to bash, when there is no/an unﬁnished command); [5]

8. Internal commands: [5+5+5]
    1. Implement `pwd` as a built-in command;
    2. Allow changing working directory using `cd`;
    3. Allow `pwd` to be piped or redirected as speciﬁed in requirement 4.;

9. Support CTRL-C: [5+3+2+10]
    1. Properly handle CTRL-C in the case of requirement 4.;
    2. Extend 9.1 to support subtasks 6.1 to 6.3;
    3. Extend 9.2 to support requirement 7., especially on an incomplete input;
    4. Extend 9.3 to support requirement 6.;

10. Support quotes: [5+2+3+5]
    1. Handle single and double quotes (e.g. `echo "de'f' ghi" '123"a"bc' a b c`);
    2. Extend 10.1 to support requirement 4. and subtasks 6.1 to 6.3;
    3. Extend 10.2 in the case of incomplete quotes (e.g. Input `echo "de`, hit enter and input `cd"`);
    4. Extend 10.3 to support requirements 4. and 6., together with subtask 9.3;

11. Wait for the command to be completed when encountering >, <, or |: [3+2]
    1. Support requirements 3. and 4. together with subtasks 6.1 to 6.3;
    2. Extend 11.1 to support requirement 10.;

12. Handle errors for all supported features. [10] Note: a list of test cases will be published at a later stage. Marks will be awarded based on the number of cases that are correctly handled, i.e. if only if:

    * A precise error message is displayed (e.g. simply saying “error happened!” is not enough);

    * The program continues executing normally after the error is identiﬁed and handled;

13. A command ending with an & should be run in background. [10]

    1. For any background job, the shell should print out the command line, prepended with the job ID and the process ID (e.g. if the two lines `/bin/ls &` and `/bin/ls | cat &` are input the output could be the two lines [1] `(32757) /bin/ls &` and [2] `(32758) (32759) /bin/ls | cat &` );
    2. Implement the command jobs which prints a list of background tasks together with their running states (e.g. in the previous case output the two lines [1] `done /bin/ls &` and [2] `running /bin/ls | cat &`);
    
## Testcase

Testcases can be found [here](https://github.com/tc-imba/VE482-FA2018-public/tree/master/p1/test) which can be driven by the driver [here](https://github.com/tc-imba/VE482-FA2018-public/tree/master/p1/driver). Credit to @tc-imba.

## Known Issues

* Currently I can't figure out why the pipe file close of the subprocess will affect the pipe file close of the main processs.

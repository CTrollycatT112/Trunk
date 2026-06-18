## How to contribute

Right now, I don't have very strict contribution rules.
Trunk is a basic hobby operating system, not a production ready real world operating system.
However, there is some rules to follow

1. Any new file, function, or section requires you to go to 'dev/COMMENTS.cpp' and copy the comment format.
2. When making a pull request, create a new branch with the name of your feature, ex: "Driver: Keyboard"
3. Make sure when writing code, use modern C++ 23 style.
4. Make sure that all of your .cpp or .h files follow our namespace structure, for tklib it's just tklib:: for all of them, however for trunk. it's trunk::subsystem, ex: trunk::gdt
   The only exception is Drivers, which are 3 layers. trunk::drivers::NAME, ex: trunk::drivers::serial
5. Make sure to test your contribution throughly, test every feature, add debug messages, make sure it works 100%.
6. Contributing mean's your code is passed under Apache 2.0 license.
7. You also need to add your information to CREDITS.

8. PascalCase vs snake_case

So for the start of the project, I planned to use snake_case.
However, I soon learned I prefer PascalCase.
But it's too late to change everything.

So I've established a rule, It may not be the most consistent, but this Is a hobby project, It's fine.

Everything will be snake_case, EXCEPT:

1. Boot functions (specifically, TrSystemStartup, Trkload(), and TrkStartup())
2. API functions (internal are snake_case, but api functions (like kmalloc()) shall be PascalCase preferably).

A note on API functions:
It's only considered an 'API function' If it's a part of a system that developers will call a lot
For example, RegisterInterruptHandler is an 'API function' because devs call it in normal code
But, execute_interrupt_handler is not, because it's called internally.

Loggers such as Serial are not included.

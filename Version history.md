CSOPSEY MO1 v1
- Complete command implementations.
- The logic for computing cpu utilization is questionable to say the least.
- Scheduler is not yet synchronize with cpu clock. 
- Incorrect interpretations of program parameters. batch-process-freq and delay-per-exec values are interpreted as miliseconds when it should have been interpreted as cpu cycles.

CSOPSEY MO1 v2
- Fixed all issues of v1.

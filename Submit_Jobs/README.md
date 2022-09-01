# How to submit jobs using HTCondor?

The file fct_job.sh contains the executable you want to run and submit as a job to the farm.
First, it loads in the environment that is given in o2_env.sh, then it executes the command to run an O2 simulation.
ALIBUILD_WORK_DIR needs to be changed for the every user (work directory of alibuild)
However, when a job is submitted to HTCondor, HTCondor cannot just access any files (o2_env.sh in this case) in the same folder as where the job is submitted from becasue it doesn't now.
You have to tell HTCondor which files to provide and "pass on to" the job. This is done in the fct_job.sub file.

The fct_job.sub file tells HTCondor
- The executable (executable =)
- Where to put the log (log =)
- Where to put the executable's output that normally gets outputted to the terminal (output =)
- Where, if any, to put the errors it encountered during running (error =)
- Whether to pass on the environment loaded (getenv =). I am not sure if this is needed becasue you already pass on the environment in the form of a file (o2_env.sh)
- Whether to transfer files (should_transfer_files =).
- The initial directory where the output of individual jobs will be placed (initialdir). In the example this is the "job" directory 0 to 19
- Which input files are needed (transfer_input_files =). Here you also need to put any other input files you might need, like for FCTBase.configFile =
- Tells the requirements, for example on which machine it needs to ru (requirements =)\
- How many jobs it needs to submit (queue =). In the example this is 20 jobs (0 to 19).

These jobs are submitted with
```
condor_submit fct_job.sub
```

The environment in the file o2_env.sh is created by running prepare_env.sh

o2_env.sh is depended on which version of O2 you use, therefore this is only an example. I encourage you to take a look inside of this file and try to understand what is going on and which paths are chosen. These are the paths that are used when alienv is loaded for O2. It will help you if you understand at least a little of what is going on :)

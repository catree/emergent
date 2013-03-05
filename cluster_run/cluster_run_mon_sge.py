#!/usr/bin/env python
from cluster_run_mon_lib import *
# get all the base code -- this file should be on the python path or 
# (more simply) just copied over to same directory where this runs
# suggest putting both in ~/cluster_run_<cluster_name>

#############################################################################
# STANDARD USER CONFIGURABLE PARAMETERS

# name of queue -- used for a few things -- replace with actual!
clust_queue = ""

# full path to single processor job submission script
# STRONGLY recommend using the pyqsub based commands avail in 
# emergent/cluster_run/ directory (where this script lives as well)
#
# the sp_qsub_cmd takes args of <n_threads> <run_time> <full_command>
sp_qsub_cmd = 'sp_qsub_q'
# can add an automatic -q <queue> arg here to specify a queue
# in general it is best to have a different script for each queue
# because the emergent preferences have relevant settings for them
# sp_qsub_args = "-q " + clust_queue
sp_qsub_args = ""

# the dm_qsub_cmd takes args of <mpi_nodes> <n_threads> <run_time> <full_command>
dm_qsub_cmd = 'dm_qsub_q'
# dm_qsub_args = "-q " + clust_queue
dm_qsub_args = ""

# it is essential that these scripts return the cluster job number in the format
# created: JOB.<jobid>.sh -- we parse that return val to get the jobid to monitor
# further (you can of course do this in some other way by editing code below)

# qstat-like command -- for quering a specific job_no 
# sge = qstat -j <job_no>
# moab = qstat <job_no>
# job_no will automatically be appended to end of command
qstat_cmd = "qstat"
qstat_args = "-j"  # here is where you put the -j if needed

# parser for qstat output -- output is different so need diff parsers
# options are sge for sge, moab for moab/torque
qstat_parser = "sge"

# qdel-like command -- for killing a job
# killjob is a special command that also deletes the JOB.* files -- see pykilljob
# be sure to use the _f version that does not prompt!
# in emergent/cluster_run directory
# job_no will automatically be appended to end of command
qdel_cmd = "killjob_f"
qdel_args = ""

# showq-like command -- this should return overall status of all users jobs
# and general info on status of cluster
# moab = showq 
# pyshowq for SGE (checked into emergent/cluster_run showq)
showq_cmd = "showq"
showq_args = ""
# parser function to use for showq output -- complex enough that this is most efficient way to do it
showq_parser = "pyshowq"

# number of runtime minutes during which the script will continue to update the 
# output info from the job (job_out, dat_files)
job_update_window = 3

# set to true for more debugging info
debug = False
#debug = True

# END OF STANDARD USER CONFIGURABLE PARAMETERS
#############################################################################

if __name__ == '__main__':
    try:
        if len(sys.argv) == 1:
            main()
        else:
            main_background()
    except KeyboardInterrupt:
        print '\n\nQuitting at user request (Ctrl-C).'
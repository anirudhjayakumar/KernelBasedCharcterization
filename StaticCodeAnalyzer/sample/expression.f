      program cg
c---------------------------------------------------------------------
c---------------------------------------------------------------------


      implicit none
      integer nz,na,nonzer,num_procs,num_proc_cols
      nz = na*(nonzer+1)/num_procs*(nonzer+1)+nonzer
     >              + na*(nonzer+2+num_procs/256)/num_proc_cols

      end

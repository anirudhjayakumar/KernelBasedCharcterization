      program sample

      implicit none
      integer b, c
      c = 9
      call fun(c,b)
      end

      subroutine fun(a,b)
      integer a,b
      b=a + 1
      return
      end




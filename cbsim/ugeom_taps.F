                      subroutine UGEOM_taps

************************************************************************
*               Forward Wall                                           *
*                                                                      *
* Viktor Kashevarov                                                    *
************************************************************************
      implicit none

#include "materials.inc"
#include "gctmed.inc"
#include "g77_intrinsic.inc"

      integer
     1  IVOLU     ! system volume number returned by GSVOLU
     1, ndx       ! number of detectors along x 
     1, ndy       ! number of detectors along y
     1, ix(552), iy(552)    ! loop indexes
     1, index(552)
     1, i
c     1, nd, nd1     
     1, isw        

      real
     1  set0      !distance from target to front of TAPS
     1, par(10)   ! volume parameters 
     1, rin, rof, ax1, ay1, az  
     1, t_veto, z_vbox, z_Al, t_covr
     1, x(552), y(552), z, z1, z2
     1, rmax, rpl, rmi

c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
      set0 = 175.    !distance from target to front of TAPS

      ndx=33           !number of detectors along x 
      ndy=24           !number of detectors along y
      rin=3.               !radius of inscribed circle [cm]
      rof=2*rin/sqrt(3.)   !radius of circumscribed circle
      ax1=(3*ndx+1)*rof/2.                !x 
      ay1=2*ndy*rin                       !y 
      az=25.                              !z

      t_veto = 0.5    ! thickness of veto detector
      z_vbox = 10.    ! z size of Veto box
      z_Al = 50.      ! z size of Al box
      t_covr = 0.05   ! thickness of covering for BaF2

c****************************************************************
c=== Forward Wall Mother Volume ===========>
        PAR(1)=0.
        PAR(2)=360.
        PAR(3)=6.
        PAR(4)=2.
        PAR(5)=-(z_vbox + z_Al)/2.
        PAR(6)=0.
        PAR(7)=74.5
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('FWMV','PGON',air,PAR,10,IVOLU)
        z = set0 + (z_Al - z_vbox)/2.
      call GSPOS('FWMV',1,'LABB',0.,0.,z,0,'ONLY')

c======= covering for BaF2 =========>
        PAR(5)=-az/2.
        PAR(6)=0.
        PAR(7)=rin
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('COVR','PGON',PVC,PAR,10,IVOLU)

c======= BaF2 crystal ==============>
        PAR(5)=-(az-2.5)/2.
        PAR(6)=0.
        PAR(7)=rin-t_covr
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('TAPS','PGON',BaF2,PAR,10,IVOLU)
      call GSPOS('TAPS',1,'COVR',0.,0.,-1.25,0,'ONLY')

c======= dummy =========>
        PAR(5)=-az/2.
        PAR(6)=0.
        PAR(7)=rin
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('DUMM','PGON',PVC,PAR,10,IVOLU)

c=== Veto detector box (front and back walls) =======>
        PAR(5)=-0.1    !1mm wall thickness + 1mm as effective
                       !thickness of the light guide fibers
        PAR(6)=0.
        PAR(7)=72.5
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('VDB1','PGON',PVC,PAR,10,IVOLU)
        z = -(z_vbox + z_Al)/2. - par(5)
      call GSPOS('VDB1',1,'FWMV',0.,0.,z,0,'ONLY')

        PAR(5)=-0.175   !3mm wall thickness + 0.5mm
                        !thickness of covering for BaF2
        PAR(8)=-par(5)
      call GSVOLU('VDB2','PGON',PVC,PAR,10,IVOLU)
        z = -(z_vbox + z_Al)/2. + z_vbox + par(5)
      call GSPOS('VDB2',1,'FWMV',0.,0.,z,0,'ONLY')

c======= Veto detector ==============>
        PAR(5)=-t_veto/2.
        PAR(6)=0.
        PAR(7)=rin
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('TVET','PGON',scint,PAR,10,IVOLU)

c========== Al box for TAPS =============>
        PAR(5)=-25.
        PAR(6)=72.5
        PAR(7)=74.5
        PAR(8)=-par(5)
        PAR(9)= par(6)
        PAR(10)=par(7)
      call GSVOLU('ABOX','PGON',Al,PAR,10,IVOLU)
        z = z_vbox/2.
      call GSPOS('ABOX',1,'FWMV',0.,0.,z,0,'ONLY')

c======= BaF2 crystal(cylider part) ==============>
        PAR(1)=0.
        PAR(2)=2.7
        PAR(3)=1.25
      call GSVOLU('TEND','TUBE',BaF2,PAR,3,IVOLU)
      call GSPOS('TEND',1,'COVR',0.,0.,11.25,0,'ONLY')
c======= Air(for the cylider part) ==============>
        PAR(1)=par(2)
        PAR(2)=5.9/2.
        PAR(3)=1.25
      call GSVOLU('AIRC','TUBE',air,PAR,3,IVOLU)
      call GSPOS('AIRC',1,'COVR',0.,0.,11.25,0,'ONLY')
*******************************************************************

c=== positioning COVR(+BaF2), Veto, and DUMM =========>

        z1 = -(z_Al - z_vbox - az)/2.              !COVR(+BaF2)
        z2 = -(z_Al - z_vbox + t_veto)/2. - 0.35   !Veto

	open (unit=1, file='taps.dat')
	do i=1, 552
c	do i=1, 127
c	do i=0, 551
	read (1, 1001) index(i), ix(i), iy(i)
1001    format (5x, i3, 3x, i3, 3x, i3)
		x(i) = ix(i) * 1.5 * 6.0 / sqrt(3.)
		y(i) = iy(i) * 6. / 2.

c		print  *, i, x(i), y(i), set0
c		if(index(i).lt.510.)then
		if(index(i).lt.510.)then
			call GSPOS('COVR',i,'FWMV',x(i),y(i),z1,0,'ONLY')
        		call GSPOS('TVET',i,'FWMV',x(i),y(i),z2,0,'ONLY')
c			call GSPOS('COVR',index(i),'FWMV',x(i),y(i),z1,0,'ONLY')
c        		call GSPOS('TVET',index(i),'FWMV',x(i),y(i),z2,0,'ONLY')
c		else if(index(i).lt.552.)then
		else if(index(i).le.552.)then		
        		call GSPOS('DUMM',i,'FWMV',x(i),y(i),z1,0,'ONLY')
c			call GSPOS('DUMM',index(i),'FWMV',x(i),y(i),z1,0,'ONLY')
		endif

	enddo

c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

      return
      end
c====================================================================!

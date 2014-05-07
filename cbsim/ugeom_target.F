               subroutine UGEOM_target

***********************************************************************
*      tmat = LiqH2   -->  LH2 target                                 *
*      tmat = LiqD2   -->  LD2 target                                 *
***********************************************************************
* Viktor Kashevarov                                                   *
***********************************************************************

       implicit none
#include "materials.inc"
#include "g77_intrinsic.inc"

      integer
     1 IVOLU       ! system volume number returned by GSVOLU
     
      real
     1   par(6)   ! volume parameters
     1,  r_CFK
     1,  r_Cu
     1,  r_KAPT
     1,  rmax
     1,  t_CFK
     1,  t_Cu
     1,  t_KAPT
     1,  theta_1
     1,  theta_2
     1,  z
     1,  zc
     1,  zm
     1,  z_Cu
     1,  z_KAPT
     1,  z0_LD2
     1,  z1_LD2
************************************************************

c======================================================================
        zm = (27.3+0.66)/2         ! z/2 for mother volume (CFK-Rohr)
        r_CFK = 4.1                ! radius
        t_CFK = 0.1                ! thickness
          z_KAPT = 18.5
          r_KAPT = 2.
          t_KAPT = 0.012
        r_Cu = 1.3
        t_Cu = 0.1
          z0_LD2 = 3.76       ! cylindrical part of target
          z1_LD2 = 0.5        ! spherical part of target
c target lenght = z0_LD2 + 2*z1_LD2

        theta_1 = 180. - 2*atand(r_KAPT/z1_LD2)
        rmax = r_KAPT/sind(theta_1)
        theta_2 = 180. - asind(r_Cu/rmax)

      z_Cu = z_KAPT - z0_LD2 - z1_LD2 + 
     &       rmax - sqrt(rmax**2 - r_Cu**2)
c======================================================================

c========== mother volume  ===========>
      PAR(1) = 0.           
      PAR(2) = r_CFK        
      PAR(3) = zm
      call GSVOLU('TRGT','TUBE',vacuum,PAR,3,IVOLU) 
        zc = zm - (z_KAPT - z0_LD2/2)       ! -2.55      
      call GSPOS('TRGT',1,'CCUT',0.,0.,zc,0,'ONLY')   

c=========== CFK tube + window ===============>
      PAR(1) = r_CFK - t_CFK           
      PAR(2) = r_CFK         
      PAR(3) = zm - 0.13           !0.26 --> Fla1
      call GSVOLU('CFKT','TUBE',carbon,PAR,3,IVOLU) 
        z = -0.13      
      call GSPOS('CFKT',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(1) = 3.           
      PAR(2) = r_CFK        
      PAR(3) = 0.13       !0.26/2
      call GSVOLU('FLA1','TUBE',Al,PAR,3,IVOLU) 
        z = zm - par(3)       
      call GSPOS('FLA1',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(2) = r_CFK - t_CFK     !- delta       
      PAR(1) = par(2) - 0.1           
      PAR(3) = 0.2            !0.4/2
      call GSVOLU('FLA2','TUBE',Al,PAR,3,IVOLU) 
        z = zm - 0.26 - par(3)       
      call GSPOS('FLA2',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(1) = 0.        
      PAR(2) = 3.           
      PAR(3) = t_KAPT/2
      call GSVOLU('KAPD','TUBE',kapton,PAR,3,IVOLU) 
        z = zm - par(3)       
      call GSPOS('KAPD',1,'TRGT',0.,0.,z,0,'ONLY')   

c======= Kapton tube + windows ===================>
      PAR(1) = r_KAPT - t_KAPT                   !R_min
      PAR(2) = r_KAPT               !R_max
      par(3) = z_KAPT/2.            !z/2
      call GSVOLU('KAPA','TUBE',kapton,PAR,3,IVOLU)        
        z=par(3)-zm      
      call GSPOS('KAPA',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(1) = rmax - t_KAPT              !R_min
      PAR(2) = rmax            !R_max
      par(3) = 0.              !theta_min        
      par(4) = theta_1         !theta_max
      par(5) = 0.              !phi_min
      par(6) = 360.            !phi_max
      call GSVOLU('KAPB','SPHE',kapton,PAR,6,IVOLU)        
        z = (z_KAPT + z1_LD2 - rmax) - zm      
      call GSPOS('KAPB',1,'TRGT',0.,0.,z,0,'MANY')   

      PAR(1) = rmax - t_KAPT              !R_min
      PAR(2) = rmax            !R_max
      par(3) = theta_2         !theta_min        
      par(4) = 180.            !theta_max
      par(5) = 0.              !phi_min
      par(6) = 360.            !phi_max
      call GSVOLU('KAPC','SPHE',kapton,PAR,6,IVOLU)        
        z = (z_KAPT - z0_LD2 - z1_LD2 + rmax) - zm      
      call GSPOS('KAPC',1,'TRGT',0.,0.,z,0,'MANY')   

c======= LH2 or LD2 ===================>
      PAR(1) = 0.                           !R_min
      PAR(2) = r_KAPT - t_KAPT              !R_max
      par(3) = (z_KAPT - z_Cu)/2.           !z/2
      call GSVOLU('LD2A','TUBE',tmat,PAR,3,IVOLU)
        z = (z_KAPT - par(3)) - zm      
      call GSPOS('LD2A',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(2) = rmax-t_KAPT        !R_max
      PAR(1) = par(2) - z1_LD2    !R_min
      par(3) = 0.                 !theta_min        
      par(4) = theta_1            !theta_max
      par(5) = 0.                 !phi_min
      par(6) = 360.               !phi_max
      call GSVOLU('LD2B','SPHE',tmat,PAR,6,IVOLU)        
        z = (z_KAPT + z1_LD2 - rmax) - zm      
      call GSPOS('LD2B',1,'TRGT',0.,0.,z,0,'MANY')   

      PAR(2) = rmax - t_KAPT       !R_max
      PAR(1) = par(2) - z1_LD2     !R_min
      par(3) = theta_2             !theta_min        
      par(4) = 180.                !theta_max
      par(5) = 0.                  !phi_min
      par(6) = 360.                !phi_max
      call GSVOLU('LD2C','SPHE',tmat,PAR,6,IVOLU)        
        z = (z_KAPT - z0_LD2 - z1_LD2 + rmax) - zm      
      call GSPOS('LD2C',1,'TRGT',0.,0.,z,0,'MANY')   

      PAR(1) = r_Cu                   !R_min
      PAR(2) = r_KAPT-t_KAPT           !R_max
      par(3) = z_Cu/2.               !z/2
      call GSVOLU('LD2D','TUBE',tmat,PAR,3,IVOLU)
        z = par(3) - zm      
      call GSPOS('LD2D',1,'TRGT',0.,0.,z,0,'MANY')   

c======= Copper tube ===================>
      PAR(1) = r_Cu - t_Cu          !R_min
      PAR(2) = r_Cu                 !R_max
      PAR(3) = z_Cu/2
      call GSVOLU('CU64','TUBE',copper,PAR,3,IVOLU)       
        z = par(3) - zm      
      call GSPOS('CU64',1,'TRGT',0.,0.,z,0,'ONLY')   

c==========================================================>
      return
      end
c====================================================================!

    

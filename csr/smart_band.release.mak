###########################################################
# Makefile generated by xIDE for uEnergy                   
#                                                          
# Project: smart_band
# Configuration: Release
# Generated: ���� ���� 25 11:53:36 2013
#                                                          
# WARNING: Do not edit this file. Any changes will be lost 
#          when the project is rebuilt.                    
#                                                          
###########################################################

XIDE_PROJECT=smart_band
XIDE_CONFIG=Release
OUTPUT=smart_band
OUTDIR=C:/Users/Administrator/Documents/GitHub/Rhythm/csr
DEFS=

LIBRARY_VERSION=Auto
SWAP_INTO_DATA=0
USE_FLASH=0
ERASE_NVM=1
CSFILE_CSR100x=
CSFILE_CSR101x_A05=
MASTER_DB=app_gatt_db.db

DBS=\
\
      app_gatt_db.db

INPUTS=\
      smart_band.c\
      $(DBS)

KEYR=


-include smart_band.mak
include $(SDK)/genmakefile.uenergy
